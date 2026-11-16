/* AFR2-20261110-16 / AFR2-20261111-08. Sends a whole GOTO route to the robot and follows it.
 *
 * AFR2-20261111-08: the route is uploaded ONCE (robot.route.begin / leg / start, EX01 0x46..0x48) and the ATmega runs every leg
 * locally, aiming each one at the accumulated target in the start frame (leg errors do not add up). After START the Web only reads
 * robot.route.status (0x49) every POLL_MS: current leg, legs done, fault and the pose in the start frame, to validate each node and
 * draw the real trajectory over the planned one. A WebSocket or Wi-Fi drop does not stop the car: polling resumes on reconnect and
 * the final report (state, pose) stays in the car until the next route. Previously each leg was a separate robot.goto.start and a
 * drop stopped the run. Arcs and curves travel as their sampled polyline. */
import { useCallback, useEffect, useRef, useState } from 'react';
import { useConnection } from '../../connection/ConnectionContext';
import type { GotoPlan } from './gotoSimulation';

/** thetaMrad (optional, default 0) turns while translating; x=y=0 with theta is an in-place turn. `segment` = command index of the plan. */
export interface GotoLeg { xMm: number; yMm: number; thetaMrad?: number; segment?: number }
export type GotoRunPhase = 'idle' | 'uploading' | 'running' | 'completed' | 'cancelled' | 'failed';
export interface RoutePose { xMm: number; yMm: number; thetaMrad: number }
export interface GotoRunState {
  phase: GotoRunPhase; leg: number; legs: number; done: number; message: string | null;
  /** Pose informed by the car, in the start frame (X forward, Y left). */
  pose: RoutePose | null; trail: RoutePose[]; legsMeta: GotoLeg[];
}
/** How a run ended, for the minimum-tests log. `stop` = STOP pressed; `cancelled` = cancelled by the app or the car;
 * `fault` = firmware fault F1..F4. */
export type GotoRunCause = 'completed' | 'stop' | 'cancelled' | 'fault' | 'failed' | 'timeout' | 'rejected';
export interface GotoRunResult { cause: GotoRunCause; leg: number; legs: number; fault: number | null; message: string | null; pose: RoutePose | null; done: number; elapsedMs: number | null }

/** Firmware limits: |x|,|y| <= 5000 mm per leg, up to 32 legs. */
const LEG_MAX_MM = 5000;
const LEG_MIN_MM = 20;
const CURVE_STEP_MM = 150;
export const ROUTE_MAX_LEGS = 32;
const LEG_TIMEOUT_MS = 65_000;
const POLL_MS = 300;
const TRAIL_STEP_MM = 5;

/** AFR2-20261113-01: los tramos salen de los cuadros de pose del plan. Una traslación lleva el cambio de rumbo que ocurre
 * mientras avanza (curvas en «Seguir camino»); un giro en el lugar (esquina o nodo de giro) es un tramo x=y=0 con theta.
 * X/Y siguen en el marco de partida y theta es incremental: el ATmega acumula el rumbo objetivo. */
