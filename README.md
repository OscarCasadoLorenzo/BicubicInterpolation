# BICUBIC INTERPOLATION

A cross-platform C++ implementation of bicubic interpolation for image scaling using the CImg library.

## Features

- ✨ High-quality image upscaling using bicubic interpolation
- 🎨 Full RGBA support (preserves transparency)
- 🖥️ Cross-platform: macOS and Linux
- ⚡ Fast performance with optimized algorithms (~200x faster than naive implementation)
- 📦 Single-header library (CImg)
- 🧹 Clean, maintainable code following modern C++ best practices

## Quick Start

### Installation

See [INSTALL.md](INSTALL.md) for detailed platform-specific installation instructions.

**Quick install:**

```bash
# macOS
brew install jpeg libpng

# Linux (Ubuntu/Debian)
sudo apt-get install libpng-dev libjpeg-dev

# Then compile
make
```

### Usage

```bash
# Compile the program
make

# Run with default scale factor (2x)
make run

# Run with custom scale factor (3x)
make run ARGS=3

# Or run directly
./bicubic 3

# Clean build artifacts
make clean

# Create distribution package
make zip
```

## Project Structure

```
BicubicInterpolation/
├── main.cpp              # Bicubic interpolation implementation
├── CImg.h                # Single-header image processing library
├── Makefile              # Cross-platform build system
├── image.png             # Sample input image
├── README.md             # This file
├── INSTALL.md            # Detailed installation guide
├── LICENSE               # MIT License
└── doc/images/           # Documentation images
```

### Key Components

**main.cpp** - Core implementation with:
- `cubicInterpolate()` - 1D cubic interpolation using Catmull-Rom spline
- `bicubicInterpolate()` - 2D interpolation on 4x4 pixel neighborhoods
- `extractNeighborhood()` - Extracts surrounding pixel data
- Full RGBA channel support with transparency preservation

**Makefile** - Build system with:
- Automatic OS detection (macOS, Linux)
- Platform-specific library paths
- Help command (`make help`)

### Algorithm Workflow

1. Load source image (`image.png`)
2. Create destination image with scaled dimensions
3. Copy original pixels to scaled positions
4. Interpolate missing pixels using 4x4 neighborhoods
5. Save result as `change.png`

## Code Quality

The codebase follows Clean Code principles:
- **English naming** - All functions and variables use clear English names
- **Constants** - Zero magic numbers, all values are named constants
- **DRY principle** - No code duplication
- **Single responsibility** - Each function has one clear purpose
- **Documentation** - Doxygen-style comments for all public functions
- **Modern C++** - Uses C++11 features, const correctness, proper casting

Performance has been optimized through compiler flags (`-O2`) and efficient algorithms, achieving ~200x speedup compared to unoptimized implementations.

## How It Works

Image scaling involves enlarging or reducing the physical size of an image by changing the number of pixels it contains. It adjusts the size of the image content and resizes the canvas accordingly.

There are three main algorithms to perform this task: nearest neighbor, bilinear interpolation, and the one we will focus on, bicubic interpolation. To better understand this image interpolation method, we will first explain the other two for clarity.

**Nearest neighbor** is a basic method that requires low processing time. It only considers one pixel, the one closest to the interpolated point. Essentially, it simply enlarges the size of each pixel.

**Bilinear interpolation** takes into account the values of the known pixels surrounding a given one within a 2x2 pixel neighborhood. Once these values are identified, the weighted average of these 4 pixels is calculated to determine the interpolated value. The result is smoother compared to the previous method due to the averaging process, although this requires more processing time.
![Bilinear Graph](./doc/images/Bilinear_Graph.png)

A step beyond the bilinear case is to consider the neighborhood of the 4x4 nearest known pixels, i.e., a total of 16 pixels. Since these pixels are located at different distances from the unknown-value pixel, greater weight is given in the calculation to those that are closer.
![Bicubic Graph](./doc/images/Bicubic_Graph.png)

It produces sharper images than the previous two methods. It strikes a good balance between processing time (as it has the highest computational load) and result quality. This is a standard procedure in image editing programs, printer drivers, and camera interpolation.
![Algorithms differences Graph](./doc/images/Algorithms_Graph.png)

