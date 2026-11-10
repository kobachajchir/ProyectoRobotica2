/* AFR2-20261110-16. Sends a whole simulated GOTO route to the robot as consecutive
 * relative robot.goto.start legs (the firmware accepts one straight target at a time).
 * Each leg waits for robot.state to report the mission finished before the next is sent.
 * Arcs and curves are sent as their sampled polyline, so the car stops briefly at each
 * vertex; heading stays constant (theta 0), as in the simulator. */
import { useCallback, useEffect, useRef, useState } from 'react';
import { useConnection } from '../../connection/ConnectionContext';
import { parseRobotState } from '../../../protocol/robotState';
import type { GotoPlan } from './gotoSimulation';

export interface GotoLeg { xMm: number; yMm: number }
export type GotoRunPhase = 'idle' | 'running' | 'completed' | 'cancelled' | 'failed';
export interface GotoRunState { phase: GotoRunPhase; leg: number; legs: number; message: string | null }

/** Firmware limits: |x|,|y| <= 5000 mm per mission and a 10 mm arrival tolerance. */
const LEG_MAX_MM = 5000;
const LEG_MIN_MM = 20;
const CURVE_STEP_MM = 150;
const LEG_TIMEOUT_MS = 65_000;
const POLL_MS = 250;

export function gotoRouteLegs(plan: GotoPlan): GotoLeg[] {
  const legs: GotoLeg[] = [];
  let from = { xMm: 0, yMm: 0 };
  const push = (to: { xMm: number; yMm: number }) => {
    const dx = to.xMm - from.xMm, dy = to.yMm - from.yMm, length = Math.hypot(dx, dy);
    if (length < LEG_MIN_MM) return;
    const parts = Math.ceil(Math.max(Math.abs(dx), Math.abs(dy)) / LEG_MAX_MM);
    for (let i = 1; i <= parts; i++) {
      const x = Math.round(from.xMm + dx * i / parts), y = Math.round(from.yMm + dy * i / parts);
      const previous = i === 1 ? from : { xMm: Math.round(from.xMm + dx * (i - 1) / parts), yMm: Math.round(from.yMm + dy * (i - 1) / parts) };
      legs.push({ xMm: x - previous.xMm, yMm: y - previous.yMm });
    }
    from = to;
  };
  for (const segment of plan.segments) {
    const points = segment.points.slice(1);
    points.forEach((point, index) => {
      const last = index === points.length - 1;
      if (last || Math.hypot(point.xMm - from.xMm, point.yMm - from.yMm) >= CURVE_STEP_MM) push(point);
    });
  }
  return legs;
}

const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));

export function useGotoRouteRunner() {
  const { requestCommand, setOperationActivity, stopRecord } = useConnection();
  const [state, setState] = useState<GotoRunState>({ phase: 'idle', leg: 0, legs: 0, message: null });
  const generation = useRef(0);
  const stopAtStart = useRef(stopRecord.lastIssuedAtMs);
  const latestStop = useRef(stopRecord.lastIssuedAtMs); latestStop.current = stopRecord.lastIssuedAtMs;

  const abort = useCallback((message: string) => {
    ++generation.current;
    setOperationActivity('command', false);
    setState(previous => previous.phase === 'running' ? { ...previous, phase: 'cancelled', message } : previous);
  }, [setOperationActivity]);

  // A STOP from anywhere ends the route; the firmware already halted the current leg.
  useEffect(() => {
    if (state.phase === 'running' && stopRecord.lastIssuedAtMs !== stopAtStart.current) abort('Recorrido detenido por STOP.');
  }, [stopRecord.lastIssuedAtMs, state.phase, abort]);
  useEffect(() => () => { ++generation.current; }, []);

  const run = useCallback(async (legs: GotoLeg[], maxSpeedMmS: number) => {
    if (!legs.length) return;
    const own = ++generation.current;
    stopAtStart.current = latestStop.current;
    const alive = () => own === generation.current && latestStop.current === stopAtStart.current;
    setOperationActivity('command', true);
    setState({ phase: 'running', leg: 0, legs: legs.length, message: null });
    try {
      for (const [index, leg] of legs.entries()) {
        if (!alive()) return;
        setState({ phase: 'running', leg: index + 1, legs: legs.length, message: null });
        await requestCommand('robot.goto.start', { xMm: leg.xMm, yMm: leg.yMm, thetaMrad: 0, maxSpeedMmS }, 3000);
        const acceptedAt = Date.now();
        for (;;) {
          await sleep(POLL_MS);
          if (!alive()) return;
          if (Date.now() - acceptedAt > LEG_TIMEOUT_MS) throw new Error(`El tramo ${index + 1} no terminó en ${LEG_TIMEOUT_MS / 1000} s.`);
          let snapshot;
          try {
            const response = await requestCommand<Record<string, unknown>>('robot.state', {}, 2500);
            if (response.status !== 'completed') continue;
            snapshot = parseRobotState(response.payload);
          } catch { continue; }   // one lost read is not a mission result
          if (snapshot.fault) throw new Error(`Fallo ${snapshot.fault} durante el tramo ${index + 1}.`);
          if (snapshot.gotoState === 'running' || snapshot.mode === 'goto') continue;
          if (snapshot.gotoState === 'completed') break;
          if (snapshot.gotoState === 'cancelled') throw new Error(`Tramo ${index + 1} cancelado en el auto.`);
          if (snapshot.gotoState === 'failed') throw new Error(`El auto no llegó al destino del tramo ${index + 1}.`);
        }
      }
      if (alive()) setState({ phase: 'completed', leg: legs.length, legs: legs.length, message: null });
    } catch (error) {
      if (alive()) setState(previous => ({ ...previous, phase: 'failed', message: error instanceof Error ? error.message : 'El auto rechazó el recorrido.' }));
    } finally {
      if (own === generation.current) setOperationActivity('command', false);
    }
  }, [requestCommand, setOperationActivity]);

  const cancel = useCallback(async () => {
    abort('Recorrido cancelado.');
    try { await requestCommand('robot.goto.cancel', {}, 3000); } catch { /* STOP remains available */ }
  }, [abort, requestCommand]);

  return { state, run, cancel };
}