const TURN_LEG_MAX_MRAD = 3140;
const TURN_LEG_MIN_MRAD = 26;
export function gotoRouteLegs(plan: GotoPlan): GotoLeg[] {
  const legs: GotoLeg[] = [];
  let from = { xMm: 0, yMm: 0 }, legTheta = 0;
  const pushMove = (to: { xMm: number; yMm: number }, theta: number, segment: number) => {
    const dx = to.xMm - from.xMm, dy = to.yMm - from.yMm, length = Math.hypot(dx, dy);
    if (length < LEG_MIN_MM) return;
    const totalTurn = Math.round((theta - legTheta) * 1000);
    const parts = Math.max(Math.ceil(Math.max(Math.abs(dx), Math.abs(dy)) / LEG_MAX_MM), Math.ceil(Math.abs(totalTurn) / TURN_LEG_MAX_MRAD));
    let sentTurn = 0;
    for (let i = 1; i <= parts; i++) {
      const x = Math.round(from.xMm + dx * i / parts), y = Math.round(from.yMm + dy * i / parts);
      const previous = i === 1 ? from : { xMm: Math.round(from.xMm + dx * (i - 1) / parts), yMm: Math.round(from.yMm + dy * (i - 1) / parts) };
      const turn = Math.round(totalTurn * i / parts) - sentTurn; sentTurn += turn;
      legs.push({ xMm: x - previous.xMm, yMm: y - previous.yMm, thetaMrad: turn, segment });
    }
    from = to; legTheta += sentTurn / 1000;
  };
  const pushTurn = (theta: number, segment: number) => {
    const total = Math.round((theta - legTheta) * 1000);
    if (Math.abs(total) < TURN_LEG_MIN_MRAD) return;
    // Partes iguales (180° = 2 × 90°, nunca un resto de 1 mrad).
    const parts = Math.ceil(Math.abs(total) / TURN_LEG_MAX_MRAD);
    let sent = 0;
    for (let i = 1; i <= parts; i++) { const turn = Math.round(total * i / parts) - sent; sent += turn; legs.push({ xMm: 0, yMm: 0, thetaMrad: turn, segment }); }
    legTheta += sent / 1000;
  };
  plan.segments.forEach((segment, index) => {
    const frames = segment.frames;
    for (let at = 1; at < frames.length; at++) {
      const frame = frames[at], previous = frames[at - 1];
      const rotation = Math.hypot(frame.xMm - previous.xMm, frame.yMm - previous.yMm) < .5;
      if (rotation) { pushMove(previous, previous.theta, index); pushTurn(frame.theta, index); continue; }
      const next = frames[at + 1];
      const corner = !next || Math.hypot(next.xMm - frame.xMm, next.yMm - frame.yMm) < .5;
      if (corner || Math.hypot(frame.xMm - from.xMm, frame.yMm - from.yMm) >= CURVE_STEP_MM) pushMove(frame, frame.theta, index);
    }
    // Rumbo final del tramo si quedó un resto chico sin traslación que lo lleve.
    const last = frames[frames.length - 1];
    if (last) pushTurn(last.theta, index);
  });
  return legs;
}

const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));
const routeToken = () => 1 + Math.floor(Math.random() * 0x7ffffffe);
const STATE_CAUSE: Record<string, GotoRunCause> = { completed: 'completed', cancelled: 'cancelled', failed: 'failed' };
interface StatusPayload { token: number; state: string; count: number; leg: number; done: number; fault: number; pose: RoutePose; elapsedMs: number }

const IDLE: GotoRunState = { phase: 'idle', leg: 0, legs: 0, done: 0, message: null, pose: null, trail: [], legsMeta: [] };