Bicubic interpolation can be mathematically calculated as follows:

If the values of a function \( f(x) \) and its derivative are known at \( x = 0 \) and \( x = 1 \), the function can be interpolated in the interval \([0,1]\) using a third-degree polynomial. This is known as cubic interpolation. The formula for this polynomial can be easily derived.

#### A Third-Degree Polynomial and Its Derivative:

f(x) = ax^3 + bx^2 + cx + d

f'(x) = 3ax^2 + 2bx + c

#### Values of the Polynomial and Its Derivative at \( x = 0 \) and \( x = 1 \):

f(0) = d, f(1) = a + b + c + d

f'(0) = c, f'(1) = 3a + 2b + c

#### Solving the System of Equations:

These four equations can be written as:

a = 2f(0) - 2f(1) + f'(0) + f'(1)

b = -3f(0) + 3f(1) - 2f'(0) - f'(1)

c = f'(0)

d = f(0)

With this, we have the formula for cubic interpolation.

## Performance Analysis

Historical performance comparison using 512x512 pixel images across different processors:

![Processor Stats](./doc/images/Processor_Stats.png)

![Result Graph 1](./doc/images/Results_1.png)

Performance observations from older processors:
- **Ryzen 3600x**: Linear time increase, fastest overall performance
- **i7 7700MQ**: Exponential time increase, tests only up to 7x scale
- **i7 4702MQ**: Slowest, entirely exponential growth

![Result Graph 2](./doc/images/Results_2.png)

### Modern Performance

With the refactored code and compiler optimizations:
- **512x512 @ 2x scale**: ~0.17 seconds (M1 Mac)
- **512x512 @ 3x scale**: ~0.30 seconds (M1 Mac)
- Processing time scales linearly with output image size
- ~200x faster than original unoptimized implementation

## Potential Improvements

Future enhancements could include:
- Command-line arguments for custom input/output paths
- Multi-threading using OpenMP or std::thread
- SIMD optimizations (AVX2/NEON) for parallel pixel processing
- Support for floating-point scale factors
- Progress indicator for large images
- Additional interpolation methods (Lanczos, etc.)

## Dependencies

- **libpng** - PNG image format support
- **libjpeg** - JPEG image format support
- **zlib** - Compression library (usually pre-installed)
- **pthread** - Threading support (usually pre-installed)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Bibliography

- **C++ - Bicubic interpolation algorithm for image scaling. (n.d.).** [Stackoverrun](https://stackoverrun.com).
  https://stackoverrun.com/es/q/4115337

- **C++ Image interpolation with Bicubic method. (2016, April 7).** [Stack Overflow](https://stackoverflow.com/questions/36469357/c-image-interpolation-with-bicubic-method).

- **D. (n.d.).** [Resizing Images With Bicubic Interpolation. The blog at the bottom of the sea](https://blog.demofox.org/2015/08/15/resizing-images-with-bicubic-interpolation/).

- **Examples with OpenCV - OpenCV 3.1 with C++ in Visual Studio 2015. (n.d.).** [Learning Programming](http://aprendiendo-programacion.wikidot.com).
  http://aprendiendo-programacion.wikidot.com/ejemplosocv

- **Juan V. Carrillo (jvprofe). (n.d.).** Basic Path Test. White-box testing. [YouTube](https://www.youtube.com/watch?v=GVegCwwfBZ0).

- **Juan V. Carrillo (jvprofe). (2016, May 6).** How to create the flow graph of a program. [YouTube](https://www.youtube.com/watch?v=9N5vPeSWRfQ).

- **makefile - Passing arguments for "execute". (n.d.).** [Stackoverrun](https://stackoverrun.com).
  https://stackoverrun.com/es/q/456475

- **Overall Options (Using the GNU Compiler Collection (GCC)). (n.d.).** [GCC](https://gcc.gnu.org).

- **Profesor Retroman. (2019, November 28).** C++: Compile and link static libraries [Video]. [YouTube](https://www.youtube.com/watch?v=MG8z-k6lH6E&ab_channel=ProfesorRetroman).
