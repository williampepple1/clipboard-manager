# Clipboard History Manager

<p align="center">
  <img src="resources/app_icon.png" alt="Clipboard Manager Icon" width="128" height="128">
</p>

A lightweight Windows clipboard history manager built with Qt6. Stores up to 50 copied items in a stack-based history, allowing you to quickly access and reuse previously copied content.

## Features

- **50-Item History** – Automatically stores up to 50 clipboard entries
- **Stack Behavior** – Newest items appear at the top; oldest are removed when limit is reached
- **Text & Image Support** – Tracks both text snippets and copied images
- **Duplicate Prevention** – Won't add the same content twice in a row
- **System Tray Integration** – Runs quietly in the background
- **One-Click Restore** – Double-click any item to copy it back to clipboard
- **Timestamps** – See exactly when each item was copied
- **No Installation Required** – Portable version available

## Download

| File | Description |
|------|-------------|
| `ClipboardManager-Setup.exe` | Windows installer (~13 MB) |
| `ClipboardManager-Setup.zip` | Portable version - extract and run (~16 MB) |

## Usage

1. **Run the application** – Launch `clipboard-manager.exe`
2. **Copy anything** – Text or images are automatically saved to history
3. **Access history** – Click the system tray icon or window
4. **Restore an item** – Double-click any entry to copy it back
5. **Minimize to tray** – Close the window to keep it running in background

### Keyboard & Mouse

| Action | Result |
|--------|--------|
| Double-click item | Copy to clipboard |
| Click item + Copy button | Copy to clipboard |
| Click item + Delete button | Remove from history |
| Clear All button | Remove all items |
| Close window | Minimize to system tray |
| Double-click tray icon | Show/hide window |
| Right-click tray icon | Context menu |

## Building from Source

### Prerequisites

- Qt 6.x with MinGW
- CMake 3.16+

### Build Steps

```bash
# Configure
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"

# Build
cmake --build . --config Release

# Deploy Qt dependencies
windeployqt --release clipboard-manager.exe
```

### Create Installer

Requires [Inno Setup](https://jrsoftware.org/isinfo.php):

```bash
iscc installer.iss
```

## Project Structure

```
clipboard-manager/
├── main.cpp              # Application entry point
├── mainwindow.cpp/h      # Main window and clipboard logic
├── mainwindow.ui         # Qt Designer UI file
├── CMakeLists.txt        # CMake build configuration
├── installer.iss         # Inno Setup installer script
├── create_icon.py        # Icon generation script
└── resources/
    ├── app_icon.ico      # Windows executable icon
    ├── app_icon.png      # Application icon
    ├── app.rc            # Windows resource file
    └── resources.qrc     # Qt resource file
```

## License

MIT License - feel free to use and modify.
