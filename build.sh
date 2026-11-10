#!/usr/bin/env bash
set -e

# --- CONFIGURATION ---
PROJECT_NAME="arm-cortex-m4-can-bootloader"
BUILD_DIR="build"
LINKER_SCRIPT="STM32F303xx_FLASH.ld"

OPENOCD_INTERFACE="stlink" # or jlink, etc.
OPENOCD_TARGET="stm32f3x"
OPENOCD_CFG="/usr/share/openocd/scripts/interface/${OPENOCD_INTERFACE}.cfg"
OPENOCD_TARGET_CFG="/usr/share/openocd/scripts/target/${OPENOCD_TARGET}.cfg"

ST_FLASH_BIN="${BUILD_DIR}/${PROJECT_NAME}.bin"

# --- HELP ---
if [ "$1" = "help" ]; then
  echo "The following arguments can be appended to ./build.sh:"
  echo "-----------------------------------------------------"
  echo "*clean - cleans the build directory of generated files by ninja"
  echo "*flash_openocd - flashes the built code to the STM32 using OpenOCD"
  echo "*flash_st - flashes the built code to the STM32 using ST-LINK"
  echo "*help - displays this menu"
  exit 0
fi

# --- CLEAN OPTION ---
if [ "$1" = "clean" ]; then
  rm -rf "${BUILD_DIR}"
  echo "Cleaned build directory."
  exit 0
fi

# --- BUILD OPTION ---
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake if build.ninja doesn't exist
if [ ! -f build.ninja ]; then
  cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
fi

# Build
ninja
cd ..

# --- FLASH OPTION: openocd ---
if [ "$1" = "flash_openocd" ]; then
  echo "Flashing with OpenOCD..."
  openocd -f "${OPENOCD_CFG}" -f "${OPENOCD_TARGET_CFG}" \
    -c "program ${BUILD_DIR}/${PROJECT_NAME}.elf verify reset exit"
  exit 0
fi

# --- FLASH OPTION: st_flash ---
if [ "$1" = "flash_st" ]; then
  echo "Flashing with ST-LINK..."
  st-flash write "${ST_FLASH_BIN}" 0x8000000
  exit 0
fi

echo "Build complete. Use './build.sh flash_openocd' or './build.sh flash_st' to flash."
