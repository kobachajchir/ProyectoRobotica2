/*
 * ssd1306.c
 *
 *  Created on: 14/04/2018
 *  Update on: 10/04/2019
 *      Author: Andriy Honcharenko
 *      version: 2
 *
 *  Modify on: 06/11/2021
 *      Author: Roberto Benjami
 *  Added features in DMA mode:
 *  - ssd1306_UpdateScreen_H works without blocking
 *  - you can query that UpdateScreen is complete (if ssd1306_UpdateScreenCompleted_H(h) == 1)
 *  - callback function if UpdateScreen is complete (ssd1306_UpdateCompletedCallback)
 *  Added features in DMA mode with continuous display update:
 *  - enable continuous display update
 *  - disable continuous display update
 *  - enable raster interrupt(s) of PAGEx (you can set which PAGE(s) )
 *
 *  Modified for I2C Manager arbitration (Koba):
 *  - When SSD1306_USE_I2C_MANAGER==1, the OLED never owns the bus permanently.
 *  - Update is event-driven: UpdateScreen() requests the bus; on grant, it runs a TX DMA state-machine.
 *  - No HAL_I2C_MemTxCpltCallback implementation here; completion must be routed via I2C_Manager_OnTxCplt/OnRxCplt().
 */

#include <math.h>
#include "ssd1306.h"

#if SSD1306_USE_I2C_MANAGER == 1 && SSD1306_CONTUPDATE == 1
#error "SSD1306_CONTUPDATE must be 0 when using I2C Manager (bus arbitration)."
#endif

#if SSD1306_USE_DMA == 0 && SSD1306_CONTUPDATE == 1
#error SSD1306_CONTUPDATE only in DMA MODE !
#endif

// Screen object

// Screenbuffer


// h->SSD1306 display geometry


//
//  Get a width and height screen size
//
static uint16_t width(SSD1306_Handle *h)  { return SSD1306_WIDTH; }
static uint16_t height(SSD1306_Handle *h) { return SSD1306_HEIGHT; }

uint16_t ssd1306_GetWidth_H(SSD1306_Handle *h)  { return SSD1306_WIDTH; }
uint16_t ssd1306_GetHeight_H(SSD1306_Handle *h) { return SSD1306_HEIGHT; }

SSD1306_COLOR ssd1306_GetColor_H(SSD1306_Handle *h) { return h->SSD1306.Color; }

void ssd1306_SetColor_H(SSD1306_Handle *h, SSD1306_COLOR color) { h->SSD1306.Color = color; }

/* -------------------------------------------------------------------------- */
/* I2C MANAGER INTEGRATION (event-driven DMA update)                           */
/* -------------------------------------------------------------------------- */
#if SSD1306_USE_I2C_MANAGER == 1




static uint8_t OLED_ShouldKeepBus(SSD1306_Handle *h, uint8_t page, uint8_t max_pages)
{
  I2C_PolicyMode policy;

  if ((page + 1u) >= max_pages) {
    return 0u;
  }
  if (!h->bus) {
    return 0u;
  }

  policy = h->bus->policy(h->bus->ctx);
  if (policy == I2C_POLICY_PRIORITIZE_MPU) {
    return 0u;
  }
  if (policy == I2C_POLICY_PRIORITIZE_OLED) {
    return 1u;
  }

  return ((page & 1u) == 0u) ? 1u : 0u;
}

// simple state machine for page update











// we reuse a single byte buffer for commands (DMA needs RAM address)

// manager callbacks
static void OLED_I2C_RequestApproved(void *ctx);
static void OLED_I2C_TxComplete(void *ctx);
static void OLED_I2C_TransferComplete(SSD1306_Handle *h);
static void OLED_I2C_Error(void *ctx, I2C_ManagerError err);
static uint8_t OLED_QueueHasPending(SSD1306_Handle *h);
static uint8_t OLED_QueueMaxPrio(SSD1306_Handle *h);
static uint8_t OLED_QueueSelectNext(SSD1306_Handle *h, uint8_t *slot_idx);
static uint8_t OLED_StartNextQueued(SSD1306_Handle *h);
static uint8_t OLED_QueueFindByPage(SSD1306_Handle *h, uint8_t page, uint8_t *slot_idx);
static AM_Result OLED_QueueUpsert(SSD1306_Handle *h, uint8_t page, I2C_RequestPrio prio, uint8_t keep_bus);
static AM_Result OLED_QueueDirectCommand(SSD1306_Handle *h, const uint8_t *commands, uint8_t len);
static uint8_t OLED_StartDirectCommand(SSD1306_Handle *h);
static void OLED_DirectCommandTxComplete(SSD1306_Handle *h);
static void OLED_SendForState(SSD1306_Handle *h, uint8_t page, OLED_SM_State state);

static void OLED_WriteDma(SSD1306_Handle *h, uint16_t reg, uint8_t *data, uint16_t len)
{
  if (h->bus->hardware->mem_write_dma(h->bus->hardware_ctx,
                                     SSD1306_I2C_ADDR, reg, 1u, data, len) != AM_OK) {
    if (h->bus->transfer_failed) {
      h->bus->transfer_failed(h->bus->ctx);
    } else {
      OLED_I2C_Error(h, I2C_MAN_ERR_INTERNAL);
      (void)h->bus->release(h->bus->ctx);
    }
  }
}

// helper: start first step of a frame
static void OLED_StartFrame(SSD1306_Handle *h, uint8_t page)
{
  h->oled_page = page;

   // BREAKPOINT: inicio de frame OLED (page 0)
  h->oled_cmd_byte = (uint8_t)(0xB0u + h->oled_page);
  OLED_WriteDma(h, 0x00u, &h->oled_cmd_byte, 1u);
}

static uint8_t OLED_QueueHasPending(SSD1306_Handle *h)
{
  if (h->oled_direct_pending || h->oled_direct_inflight) {
    return 1;
  }

  for (uint8_t i = 0; i < 8; i++) {
    if (h->g_page_q[i].used) {
      return 1;
    }
  }
  return 0;
}

static uint8_t OLED_QueueMaxPrio(SSD1306_Handle *h)
{
  I2C_RequestPrio max_prio = I2C_REQ_PRIO_LOW;
  if (h->oled_direct_pending || h->oled_direct_inflight) {
    max_prio = I2C_REQ_PRIO_HIGH;
  }

  for (uint8_t i = 0; i < 8; i++) {
    if (h->g_page_q[i].used && h->g_page_q[i].prio > max_prio) {
      max_prio = h->g_page_q[i].prio;
    }
  }
  return (uint8_t)max_prio;
}

