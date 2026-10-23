#ifndef AFR2_BOARD_PINMAP_H
#define AFR2_BOARD_PINMAP_H
#include <avr/io.h>
/* OSOYOO + Mega oficial. UI propuesta: no cableado confirmado. */
/* FR_PWM: D9/PH6, documented-unverified-physical */
#define AFR2_FR_PWM_DDR DDRH
#define AFR2_FR_PWM_PORT PORTH
#define AFR2_FR_PWM_PIN PINH
#define AFR2_FR_PWM_BIT 6u
/* FL_PWM: D10/PB4, documented-unverified-physical */
#define AFR2_FL_PWM_DDR DDRB
#define AFR2_FL_PWM_PORT PORTB
#define AFR2_FL_PWM_PIN PINB
#define AFR2_FL_PWM_BIT 4u
/* RR_PWM: D11/PB5, documented-unverified-physical */
#define AFR2_RR_PWM_DDR DDRB
#define AFR2_RR_PWM_PORT PORTB
#define AFR2_RR_PWM_PIN PINB
#define AFR2_RR_PWM_BIT 5u
/* RL_PWM: D12/PB6, documented-unverified-physical */
#define AFR2_RL_PWM_DDR DDRB
#define AFR2_RL_PWM_PORT PORTB
#define AFR2_RL_PWM_PIN PINB
#define AFR2_RL_PWM_BIT 6u
/* FR_IN1: D22/PA0, documented-unverified-physical */
#define AFR2_FR_IN1_DDR DDRA
#define AFR2_FR_IN1_PORT PORTA
#define AFR2_FR_IN1_PIN PINA
#define AFR2_FR_IN1_BIT 0u
/* FR_IN2: D24/PA2, documented-unverified-physical */
#define AFR2_FR_IN2_DDR DDRA
#define AFR2_FR_IN2_PORT PORTA
#define AFR2_FR_IN2_PIN PINA
#define AFR2_FR_IN2_BIT 2u
/* FL_IN1: D26/PA4, documented-unverified-physical */
#define AFR2_FL_IN1_DDR DDRA
#define AFR2_FL_IN1_PORT PORTA
#define AFR2_FL_IN1_PIN PINA
#define AFR2_FL_IN1_BIT 4u
/* FL_IN2: D28/PA6, documented-unverified-physical */
#define AFR2_FL_IN2_DDR DDRA
#define AFR2_FL_IN2_PORT PORTA
#define AFR2_FL_IN2_PIN PINA
#define AFR2_FL_IN2_BIT 6u
/* RR_IN1: D5/PE3, documented-unverified-physical */
#define AFR2_RR_IN1_DDR DDRE
#define AFR2_RR_IN1_PORT PORTE
#define AFR2_RR_IN1_PIN PINE
#define AFR2_RR_IN1_BIT 3u
/* RR_IN2: D6/PH3, documented-unverified-physical */
#define AFR2_RR_IN2_DDR DDRH
#define AFR2_RR_IN2_PORT PORTH
#define AFR2_RR_IN2_PIN PINH
#define AFR2_RR_IN2_BIT 3u
/* RL_IN1: D7/PH4, documented-unverified-physical */
#define AFR2_RL_IN1_DDR DDRH
#define AFR2_RL_IN1_PORT PORTH
#define AFR2_RL_IN1_PIN PINH
#define AFR2_RL_IN1_BIT 4u
/* RL_IN2: D8/PH5, documented-unverified-physical */
#define AFR2_RL_IN2_DDR DDRH
#define AFR2_RL_IN2_PORT PORTH
#define AFR2_RL_IN2_PIN PINH
#define AFR2_RL_IN2_BIT 5u
/* SERVO: D13/PB7, documented-unverified-physical */
#define AFR2_SERVO_DDR DDRB
#define AFR2_SERVO_PORT PORTB
#define AFR2_SERVO_PIN PINB
#define AFR2_SERVO_BIT 7u
/* HCSR04_TRIG: D30/PC7, documented-unverified-physical */
#define AFR2_HCSR04_TRIG_DDR DDRC
#define AFR2_HCSR04_TRIG_PORT PORTC
#define AFR2_HCSR04_TRIG_PIN PINC
#define AFR2_HCSR04_TRIG_BIT 7u
/* HCSR04_ECHO: D31/PC6, documented-unverified-physical */
#define AFR2_HCSR04_ECHO_DDR DDRC
#define AFR2_HCSR04_ECHO_PORT PORTC
#define AFR2_HCSR04_ECHO_PIN PINC
#define AFR2_HCSR04_ECHO_BIT 6u
/* LINE_IR1: A4/PF4, documented-unverified-physical */
#define AFR2_LINE_IR1_DDR DDRF
#define AFR2_LINE_IR1_PORT PORTF
#define AFR2_LINE_IR1_PIN PINF
#define AFR2_LINE_IR1_BIT 4u
/* LINE_IR2: A3/PF3, documented-unverified-physical */
#define AFR2_LINE_IR2_DDR DDRF
#define AFR2_LINE_IR2_PORT PORTF
#define AFR2_LINE_IR2_PIN PINF
#define AFR2_LINE_IR2_BIT 3u
/* LINE_IR3: A2/PF2, documented-unverified-physical */
#define AFR2_LINE_IR3_DDR DDRF
#define AFR2_LINE_IR3_PORT PORTF
#define AFR2_LINE_IR3_PIN PINF
#define AFR2_LINE_IR3_BIT 2u
/* LINE_IR4: A1/PF1, documented-unverified-physical */
#define AFR2_LINE_IR4_DDR DDRF
#define AFR2_LINE_IR4_PORT PORTF
#define AFR2_LINE_IR4_PIN PINF
#define AFR2_LINE_IR4_BIT 1u
/* LINE_IR5: A0/PF0, documented-unverified-physical */
#define AFR2_LINE_IR5_DDR DDRF
#define AFR2_LINE_IR5_PORT PORTF
#define AFR2_LINE_IR5_PIN PINF
#define AFR2_LINE_IR5_BIT 0u
/* ESP_TX: D18/PD3, documented-unverified-physical */
#define AFR2_ESP_TX_DDR DDRD
#define AFR2_ESP_TX_PORT PORTD
#define AFR2_ESP_TX_PIN PIND
#define AFR2_ESP_TX_BIT 3u
/* ESP_RX: D19/PD2, documented-unverified-physical */
#define AFR2_ESP_RX_DDR DDRD
#define AFR2_ESP_RX_PORT PORTD
#define AFR2_ESP_RX_PIN PIND
#define AFR2_ESP_RX_BIT 2u
/* OLED_SDA: D20/PD1, proposed-new */
#define AFR2_OLED_SDA_DDR DDRD
#define AFR2_OLED_SDA_PORT PORTD
#define AFR2_OLED_SDA_PIN PIND
#define AFR2_OLED_SDA_BIT 1u
/* OLED_SCL: D21/PD0, proposed-new */
#define AFR2_OLED_SCL_DDR DDRD
#define AFR2_OLED_SCL_PORT PORTD
#define AFR2_OLED_SCL_PIN PIND
#define AFR2_OLED_SCL_BIT 0u
/* ENC_UI_CLK: D40/PG1, proposed-new */
#define AFR2_ENC_UI_CLK_DDR DDRG
#define AFR2_ENC_UI_CLK_PORT PORTG
#define AFR2_ENC_UI_CLK_PIN PING
#define AFR2_ENC_UI_CLK_BIT 1u
/* ENC_UI_DT: D41/PG0, proposed-new */
#define AFR2_ENC_UI_DT_DDR DDRG
#define AFR2_ENC_UI_DT_PORT PORTG
#define AFR2_ENC_UI_DT_PIN PING
#define AFR2_ENC_UI_DT_BIT 0u
/* ENC_UI_SW: D42/PL7, proposed-new */
#define AFR2_ENC_UI_SW_DDR DDRL
#define AFR2_ENC_UI_SW_PORT PORTL
#define AFR2_ENC_UI_SW_PIN PINL
#define AFR2_ENC_UI_SW_BIT 7u

#endif
