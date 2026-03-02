# MicroForge

A realtime granular audio effect plugin based on concepts from Curtis Roads' "Microsound" book.

## About

MicroForge is a granular synthesis audio plugin developed as part of a Bachelor's thesis at Berliner Hochschule für Technik (BHT). The plugin implements real-time granular audio processing techniques inspired by Curtis Roads' seminal work on microsound synthesis.

## License

This project is licensed under the GNU General Public License v3.0 (GPLv3) - see the [LICENSE](LICENSE) file for details.

This means:
- You are free to use, modify, and distribute this software
- Any modifications or derivative works must also be released under GPLv3
- The source code must be made available when distributing the software

### JUCE Framework

This project uses the JUCE framework under the GPLv3 license. For more information about JUCE licensing, visit https://juce.com/juce-7-licence

## Development Status

⚠️ This project is currently under active development as part of academic research. Features and functionality may change.

## Building

### Prerequisites

- CMake (version 3.24 or higher)
- C++23 compatible compiler
  - macOS: Xcode 14 or later
  - Windows: Visual Studio 2022 or later
  - Linux: GCC 12+ or Clang 15+
- Internet connection (for first build - JUCE will be downloaded automatically via CPM)

### Build Instructions

#### macOS / Linux
```bash
# Clone the repository
git clone https://github.com/bkschaefer/MicroForge
cd MicroForge

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# The plugin will be automatically copied to your system plugin folders
```

#### Windows
```bash
# Clone the repository
git clone https://github.com/bkschaefer/MicroForge
cd MicroForge

# Create build directory
mkdir build
cd build

# Configure (for Visual Studio)
cmake ..

# Build
cmake --build . --config Release

# The plugin will be automatically copied to your system plugin folders
```

### Plugin Formats

The build process generates the following plugin formats:
- VST3
- AU (macOS only)
- Standalone application

### Note

On the first build, CMake will automatically download JUCE via CPM (C++ Package Manager). This requires an internet connection and may take a few minutes.

The plugin is automatically installed to your system's plugin directories after building (COPY_PLUGIN_AFTER_BUILD is enabled).

## Author

Benedikt Schäfer  
Bachelor Thesis Project, 2026  
Berliner Hochschule für Technik