static uint8_t OLED_QueueSelectNext(SSD1306_Handle *h, uint8_t *slot_idx)
{
  uint8_t best = 0xFFu;
  I2C_RequestPrio best_prio = I2C_REQ_PRIO_LOW;
  uint32_t best_seq = 0xFFFFFFFFu;

  for (uint8_t i = 0; i < 8; i++) {
    if (!h->g_page_q[i].used) {
      continue;
    }
    if (best == 0xFFu ||
        h->g_page_q[i].prio > best_prio ||
        (h->g_page_q[i].prio == best_prio && h->g_page_q[i].seq < best_seq)) {
      best = i;
      best_prio = h->g_page_q[i].prio;
      best_seq = h->g_page_q[i].seq;
    }
  }

  if (best == 0xFFu) {
    return 0;
  }

  *slot_idx = best;
  return 1;
}

static uint8_t OLED_StartNextQueued(SSD1306_Handle *h)
{
  uint8_t slot_idx = 0xFFu;

  if (OLED_StartDirectCommand(h)) {
    return 1u;
  }

  if (!OLED_QueueSelectNext(h, &slot_idx)) {
    return 0u;
  }

  h->oled_inflight_page = h->g_page_q[slot_idx].page;
  h->g_page_q[slot_idx].state = OLED_SM_CMD_PAGE;
   // BREAKPOINT: bus I2C otorgado al OLED (update pendiente)
  OLED_StartFrame(h, h->oled_inflight_page);
  return 1u;
}

static uint8_t OLED_QueueFindByPage(SSD1306_Handle *h, uint8_t page, uint8_t *slot_idx)
{
  for (uint8_t i = 0; i < 8; i++) {
    if (h->g_page_q[i].used && h->g_page_q[i].page == page) {
      *slot_idx = i;
      return 1;
    }
  }
  return 0;
}

static AM_Result OLED_QueueUpsert(SSD1306_Handle *h, uint8_t page, I2C_RequestPrio prio, uint8_t keep_bus)
{
  uint8_t free_idx = 0xFFu;
  uint8_t victim = 0xFFu;
  I2C_RequestPrio victim_prio = I2C_REQ_PRIO_HIGH;
  uint32_t victim_seq = 0;

  for (uint8_t i = 0; i < 8; i++) {
    if (h->g_page_q[i].used && h->g_page_q[i].page == page) {
      h->g_page_q[i].prio = (h->g_page_q[i].prio < prio) ? prio : h->g_page_q[i].prio;
      h->g_page_q[i].keep_bus = keep_bus;
      h->g_page_q[i].seq = h->g_seq++;
      if (page == h->oled_inflight_page && h->g_page_q[i].state != OLED_SM_IDLE) {
        h->g_page_q[i].pending_refresh = 1;
      } else {
        h->g_page_q[i].state = OLED_SM_IDLE;
        h->g_page_q[i].pending_refresh = 0;
      }
      return AM_OK;
    }
    if (!h->g_page_q[i].used && free_idx == 0xFFu) {
      free_idx = i;
    }
  }

  if (free_idx != 0xFFu) {
    h->g_page_q[free_idx].used = 1;
    h->g_page_q[free_idx].page = page;
    h->g_page_q[free_idx].prio = prio;
    h->g_page_q[free_idx].keep_bus = keep_bus;
    h->g_page_q[free_idx].state = OLED_SM_IDLE;
    h->g_page_q[free_idx].pending_refresh = 0;
    h->g_page_q[free_idx].seq = h->g_seq++;
    return AM_OK;
  }

  for (uint8_t i = 0; i < 8; i++) {
    if (!h->g_page_q[i].used) {
      continue;
    }
    if (victim == 0xFFu ||
        h->g_page_q[i].prio < victim_prio ||
        (h->g_page_q[i].prio == victim_prio && h->g_page_q[i].seq < victim_seq)) {
      victim = i;
      victim_prio = h->g_page_q[i].prio;
      victim_seq = h->g_page_q[i].seq;
    }
  }

  if (victim != 0xFFu) {
    h->g_page_q[victim].used = 1;
    h->g_page_q[victim].page = page;
    h->g_page_q[victim].prio = prio;
    h->g_page_q[victim].keep_bus = keep_bus;
    h->g_page_q[victim].state = OLED_SM_IDLE;
    h->g_page_q[victim].pending_refresh = 0;
    h->g_page_q[victim].seq = h->g_seq++;
    return AM_OK;
  }

  return AM_ERROR;
}

static void OLED_I2C_RequestApproved(void *ctx)
{
  SSD1306_Handle *h=ctx;
  (void)ctx;
  if (h->oled_inflight_page != 0xFFu || h->oled_direct_inflight) {
    return;
  }
  if (!OLED_StartNextQueued(h)) {
    if (h->bus) {
      (void)h->bus->release(h->bus->ctx);
    }
    return;
  }

  // If we got granted while already mid-transfer, do nothing.
  // Transfers advance when the main-loop manager dispatches DMA completion.
}

static void OLED_I2C_TxComplete(void *ctx)
{
  SSD1306_Handle *h=ctx;
  (void)ctx;
  if (h->bus && !h->bus->is_owner(h->bus->ctx)) {
    return;
  }
  if (h->oled_direct_inflight) {
    OLED_DirectCommandTxComplete(h);
    return;
  }
  if (h->oled_inflight_page == 0xFFu) {
    return;
  }
  OLED_I2C_TransferComplete(h);
}

