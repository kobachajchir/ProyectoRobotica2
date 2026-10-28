// AFR2-20261025-12. Own development-only TCP proxy; no external dependencies.
// Forward the existing loopback server, including HTTP upgrades and HMR.
import { createConnection, createServer } from 'node:net';
import { networkInterfaces } from 'node:os';

function port(name, fallback) {
  const value = Number(process.env[name] ?? fallback);
  if (!Number.isInteger(value) || value < 1 || value > 65535) {
    throw new Error(`${name} debe ser un puerto entre 1 y 65535.`);
  }
  return value;
}
const listenPort = port('AFR2_LAN_PORT', 5180);
const targetPort = port('AFR2_DEV_PORT', 5173);
if (listenPort === targetPort) throw new Error('El proxy y Vite deben usar puertos distintos.');
const sockets = new Set();
function track(socket) {
  sockets.add(socket);
  socket.once('close', () => sockets.delete(socket));
  socket.setNoDelay(true);
  return socket;
}
const server = createServer({ allowHalfOpen: true }, client => {
  track(client).pause();
  const upstream = track(createConnection({
    host: 'localhost', port: targetPort, autoSelectFamily: true, allowHalfOpen: true,
  }));
  client.once('error', () => upstream.destroy());
  upstream.once('error', error => {
    console.error(`No se pudo reenviar a localhost:${targetPort}: ${error.code ?? error.message}`);
    client.destroy();
  });
  client.once('close', () => upstream.destroy());
  // Normal EOF is forwarded by pipe/end; let pending client writes drain.
  upstream.once('close', () => { if (!upstream.readableEnded) client.destroy(); });
  upstream.setTimeout(5000, () => upstream.destroy(new Error('Tiempo de conexión agotado.')));
  upstream.once('connect', () => {
    upstream.setTimeout(0);
    client.pipe(upstream);
    upstream.pipe(client);
  });
});
server.once('error', error => {
  console.error(`No se pudo abrir el proxy en el puerto ${listenPort}: ${error.code ?? error.message}`);
  process.exitCode = 1;
});
server.listen(listenPort, '0.0.0.0', () => {
  console.log(`Proxy LAN AFR2 activo: puerto ${listenPort} -> localhost:${targetPort}`);
  const addresses = new Set(Object.values(networkInterfaces()).flat()
    .filter(address => address && address.family === 'IPv4' && !address.internal && !address.address.startsWith('169.254.'))
    .map(address => address.address));
  for (const address of addresses) console.log(`  http://${address}:${listenPort}/`);
  console.log('Mantener Vite abierto. Ctrl+C cierra el proxy.');
});
function close() {
  server.close();
  for (const socket of sockets) socket.destroy();
}
process.once('SIGINT', close);
process.once('SIGTERM', close);
