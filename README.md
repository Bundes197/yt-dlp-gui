# yt-dlp-gui
A simple, cross-platform GUI extension for YouTube downloader yt-dlp, built with C++ and Qt.

## Features
* Download single videos or entire playlists.
* Choose between downloading Audio only (MP3, M4A, FLAC, etc.) or Video (MP4, MKV, WebM, etc.).
* Built-in rate-limiting to avoid IP bans.
* Automatic dark/light mode switching based on your OS settings.

## Prerequisites
For this app to work, you **must** have `yt-dlp` and `ffmpeg` installed on your system. The app will search for these binaries automatically.
For more info, check [yt-dlp](https://github.com/yt-dlp/yt-dlp) and [ffmpeg](https://ffmpeg.org).
### Windows
#### WinGet
The easiest way to install the dependencies on Windows is using [WinGet](https://learn.microsoft.com/en-us/windows/package-manager/winget/)
```shell
winget install yt-dlp.yt-dlp && winget install Gyan.FFmpeg
```
#### Direct download
1. Download `yt-dlp.exe` from the [yt-dlp releases page](https://github.com/yt-dlp/yt-dlp/releases).
2. Download `ffmpeg.exe` from the [official FFmpeg site](https://ffmpeg.org/download.html).
3. Place both `.exe` files into a folder that is in your system's `PATH`, or put them in the same directory as the `yt-dlp-gui` executable.

### macOS
#### Homebrew
The easiest way to install the dependencies on macOS is using [Homebrew](https://brew.sh/):
```bash
brew install yt-dlp ffmpeg
```
### Linux
You can install the dependencies using your package manager
#### Ubuntu/Debian/Mint
```bash
sudo apt update
sudo apt install yt-dlp ffmpeg
```
#### Arch/Manjaro
```bash
sudo pacman -Syu ffmpeg yt-dlp
```
#### Fedora/Red Hat
```bash
sudo dnf install ffmpeg yt-dlp
```

## Installation
Go to the **Releases** page of this repository.
Download the package for your operating system:
* Windows: Download the `.zip` file, extract it and run `yt-dlp-gui.exe`.
* macOS: Download the `.dmg` file, open it and drag the app to your Applications folder.
* Linux: Download the `.zip` file, extract it and run the executable.

## Building from source
If you want to build the project from source, you will need **CMake** and **Qt 6**.
```
mkdir build
cd build
cmake ..
cmake --build .
```