# Bind Hotkeys to OBS Docks

A third party plugin that lets you assign keyboard shortcuts to any dock panel, so you can instantly focus them without touching the mouse.

## What it does

OBS has many dock panels (Scene List, Sources, Audio Mixer, etc.) but there is no built-in way to focus them with a hotkey. This plugin automatically discovers all docks available in your OBS window — including docks added by other plugins — and registers a hotkey entry for each one under **Settings → Hotkeys**.

## Features

- Automatically detects all dock panels, including those added by third-party plugins
- Registers a **"Focus Dock: ..."** hotkey for each panel found
- Hotkey bindings are saved per profile and restored on startup
- Rescans for new docks every few seconds, so late-loading plugin docks are picked up automatically
- If a dock is hidden, the hotkey will make it visible and bring it to focus

## Installation

1. Download the latest release for your platform from the [Releases](../../releases) page
2. Extract and copy the files to your OBS plugins folder:
   - **Windows**: `C:\Program Files\obs-studio\obs-plugins\64bit\`
   - **macOS**: `~/Library/Application Support/obs-studio/plugins/`
3. Restart OBS Studio

## Usage

1. Open OBS and go to **Tools → Settings → Hotkeys**
2. Scroll down and look for entries labeled **"Focus Dock: Scene List"**, **"Focus Dock: Sources"**, etc.
3. Assign a key combination to the docks you want
4. Press the hotkey at any time to instantly focus that dock

## Building from source

Requirements:
- CMake 3.28 or higher
- Qt6
- OBS Studio source or pre-built dependencies

```bash
cmake -S . -B build
cmake --build build
```

The plugin name, version and author info are configured in `buildspec.json` at the root of the project.

## Contributing

Issues and pull requests are welcome. Please open an issue first if you plan a larger change.

## License

This project follows the same license as the OBS Plugin Template it is based on. See [LICENSE](LICENSE) for details.
