# Cross-platform Makefile for Bicubic Interpolation
# Automatically detects OS and sets appropriate compiler flags

# Configuration
ARGS = 2
TARGET = bicubic
CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall

# Detect operating system
UNAME_S := $(shell uname -s)

# Base flags (no display)
LDFLAGS = -Dcimg_use_png -Dcimg_display=0
LIBS = -lpng -lz -ljpeg -lpthread

# macOS specific settings
ifeq ($(UNAME_S),Darwin)
	BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo "/opt/homebrew")
	INCLUDES = -I$(BREW_PREFIX)/include -I$(BREW_PREFIX)/opt/jpeg/include
	LIB_PATHS = -L$(BREW_PREFIX)/lib -L$(BREW_PREFIX)/opt/jpeg/lib
endif

# Linux specific settings
ifeq ($(UNAME_S),Linux)
	INCLUDES = 
	LIB_PATHS = 
	# Uncomment the next line if you want X11 display support on Linux
	# LDFLAGS += -lX11
endif

# Windows (MinGW/MSYS2) specific settings
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
	TARGET = bicubic.exe
	INCLUDES = 
	LIB_PATHS = 
endif

# Build target
$(TARGET): main.cpp image.png
	@echo "Compiling for $(UNAME_S)..."
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET) $(INCLUDES) $(LIB_PATHS) $(LDFLAGS) $(LIBS)
	@echo "Build complete: $(TARGET)"

# Run the program
run: $(TARGET)
	@echo "Running bicubic interpolation with scale factor $(ARGS)..."
	./$(TARGET) $(ARGS)

# Build and run
all: $(TARGET) run

# Create distribution package
zip: Makefile main.cpp CImg.h image.png README.md
	@echo "Creating distribution package..."
	zip -r bicubic-interpolation.zip Makefile main.cpp CImg.h image.png README.md doc/

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(TARGET) bicubic a.out change.png *.zip

# Help message
help:
	@echo "Bicubic Interpolation - Makefile Commands:"
	@echo "  make          - Compile the program"
	@echo "  make run      - Run with scale factor $(ARGS)"
	@echo "  make ARGS=3   - Compile and set scale factor to 3"
	@echo "  make all      - Compile and run"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make zip      - Create distribution package"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Detected OS: $(UNAME_S)"

.PHONY: run all clean zip help
