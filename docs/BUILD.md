# Build Instructions

## Overview

This project uses CMake with FetchContent to automatically download and build most dependencies. You do **not** need to manually install libraries like GLFW, FreeType, HarfBuzz, libpng, or zlib - CMake will fetch and build these automatically.

## System Requirements

While most dependencies are managed via FetchContent, some OS-level graphics libraries must be installed on your system:

### Linux (Ubuntu/Debian)

```bash
sudo apt-get install -y \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libwayland-dev
```

### Linux (Fedora/RHEL)

```bash
sudo dnf install -y \
    libX11-devel \
    libXrandr-devel \
    libXinerama-devel \
    libXcursor-devel \
    libXi-devel \
    mesa-libGL-devel \
    wayland-devel
```

### macOS

On macOS, X11 and OpenGL are provided by the system. No additional packages are required.

### Windows

On Windows, OpenGL is provided by graphics drivers. No additional packages are required.

## Why These System Libraries?

These are fundamental OS graphics libraries that integrate with your display server and graphics drivers:

- **X11/Wayland**: Window management on Linux
- **OpenGL/Mesa**: Graphics rendering API
- **System display libraries**: Required by GLFW for window creation

These cannot be "fetched" via CMake because they're part of the operating system's graphics stack.

## Dependencies Managed by FetchContent

The following libraries are automatically downloaded and built by CMake:

- **GLFW 3.4**: Windowing and input handling
- **FreeType 2.14.1**: Font rendering with emoji support
- **HarfBuzz 8.3.0**: Text shaping
- **libpng 1.6.43**: PNG image support (for emoji)
- **zlib 1.3.1**: Compression library (required by libpng)
- **LunaSVG**: SVG rendering (for SVG emoji)
- **glad**: OpenGL loader

## Optional Dependencies

### Skia (Advanced Emoji Rendering)

**Skia is completely optional** and provides advanced COLRv1 emoji rendering with gradients. The project works perfectly fine without it, using PNG/bitmap emoji instead.

Skia cannot be FetchContent'd because it uses Google's GN build system and has a complex build process. If you want to enable Skia support, see [SKIA_SETUP.md](SKIA_SETUP.md).

To disable Skia warnings during build:
```bash
cmake -B build -S . -DUSE_SKIA=OFF
```

## Building

```bash
# Configure (recommended to use Ninja)
cmake -G Ninja -B build -S .

# Build
cmake --build build

# Run
./build/bin/ImBored
```

### Build Options

#### Skia Support

By default, CMake will look for Skia but won't fail if it's not found:

```bash
# Explicitly disable Skia (suppresses "not found" messages)
cmake -B build -S . -DUSE_SKIA=OFF

# Enable Skia (default, will use if found in thirdparty/skia/)
cmake -B build -S . -DUSE_SKIA=ON
```

See [SKIA_SETUP.md](SKIA_SETUP.md) for Skia build and installation instructions if you want advanced COLRv1 emoji rendering.

#### Build Type

```bash
# Debug build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# Release build (default, with LTO optimization)
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
```

## Troubleshooting

### "Could NOT find X11"

Install the X11 development libraries listed in the System Requirements section.

### "GL/gl.h: No such file or directory"

Install OpenGL/Mesa development libraries:
- Ubuntu/Debian: `sudo apt-get install libgl1-mesa-dev`
- Fedora/RHEL: `sudo dnf install mesa-libGL-devel`

### "wayland-client.h: No such file or directory"

Install Wayland development libraries:
- Ubuntu/Debian: `sudo apt-get install libwayland-dev`
- Fedora/RHEL: `sudo dnf install wayland-devel`

### Slow first build

The first build will download and compile all dependencies (GLFW, FreeType, HarfBuzz, etc.). This can take 5-10 minutes. Subsequent builds will be much faster as dependencies are cached.

## Clean Build

To completely rebuild everything:

```bash
rm -rf build
cmake -B build -S .
cmake --build build
```
