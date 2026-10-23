import React from 'react';
import {createRoot} from 'react-dom/client';
import './style.css';
const sections=['Control Mecanum','GOTO relativo','Pruebas por rueda','Sensores de línea y HC-SR04','Wi-Fi','OLED y navegación'];
function App(){return <main><p>PROYECTO INDEPENDIENTE</p><h1>AutoFinalRobo2</h1><p>Base de interfaz. El enlace con ESP12S y ATmega todavía no está implementado.</p><ul>{sections.map(s=><li key={s}>{s}<span>Pendiente de integración</span></li>)}</ul><p>No se envían órdenes al vehículo.</p></main>}
createRoot(document.getElementById('root')).render(<App/>);
