@echo off
REM Run the Windows build executable from this folder
cd /d "%~dp0"
if exist pixelball_menu.exe (
    echo Starting pixelball_menu.exe
    start "PixelBall" "%~dp0pixelball_menu.exe"
) else (
    echo pixelball_menu.exe not found in %~dp0
    echo Build it with build_windows.sh or copy the executable here.
)
