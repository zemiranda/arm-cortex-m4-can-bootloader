#ifndef INC_CAN_PROTOCOL_H_
#define INC_CAN_PROTOCOL_H_

#include "bootloader.h"

#define FRAME_DATA_SIZE 8U
#define MAGIC_FRAME_EXPECTED_DATA                                              \
  {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}

/* Function definitions */

bl_status_t
can_protocol_poll_frame(CAN_HandleTypeDef *hcan, CAN_RxHeaderTypeDef *rxHeader,
                        uint8_t rxData[], const uint8_t expected_data[],
                        uint8_t data_size, uint32_t timeout, uint32_t rxFifo);

#endif
