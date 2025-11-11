# ImBored Project Guidelines

## Project Overview
**ImBored** is a modern C++20 GUI application with a modular architecture built using:
- **OpenGL** for rendering
- **GLFW** for window management
- **ImGui** for the user interface
- **FreeType** for font rendering with emoji support
- **HarfBuzz** for text shaping

The project uses **CMake** with **Ninja** as the build system.

## Repository Structure

```
ImBored/
├── CMakeLists.txt           # Root CMake configuration
├── main.cpp                 # Application entry point
├── include/                 # Public headers
│   ├── core/               # Core subsystem (Window, etc.)
│   ├── rendering/          # Rendering subsystem (Renderer, etc.)
│   └── ui/                 # UI subsystem (FontManager, etc.)
├── src/                    # Implementation files
│   ├── CMakeLists.txt
│   ├── core/               # Core implementation
│   ├── rendering/          # Rendering implementation
│   └── ui/                 # UI implementation
├── thirdparty/             # External libraries
│   ├── imgui/              # ImGui with backends
│   └── glad/               # OpenGL loader
├── resources/              # Assets (fonts, etc.)
├── build/                  # Build output directory
└── .idea/                  # IDE configuration
```

## Architecture

### Modular Design
The project follows a modular architecture with three main subsystems:

1. **Core** (`ImBored::Core`)
   - Window management via GLFW
   - System initialization and cleanup

2. **Rendering** (`ImBored::Rendering`)
   - OpenGL rendering pipeline
   - Viewport management
   - Graphics operations

3. **UI** (`ImBored::UI`)
   - Font management with FreeType
   - ImGui integration
   - User interface components

Each module is built as a separate library and linked to the main executable.

## Build Instructions

### Prerequisites
- CMake 4.0+
- C++20 compiler (MSVC recommended on Windows)
- Ninja build system (optional but recommended)

### Build Steps
```bash
# Configure with Ninja (recommended)
cmake -G Ninja -B build -S .

# Or configure with default generator
cmake -B build -S .

# Build
cmake --build build

# Run
./build/bin/ImBored
```

## Coding Standards

### C++ Version
- Use **C++20** features
- Modern C++ best practices
- Avoid C-style casts; use C++ casts

### Naming Conventions
- Classes: `PascalCase`
- Functions: `camelCase`
- Member variables: `camelCase`
- Private members: prefix with `_` (optional)

### File Organization
- Headers in `include/` with `.hpp` extension
- Implementation in `src/` with `.cpp` extension
- Group related functionality in subdirectories

### Include Path
- Use relative includes from `include/` directory
- Example: `#include "include/core/window.hpp"`

## Dependencies

### External Libraries (auto-fetched via CMake)
- **GLFW 3.4**: Window and input management
- **HarfBuzz 8.3.0**: Text shaping
- **FreeType VER-2-14-1**: Font rendering
- **ImGui** (bundled in thirdparty)

### Static Libraries (built locally)
- **glad**: OpenGL loader from `thirdparty/glad/`
- **imgui**: ImGui implementation from `thirdparty/imgui/`

## Resources

### Assets Location
All resources (fonts, images, etc.) are stored in `resources/` and copied to the output directory during build:
- `Quicksand-Regular.ttf` - Main UI font
- `NotoColorEmoji-Regular.ttf` - Color emoji font (supports full emoji range)

### Color Emoji Support
The application includes full **color emoji support** via FreeType:
- Loads NotoColorEmoji font with comprehensive Unicode ranges
- Supported emoji categories:
  - **0x1F300 - 0x1F9FF**: Emoticons, symbols, pictographs, miscellaneous
  - **0x2600 - 0x27BF**: Miscellaneous symbols
  - **0x2700 - 0x27EF**: Dingbats
  - **0xFE00 - 0xFE0F**: Variation selectors
- Fonts are merged, so emojis blend seamlessly with text
- Uses `ImGuiFreeTypeLoaderFlags_LoadColor` for proper color rendering
- Pixel snapping disabled for better emoji appearance

## Common Tasks

### Adding a New Module
1. Create header files in `include/module_name/`
2. Create implementation in `src/module_name/`
3. Add `CMakeLists.txt` in `src/module_name/`
4. Update root `CMakeLists.txt` to include and link the module

### Adding New Code
1. Place headers in appropriate `include/subsystem/` directory
2. Place implementation in corresponding `src/subsystem/` directory
3. Update CMakeLists.txt in that subsystem
4. Include using `#include "include/subsystem/file.hpp"`

### Building with Different Generators
```bash
# Visual Studio
cmake -G "Visual Studio 17 2022" -B build -S .

# LLVM/Clang
cmake -G Ninja -B build -S . -DCMAKE_CXX_COMPILER=clang++

# GCC
cmake -G Ninja -B build -S . -DCMAKE_CXX_COMPILER=g++
```

## Important Notes

- **DPI Awareness**: Handled automatically for Windows via manifest settings
- **Static Runtime**: MSVC uses static runtime linkage (`/MT` or `/MTd`)
- **ImGui Rendering**: Happens every frame; no polling-based updates
- **Font Loading**: Uses FreeType with ImGui integration; supports colored emoji
- **Keyboard Navigation**: Enabled by default in ImGui

## Debugging

### Building in Debug Mode
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Common Issues
- **CMake not finding Ninja**: Install Ninja or use default generator
- **Font loading fails**: Verify `resources/` directory exists in output directory
- **OpenGL errors**: Check graphics card drivers support OpenGL 3.3+
