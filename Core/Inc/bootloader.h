#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#define

typedef enum {
  BL_WAIT = 0,
  BL_ERASE,
  BL_READY,
  BL_FLASH,
  BL_JUMP,
  BL_CRC,
  BL_APP_INVALID
} bl_state_t

#endif
