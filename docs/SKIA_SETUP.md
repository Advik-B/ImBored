# Skia Setup for COLRv1 Emoji Rendering

This document explains how to build and integrate Skia for full-color COLRv1 emoji support.

## Why Skia?

NotoColorEmoji uses COLRv1 format which requires paint rendering (gradients, transforms, compositing).
Skia provides the necessary graphics primitives to render these complex emoji glyphs.

## Size Optimization

With proper configuration, Skia impact on binary size is minimal:
- Full Skia library: ~200MB
- Minimal static build: ~40-70MB  
- **Minimal + LTO + dead code elimination: 5-20MB** ✅

## Build Instructions

### Option 1: Minimal Skia Build (Recommended)

```bash
# Clone Skia
git clone https://skia.googlesource.com/skia.git
cd skia
python3 tools/git-sync-deps

# Configure minimal build
bin/gn gen out/Static --args='
  is_official_build=true
  is_component_build=false
  skia_use_system_expat=false
  skia_use_system_libjpeg_turbo=false
  skia_use_system_libpng=false
  skia_use_system_libwebp=false
  skia_use_system_zlib=false
  skia_use_system_icu=false
  skia_use_system_harfbuzz=false
  skia_enable_gpu=false
  skia_enable_skottie=false
  skia_enable_pdf=false
  skia_enable_particles=false
  skia_enable_skshaper=false
  skia_enable_svg=false
  extra_cflags=["-flto", "-O3"]
  extra_ldflags=["-flto", "-Wl,--gc-sections"]
'

# Build
ninja -C out/Static

# Install to system or ImBored/thirdparty/skia
sudo cp out/Static/libskia.a /usr/local/lib/
sudo cp -r include/* /usr/local/include/skia/
```

### Option 2: Use Prebuilt Skia

Download minimal prebuilt binaries from https://github.com/google/skia/releases

### Option 3: System Package (if available)

```bash
# Some distributions provide Skia packages
sudo apt-get install libskia-dev  # Debian/Ubuntu (if available)
```

## CMake Integration

Once Skia is installed:

```bash
cd ImBored
cmake -B build -S . -DUSE_SKIA=ON
cmake --build build
```

To disable Skia (fallback to grayscale):

```bash
cmake -B build -S . -DUSE_SKIA=OFF
cmake --build build
```

## Verifying Integration

After building, check for color emoji support:

```bash
./build/bin/ImBored
# You should see "Sk

ia-based COLRv1 rendering enabled" in the console
```

## Troubleshooting

### Skia not found
Ensure libskia.a and headers are in CMake search paths:
```bash
cmake -B build -S . -DSkia_DIR=/path/to/skia/cmake
```

### LTO issues
If LTO causes linking problems, disable it:
```bash
# In Skia build args:
extra_cflags=["-O3"]  # Remove -flto
extra_ldflags=["-Wl,--gc-sections"]  # Remove -flto
```

### Large binary size
Verify dead code elimination is working:
```bash
strip build/bin/ImBored  # Remove debug symbols
ls -lh build/bin/ImBored  # Should be <50MB with full features
```

## Performance

With Skia integration:
- Emoji atlas build time: ~50-100ms (one-time on startup)
- Runtime performance: Negligible (uses pre-rendered texture atlas)
- Memory: ~2-8MB for emoji texture atlas

## Alternative: Bitmap Emoji Font

If Skia setup is too complex, consider using Twemoji or Apple Color Emoji (CBDT format):
- No Skia dependency
- Immediate color support
- Slightly larger font files
- No gradient/transform support (simple color bitmaps)
