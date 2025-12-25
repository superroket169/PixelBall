#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$ROOT/src"
OUT_DIR="$ROOT/build"

echo "Building PixelBall (Linux) -> $OUT_DIR"
mkdir -p "$OUT_DIR"

echo "  Building menu binary..."
g++ -std=c++17 -I"$SRC" -pthread \
  "$SRC/menu_main.cpp" \
  "$SRC/core/Game.cpp" \
  "$SRC/physics/Physics.cpp" \
  "$SRC/input/Input.cpp" \
  "$SRC/resource/Resources.cpp" \
  "$SRC/render/Renderer.cpp" \
  "$SRC/connection/Server.cpp" \
  "$SRC/connection/Client.cpp" \
  "$SRC/ui/Menu.cpp" \
  -O2 -o "$OUT_DIR/pixelball_menu" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

echo "  Building local demo binary..."
g++ -std=c++17 -I"$SRC" -pthread \
  "$SRC/local_main.cpp" \
  "$SRC/core/Game.cpp" \
  "$SRC/physics/Physics.cpp" \
  "$SRC/input/Input.cpp" \
  "$SRC/resource/Resources.cpp" \
  "$SRC/render/Renderer.cpp" \
  -O2 -o "$OUT_DIR/pixelball_local" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

echo "Build complete: $OUT_DIR/pixelball_menu and $OUT_DIR/pixelball_local"
