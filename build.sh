#!/bin/bash
# CherryNote - Ultra-Lightweight Floating Notes Widget
# Compile with GCC (works on Linux with Wine or WSL)

echo "Compiling CherryNote..."
gcc -o CherryNote.exe CherryNote.c -luser32 -lgdi32 -lkernel32 -O2

if [ $? -eq 0 ]; then
    echo ""
    echo "============================================================"
    echo "CherryNote compiled successfully!"
    echo "Output: CherryNote.exe"
    echo "============================================================"
    echo ""
    echo "To run the application: ./CherryNote.exe"
    echo ""
    echo "KEYBOARD SHORTCUTS:"
    echo "  Ctrl + Enter  = Add new numbered task"
    echo "  Ctrl + D      = Toggle task completion checkmark"
    echo ""
else
    echo "Compilation failed!"
    exit 1
fi
