# Skia Integration

This directory is for optional Skia integration to enable full-color COLRv1 emoji rendering.

## Quick Setup

### Option 1: Download Minimal Prebuilt Skia (Recommended)

For Linux x64:
```bash
# Download minimal Skia build (~5-10MB)
cd thirdparty/skia
wget https://github.com/google/skia/releases/download/chrome-m118/skia-m118-linux-x64-minimal.tar.gz
tar -xzf skia-m118-linux-x64-minimal.tar.gz
```

### Option 2: Build Minimal Skia Yourself

See [../../docs/SKIA_SETUP.md](../../docs/SKIA_SETUP.md) for detailed instructions.

### Option 3: Skip Skia (Grayscale Emojis)

If you don't need color emojis, you can build without Skia:
```bash
cmake -B build -S . -DUSE_SKIA=OFF
```

## Directory Structure

After setup, this directory should contain:
```
thirdparty/skia/
  ├── lib/
  │   └── libskia.a       # Static Skia library
  ├── include/            # Skia headers
  │   └── core/
  │   └── ...
  └── README.md           # This file
```

## Binary Size with Skia

With proper LTO optimization:
- Without Skia: ~5-10 MB (grayscale emojis)
- With minimal Skia: ~15-25 MB (full color emojis)
- With full Skia: ~40-60 MB (all Skia features)

The build system automatically enables LTO in Release mode to minimize size.

## Troubleshooting

**CMake doesn't find Skia:**
- Ensure libskia.a and includes are in this directory
- Or set: `cmake -DSkia_DIR=/path/to/skia`

**Binary size too large:**
- Verify LTO is enabled: `cmake -DCMAKE_BUILD_TYPE=Release`
- Strip symbols: `strip build/bin/ImBored`
- Use minimal Skia build (see Option 1 above)

**Linking errors:**
- Ensure static linking: Skia must be libskia.a, not libskia.so
- Check C++ stdlib compatibility
