#include "bootloader.h"
#include "can_protocol.h"
#include "stm32f3xx_hal.h"
#include <string.h>

bl_status_t bootloader_wait_for_magic_frame(CAN_HandleTypeDef *hcan,
                                            uint8_t rxData[],
                                            size_t rxData_len) {

  CAN_RxHeaderTypeDef rxHeader;

  if (rxData == NULL || hcan == NULL || rxData_len == 0) {
    return BL_ERROR;
  }

  // Reset CAN rxData array to 0
  memset(rxData, 0, rxData_len);
  uint32_t can_id = can_protocol_format_id(
      BL_PRIORITY_ID, BL_EXT_DATA_PAGE_ID, BL_DATA_PAGE_ID,
      BL_WAIT_MAGIC_PACKET_ID, BL_NODE_ID, BL_FLASHER_NODE_ID);

  return can_protocol_poll_frame(hcan, &rxHeader, rxData, FRAME_DATA_SIZE,
                                 WAIT_MAGIC_TIMEOUT, CAN_RX_FIFO0, can_id);
}

bl_status_t bootloader_erase_flash(uint32_t application_size) {
  HAL_StatusTypeDef hal_status = HAL_OK;

  // Validate the application address is inside flash boundaries
  if (APP_ADDRESS < FLASH_BASE_ADDRESS ||
      APP_ADDRESS >= (FLASH_BASE_ADDRESS + FLASH_SIZE)) {
    // TODO: Send a CAN frame with the bad address definition
    return BL_ERROR;
  }

  // Compare the flash size to be sent with the available flash memory
  // boundaries
  if ((application_size == 0U) ||
      (application_size > ((FLASH_BASE_ADDRESS + FLASH_SIZE) - APP_ADDRESS))) {
    // TODO: Send a CAN frame with invalid application size
    return BL_ERROR;
  }

  // Wait for a current operation on flash to end
  if (FLASH_WaitForLastOperation((uint32_t)ERASE_FLASH_TIMEOUT) == HAL_OK) {

    // Disable interrupts
    __disable_irq();

    //__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY)

    hal_status = HAL_FLASH_Unlock();

    if (hal_status != HAL_OK) {
      return BL_ERROR;
    }

    // Clear Flash flags (EOP, PGERR, WRPERR) before erasing (Reference manual
    // RM0316 4.4 page 79)
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR |
                           FLASH_FLAG_WRPERR);

    FLASH_EraseInitTypeDef pEraseInit;
    uint32_t PageError = 0;

    pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    pEraseInit.PageAddress = APP_ADDRESS;
    pEraseInit.NbPages = PAGES_TO_ERASE_COUNT;

    hal_status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

    HAL_FLASH_Lock();

    if (hal_status != HAL_OK) {
      // TODO: Send a can frame with the erase error and faulty address
      return BL_ERROR;
    }

    // Flash erased successfully
    return BL_OK;
  } else {
    // Last operation on falsh didnt finish on time
    return BL_TIMEOUT;
  }
}