export function useGotoRouteRunner() {
  const { requestCommand, setOperationActivity, stopRecord } = useConnection();
  const [state, setState] = useState<GotoRunState>(IDLE);
  const generation = useRef(0);
  const latestStop = useRef(stopRecord.lastIssuedAtMs); latestStop.current = stopRecord.lastIssuedAtMs;
  useEffect(() => () => { ++generation.current; }, []);

  const run = useCallback(async (legs: GotoLeg[], maxSpeedMmS: number): Promise<GotoRunResult> => {
    const base: GotoRunResult = { cause: 'rejected', leg: 0, legs: legs.length, fault: null, message: null, pose: null, done: 0, elapsedMs: null };
    if (!legs.length) return { ...base, message: 'No hay tramos para enviar.' };
    if (legs.length > ROUTE_MAX_LEGS) return { ...base, message: `El auto acepta hasta ${ROUTE_MAX_LEGS} tramos por recorrido (este tiene ${legs.length}).` };
    const own = ++generation.current, token = routeToken(), stopAtStart = latestStop.current;
    const mine = () => own === generation.current;
    setState({ ...IDLE, phase: 'uploading', legs: legs.length, legsMeta: legs, message: 'Cargando el recorrido en el auto…' });
    // Upload: the only part that needs the link. The STOP fence applies here.
    setOperationActivity('command', true);
    try {
      await requestCommand('robot.route.begin', { token, count: legs.length, maxSpeedMmS }, 3000);
      for (const [index, leg] of legs.entries()) {
        if (!mine() || latestStop.current !== stopAtStart) return { ...base, cause: 'stop', message: 'STOP antes de iniciar el recorrido.' };
        await requestCommand('robot.route.leg', { token, index, xMm: Math.round(leg.xMm), yMm: Math.round(leg.yMm), thetaMrad: Math.round(leg.thetaMrad ?? 0) }, 3000);
      }
      await requestCommand('robot.route.start', { token }, 3000);
    } catch (error) {
      const message = error instanceof Error ? error.message : 'El auto rechazó el recorrido.';
      if (mine()) setState(previous => ({ ...previous, phase: 'failed', message }));
      return { ...base, message };
    } finally {
      // From START on the car runs alone: no Web operation pending, so losing focus or the link does not send STOP.
      setOperationActivity('command', false);
    }
    // Follow: read status until a final state. Lost reads (WS/Wi-Fi drop) are skipped; the car keeps the report.
    setState(previous => ({ ...previous, phase: 'running', message: null }));
    const deadline = Date.now() + legs.length * LEG_TIMEOUT_MS + 10_000;
    let trail: RoutePose[] = [{ xMm: 0, yMm: 0, thetaMrad: 0 }];
    let last: StatusPayload | null = null;
    while (mine()) {
      await sleep(POLL_MS);
      if (!mine()) break;
      if (Date.now() > deadline) {
        const result: GotoRunResult = { ...base, cause: 'timeout', message: 'El auto no informó el final del recorrido a tiempo.', pose: last?.pose ?? null, done: last?.done ?? 0, leg: (last?.leg ?? 0) + 1 };
        setState(previous => ({ ...previous, phase: 'failed', message: result.message }));
        return result;
      }
      let status: StatusPayload;
      try {
        const response = await requestCommand<StatusPayload>('robot.route.status', { token }, 2500);
        if (response.status !== 'completed' || response.payload.token !== token) continue;
        status = response.payload;
      } catch { continue; } // one lost read (or a WS drop) is not a mission result
      last = status;
      const tail = trail[trail.length - 1];
      if (status.pose && Math.hypot(status.pose.xMm - tail.xMm, status.pose.yMm - tail.yMm) >= TRAIL_STEP_MM) trail = [...trail, status.pose].slice(-2000);
      setState(previous => ({ ...previous, phase: 'running', leg: Math.min(status.count, status.leg + 1), done: status.done, pose: status.pose ?? previous.pose, trail }));
      const cause = STATE_CAUSE[status.state];
      if (!cause) continue;
      const stopped = latestStop.current !== stopAtStart && cause === 'cancelled';
      const result: GotoRunResult = {
        cause: status.fault ? 'fault' : stopped ? 'stop' : cause, fault: status.fault || null, leg: Math.min(status.count, status.leg + 1), legs: status.count,
        message: cause === 'completed' ? null : status.fault ? `Falla F${status.fault} en el tramo ${status.leg + 1}.` : stopped ? 'STOP durante el recorrido.' : cause === 'cancelled' ? 'Recorrido cancelado en el auto.' : `El auto no llegó al tramo ${status.leg + 1}.`,
        pose: status.pose ?? null, done: status.done, elapsedMs: status.elapsedMs,
      };
      setState(previous => ({ ...previous, phase: cause === 'completed' ? 'completed' : cause === 'cancelled' ? 'cancelled' : 'failed', message: result.message, done: status.done }));
      return result;
    }
    return { ...base, cause: 'cancelled', message: 'Seguimiento del recorrido cancelado en la Web.', pose: last?.pose ?? null, done: last?.done ?? 0 };
  }, [requestCommand, setOperationActivity]);

  /** Stops the car's route (robot.goto.cancel) and keeps reading its final report. */
  const cancel = useCallback(async () => {
    try { await requestCommand('robot.goto.cancel', {}, 3000); } catch { /* STOP remains available */ }
  }, [requestCommand]);
  /** Stop following in the Web only (the car keeps running). */
  const forget = useCallback(() => { ++generation.current; setState(IDLE); }, []);

  return { state, run, cancel, forget };
}
