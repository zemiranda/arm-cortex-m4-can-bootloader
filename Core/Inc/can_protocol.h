#ifndef INC_CAN_PROTOCOL_H_
#define INC_CAN_PROTOCOL_H_

#include "bootloader.h"

#define BL_USE_EXTENDED_ID 1

// Temporary Ids
#define BL_PRIORITY_ID 0x1
#define BL_EXT_DATA_PAGE_ID 0x0
#define BL_DATA_PAGE_ID 0x0

/* Node CAN IDs */

// Flasher CAN node id
#define BL_FLASHER_NODE_ID 0x2
// Current system CAN node id, since this will be dynamically changed by
// hardware 4 bit GPIO combination this define is temporary
#define BL_NODE_ID 0x1

/* Packet CAN IDs */

#define BL_WAIT_MAGIC_PACKET_ID 0x1
#define BL_ERASE_SUCCESS_PACKET_ID 0x2

/* Other definitions */

#define FRAME_DATA_SIZE 8U

// Possible can protocol that will be followed (29-bit J1939-style
// Extended ID layout) [Priority (3 bits)] | [Reserved (1 bit)] |
// [Message/Packet Type (8 bits)] | [Target/Dest ID (8 bits)] | [Source
// Node ID (8 bits)]

/* Function definitions */

bl_status_t can_protocol_poll_frame(CAN_HandleTypeDef *hcan,
                                    CAN_RxHeaderTypeDef *rxHeader,
                                    uint8_t rxData[], uint8_t data_size,
                                    uint32_t timeout, uint32_t rxFifo,
                                    uint32_t can_id);

uint32_t can_protocol_format_id(uint8_t priority, uint8_t ext_data_page,
                                uint8_t data_page, uint8_t packet_type,
                                uint8_t target_node_id, uint8_t source_node_id);

bl_status_t can_protocol_send_frame(CAN_HandleTypeDef *hcan, uint32_t can_id,
                                    uint8_t txData[]);

#endif
