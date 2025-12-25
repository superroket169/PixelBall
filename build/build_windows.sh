#!/usr/bin/env bash
set -euo pipefail

# Windows/MinGW build script.
# This script attempts to use an available MinGW g++ (cross or native).
# It links the C++ runtime statically but expects SFML libraries to be available
# as DLLs (sfml-*.dll) and SFML import libs in the link path.

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/src"
OUT_DIR="$ROOT/build"

echo "Building PixelBall (Windows MinGW) -> $OUT_DIR"
mkdir -p "$OUT_DIR"

echo "  Copying SFML DLLs into build folder (only SFML DLLs, not C++ runtimes)..."
# locate SFML dlls under user's Codes/SFML if present
SFML_SEARCH_DIRS=("$HOME/Codes/SFML" "$HOME/Documents/Codes/SFML" "$HOME/Downloads/SFML")
found_any=0
for SFML_SEARCH_DIR in "${SFML_SEARCH_DIRS[@]}"; do
  if [ -d "$SFML_SEARCH_DIR" ]; then
    echo "    scanning $SFML_SEARCH_DIR"
    for f in $(find "$SFML_SEARCH_DIR" -type f -iname "sfml-*.dll" -print 2>/dev/null); do
      echo "      copying $(basename "$f")"
      cp -u "$f" "$OUT_DIR/" || echo "        failed to copy $f"
      found_any=1
    done
  fi
done
if [ "$found_any" -eq 0 ]; then
  echo "    no sfml-*.dll files found in standard locations; skipping DLL copy."
fi

# Prefer cross-compiler if present
if command -v x86_64-w64-mingw32-g++ >/dev/null 2>&1; then
  CXX=x86_64-w64-mingw32-g++
elif command -v i686-w64-mingw32-g++ >/dev/null 2>&1; then
  CXX=i686-w64-mingw32-g++
elif command -v g++ >/dev/null 2>&1; then
  CXX=g++
else
  echo "No suitable g++ found in PATH (need MinGW)." >&2
  exit 1
fi

# Build: static C++ runtime, dynamic SFML (DLLs). User must provide SFML import libs in linker path.

# Detect SFML include and lib dirs
SFML_INCLUDE=""
SFML_LIB=""
SFML_INC_CAND=("$HOME/Codes/SFML/include" "$HOME/Documents/Codes/SFML/include" "/usr/local/include" )
SFML_LIB_CAND=("$HOME/Codes/SFML/build-mingw/lib" "$HOME/Documents/Codes/SFML/build-mingw/lib" "/usr/local/lib")
for p in "${SFML_INC_CAND[@]}"; do
  if [ -d "$p/SFML" ]; then
    SFML_INCLUDE="$p"
    break
  fi
done
for p in "${SFML_LIB_CAND[@]}"; do
  if [ -d "$p" ]; then
    SFML_LIB="$p"
    break
  fi
done
if [ -n "$SFML_INCLUDE" ]; then
  echo "  Using SFML include: $SFML_INCLUDE"
fi
if [ -n "$SFML_LIB" ]; then
  echo "  Using SFML lib: $SFML_LIB"
fi

echo "  Building menu binary (Windows)..."
$CXX -std=c++17 -I"$SRC" ${SFML_INCLUDE:+-I"$SFML_INCLUDE"} -pthread \
  -static-libstdc++ -static-libgcc \
  "$SRC/menu_main.cpp" \
  "$SRC/core/Game.cpp" \
  "$SRC/physics/Physics.cpp" \
  "$SRC/input/Input.cpp" \
  "$SRC/resource/Resources.cpp" \
  "$SRC/render/Renderer.cpp" \
  "$SRC/connection/Server.cpp" \
  "$SRC/connection/Client.cpp" \
  "$SRC/ui/Menu.cpp" \
  -O2 -o "$OUT_DIR/pixelball_menu.exe" ${SFML_LIB:+-L"$SFML_LIB"} -lws2_32 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

echo "  Building local demo binary (Windows)..."
$CXX -std=c++17 -I"$SRC" ${SFML_INCLUDE:+-I"$SFML_INCLUDE"} -pthread \
  -static-libstdc++ -static-libgcc \
  "$SRC/local_main.cpp" \
  "$SRC/core/Game.cpp" \
  "$SRC/physics/Physics.cpp" \
  "$SRC/input/Input.cpp" \
  "$SRC/resource/Resources.cpp" \
  "$SRC/render/Renderer.cpp" \
  -O2 -o "$OUT_DIR/pixelball_local.exe" ${SFML_LIB:+-L"$SFML_LIB"} -lws2_32 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

echo "Windows build complete: $OUT_DIR/pixelball_menu.exe and $OUT_DIR/pixelball_local.exe"
echo "SFML DLLs (sfml-*.dll) were copied from $SFML_SEARCH_DIR if present. Ensure MinGW import libs are available when linking."