static void OLED_I2C_TransferComplete(SSD1306_Handle *h)
{
  // OLED only does TX in this driver. If manager passes RX, ignore safely.
  if (h->oled_inflight_page == 0xFFu) {
    // spurious callback
    if (h->bus) {
      (void)h->bus->release(h->bus->ctx);
    }
    return;
  }

  uint8_t slot_idx = 0xFFu;
  if (!OLED_QueueFindByPage(h, h->oled_inflight_page, &slot_idx)) {
    h->oled_inflight_page = 0xFFu;
    if (h->bus) {
      (void)h->bus->release(h->bus->ctx);
    }
    return;
  }
  
  switch (h->g_page_q[slot_idx].state)
  {
    case OLED_SM_CMD_PAGE:
      h->g_page_q[slot_idx].state = OLED_SM_CMD_LOWCOL;
       // BREAKPOINT: comando de low column
      OLED_SendForState(h, h->oled_inflight_page, OLED_SM_CMD_LOWCOL);
      break;

    case OLED_SM_CMD_LOWCOL:
      h->g_page_q[slot_idx].state = OLED_SM_CMD_HIGHCOL;
       // BREAKPOINT: comando de high column
      OLED_SendForState(h, h->oled_inflight_page, OLED_SM_CMD_HIGHCOL);
      break;

    case OLED_SM_CMD_HIGHCOL:
      h->g_page_q[slot_idx].state = OLED_SM_DATA;
       // BREAKPOINT: envío de datos de página
      OLED_SendForState(h, h->oled_inflight_page, OLED_SM_DATA);
      break;

    case OLED_SM_DATA:
      h->g_page_q[slot_idx].state = OLED_SM_PAGE_DONE;
       // BREAKPOINT: frame OLED completo

      if (h->on_page) h->on_page(h->callback_ctx,h->oled_inflight_page);
      uint8_t keep_bus = h->g_page_q[slot_idx].keep_bus;
      uint8_t needs_refresh = h->g_page_q[slot_idx].pending_refresh;
      if (needs_refresh) {
        h->g_page_q[slot_idx].pending_refresh = 0;
        h->g_page_q[slot_idx].state = OLED_SM_IDLE;
        h->g_page_q[slot_idx].seq = h->g_seq++;
      } else {
        h->g_page_q[slot_idx].used = 0;
      }
      h->oled_inflight_page = 0xFFu;

      if (keep_bus != 0u && OLED_QueueHasPending(h)) {
        if (OLED_StartNextQueued(h)) {
          break;
        }
      }

      if (h->bus) {
        (void)h->bus->release(h->bus->ctx);
      }

      if (OLED_QueueHasPending(h) && h->bus) {
        (void)h->bus->request(h->bus->ctx, (I2C_RequestPrio)OLED_QueueMaxPrio(h));
      } else if (!OLED_QueueHasPending(h)) {
        if (h->on_presented) h->on_presented(h->callback_ctx);
        if (h->on_empty) h->on_empty(h->callback_ctx);
      }
      break;

    default:
      // Fail-safe: never deadlock the bus
      h->oled_inflight_page = 0xFFu;
      if (h->bus) {
        (void)h->bus->release(h->bus->ctx);
      }
      break;
  }
}

static void OLED_I2C_Error(void *ctx, I2C_ManagerError err)
{
  SSD1306_Handle *h=ctx;
  (void)ctx;
  (void)err;
  h->oled_error_count++;
  memset(h->g_page_q, 0, sizeof(h->g_page_q));
  h->oled_direct_pending = 0u;
  h->oled_direct_inflight = 0u;
  h->oled_direct_pending_len = 0u;
  h->oled_direct_active_len = 0u;
  h->oled_direct_pos = 0u;
  h->oled_inflight_page = 0xFFu;
  /* The manager stopped DMA and dropped ownership before this callback. */
}

static AM_Result OLED_QueueDirectCommand(SSD1306_Handle *h, const uint8_t *commands, uint8_t len)
{
  if (commands == NULL || len == 0u || len > sizeof(h->oled_direct_pending_buf)) {
    return AM_ERROR;
  }

  for (uint8_t i = 0u; i < len; i++) {
    h->oled_direct_pending_buf[i] = commands[i];
  }
  h->oled_direct_pending_len = len;
  h->oled_direct_pending = 1u;

  if (h->bus != NULL &&
      !h->oled_direct_inflight &&
      h->oled_inflight_page == 0xFFu) {
    (void)h->bus->request(h->bus->ctx, I2C_REQ_PRIO_HIGH);
  }

  return AM_OK;
}

static uint8_t OLED_StartDirectCommand(SSD1306_Handle *h)
{
  if (!h->oled_direct_pending) {
    return 0u;
  }

  for (uint8_t i = 0u; i < h->oled_direct_pending_len; i++) {
    h->oled_direct_active_buf[i] = h->oled_direct_pending_buf[i];
  }
  h->oled_direct_active_len = h->oled_direct_pending_len;
  h->oled_direct_pending = 0u;
  h->oled_direct_pending_len = 0u;
  h->oled_direct_pos = 0u;
  h->oled_direct_inflight = 1u;
  h->oled_cmd_byte = h->oled_direct_active_buf[0];
  OLED_WriteDma(h, 0x00u, &h->oled_cmd_byte, 1u);

  return 1u;
}

static void OLED_DirectCommandTxComplete(SSD1306_Handle *h)
{
  h->oled_direct_pos++;
  if (h->oled_direct_pos < h->oled_direct_active_len) {
    h->oled_cmd_byte = h->oled_direct_active_buf[h->oled_direct_pos];
    OLED_WriteDma(h, 0x00u, &h->oled_cmd_byte, 1u);
    return;
  }

  h->oled_direct_inflight = 0u;
  h->oled_direct_active_len = 0u;
  h->oled_direct_pos = 0u;

  if (OLED_StartNextQueued(h)) {
    return;
  }

  if (h->bus) {
    (void)h->bus->release(h->bus->ctx);
  }
}

static void OLED_SendForState(SSD1306_Handle *h, uint8_t page, OLED_SM_State state)
{
  switch (state)
  {
    case OLED_SM_CMD_LOWCOL:
      h->oled_cmd_byte = (uint8_t)SETLOWCOLUMN;
      OLED_WriteDma(h, 0x00u, &h->oled_cmd_byte, 1u);
      break;

    case OLED_SM_CMD_HIGHCOL:
      h->oled_cmd_byte = (uint8_t)SETHIGHCOLUMN;
      OLED_WriteDma(h, 0x00u, &h->oled_cmd_byte, 1u);
      break;

    case OLED_SM_DATA:
      OLED_WriteDma(h, 0x40u, &h->SSD1306_Buffer[SSD1306_WIDTH * page], width(h));
      break;

    default:
      break;
  }
}

#endif /* SSD1306_USE_I2C_MANAGER */

AM_Result ssd1306_BindPort_H(SSD1306_Handle *h, const AM_I2C_DevicePort *port)
{
    if (!port || !port->hardware || !port->bind || !port->request || !port->release) return AM_ERROR;
    memset(h,0,sizeof(*h)); h->bus=port;
    h->display_geometry=SSD1306_GEOMETRY;
    h->oled_rotation=OLED_ROTATION_180;
    h->oled_inflight_page=0xFFu;
    return port->bind(port->ctx,SSD1306_ADDRESS,1u,h,
                      OLED_I2C_RequestApproved,OLED_I2C_TxComplete,NULL,OLED_I2C_Error);
}

AM_Result ssd1306_RequestPageUpdate_H(SSD1306_Handle *h, uint8_t page,
                                            I2C_RequestPrio prio,
                                            uint8_t keep_bus)
{
#if SSD1306_USE_I2C_MANAGER == 1
  uint8_t max_pages = (uint8_t)(SSD1306_HEIGHT / 8);
  
  if (page >= max_pages) {
    return AM_ERROR;
  }
  if (OLED_QueueUpsert(h, page, prio, keep_bus) != AM_OK) {
    return AM_ERROR;
  }
  if (h->oled_inflight_page == 0xFFu && h->bus) {
    (void)h->bus->request(h->bus->ctx, prio);
  }
  return AM_OK;
#else
  (void)page;
  (void)prio;
  (void)keep_bus;
  return AM_ERROR;
#endif
}


