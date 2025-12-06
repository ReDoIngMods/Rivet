# Rivet Installation Guide

## Overview

Rivet is a versatile modding framework that allows you to easily inject custom DLLs into your game. This guide will walk you through the installation process to get Rivet up and running.

## Installation Steps

1. **Download Rivet**: Obtain the latest version of Rivet from releases (e.g. `Rivet_0.1.0-alpha.zip`).
2. **Extract Files**: Unzip the downloaded file to a temporary location.
3. **Copy DLLs**: Copy all the extracted files into your game's main directory (where the game's executable is located). (you can find this by right-clicking the game in your library and selecting "Properties" -> "Local Files" -> "Browse Local Files").
4. **Launch the Game**: Start your game as you normally would. Rivet will automatically create a `Rivet.ini` file in the game's directory on the first run.
5. **Configure Rivet**: Optionally you can configure Rivet by editing the `Rivet.ini` file or by using command line arguments. For more details on configuration options, refer to the [Configuration of Rivet](./config.md) guide.