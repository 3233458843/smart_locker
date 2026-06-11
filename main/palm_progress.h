#ifndef PALM_PROGRESS_H
#define PALM_PROGRESS_H

#include "freertos/FreeRTOS.h"
#include <stdint.h>
#include <stdbool.h>

extern uint8_t g_xst_palm_progress;
extern bool g_palm_progress_updated;

void palm_progress_update(TickType_t now);
void palm_progress_reset(void);
void palm_progress_on_xst_progress(uint8_t progress);

#endif
