# Biblioteca inicial. No hay firmware enlazable ni upload hasta integrar plataforma.
CC = avr-gcc
AR = avr-ar
CFLAGS = -mmcu=atmega2560 -DF_CPU=16000000UL -std=gnu11 -Os -Wall -Wextra -ffunction-sections -fdata-sections -Iinclude
SOURCES = src/services/afr2_i2c_manager.c src/display/afr2_oled.c src/display/font7x10.c src/input/afr2_encoder_ui.c src/ui/afr2_screens.c
OBJECTS = $(SOURCES:.c=.o)
.DEFAULT_GOAL := help
.PHONY: help library
help:
	@echo Biblioteca parcial AVR C. Leer INFORME_ADAPTACION.md antes de compilar.
	@echo No se compila automaticamente. Objetivo futuro explicito: make library
library: libafr2_ui.a
libafr2_ui.a: $(OBJECTS)
	$(AR) rcs $@ $^
