@echo off
set ROOT=%~dp0
set SRC=%ROOT%src
set OUT=%ROOT%pixelball_local.exe

echo Compiling PixelBall local demo...
g++ -std=c++17 -I"%SRC%" -pthread ^
  "%SRC%\local_main.cpp" ^
  "%SRC%\core\Game.cpp" ^
  "%SRC%\physics\Physics.cpp" ^
  "%SRC%\input\Input.cpp" ^
  "%SRC%\resource\Resources.cpp" ^
  "%SRC%\render\Renderer.cpp" ^
  "%SRC%\connection\Server.cpp" ^
  "%SRC%\connection\Client.cpp" ^
  -O2 -o "%OUT%" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network

echo Running...
"%OUT%"
