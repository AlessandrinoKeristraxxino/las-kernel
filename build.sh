#!/bin/bash
# Build script for LAS Kernel
# This script checks prerequisites and builds the kernel

set -e

echo "=== LAS Kernel Build System ==="
echo ""

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check for required tools
echo "[*] Checking prerequisites..."

# Check for rustup/cargo
if ! command -v cargo &> /dev/null; then
    echo -e "${RED}✗ cargo not found. Install Rust from https://rustup.rs/${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Rust toolchain found${NC}"

# Check for nasm
if ! command -v nasm &> /dev/null; then
    echo -e "${YELLOW}✗ nasm not found. Attempting to install...${NC}"
    
    if command -v apt-get &> /dev/null; then
        echo "   Installing nasm via apt-get..."
        sudo apt-get update && sudo apt-get install -y nasm
    elif command -v brew &> /dev/null; then
        echo "   Installing nasm via Homebrew..."
        brew install nasm
    elif command -v pacman &> /dev/null; then
        echo "   Installing nasm via pacman..."
        sudo pacman -S nasm
    elif command -v yum &> /dev/null; then
        echo "   Installing nasm via yum..."
        sudo yum install -y nasm
    else
        echo -e "${RED}✗ Could not install nasm automatically.${NC}"
        echo "   Please install nasm manually:"
        echo "   - Ubuntu/Debian: sudo apt-get install nasm"
        echo "   - macOS: brew install nasm"
        echo "   - Arch: sudo pacman -S nasm"
        exit 1
    fi
fi
echo -e "${GREEN}✓ nasm found${NC}"

# Check for gcc/cc (needed for build.rs cc crate)
if ! command -v cc &> /dev/null && ! command -v gcc &> /dev/null; then
    echo -e "${YELLOW}✗ C compiler not found. Attempting to install...${NC}"
    
    if command -v apt-get &> /dev/null; then
        echo "   Installing build-essential via apt-get..."
        sudo apt-get update && sudo apt-get install -y build-essential
    elif command -v brew &> /dev/null; then
        echo "   Installing Xcode Command Line Tools..."
        xcode-select --install
    else
        echo -e "${RED}✗ Could not install C compiler automatically.${NC}"
        echo "   Please install a C compiler manually."
        exit 1
    fi
fi
echo -e "${GREEN}✓ C compiler found${NC}"

# Ensure the Rust nightly toolchain is up to date
echo ""
echo "[*] Updating Rust nightly toolchain..."
rustup update nightly
rustup component add rust-src --toolchain nightly
rustup target add x86_64-unknown-none --toolchain nightly
echo -e "${GREEN}✓ Toolchain updated${NC}"

# Build the kernel
echo ""
echo "[*] Building LAS Kernel..."
echo "    Target: x86_64-unknown-none (bare metal)"
echo ""

if cargo build --target x86_64-unknown-none --release; then
    echo ""
    echo -e "${GREEN}=== BUILD SUCCESSFUL ===${NC}"
    echo "    Kernel binary: target/x86_64-unknown-none/release/las-kernel"
    echo ""
else
    echo ""
    echo -e "${RED}=== BUILD FAILED ===${NC}"
    echo "    Please check the errors above."
    exit 1
fi
