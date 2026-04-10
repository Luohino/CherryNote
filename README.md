# CherryNote - Ultra-Lightweight Floating Notes Widget

## Build Instructions

### Prerequisites
- **GCC/MinGW** installed and in system PATH
- **Windows 7 or later** (tested on Windows 10/11)

### Compilation

**Windows (Command Prompt/PowerShell):**
```
gcc -o CherryNote.exe CherryNote.c -mwindows -luser32 -lgdi32 -lkernel32 -O2
```

**Or use the build batch file:**
```
build.bat
```

**Linux/WSL:**
```
bash build.sh
```

## Features

- **Ultra-Lightweight**
- Single .exe file (~50-80KB)
- Minimal memory footprint
- No external dependencies

- **Always-On-Top Floating Window**
- Cherry red gradient background (#8B0000 to #FF2D2D)
- Draggable from title bar
- Stays above all other windows (WS_EX_TOPMOST)

- **Smart Task Management**
- Auto-numbered tasks with **Ctrl + Enter**
- Toggle completion status with **Ctrl + D**
- Checkmark (✔) marks completed tasks

- **Auto-Save**
- Notes automatically saved to `CherryNote.txt`
- Previous notes loaded on startup
- No data loss

- **Minimal UI**
- Flat design
- No shadows or heavy effects
- Clean, distraction-free interface

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| **Ctrl + Enter** | Insert new numbered task |
| **Ctrl + D** | Toggle task completion checkmark |

## Technical Details

**Language:** Pure C (C89 compatible)
**API:** Win32 only
**Compiler:** GCC (MinGW)
**Size:** Single executable file
**Performance:** <5MB RAM usage, zero background processes

## Files

- `CherryNote.c` - Main application source code
- `CherryNote.exe` - Compiled executable (after build)
- `CherryNote.txt` - Auto-saved notes file
- `build.bat` - Windows build script
- `build.sh` - Linux/WSL build script

## Code Structure

```
CherryNote.c
├── Main Entry Point (WinMain)
├── Window Creation & Management
├── Edit Control Implementation
├── Event Handlers
│   ├── WM_PAINT (rendering)
│   ├── WM_LBUTTONDOWN (drag start)
│   ├── WM_MOUSEMOVE (drag tracking)
│   └── WM_KEYDOWN (shortcuts)
├── Utility Functions
│   ├── SaveNotes()
│   ├── LoadNotes()
│   ├── GetNextTaskNumber()
│   ├── ToggleTaskCompletion()
│   └── UpdateTaskCount()
└── File I/O & Auto-save
```

## Performance

- **Memory Usage:** ~2-5MB (includes Windows framework overhead)
- **Startup Time:** <100ms
- **CPU Usage:** Negligible when idle
- **Disk I/O:** Only on changes (throttled to 1-second intervals)

## Window Size

- **Width:** 280 pixels
- **Height:** 420 pixels
- **Resizable:** No (locked for consistency)
- **Movable:** Yes (drag from title bar)

## Notes Format

Tasks are stored as plain text with auto-numbering:
```
1. Buy milk
2. Call mom ✔
3. Write code
```

Completed tasks show the checkmark (✔) at the end.

## Troubleshooting

**"gcc is not recognized"**
- Install MinGW: https://www.mingw-w64.org/
- Add MinGW `bin` folder to System PATH

**Window appears but won't respond**
- Press Alt+F4 to close
- Check if another instance is running

**Notes not saving**
- Ensure write permissions in the application directory
- Check that `CherryNote.txt` isn't read-only

## Future Enhancements

- [ ] Custom color themes
- [ ] Font size adjustment
- [ ] Multiple note windows
- [ ] Hotkey to show/hide
- [ ] System tray integration
- [ ] Rich text formatting

---

**CherryNote** - Minimal. Lightweight. Always There.
