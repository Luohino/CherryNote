@echo off
REM CherryNote - Ultra-Lightweight Floating Notes Widget
REM Compile with MinGW GCC

REM Check if GCC is available
gcc --version >nul 2>&1
if errorlevel 1 (
    echo Error: GCC is not installed or not in PATH
    echo Please install MinGW and add gcc.exe to your system PATH
    pause
    exit /b 1
)

echo Compiling CherryNote...
gcc -o CherryNote.exe CherryNote.c -mwindows -luser32 -lgdi32 -lkernel32 -O2

if errorlevel 1 (
    echo Compilation failed!
    pause
    exit /b 1
)

echo.
echo ============================================================
echo CherryNote compiled successfully!
echo Output: CherryNote.exe
echo ============================================================
echo.
echo To run the application, type: CherryNote.exe
echo or double-click CherryNote.exe in File Explorer
echo.
echo KEYBOARD SHORTCUTS:
echo   Ctrl + Enter  = Add new numbered task
echo   Ctrl + D      = Toggle task completion checkmark
echo.
pause
