#ifndef AFR2_DASHBOARD_H
#define AFR2_DASHBOARD_H
#include <stdint.h>
#define AFR2_DASHBOARD_IDLE_MS 3000UL
#define AFR2_DASHBOARD_PAGE_MS 4000UL
/* Main-loop owned. No extra framebuffer, heap, hardware reads or timers. */
typedef struct {
    uint32_t input_ms,rotate_ms,network_boot;
    uint16_t network_revision,battery_mv,ssid_offset,ip_offset;
    uint8_t valid,held,armed,mask,selected,drawn,mode,header_flags;
} Afr2DashboardState;
#endif
