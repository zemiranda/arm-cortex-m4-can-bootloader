#include "can_protocol.h"
#include "bootloader.h"
#include "stm32f3xx_hal.h"
#include <stdint.h>

#if (BL_USE_EXTENDED_ID)

// Possible can protocol that will be followed (29-bit J1939-style Extended ID
// layout) [Priority (3 bits)] | [Extra Page (1 bit)] | [Page (1 bit)] |
// [Message/Packet Type (8 bits)] | [Target/Dest ID (8 bits)] | [Source Node ID
// (8 bits)]
uint32_t can_protocol_format_id(uint8_t priority, uint8_t ext_data_page,
                                uint8_t data_page, uint8_t packet_type,
                                uint8_t target_node_id,
                                uint8_t source_node_id) {
  return (((uint32_t)priority & 0x07U) << 26U) |
         (((uint32_t)ext_data_page & 0x01U) << 25U) |
         (((uint32_t)data_page & 0x01U) << 24U) |
         (((uint32_t)packet_type) << 16U) | (((uint32_t)target_node_id) << 8U) |
         (((uint32_t)source_node_id) << 0U);
}

// Verify timeout cast?
bl_status_t can_protocol_poll_frame(CAN_HandleTypeDef *hcan,
                                    CAN_RxHeaderTypeDef *rxHeader,
                                    uint8_t rxData[], uint8_t data_size,
                                    uint32_t timeout, uint32_t rxFifo,
                                    uint32_t can_id) {

  uint32_t tick_start = HAL_GetTick();

  while ((HAL_GetTick() - tick_start) > timeout) {
    // Check if the Fifo has messages
    if (HAL_CAN_GetRxFifoFillLevel(hcan, rxFifo) > 0) {
      // Get message
      HAL_StatusTypeDef hal_status =
          HAL_CAN_GetRxMessage(hcan, rxFifo, rxHeader, rxData);

      if (hal_status == HAL_OK) {
        if (rxHeader->ExtId == can_id) {
          return BL_OK;
        }
      } else {
        return BL_ERROR;
      }
    }
  }
  return BL_TIMEOUT;
}

bl_status_t can_protocol_send_frame(CAN_HandleTypeDef *hcan, uint32_t can_id,
                                    uint8_t txData[]) {

  CAN_TxHeaderTypeDef txHeader = {.StdId = can_id,
                                  .IDE = CAN_ID_EXT,
                                  .RTR = CAN_RTR_DATA,
                                  .DLC = 8U,
                                  .TransmitGlobalTime = DISABLE};
  uint32_t txMailbox;
  if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) > 0U) {
    if (HAL_CAN_AddTxMessage(hcan, &txHeader, txData, &txMailbox) != HAL_OK) {
      // Handle Tx error
    }
  }
  return BL_OK;
}

#else
// TODO: Support for Std Id

#endif
