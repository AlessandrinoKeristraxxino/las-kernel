#!/bin/bash
# Build script for LAS Kernel
# This script checks prerequisites, builds the kernel and creates a bootable ISO.

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ISO_ROOT_DIR="${ROOT_DIR}/iso_root"
OUT_DIR="${ROOT_DIR}/out"
KERNEL_BINARY="${ROOT_DIR}/target/x86_64-unknown-none/release/las-kernel"
ISO_FILE="${OUT_DIR}/las-kernel.iso"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

ensure_sudo() {
    if command -v sudo >/dev/null 2>&1; then
        echo "sudo"
    else
        echo ""
    fi
}

install_package() {
    local package_name="$1"
    local sudo_cmd
    sudo_cmd="$(ensure_sudo)"

    if command -v apt-get >/dev/null 2>&1; then
        if [[ -n "${sudo_cmd}" ]]; then
            sudo apt-get update && sudo apt-get install -y "${package_name}"
        else
            apt-get update && apt-get install -y "${package_name}"
        fi
    elif command -v brew >/dev/null 2>&1; then
        brew install "${package_name}"
    elif command -v pacman >/dev/null 2>&1; then
        if [[ -n "${sudo_cmd}" ]]; then
            sudo pacman -S --noconfirm "${package_name}"
        else
            pacman -S --noconfirm "${package_name}"
        fi
    elif command -v yum >/dev/null 2>&1; then
        if [[ -n "${sudo_cmd}" ]]; then
            sudo yum install -y "${package_name}"
        else
            yum install -y "${package_name}"
        fi
    else
        echo -e "${RED}✗ Could not install ${package_name} automatically.${NC}"
        echo "   Please install it manually and rerun the script."
        exit 1
    fi
}

ensure_tool() {
    local tool_name="$1"
    local package_name="$2"

    if ! command -v "${tool_name}" >/dev/null 2>&1; then
        echo -e "${YELLOW}✗ ${tool_name} not found. Attempting to install...${NC}"
        install_package "${package_name}"
    fi

    echo -e "${GREEN}✓ ${tool_name} found${NC}"
}

configure_iso_root() {
    mkdir -p "${ISO_ROOT_DIR}/boot" "${ISO_ROOT_DIR}/EFI/BOOT" "${OUT_DIR}"

    if [[ -d "${ISO_ROOT_DIR}/boot/limine" ]]; then
        cp -f "${ISO_ROOT_DIR}/boot/limine/limine-bios-cd.bin" "${ISO_ROOT_DIR}/boot/"
        cp -f "${ISO_ROOT_DIR}/boot/limine/limine-bios.sys" "${ISO_ROOT_DIR}/boot/"
        cp -f "${ISO_ROOT_DIR}/boot/limine/limine-uefi-cd.bin" "${ISO_ROOT_DIR}/boot/"
    fi

    if [[ -f "${KERNEL_BINARY}" ]]; then
        cp -f "${KERNEL_BINARY}" "${ISO_ROOT_DIR}/boot/kernel"
    else
        echo -e "${RED}✗ Kernel binary not found at ${KERNEL_BINARY}${NC}"
        exit 1
    fi

    if [[ -f "${ISO_ROOT_DIR}/boot/limine-uefi-cd.bin" ]]; then
        cp -f "${ISO_ROOT_DIR}/boot/limine-uefi-cd.bin" "${ISO_ROOT_DIR}/EFI/BOOT/BOOTX64.EFI"
    fi

    cat > "${ISO_ROOT_DIR}/limine.cfg" <<'EOF'
TIMEOUT=3
DEFAULT_ENTRY=LasOS

:LasOS
PROTOCOL=limine
KERNEL_PATH=boot():/boot/kernel
KERNEL_CMDLINE=quiet
EOF
}

build_iso() {
    local xorriso_bin
    xorriso_bin="$(command -v xorriso || true)"

    if [[ -z "${xorriso_bin}" ]]; then
        echo -e "${RED}✗ xorriso not found. Please install it and rerun the script.${NC}"
        exit 1
    fi

    echo ""
    echo "[*] Creating bootable ISO image..."
    xorriso -as mkisofs \
        -b boot/limine-bios-cd.bin \
        -no-emul-boot \
        -boot-load-size 4 \
        -boot-info-table \
        --efi-boot boot/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        -o "${ISO_FILE}" \
        "${ISO_ROOT_DIR}"

    echo -e "${GREEN}✓ ISO image created: ${ISO_FILE}${NC}"
}

echo "=== LAS Kernel Build System ==="
echo ""

echo "[*] Checking prerequisites..."

if ! command -v cargo >/dev/null 2>&1; then
    echo -e "${RED}✗ cargo not found. Install Rust from https://rustup.rs/${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Rust toolchain found${NC}"

if ! command -v nasm >/dev/null 2>&1; then
    echo -e "${YELLOW}✗ nasm not found. Attempting to install...${NC}"
    install_package "nasm"
fi
echo -e "${GREEN}✓ nasm found${NC}"

if ! command -v cc >/dev/null 2>&1 && ! command -v gcc >/dev/null 2>&1; then
    echo -e "${YELLOW}✗ C compiler not found. Attempting to install...${NC}"
    install_package "build-essential"
fi
echo -e "${GREEN}✓ C compiler found${NC}"

ensure_tool "xorriso" "xorriso"

if ! rustup toolchain list | grep -q nightly; then
    echo ""
    echo "[*] Installing Rust nightly toolchain..."
    rustup toolchain install nightly
fi
rustup component add rust-src --toolchain nightly
rustup target add x86_64-unknown-none --toolchain nightly

echo ""
echo "[*] Building LAS Kernel..."
echo "    Target: x86_64-unknown-none (bare metal)"
echo ""

if cargo build --target x86_64-unknown-none --release; then
    echo ""
    echo -e "${GREEN}=== BUILD SUCCESSFUL ===${NC}"
    echo "    Kernel binary: ${KERNEL_BINARY}"
else
    echo ""
    echo -e "${RED}=== BUILD FAILED ===${NC}"
    echo "    Please check the errors above."
    exit 1
fi

configure_iso_root
build_iso

echo ""
echo -e "${GREEN}=== ISO READY ===${NC}"
echo "    ISO file: ${ISO_FILE}"
