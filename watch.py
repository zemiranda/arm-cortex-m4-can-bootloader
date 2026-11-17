import os
import socket
import struct
import subprocess
import sys
import time

ELF_PATH = (
    sys.argv[1] if len(sys.argv) > 1 else "build/Debug/arm-cortex-m4-can-bootloader.elf"
)

# Map enum integer values to human-readable state names
BOOT_STATES = {
    0: "BOOT_IDLE",
    1: "BOOT_HANDSHAKE",
    2: "BOOT_FLASHING",
    3: "BOOT_VERIFY",
    4: "BOOT_JUMP",
    5: "BOOT_ERROR",
}

# Set type to "enum" (auto-detects 1, 2, or 4 bytes from ELF) and attach optional name map
WATCH_VARS = {
    "bl_state": ("enum", BOOT_STATES),
    # "other_var": ("u32", None),
}

SIZE_TO_FMT = {
    1: (8, "<B"),
    2: (16, "<H"),
    4: (32, "<I"),
}

TYPE_MAP = {
    "u8": (8, "<B"),
    "i8": (8, "<b"),
    "u16": (16, "<H"),
    "i16": (16, "<h"),
    "u32": (32, "<I"),
    "i32": (32, "<i"),
    "f32": (32, "<f"),
}


def resolve_symbols(elf_path, names):
    # -S prints: <addr> <size> <type> <name>
    out = subprocess.check_output(["arm-none-eabi-nm", "-S", elf_path], text=True)
    symbols = {}
    for line in out.splitlines():
        parts = line.split()
        if len(parts) != 4:
            continue
        addr_str, size_str, sym_type, sym_name = parts
        if sym_type not in ("b", "B", "d", "D"):
            continue
        for name in names:
            if sym_name == name or sym_name.startswith(f"{name}."):
                symbols[name] = (int(addr_str, 16), int(size_str, 16))
    return symbols


def tcl_cmd(sock, cmd):
    sock.sendall(f"{cmd}\x1a".encode())
    buf = bytearray()
    while not buf.endswith(b"\x1a"):
        chunk = sock.recv(1024)
        if not chunk:
            raise ConnectionError("OpenOCD closed TCL connection")
        buf.extend(chunk)
    return buf[:-1].decode().strip()


def main():
    last_mtime = 0.0
    symbols = {}
    sock = socket.create_connection(("localhost", 6666), timeout=2.0)

    try:
        while True:
            try:
                mtime = os.path.getmtime(ELF_PATH)
                if mtime != last_mtime:
                    time.sleep(0.05)
                    symbols = resolve_symbols(ELF_PATH, WATCH_VARS.keys())
                    last_mtime = mtime
            except FileNotFoundError:
                print("\033[H\033[JWaiting for .elf build...", flush=True)
                time.sleep(0.2)
                continue

            lines = [
                f"ELF: {ELF_PATH} (updated {time.strftime('%H:%M:%S', time.localtime(last_mtime))})\n"
            ]
            for name, (ctype, enum_map) in WATCH_VARS.items():
                if name not in symbols:
                    lines.append(f"{name:<20} [NOT FOUND - add volatile]")
                    continue

                addr, sym_size = symbols[name]
                if ctype == "enum":
                    bits, fmt = SIZE_TO_FMT.get(sym_size, (32, "<I"))
                else:
                    bits, fmt = TYPE_MAP[ctype]

                raw_str = tcl_cmd(sock, f"read_memory 0x{addr:08x} {bits} 1")
                raw_int = int(raw_str, 0)
                val = struct.unpack(fmt, raw_int.to_bytes(bits // 8, "little"))[0]

                if enum_map is not None:
                    label = enum_map.get(val, "UNKNOWN")
                    lines.append(
                        f"{name:<20} [0x{addr:08x} | {sym_size}B] = {val} ({label})"
                    )
                elif ctype == "f32":
                    lines.append(f"{name:<20} [0x{addr:08x}] = {val:.4f}")
                else:
                    lines.append(f"{name:<20} [0x{addr:08x}] = {val}")

            print("\033[H\033[J" + "\n".join(lines), flush=True)
            time.sleep(0.02)
    except KeyboardInterrupt:
        pass
    finally:
        sock.close()


if __name__ == "__main__":
    main()
