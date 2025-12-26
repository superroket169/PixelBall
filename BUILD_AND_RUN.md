# Build & Run Instructions

This file records the exact commands used to build the project (Linux and Windows cross-build) and to test the Windows EXE under Wine. Keep this file with the repo so you can reproduce the steps later.

## Linux (native)

- Make the Linux build script executable and run it:

```bash
chmod +x build/build_linux.sh
./build/build_linux.sh
# Run the Linux menu binary
./build/pixelball_menu
```

## Windows (cross-compile with MinGW on Linux)

- Steps we used (from repo root):

```bash
# ensure the Windows build script is executable
chmod +x build/build_windows.sh

# run the Windows build (uses x86_64-w64-mingw32-g++ if available)
./build/build_windows.sh

# After the build you will find:
#   build/pixelball_menu.exe    build/pixelball_local.exe

# For Wine testing we copied libwinpthread-1.dll into build/:
cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll build/  # path may vary per host

# If Wine reports missing libstdc++-6.dll or libgcc_s_seh-1.dll, copy them as well
# Example (path may vary):
cp /usr/x86_64-w64-mingw32/lib/libstdc++-6.dll build/
cp /usr/x86_64-w64-mingw32/lib/libgcc_s_seh-1.dll build/

# Quick run under Wine:
wine ./build/pixelball_menu.exe --version
```

## Repro script

There is a helper script that automates these Windows reproduction steps:

```bash
build/reproduce_windows.sh
```

## Notes
- The Windows build script tries to statically link SFML (`-DSFML_STATIC`) and link against static SFML archives in `SFML/build-mingw/lib` if available. If your SFML tree only has import DLLs, the produced EXE may still depend on `sfml-*.dll` files and the MinGW runtime DLLs.
- To produce a single EXE with no SFML DLLs, build SFML as static libraries for MinGW and ensure the static `.a` libs are present in the SFML lib path.
- If you need help packaging a single EXE for distribution (no external DLLs), I can add a packaging script or attempt a full static build of SFML on this host.