/* -------------------------------------------------------------------------- */
/* INIT                                                                        */
/* -------------------------------------------------------------------------- */

// A blocking command write used ONLY during init (safe and simple).
static void ssd1306_WriteCommand_Blocking(SSD1306_Handle *h, uint8_t command)
{
  (void)h->bus->hardware->mem_write(h->bus->hardware_ctx, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);
}

//  Initialize the oled screen
uint8_t ssd1306_Init_H(SSD1306_Handle *h)
{
  h->oled_error_count = 0u;
   // BREAKPOINT: inicio de ssd1306_Init_H
  /* Check if LCD connected to I2C */
#if SSD1306_USE_I2C_MANAGER == 1
  // Manager must be initialized+registered BEFORE init:
  // I2C_Manager_RegisterDevice(hmgr, DEVICE_ID_OLED, SSD1306_ADDRESS, prio, ctx, OLED_I2C_RequestApproved, OLED_I2C_TxComplete, NULL, NULL)
  // But we still check readiness via HAL here to keep init robust:
  if (h->bus->hardware->probe(h->bus->hardware_ctx,
                            SSD1306_I2C_ADDR,
                            SSD1306_DEVICE_READY_TRIALS,
                            SSD1306_DEVICE_READY_TIMEOUT_MS) != AM_OK)
  {
    h->SSD1306.Initialized = 0;
    return 0;
  }
#endif
#if SSD1306_USE_I2C_MANAGER == 1
   // BREAKPOINT: OLED listo en I2C (init)
#else
  if (h->bus->hardware->probe(h->bus->hardware_ctx,
                            SSD1306_I2C_ADDR,
                            SSD1306_DEVICE_READY_TRIALS,
                            SSD1306_DEVICE_READY_TIMEOUT_MS) != AM_OK)
  {
    h->SSD1306.Initialized = 0;
    /* Return false */
    return 0;
  }
#endif

  // Wait for the screen to boot
  h->bus->hardware->delay_ms(h->bus->hardware_ctx, SSD1306_POWER_ON_DELAY_MS);

  /* Init LCD (BLOCKING ONLY HERE) */
  ssd1306_WriteCommand_Blocking(h, DISPLAYOFF);
  ssd1306_WriteCommand_Blocking(h, SETDISPLAYCLOCKDIV);
  ssd1306_WriteCommand_Blocking(h, 0xF0); // Increase speed of the display max ~96Hz
  ssd1306_WriteCommand_Blocking(h, SETMULTIPLEX);
  ssd1306_WriteCommand_Blocking(h, height(h) - 1);
  ssd1306_WriteCommand_Blocking(h, SETDISPLAYOFFSET);
  ssd1306_WriteCommand_Blocking(h, 0x00);
  ssd1306_WriteCommand_Blocking(h, SETSTARTLINE);
  ssd1306_WriteCommand_Blocking(h, CHARGEPUMP);
  ssd1306_WriteCommand_Blocking(h, 0x14);
  ssd1306_WriteCommand_Blocking(h, MEMORYMODE);
  ssd1306_WriteCommand_Blocking(h, 0x00);
  ssd1306_WriteCommand_Blocking(h, SEGREMAP | 0x01);
  ssd1306_WriteCommand_Blocking(h, COMSCANDEC);
  ssd1306_WriteCommand_Blocking(h, SETCOMPINS);

  if (h->display_geometry == GEOMETRY_128_64)
  {
    ssd1306_WriteCommand_Blocking(h, 0x12);
  }
  else if (h->display_geometry == GEOMETRY_128_32)
  {
    ssd1306_WriteCommand_Blocking(h, 0x02);
  }

  ssd1306_WriteCommand_Blocking(h, SETCONTRAST);

  if (h->display_geometry == GEOMETRY_128_64)
  {
    ssd1306_WriteCommand_Blocking(h, SSD1306_CONTRAST_128X64_DEFAULT);
  }
  else if (h->display_geometry == GEOMETRY_128_32)
  {
    ssd1306_WriteCommand_Blocking(h, SSD1306_CONTRAST_128X32_DEFAULT);
  }

  ssd1306_WriteCommand_Blocking(h, SETPRECHARGE);
  ssd1306_WriteCommand_Blocking(h, 0xF1);
  ssd1306_WriteCommand_Blocking(h, SETVCOMDETECT); //0xDB, (additionally needed to lower the contrast)
  ssd1306_WriteCommand_Blocking(h, 0x40);          //0x40 default, to lower the contrast, put 0
  ssd1306_WriteCommand_Blocking(h, DISPLAYALLON_RESUME);
  ssd1306_WriteCommand_Blocking(h, NORMALDISPLAY);
  ssd1306_WriteCommand_Blocking(h, 0x2e);            // stop scroll
  ssd1306_WriteCommand_Blocking(h, DISPLAYON);
   // BREAKPOINT: comandos de init enviados

  // Set default values for screen object
  h->SSD1306.CurrentX = 0;
  h->SSD1306.CurrentY = 0;
  h->SSD1306.Color = Black;

  // Clear screen
  ssd1306_Clear_H(h);

#if SSD1306_CONTUPDATE == 1
  // Continuous Update on (ONLY when not using manager)
  ssd1306_ContUpdateEnable_H(h);
#endif

  // Flush buffer to screen (non-blocking in DMA mode)
   // BREAKPOINT: primer UpdateScreen del OLED
  ssd1306_UpdateScreen_H(h);

  h->SSD1306.Initialized = 1;

  /* Return OK */
  return 1;
}

uint8_t ssd1306_GetDefaultContrast_H(SSD1306_Handle *h)
{
  if (h->display_geometry == GEOMETRY_128_32) {
    return SSD1306_CONTRAST_128X32_DEFAULT;
  }

  return SSD1306_CONTRAST_128X64_DEFAULT;
}

AM_Result ssd1306_SetContrast_H(SSD1306_Handle *h, uint8_t contrast)
{
#if SSD1306_USE_I2C_MANAGER == 1
  const uint8_t commands[] = { SETCONTRAST, contrast };
  return OLED_QueueDirectCommand(h, commands, (uint8_t)sizeof(commands));
#else
  ssd1306_WriteCommand_H(h, SETCONTRAST);
  ssd1306_WriteCommand_H(h, contrast);
  return AM_OK;
#endif
}

OledRotation_t ssd1306_GetRotation_H(SSD1306_Handle *h)
{
  return h->oled_rotation;
}

