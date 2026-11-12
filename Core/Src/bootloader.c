#include "bootloader.h"
#include "can_protocol.h"

bl_status_t bootloader_wait_for_magic_frame(CAN_HandleTypeDef *hcan) {

  CAN_RxHeaderTypeDef rxHeader;
  uint8_t rxData[8] = {0};

  return can_protocol_poll_frame(
      hcan, &rxHeader, rxData, (const uint8_t[])MAGIC_FRAME_EXPECTED_DATA,
      FRAME_DATA_SIZE, WAIT_MAGIC_TIMEOUT, CAN_RX_FIFO0);
}
