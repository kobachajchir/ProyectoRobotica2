#pragma once
#include <Arduino.h>
// OSOYOO Mecanum 4WD - asignacion documental de pines
// Fuente: ManualOsoyoo.pdf (pp. 14-17, 26-27, 33-34, 38, 43)
// NO incluye logica de giro, polaridad del driver ni codigo validado.

namespace OsoyooPins {
  // Model Y - grupo M_A
  constexpr int MA_ENA = 11;
  constexpr int MA_IN1 = 5;
  constexpr int MA_IN2 = 6;
  constexpr int MA_IN3 = 7;
  constexpr int MA_IN4 = 8;
  constexpr int MA_ENB = 12;

  // Model Y - grupo M_B
  constexpr int MB_ENA = 9;
  constexpr int MB_IN1 = 22;
  constexpr int MB_IN2 = 24;
  constexpr int MB_IN3 = 26;
  constexpr int MB_IN4 = 28;
  constexpr int MB_ENB = 10;

  // Sensores
  constexpr int SERVO_SIGNAL = 13;
  constexpr int ULTRASONIC_TRIG = 30;
  constexpr int ULTRASONIC_ECHO = 31;
  constexpr int LINE_IR1 = A4;
  constexpr int LINE_IR2 = A3;
  constexpr int LINE_IR3 = A2;
  constexpr int LINE_IR4 = A1;
  constexpr int LINE_IR5 = A0;

  // UART1 compartida entre Bluetooth y WiFi segun montaje:
  // Arduino Mega TX1 = D18 -> B_RX o E_RX
  // Arduino Mega RX1 = D19 <- B_TX o E_TX
  constexpr int UART1_TX = 18;
  constexpr int UART1_RX = 19;
}

// Salidas de potencia (no pines Arduino):
// FL (front-left)   -> BK3/BK4
// FR (front-right)  -> BK1/BK2
// RL (rear-left)    -> AK3/AK4
// RR (rear-right)   -> AK1/AK2
// Encoder Hall de cada motor: Blanco VCC; amarillo GND;
// naranja S1; verde S2. NO conectados en el cableado base.
// Verificar tension del encoder y signo de cada rueda con el auto real.