AM_Result ssd1306_SetRotation_H(SSD1306_Handle *h, OledRotation_t rotation)
{
  AM_Result status = AM_OK;
  const uint8_t commands0[] = { SEGREMAP, COMSCANINC };
  const uint8_t commands180[] = { (uint8_t)(SEGREMAP | 0x01u), COMSCANDEC };
  const uint8_t *commands = commands180;

  if (rotation == OLED_ROTATION_0) {
    commands = commands0;
  } else if (rotation == OLED_ROTATION_180) {
    commands = commands180;
  } else {
    return AM_ERROR;
  }

#if SSD1306_USE_I2C_MANAGER == 1
  status = OLED_QueueDirectCommand(h, commands, 2u);
#else
  ssd1306_WriteCommand_H(h, commands[0]);
  ssd1306_WriteCommand_H(h, commands[1]);
#endif

  if (status == AM_OK) {
    h->oled_rotation = rotation;
  }

  return status;
}

/* -------------------------------------------------------------------------- */
/* DRAWING                                                                      */
/* -------------------------------------------------------------------------- */

void ssd1306_Fill_H(SSD1306_Handle *h)
{
  uint32_t i;
  for(i = 0; i < sizeof(h->SSD1306_Buffer); i++)
  {
    h->SSD1306_Buffer[i] = (h->SSD1306.Color == Black) ? 0x00 : 0xFF;
  }
}

void ssd1306_DrawPixel_H(SSD1306_Handle *h, uint8_t x, uint8_t y)
{
  SSD1306_COLOR color = h->SSD1306.Color;

  if (x >= ssd1306_GetWidth_H(h) || y >= ssd1306_GetHeight_H(h))
  {
    return;
  }

  if (h->SSD1306.Inverted)
  {
    color = (SSD1306_COLOR) !color;
  }

  if (color == White)
  {
    h->SSD1306_Buffer[x + (y / 8) * width(h)] |= 1 << (y % 8);
  }
  else
  {
    h->SSD1306_Buffer[x + (y / 8) * width(h)] &= ~(1 << (y % 8));
  }
}

/* --- Rest of drawing functions unchanged (verbatim from original) --- */

