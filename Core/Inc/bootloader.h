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
  BL_APP_INVALID,
  BL_FAILED
} bl_state_t;

typedef enum { BL_OK = 0, BL_ERROR, BL_TIMEOUT, BL_BUSY } bl_status_t;

// 500 ms
#define WAIT_MAGIC_TIMEOUT 500U

// Wait for current operation on flash memory to end to erase
#define ERASE_FLASH_TIMEOUT 5000
// 2KB page size
#define FLASH_PAGE_SIZE 0x800

// 8 KB of bootloader flash memory reserved
#define APP_ADDRESS 0x08002000
// Initial address of the flash memory
#define FLASH_BASE_ADDRESS 0x08000000
// 64 KB of flash memory available
#define FLASH_SIZE 0x10000
// Number of pages available in flash memory
#define PAGES_TO_ERASE_COUNT                                                   \
  (((FLASH_BASE_ADDRESS + FLASH_SIZE) - APP_ADDRESS) / FLASH_PAGE_SIZE)

/* Function definitions */

bl_status_t bootloader_wait_for_magic_frame(CAN_HandleTypeDef *hcan,
                                            uint8_t rxData[],
                                            size_t rxData_len);

bl_status_t bootloader_erase_flash(uint32_t application_size);

#endif
