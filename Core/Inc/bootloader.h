#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#include "stm32f3xx_hal.h"

typedef enum {
  BL_WAIT = 0,
  BL_ERASE,
  BL_READY,
  BL_FLASH,
  BL_JUMP,
  BL_CRC,
  BL_APP_INVALID
} bl_state_t;

typedef enum { BL_OK = 0, BL_ERROR, BL_TIMEOUT, BL_BUSY } bl_status_t;

// 500 ms
#define WAIT_MAGIC_TIMEOUT 500U

/* Function definitions */

bl_status_t bootloader_wait_for_magic_frame(CAN_HandleTypeDef *hcan);

#endif