void ssd1306_DrawLine_H(SSD1306_Handle *h, int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep)
  {
    SWAP_INT16_T(x0, y0);
    SWAP_INT16_T(x1, y1);
  }

  if (x0 > x1)
  {
    SWAP_INT16_T(x0, x1);
    SWAP_INT16_T(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0<=x1; x0++)
  {
    if (steep)
    {
      ssd1306_DrawPixel_H(h, y0, x0);
    }
    else
    {
      ssd1306_DrawPixel_H(h, x0, y0);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void ssd1306_DrawHorizontalLine_H(SSD1306_Handle *h, int16_t x, int16_t y, int16_t length)
{
  if (y < 0 || y >= height(h)) { return; }

  if (x < 0)
  {
    length += x;
    x = 0;
  }

  if ( (x + length) > width(h))
  {
    length = (width(h) - x);
  }

  if (length <= 0) { return; }

  uint8_t * bufferPtr = h->SSD1306_Buffer;
  bufferPtr += (y >> 3) * width(h);
  bufferPtr += x;

  uint8_t drawBit = 1 << (y & 7);

  switch (h->SSD1306.Color)
  {
    case White:
      while (length--)
      {
        *bufferPtr++ |= drawBit;
      };
      break;
    case Black:
      drawBit = ~drawBit;
      while (length--)
      {
        *bufferPtr++ &= drawBit;
      };
      break;
    case Inverse:
      while (length--)
      {
        *bufferPtr++ ^= drawBit;
      }; break;
  }
}

void ssd1306_DrawVerticalLine_H(SSD1306_Handle *h, int16_t x, int16_t y, int16_t length)
{
  if (x < 0 || x >= width(h)) return;

  if (y < 0)
  {
    length += y;
    y = 0;
  }

  if ( (y + length) > height(h))
  {
    length = (height(h) - y);
  }

  if (length <= 0) return;

  uint8_t yOffset = y & 7;
  uint8_t drawBit;
  uint8_t *bufferPtr = h->SSD1306_Buffer;

  bufferPtr += (y >> 3) * width(h);
  bufferPtr += x;

  if (yOffset)
  {
    yOffset = 8 - yOffset;
    drawBit = ~(0xFF >> (yOffset));

    if (length < yOffset)
    {
      drawBit &= (0xFF >> (yOffset - length));
    }

    switch (h->SSD1306.Color)
    {
      case White:   *bufferPtr |=  drawBit; break;
      case Black:   *bufferPtr &= ~drawBit; break;
      case Inverse: *bufferPtr ^=  drawBit; break;
    }

    if (length < yOffset) return;

    length -= yOffset;
    bufferPtr += width(h);
  }

  if (length >= 8)
  {
    switch (h->SSD1306.Color)
    {
      case White:
      case Black:
        drawBit = (h->SSD1306.Color == White) ? 0xFF : 0x00;
        do {
          *bufferPtr = drawBit;
          bufferPtr += width(h);
          length -= 8;
        } while (length >= 8);
        break;
      case Inverse:
        do {
          *bufferPtr = ~(*bufferPtr);
          bufferPtr += width(h);
          length -= 8;
        } while (length >= 8);
        break;
    }
  }

  if (length > 0)
  {
    drawBit = (1 << (length & 7)) - 1;
    switch (h->SSD1306.Color)
    {
      case White:   *bufferPtr |=  drawBit; break;
      case Black:   *bufferPtr &= ~drawBit; break;
      case Inverse: *bufferPtr ^=  drawBit; break;
    }
  }
}

void ssd1306_DrawRect_H(SSD1306_Handle *h, int16_t x, int16_t y, int16_t width, int16_t height)
{
  ssd1306_DrawHorizontalLine_H(h, x, y, width);
  ssd1306_DrawVerticalLine_H(h, x, y, height);
  ssd1306_DrawVerticalLine_H(h, x + width - 1, y, height);
  ssd1306_DrawHorizontalLine_H(h, x, y + height - 1, width);
}

void ssd1306_FillRect_H(SSD1306_Handle *h, int16_t xMove, int16_t yMove, int16_t width, int16_t height)
{
  for (int16_t x = xMove; x < xMove + width; x++)
  {
    ssd1306_DrawVerticalLine_H(h, x, yMove, height);
  }
}

void ssd1306_DrawTriangle_H(SSD1306_Handle *h, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
  ssd1306_DrawLine_H(h, x1, y1, x2, y2);
  ssd1306_DrawLine_H(h, x2, y2, x3, y3);
  ssd1306_DrawLine_H(h, x3, y3, x1, y1);
}

void ssd1306_DrawFillTriangle(SSD1306_Handle *h, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
  curpixel = 0;

  deltax = abs(x2 - x1);
  deltay = abs(y2 - y1);
  x = x1;
  y = y1;

  if (x2 >= x1)
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)
  {
    xinc1 = 0;
    yinc2 = 0;
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;
  }
  else
  {
    xinc2 = 0;
    yinc1 = 0;
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    ssd1306_DrawLine_H(h, x, y, x3, y3);

    num += numadd;
    if (num >= den)
    {
      num -= den;
      x += xinc1;
      y += yinc1;
    }
    x += xinc2;
    y += yinc2;
  }
}

void ssd1306_Polyline_H(SSD1306_Handle *h, const SSD1306_VERTEX *par_vertex, uint16_t par_size)
{
  uint16_t i;
  if(par_vertex != 0)
  {
    for(i = 1; i < par_size; i++)
    {
      ssd1306_DrawLine_H(h, par_vertex[i - 1].x, par_vertex[i - 1].y, par_vertex[i].x, par_vertex[i].y);
    }
  }
  else
  {
    /*nothing to do*/
  }
  return;
}

static float ssd1306_DegToRad(SSD1306_Handle *h, float par_deg)
{
  return par_deg * 3.14f / 180.0f;
}

static uint16_t ssd1306_NormalizeTo0_360(SSD1306_Handle *h, uint16_t par_deg)
{
  uint16_t loc_angle;
  if(par_deg <= 360)
  {
    loc_angle = par_deg;
  }
  else
  {
    loc_angle = par_deg % 360;
    loc_angle = ((par_deg != 0) ? par_deg : 360);
  }
  return loc_angle;
}

void ssd1306_DrawArc_H(SSD1306_Handle *h, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep)
{
  #define CIRCLE_APPROXIMATION_SEGMENTS 36
  float approx_degree;
  uint32_t approx_segments;
  uint8_t xp1, xp2;
  uint8_t yp1, yp2;
  uint32_t count = 0;
  uint32_t loc_sweep = 0;
  float rad;

  loc_sweep = ssd1306_NormalizeTo0_360(h, sweep);

  count = (ssd1306_NormalizeTo0_360(h, start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
  approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
  approx_degree = loc_sweep / (float)approx_segments;
  while(count < approx_segments)
  {
    rad = ssd1306_DegToRad(h, count * approx_degree);
    xp1 = x + (int8_t)(sin(rad) * radius);
    yp1 = y + (int8_t)(cos(rad) * radius);
    count++;
    if(count != approx_segments)
    {
      rad = ssd1306_DegToRad(h, count * approx_degree);
    }
    else
    {
      rad = ssd1306_DegToRad(h, loc_sweep);
    }
    xp2 = x + (int8_t)(sin(rad) * radius);
    yp2 = y + (int8_t)(cos(rad) * radius);
    ssd1306_DrawLine_H(h, xp1, yp1, xp2, yp2);
  }

  return;
}

void ssd1306_DrawCircle_H(SSD1306_Handle *h, int16_t x0, int16_t y0, int16_t radius)
{
  int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  do
  {
    if (dp < 0)
      dp = dp + 2 * (++x) + 3;
    else
      dp = dp + 2 * (++x) - 2 * (--y) + 5;

    ssd1306_DrawPixel_H(h, x0 + x, y0 + y);
    ssd1306_DrawPixel_H(h, x0 - x, y0 + y);
    ssd1306_DrawPixel_H(h, x0 + x, y0 - y);
    ssd1306_DrawPixel_H(h, x0 - x, y0 - y);
    ssd1306_DrawPixel_H(h, x0 + y, y0 + x);
    ssd1306_DrawPixel_H(h, x0 - y, y0 + x);
    ssd1306_DrawPixel_H(h, x0 + y, y0 - x);
    ssd1306_DrawPixel_H(h, x0 - y, y0 - x);

  } while (x < y);

  ssd1306_DrawPixel_H(h, x0 + radius, y0);
  ssd1306_DrawPixel_H(h, x0, y0 + radius);
  ssd1306_DrawPixel_H(h, x0 - radius, y0);
  ssd1306_DrawPixel_H(h, x0, y0 - radius);
}

void ssd1306_FillCircle_H(SSD1306_Handle *h, int16_t x0, int16_t y0, int16_t radius)
{
  int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  do
  {
    if (dp < 0)
    {
      dp = dp + 2 * (++x) + 3;
    }
    else
    {
      dp = dp + 2 * (++x) - 2 * (--y) + 5;
    }

    ssd1306_DrawHorizontalLine_H(h, x0 - x, y0 - y, 2*x);
    ssd1306_DrawHorizontalLine_H(h, x0 - x, y0 + y, 2*x);
    ssd1306_DrawHorizontalLine_H(h, x0 - y, y0 - x, 2*y);
    ssd1306_DrawHorizontalLine_H(h, x0 - y, y0 + x, 2*y);

  } while (x < y);
  ssd1306_DrawHorizontalLine_H(h, x0 - radius, y0, 2 * radius);
}

void ssd1306_DrawCircleQuads_H(SSD1306_Handle *h, int16_t x0, int16_t y0, int16_t radius, uint8_t quads)
{
  int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  while (x < y)
  {
    if (dp < 0)
      dp = dp + 2 * (++x) + 3;
    else
      dp = dp + 2 * (++x) - 2 * (--y) + 5;
    if (quads & 0x1)
    {
      ssd1306_DrawPixel_H(h, x0 + x, y0 - y);
      ssd1306_DrawPixel_H(h, x0 + y, y0 - x);
    }
    if (quads & 0x2)
    {
      ssd1306_DrawPixel_H(h, x0 - y, y0 - x);
      ssd1306_DrawPixel_H(h, x0 - x, y0 - y);
    }
    if (quads & 0x4)
    {
      ssd1306_DrawPixel_H(h, x0 - y, y0 + x);
      ssd1306_DrawPixel_H(h, x0 - x, y0 + y);
    }
    if (quads & 0x8)
    {
      ssd1306_DrawPixel_H(h, x0 + x, y0 + y);
      ssd1306_DrawPixel_H(h, x0 + y, y0 + x);
    }
  }
  if (quads & 0x1 && quads & 0x8)
  {
    ssd1306_DrawPixel_H(h, x0 + radius, y0);
  }
  if (quads & 0x4 && quads & 0x8)
  {
    ssd1306_DrawPixel_H(h, x0, y0 + radius);
  }
  if (quads & 0x2 && quads & 0x4)
  {
    ssd1306_DrawPixel_H(h, x0 - radius, y0);
  }
  if (quads & 0x1 && quads & 0x2)
  {
    ssd1306_DrawPixel_H(h, x0, y0 - radius);
  }
}

void ssd1306_DrawProgressBar_H(SSD1306_Handle *h, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress)
{
  uint16_t radius = height / 2;
  uint16_t xRadius = x + radius;
  uint16_t yRadius = y + radius;
  uint16_t doubleRadius = 2 * radius;
  uint16_t innerRadius = radius - 2;

  ssd1306_SetColor_H(h, White);
  ssd1306_DrawCircleQuads_H(h, xRadius, yRadius, radius, 0x06 /*0b00000110*/);
  ssd1306_DrawHorizontalLine_H(h, xRadius, y, width - doubleRadius + 1);
  ssd1306_DrawHorizontalLine_H(h, xRadius, y + height, width - doubleRadius + 1);
  ssd1306_DrawCircleQuads_H(h, x + width - radius, yRadius, radius, 0x09 /*0b00001001*/);

  uint16_t maxProgressWidth = (width - doubleRadius + 1) * progress / 100;

  ssd1306_FillCircle_H(h, xRadius, yRadius, innerRadius);
  ssd1306_FillRect_H(h, xRadius + 1, y + 2, maxProgressWidth, height - 3);
  ssd1306_FillCircle_H(h, xRadius + maxProgressWidth, yRadius, innerRadius);
}

void ssd1306_DrawLinearProgress_H(SSD1306_Handle *h, uint8_t x,
                                uint8_t y,
                                uint8_t width,
                                uint8_t height,
                                uint32_t elapsed,
                                uint32_t total)
{
  uint8_t filled_width = 0u;

  if (width == 0u || height == 0u)
  {
    return;
  }

  ssd1306_SetColor_H(h, Black);
  ssd1306_FillRect_H(h, x, y, width, height);

  if (total == 0u)
  {
    ssd1306_SetColor_H(h, White);
    return;
  }

  if (elapsed > total)
  {
    elapsed = total;
  }

  filled_width = (uint8_t)((elapsed * width) / total);
  ssd1306_SetColor_H(h, White);
  if (filled_width > 0u)
  {
    ssd1306_FillRect_H(h, x, y, filled_width, height);
  }
}

void ssd1306_DrawBitmap_H(SSD1306_Handle *h, uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP)
{
    uint8_t bytesPerRow = (W + 7) / 8;

    for (uint8_t y = 0; y < H; y++)
    {
        for (uint8_t x = 0; x < W; x++)
        {
            uint8_t byteIndex = y * bytesPerRow + (x / 8);
            uint8_t bitIndex = x % 8;  // Cambio aquí: LSB primero en lugar de MSB

            if (pBMP[byteIndex] & (1 << bitIndex))
            {
                ssd1306_DrawPixel_H(h, X + x, Y + y);
            }
        }
    }
}

void ssd1306_DrawBitmap_MSB_H(SSD1306_Handle *h, uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP)
{
	uint8_t bytesPerRow = (W + 7) / 8;

	for (uint8_t y = 0; y < H; y++)
	{
		for (uint8_t x = 0; x < W; x++)
		{
			// CRÍTICO: Usar uint16_t para el índice (puede ser hasta 1023)
			uint16_t byteIndex = (uint16_t)y * bytesPerRow + (x / 8);
			uint8_t bitIndex = 7 - (x % 8);  // MSB primero

			if (pBMP[byteIndex] & (1 << bitIndex))
			{
				ssd1306_DrawPixel_H(h, X + x, Y + y);
			}
		}
	}
}

void ssd1306_DrawQR_Fixed_H(SSD1306_Handle *h, uint8_t startX, uint8_t startY, const uint8_t* qrData)
{
    const uint8_t QR_SIZE = 64;
    const uint8_t BYTES_PER_ROW = 8;  // 64 pixels / 8 = 8 bytes

    for (uint8_t row = 0; row < QR_SIZE; row++)
    {
        for (uint8_t col = 0; col < QR_SIZE; col++)
        {
            // Calcular índice del byte
            uint16_t byteIndex = row * BYTES_PER_ROW + (col / 8);

            // Calcular posición del bit (MSB first: bit 7 = pixel 0)
            uint8_t bitIndex = 7 - (col % 8);

            // Leer el bit
            if (qrData[byteIndex] & (1 << bitIndex))
            {
                ssd1306_DrawPixel_H(h, startX + col, startY + row);
            }
        }
    }
}
char ssd1306_WriteChar_H(SSD1306_Handle *h, char ch, FontDef Font)
{
  uint32_t i, b, j;
  uint8_t glyph;
  uint8_t compact_rows;

  if (width(h) <= (h->SSD1306.CurrentX + Font.FontWidth) ||
      height(h) <= (h->SSD1306.CurrentY + Font.FontHeight))
  {
    return 0;
  }

  if ((uint8_t)ch < 32u || (uint8_t)ch > 126u) {
    glyph = (uint8_t)('?' - 32);
  } else {
    glyph = (uint8_t)((uint8_t)ch - 32u);
  }

  compact_rows = (Font.FontWidth <= 8u) ? 1u : 0u;

  for (i = 0; i < Font.FontHeight; i++)
  {
    const uint32_t row_index = ((uint32_t)glyph * Font.FontHeight) + i;
    if (compact_rows) {
      b = ((uint32_t)((const uint8_t *)Font.data)[row_index]) << 8;
    } else {
      b = ((const uint16_t *)Font.data)[row_index];
    }

    for (j = 0; j < Font.FontWidth; j++)
    {
      if ((b << j) & 0x8000u)
      {
        ssd1306_DrawPixel_H(h, h->SSD1306.CurrentX + j, h->SSD1306.CurrentY + i);
      }
      else
      {
        h->SSD1306.Color = !h->SSD1306.Color;
        ssd1306_DrawPixel_H(h, h->SSD1306.CurrentX + j, h->SSD1306.CurrentY + i);
        h->SSD1306.Color = !h->SSD1306.Color;
      }
    }
  }

  h->SSD1306.CurrentX += Font.FontWidth;
  return ch;
}

char ssd1306_WriteString_H(SSD1306_Handle *h, char* str, FontDef Font)
{
  while (*str)
  {
    if (ssd1306_WriteChar_H(h, *str, Font) != *str)
    {
      return *str;
    }
    str++;
  }
  return *str;
}

void ssd1306_SetCursor_H(SSD1306_Handle *h, uint8_t x, uint8_t y)
{
  h->SSD1306.CurrentX = x;
  h->SSD1306.CurrentY = y;
}

void ssd1306_Clear_H(SSD1306_Handle *h)
{
  memset(h->SSD1306_Buffer, 0, SSD1306_BUFFER_SIZE);
}

AM_Result ssd1306_PresentFramebuffer_H(SSD1306_Handle *h, const uint8_t *framebuffer,
                                             uint16_t len)
{
  if (framebuffer == NULL || len != SSD1306_BUFFER_SIZE ||
      !h->SSD1306.Initialized) {
    return AM_ERROR;
  }
  if (!ssd1306_UpdateScreenCompleted_H(h)) {
    return AM_BUSY;
  }

  memcpy(h->SSD1306_Buffer, framebuffer, SSD1306_BUFFER_SIZE);
  ssd1306_UpdateScreen_H(h);

#if SSD1306_USE_DMA == 1 && SSD1306_CONTUPDATE == 0
  if (ssd1306_UpdateScreenCompleted_H(h)) {
    return AM_ERROR;
  }
#endif

  return AM_OK;
}

uint32_t ssd1306_GetErrorCount_H(SSD1306_Handle *h)
{
  return h->oled_error_count;
}

/* -------------------------------------------------------------------------- */
/* TRANSFER IMPLEMENTATIONS                                                    */
/* -------------------------------------------------------------------------- */

#if SSD1306_USE_DMA == 0

void ssd1306_WriteCommand_H(SSD1306_Handle *h, uint8_t command)
{
  h->bus->hardware->mem_write(h->bus->hardware_ctx, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);
}

void ssd1306_WriteData(SSD1306_Handle *h, uint8_t* data, uint16_t size)
{
  h->bus->hardware->mem_write(h->bus->hardware_ctx, SSD1306_I2C_ADDR, 0x40, 1, data, size, 100);
}

void ssd1306_UpdateScreen_H(SSD1306_Handle *h)
{
  uint8_t i;
  for (i = 0; i < SSD1306_HEIGHT / 8; i++)
  {
    ssd1306_WriteCommand_H(h, 0xB0 + i);
    ssd1306_WriteCommand_H(h, SETLOWCOLUMN);
    ssd1306_WriteCommand_H(h, SETHIGHCOLUMN);
    ssd1306_WriteData(h, &h->SSD1306_Buffer[SSD1306_WIDTH * i], width(h));
  }
}

#elif SSD1306_USE_DMA == 1



#if SSD1306_CONTUPDATE == 0

void ssd1306_WriteCommand_H(SSD1306_Handle *h, uint8_t command)
{
#if SSD1306_USE_I2C_MANAGER == 1
  // For commands outside init, we just request an update.
  // (You can extend this later with a command queue if needed.)
  (void)command;
  ssd1306_UpdateScreen_H(h);
#else
  while(h->ssd1306_updatestatus);
  while(!h->bus->hardware->ready(h->bus->hardware_ctx)) { };
  h->bus->hardware->mem_write_dma(h->bus->hardware_ctx, SSD1306_I2C_ADDR, 0x00, 1, &command, 1);
#endif
}

void ssd1306_UpdateScreen_H(SSD1306_Handle *h)
{
#if SSD1306_USE_I2C_MANAGER == 1
   // BREAKPOINT: solicitud de refresco OLED
  uint8_t max_pages = (uint8_t)(SSD1306_HEIGHT / 8);
  for (uint8_t page = 0; page < max_pages; page++) {
    uint8_t keep_bus = OLED_ShouldKeepBus(h, page, max_pages);
    
    (void)ssd1306_RequestPageUpdate_H(h, page, I2C_REQ_PRIO_REGULAR, keep_bus);
  }
#else
  uint8_t  command;
  if(h->ssd1306_updatestatus == 0)
  {
    h->ssd1306_updatestatus = SSD1306_HEIGHT;
    h->ssd1306_updateend = SSD1306_HEIGHT + (SSD1306_HEIGHT / 2);
    command = 0xB0;
    h->bus->hardware->mem_write_dma(h->bus->hardware_ctx, SSD1306_I2C_ADDR, 0x00, 1, &command, 1);
  }
  else if(h->ssd1306_updatestatus >= SSD1306_HEIGHT)
  {
    h->ssd1306_updatestatus -= (SSD1306_HEIGHT / 2);
    h->ssd1306_updateend = (h->ssd1306_updatestatus + (SSD1306_HEIGHT / 2 + 1)) & 0xFC;
  }
#endif
}

char ssd1306_UpdateScreenCompleted_H(SSD1306_Handle *h)
{
#if SSD1306_USE_I2C_MANAGER == 1
  return (!OLED_QueueHasPending(h) && h->oled_inflight_page == 0xFFu) ? 1 : 0;
#else
  if(h->ssd1306_updatestatus)
    return 0;
  else
    return 1;
#endif
}





#elif SSD1306_CONTUPDATE == 1

/* When using I2C Manager, CONTUPDATE must be 0, so this block is not compiled. */





void ssd1306_WriteCommand_H(SSD1306_Handle *h, uint8_t command)
{
  if(h->ssd1306_updatestatus)
  {
    while(h->ssd1306_command);
    h->ssd1306_command = command;
  }
  else
  {
    while(!h->bus->hardware->ready(h->bus->hardware_ctx)) { };
    h->i2c_command = command;
    h->bus->hardware->mem_write_dma(h->bus->hardware_ctx, SSD1306_I2C_ADDR, 0x00, 1, &h->i2c_command, 1);
  }
}

void ssd1306_ContUpdateEnable_H(SSD1306_Handle *h)
{
  if(!h->ssd1306_ContUpdate)
  {
    while(!h->bus->hardware->ready(h->bus->hardware_ctx)) { };
    h->ssd1306_updatestatus = SSD1306_HEIGHT;
    h->ssd1306_updateend = SSD1306_HEIGHT + (SSD1306_HEIGHT / 2);
    h->i2c_command = 0xB0;
    h->ssd1306_ContUpdate = 1;
    h->bus->hardware->mem_write_dma(h->bus->hardware_ctx, SSD1306_I2C_ADDR, 0x00, 1, &h->i2c_command, 1);
  }
}

void ssd1306_ContUpdateDisable_H(SSD1306_Handle *h)
{
  if(h->ssd1306_ContUpdate)
  {
    h->ssd1306_ContUpdate = 0;
    while(h->ssd1306_updatestatus) { };
  }
}

void ssd1306_SetRasterInt_H(SSD1306_Handle *h, uint8_t r)
{
  h->ssd1306_RasterIntRegs = r;
}

void ssd1306_RasterIntCallback(SSD1306_Handle *h, uint8_t r)
{
    if (h->on_raster) h->on_raster(h->callback_ctx, r);
}

#endif /* CONTUPDATE */

#endif /* USE_DMA */
