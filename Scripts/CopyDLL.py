import os
import re
import sys
import shutil
import winreg

if len(sys.argv) < 2:
    print("Usage: python script.py <file_to_copy>")
    exit()

file_to_copy = sys.argv[1]

if not os.path.isfile(file_to_copy):
    print(f"[-] File does not exist: {file_to_copy}")
    exit()

appid = 387990
print(f"[+] Looking for Steam game with AppID {appid}...")

try:
    key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\Valve\Steam")
    steamPath, _ = winreg.QueryValueEx(key, "SteamPath")
    winreg.CloseKey(key)
    print(f"[+] Found Steam path: {steamPath}")
except FileNotFoundError:
    steamPath = None
    print("[-] Steam path not found in registry.")
    exit()

gamePath = None
if steamPath and os.path.exists(steamPath):
    libraryPaths = [os.path.join(steamPath, "steamapps")]
    libraryVdf = os.path.join(steamPath, "steamapps", "libraryfolders.vdf")

    if os.path.exists(libraryVdf):
        with open(libraryVdf, "r", encoding="utf-8") as f:
            content = f.read()
            matches = re.findall(r'"\d+"\s+"([^"]+)"', content)
            libraryPaths.extend([os.path.join(m, "steamapps") for m in matches])
        print(f"[+] Found Steam libraries!")

    for library in libraryPaths:
        manifestPath = os.path.join(library, f"appmanifest_{appid}.acf")
        if os.path.exists(manifestPath):
            print(f"[+] Reading manifest: {manifestPath}")
            with open(manifestPath, "r", encoding="utf-8") as f:
                content = f.read()
                match = re.search(r'"installdir"\s+"([^"]+)"', content)
                if match:
                    gamePath = os.path.join(library, "common", match.group(1))
                    print(f"[+] Game install directory found: {gamePath}")
                    break
        else:
            print(f"[-] Manifest not found: {manifestPath}")

if not gamePath:
    print("[-] Game path could not be found.")
    exit()

releasePath = os.path.join(gamePath, "Release")

dest_file = os.path.join(releasePath, os.path.basename(file_to_copy))

shutil.copy(file_to_copy, dest_file)
print(f"[+] File copied to: {dest_file}")
