"""Stage the shared SDK and Lua hot-reload binaries as Thunderstore packages."""

from pathlib import Path
import shutil


ROOT = Path(__file__).resolve().parents[1]
SDK_SOURCE = ROOT.parent / "scrap_mechanic_sdk" / "build" / "windows" / "x64" / "release" / "scrap_mechanic_sdk.dll"
HOT_RELOAD_SOURCE = ROOT / "Mods" / "BenMcAvoy-Lua_Hot_Reload-0.1.0" / "lua_hot_reload.dll"
SDK_PACKAGE = ROOT / "Mods" / "BenMcAvoy-ScrapMechanicSDK-0.1.0"
HOT_RELOAD_PACKAGE = ROOT / "Mods" / "BenMcAvoy-Lua_Hot_Reload-0.1.0"
ICON_SOURCE = ROOT / "Data" / "icon.png"
SDK_README_SOURCE = ROOT.parent / "scrap_mechanic_sdk" / "README.md"
HOT_RELOAD_README_SOURCE = ROOT.parent / "lua_hot_reload" / "README.md"


def stage(source: Path, destination: Path) -> None:
    if not source.is_file():
        raise FileNotFoundError(f"missing build artifact: {source}")
    destination.parent.mkdir(parents=True, exist_ok=True)
    if source.resolve() == destination.resolve():
        print(f"already staged {destination}")
        return
    shutil.copy2(source, destination)
    print(f"staged {destination}")


def main() -> None:
    stage(SDK_SOURCE, SDK_PACKAGE / "scrap_mechanic_sdk.dll")
    stage(HOT_RELOAD_SOURCE, HOT_RELOAD_PACKAGE / "lua_hot_reload.dll")
    stage(ICON_SOURCE, SDK_PACKAGE / "icon.png")
    stage(ICON_SOURCE, HOT_RELOAD_PACKAGE / "icon.png")
    stage(SDK_README_SOURCE, SDK_PACKAGE / "README.md")
    stage(HOT_RELOAD_README_SOURCE, HOT_RELOAD_PACKAGE / "README.md")
    for generated_file in ("lua_hot_reload.exp", "lua_hot_reload.lib"):
        generated_path = HOT_RELOAD_PACKAGE / generated_file
        if generated_path.exists():
            generated_path.unlink()


if __name__ == "__main__":
    main()
