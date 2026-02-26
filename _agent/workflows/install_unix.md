---
description: How to install OmniGraph and its dependencies on Unix/Apple
---

This workflow describes how to build and install OmniGraph.

### Prerequisites

- CMake 3.16+
- Built dependencies (OmniCore, OmniData, OmniERP, OmniUtils) located in their respective subdirectories within the project root.

### Installation Steps

1. **Create and enter build directory**
   ```bash
   mkdir -p build && cd build
   ```

2. **Configure the project**
   ```bash
   cmake ..
   ```

3. **Build the project**
   ```bash
   make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)
   ```

4. **Install the application and libraries**
   > [!NOTE]
   > This step requires administrative privileges as it installs to `/usr/local/bin` and `/usr/local/lib/OmniSphere`.
   
   ```bash
   sudo make install
   ```

### Verification

After installation, you can verify the existence of the files:
- Executable: `/usr/local/bin/OmniGraph`
- Libraries: `/usr/local/lib/OmniSphere/libOmni*.{so,dylib}`
