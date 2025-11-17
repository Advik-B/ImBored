# Skia Setup for COLRv1 Emoji Rendering (OPTIONAL)

**Note: Skia is completely optional. The project works fine without it, using PNG/bitmap emoji instead.**

## Automatic Setup (Recommended)

**Skia is now automatically downloaded and integrated via CMake FetchContent!**

Simply build the project with Skia enabled (default):

```bash
cmake -B build -G Ninja
cmake --build build
```

That's it! CMake will:
1. Detect your platform (Linux/macOS/Windows, x64/arm64)
2. Download prebuilt Skia binaries (~60MB) from JetBrains skia-pack
3. Configure and link Skia automatically

### Disable Skia (Optional)

To build without Skia and reduce binary size:

```bash
cmake -B build -G Ninja -DUSE_SKIA=OFF
cmake --build build
```

### Use Custom Skia Prebuilts

To use your own prebuilt Skia package:

```bash
cmake -B build -G Ninja -DSKIA_PREBUILT_URL="https://your-url.com/skia.zip"
cmake --build build
```

## What is Skia?

Skia is a 2D graphics library that provides advanced rendering capabilities.

### Why Use Skia?

NotoColorEmoji's COLRv1 format requires paint rendering (gradients, transforms, compositing).
Skia provides the necessary graphics primitives to render these complex emoji glyphs.

**With Skia enabled:**
- Advanced COLRv1 emoji rendering with gradients and transforms
- Support for the latest NotoColorEmoji format
- Full-color, high-quality emoji

**Without Skia:**
- PNG/bitmap emoji (still provides color emoji)
- CBDT format emoji
- Grayscale emoji as final fallback
- Smaller binary size

**Most users can use the default (Skia enabled)** for the best emoji experience.

## Binary Size Impact

With LTO optimization enabled in Release mode:
- **Without Skia:** ~2.3 MB
- **With Skia (automated):** ~3.4 MB
- **Size increase:** ~1.1 MB (thanks to LTO and dead code elimination)

The automated FetchContent approach downloads JetBrains' optimized Skia builds which are
already configured for minimal size while maintaining COLRv1 rendering capabilities.

---

## Manual Build (Advanced Users Only)

**Note: This is no longer necessary with FetchContent integration. Only do this if you need
a custom Skia configuration with different build flags or a specific version.**

### Building Skia from Source

If you need custom build options not available in prebuilt binaries:

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

# Package it as a zip and use with SKIA_PREBUILT_URL
cd out/Static
zip -r custom-skia.zip libskia.a ../../include
```

Then use it in your build:

```bash
cmake -B build -G Ninja -DSKIA_PREBUILT_URL="file:///path/to/custom-skia.zip"
cmake --build build
```

## Troubleshooting

### Download fails

If the automatic download fails:
1. Check your internet connection
2. Try using a custom prebuilt URL: `-DSKIA_PREBUILT_URL=<url>`
3. Or disable Skia: `-DUSE_SKIA=OFF`

### Wrong platform detected

The script auto-detects platform. If it's wrong, file an issue with your system details:
- OS: `uname -s`
- Architecture: `uname -m`

### Binary size concerns

Use Release build mode for LTO optimization:
```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

Strip debug symbols for even smaller size:
```bash
strip build/bin/ImBored
```

## Previous Manual Setup Method

The old approach of manually building and placing Skia in `thirdparty/skia/` is deprecated
but still supported for backward compatibility. The FetchContent approach is now the
recommended and default method.

---

## Supported Platforms

The automated Skia setup supports:
- **Linux:** x86_64, aarch64
- **macOS:** x86_64, arm64 (Apple Silicon)
- **Windows:** x86_64

All platforms use JetBrains skia-pack prebuilt binaries (m126 release).

## References

- [JetBrains skia-pack releases](https://github.com/JetBrains/skia-pack/releases)
- [Skia official documentation](https://skia.org/)
