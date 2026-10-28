# AVR C. Default is help; build/upload never runs implicitly.
CC = avr-gcc
AR = avr-ar
OBJCOPY = avr-objcopy
CPPFLAGS = -DF_CPU=16000000UL -Iinclude -I../AutoFinalRobo2-Contracts/contracts
CFLAGS = -mmcu=atmega2560 -std=gnu11 -MMD -MP -Os -Wall -Wextra -fstack-usage -ffunction-sections -fdata-sections
SOURCES = src/services/afr2_i2c_manager.c src/display/afr2_oled.c src/display/font7x10.c src/input/afr2_encoder_ui.c src/ui/afr2_screens.c src/ui/afr2_sequence_screen.c src/ui/afr2_network_screen.c src/ui/afr2_icons.c src/services/afr2_network.c src/services/afr2_link.c src/platform/afr2_uart1.c src/platform/afr2_twi.c src/platform/afr2_board.c src/platform/afr2_motor.c src/platform/afr2_sensors.c src/platform/afr2_preferences.c src/platform/afr2_watchdog.c src/services/afr2_product.c src/services/afr2_sequence.c src/services/afr2_app.c src/services/afr2_app_wire.c src/ui/afr2_menu_catalog.c
SOURCES += src/services/afr2_sensor_commands.c src/ui/afr2_sensor_screens.c
OBJECTS = $(SOURCES:.c=.o)
# The naked reset initializer runs before main; GCC cannot emit .su for it.
src/platform/afr2_watchdog.o: CFLAGS := $(filter-out -fstack-usage,$(CFLAGS))
COMM_OBJECTS = src/communication_main.o src/services/afr2_link.o src/platform/afr2_uart1.o src/platform/afr2_board.o
ifeq ($(OS),Windows_NT)
MKDIR_BUILD = powershell.exe -NoProfile -NonInteractive -Command "[void][System.IO.Directory]::CreateDirectory('build')"
else
MKDIR_BUILD = mkdir -p build
endif
.DEFAULT_GOAL := help
.PHONY: help library firmware communication-firmware esp-bridge-firmware
help:
	@echo AVR C: leer INFORME_ADAPTACION.md y docs/COMPOSICION.md.
	@echo Bajo pedido expreso: library, firmware o communication-firmware. Sin upload.
	@echo Servicio opcional: esp-bridge-firmware BRIDGE_BAUD=115200. Reemplaza temporalmente el producto al cargarlo.
library: libafr2_ui.a
libafr2_ui.a: $(OBJECTS)
	$(AR) rcs $@ $^
firmware: build/afr2.hex
communication-firmware: build/afr2-communication.hex
# Standalone service, never part of firmware/library/default build. Rate in filename
# prevents reusing a HEX from a different fixed-baud selection.
BRIDGE_BAUD ?= 115200
esp-bridge-firmware: build/afr2-esp-bridge-$(BRIDGE_BAUD).hex
build/afr2-esp-bridge-$(BRIDGE_BAUD).elf: service/esp_uart_bridge.c
	$(MKDIR_BUILD)
	$(CC) $(CPPFLAGS) $(filter-out -fstack-usage,$(CFLAGS)) -DAFR2_BRIDGE_BAUD=$(BRIDGE_BAUD)UL -Wl,--gc-sections,-Map,build/afr2-esp-bridge-$(BRIDGE_BAUD).map -o $@ $<
build/afr2.elf: src/main.o $(OBJECTS)
	$(MKDIR_BUILD)
	$(CC) $(CFLAGS) -Wl,--gc-sections,-Map,build/afr2.map -o $@ $^ -lm
build/afr2-communication.elf: $(COMM_OBJECTS)
	$(MKDIR_BUILD)
	$(CC) $(CFLAGS) -Wl,--gc-sections,-Map,build/afr2-communication.map -o $@ $^
build/%.hex: build/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
-include $(OBJECTS:.o=.d) src/main.d src/communication_main.d
