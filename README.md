# AntoOS

**Operating System Project — ENSIMAG**

AntoOS is a minimalist educational operating system built as part of my ENSIMAG curriculum (2nd Year).  
Developed primarily in **C** with low-level **Assembly** components, it demonstrates core OS mechanisms — from bootloading to memory management, interrupts, and process scheduling — through a clean, modular architecture.

---

## 🧩 Overview

AntoOS was designed as a hands-on learning project to explore how an operating system works under the hood.  
It includes a bootloader, a simple kernel written in C, and a custom build pipeline using Make and a linker script to generate a bootable image.  

The system focuses on clarity and correctness rather than feature completeness — it’s a sandbox to experiment with kernel design and low-level programming.

---

## 🎯 Goals

- Implement a **minimal boot sequence**  
- Build a **kernel loop** handling basic scheduling  
- Implement **memory management** (paging/segmentation)  
- Handle **interrupts** and basic device I/O  
- Optionally expose **syscalls or drivers** for user-space interaction  

---

## 🧰 Prerequisites

You’ll need a basic x86 development environment with:

- `gcc` (or a cross-compiler for your target)
- `make`
- `binutils` (`ld`, `objcopy`, etc.)
- `nasm` or equivalent assembler
- `qemu` for emulation and testing

### Quick setup (Debian/Ubuntu)
```bash
sudo apt update
sudo apt install build-essential nasm qemu-system-x86
```

---

## 🧪 Building

From the project root:

```bash
make        # build all components
```

If you’re using a cross-compiler (e.g., `i686-elf-gcc`):

```bash
make CROSS_COMPILE=i686-elf-
```

The resulting kernel image or bootable binary will be placed in your `build/` or `obj/` directory.

---

## 🚀 Running & Testing

If your Makefile provides a run target:
```bash
make run
```

Or launch directly with QEMU:
```bash
qemu-system-x86_64 -kernel build/antonos.bin
# or, to boot from a disk image:
qemu-system-x86_64 -drive format=raw,file=build/antonos.img
```

---

## 🐛 Debugging

Use **GDB + QEMU** to inspect kernel behavior step-by-step.

```bash
# 1. Start QEMU and wait for GDB connection
qemu-system-x86_64 -kernel build/antonos.bin -S -gdb tcp::1234

# 2. Connect from GDB
gdb build/antonos.elf
(gdb) target remote :1234
```

You can now set breakpoints, inspect registers, and trace the kernel execution.

---

## 🤝 Contributing

Contributions and discussions are welcome!  
If you’re experimenting with new features, use topic branches and clear commit messages:

```bash
git checkout -b feat/<short-description>
```

Guidelines:
- Keep commits focused and readable  
- Document architecture decisions in `docs/`  
- Add small examples or tests when adding new functionality  

---

## 👨‍💻 Author / 🙏 Acknowledgements

**Antonio Mattar**  
ENSIMAG — ISI 2A 2024
📧 antoniomattar123@icloud.com

This project is part of the ENSIMAG ISI curriculum.  

---
