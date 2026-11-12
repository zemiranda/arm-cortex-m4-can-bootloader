#include "can_protocol.h"
#include "bootloader.h"
#include "stm32f3xx_hal.h"

#include <stdint.h>
#include <string.h>

// Verify timeout cast?
bl_status_t
can_protocol_poll_frame(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *rxHeader,
                        uint8_t rxData[], const uint8_t expected_data[],
                        uint8_t data_size, uint32_t timeout, uint32_t rxFifo) {

  uint32_t tick_start = HAL_GetTick();

  while ((HAL_GetTick() - tick_start) > timeout) {
    // Check if the Fifo has messages
    if (HAL_CAN_GetRxFifoFillLevel(hcan, rxFifo) > 0) {
      // Get message
      HAL_StatusTypeDef hal_status =
          HAL_CAN_GetRxMessage(hcan, rxFifo, rxHeader, rxData);

      if (hal_status == HAL_OK) {
        if (memcmp(rxData, expected_data, data_size) == 0) {
          return BL_OK;
        } else {
          // Wrong frame data
          continue;
        }
      } else {
        return BL_ERROR;
      }
    }
  }
  return BL_TIMEOUT;
}
