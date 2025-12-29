# Installation Guide

## Prerequisites

### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install jpeg libpng
```

### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential libpng-dev libjpeg-dev zlib1g-dev
```

### Linux (Fedora/RHEL)

```bash
sudo dnf install gcc-c++ libpng-devel libjpeg-devel zlib-devel
```

### Windows (MSYS2/MinGW)

```bash
# Install MSYS2 from https://www.msys2.org/
# Then in MSYS2 terminal:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-libpng mingw-w64-x86_64-libjpeg-turbo
```

## Building

### Using Make (Recommended)

```bash
# Compile
make

# Run with default scale factor (2)
make run

# Run with custom scale factor
make run ARGS=3

# Compile and run in one step
make all

# View all available commands
make help
```

### Manual Compilation

#### macOS

```bash
g++ main.cpp -o bicubic \
  -Dcimg_use_png -Dcimg_display=0 \
  -I/opt/homebrew/include -I/opt/homebrew/opt/jpeg/include \
  -L/opt/homebrew/lib -L/opt/homebrew/opt/jpeg/lib \
  -lpng -lz -ljpeg -lpthread -O2
```

#### Linux

```bash
g++ main.cpp -o bicubic \
  -Dcimg_use_png -Dcimg_display=0 \
  -lpng -lz -ljpeg -lpthread -O2
```

#### Windows (MinGW)

```bash
g++ main.cpp -o bicubic.exe \
  -Dcimg_use_png -Dcimg_display=0 \
  -lpng -lz -ljpeg -lpthread -O2
```

## Running

```bash
# Basic usage
./bicubic <scale_factor>

# Example: scale by factor of 2
./bicubic 2

# Example: scale by factor of 3
./bicubic 3
```

## Troubleshooting

### Issue: "jpeglib.h not found"

- **macOS**: Ensure Homebrew is installed and run `brew install jpeg`
- **Linux**: Install `libjpeg-dev` or `libjpeg-turbo-devel`
- **Windows**: Install jpeg library via MSYS2 package manager

### Issue: "png.h not found"

- **macOS**: Run `brew install libpng`
- **Linux**: Install `libpng-dev`
- **Windows**: Install libpng via MSYS2

### Issue: "CImg.h not found"

- Ensure `CImg.h` is in the same directory as `main.cpp`

### Issue: "image.png not found"

- Place your input image in the same directory and name it `image.png`
- Or modify the code to accept command-line image paths

## Dependencies

- **CImg**: Header-only C++ image processing library (included)
- **libpng**: PNG image format support
- **libjpeg**: JPEG image format support
- **zlib**: Compression library (usually pre-installed)
- **pthread**: POSIX threads (usually pre-installed on Unix systems)

## Notes

- The program creates `change.png` as output in the current directory
- Larger scale factors will significantly increase processing time
- Alpha channel (transparency) is automatically preserved for PNG images
- Display support (X11) is disabled by default for better portability
