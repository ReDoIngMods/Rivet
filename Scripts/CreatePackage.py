"""
Creates the Rivet package zip file following the Thunderstore format.
This can then be uploaded to Thunderstore or used for local installation.

It uses the following environment variables to determine file paths:
- BUILD_DIR: The build directory containing the built RivetLoader.dll and version.dll files.
- DATA_DIR: The directory containing the Rivet data files (icon.png, manifest.json, README.md, etc).
"""

from pathlib import Path
import zipfile
import io
import os

# Define build and data directories
build_dir = Path(os.getenv("BUILD_DIR", "."))
data_dir = Path(os.getenv("DATA_DIR", build_dir / "data"))

# Define file paths
rivet_loader_file = build_dir / "rivetLoader.dll"
version_file = build_dir / "version.dll"
output_file = build_dir / "RivetPack.zip"

def load_file_bytes(file_path: Path) -> bytes:
    """
    Load the contents of a file as bytes.
    
    Args:
        file_path (Path): The path to the file to load.
    
    Returns:
        bytes: The contents of the file.
    """
    with open(file_path, "rb") as f:
        return f.read()
    
def create_package_bytes() -> bytes:
    """
    Creates the Rivet package zip file with the following structure:
    RivetPack.zip
    │   icon.png
    │   manifest.json
    │   README.md
    │   version.dll
    │
    └───RivetPack
        │   version.dll
        │
        └───Rivet
            │   Loader.dll
            │   Rivet.ini
            │   Rivet.log
            │
            └───Mods
    """
    
    # Validate required files exist
    assert rivet_loader_file.exists(), f"Missing: {rivet_loader_file}"
    assert version_file.exists(), f"Missing: {version_file}"
    assert data_dir.exists(), f"Missing: {data_dir}"

    # Create a zip package containing Rivet data and version files
    rivet_loader_data = load_file_bytes(rivet_loader_file)
    version_data = load_file_bytes(version_file)

    rivet_package_data = io.BytesIO()
    with zipfile.ZipFile(rivet_package_data, mode="w", compression=zipfile.ZIP_DEFLATED) as package:
        # Add data files (icon.png, manifest.json, README.md, etc)
        for data_file in data_dir.iterdir():
            package.writestr(data_file.name, load_file_bytes(data_file))
        
        # Add version.dll
        package.writestr("RivetPack/version.dll", version_data)

        # Add RivetPack/Rivet/Loader.dll
        package.writestr("RivetPack/Rivet/Loader.dll", rivet_loader_data)

        # Add dummy files Rivet.ini, Rivet.log, and Mods/ directory
        package.writestr("RivetPack/Rivet/Mods/", b"")
        package.writestr("RivetPack/Rivet/Rivet.ini", b"")
        package.writestr("RivetPack/Rivet/Rivet.log", b"")

    return rivet_package_data.getvalue()

if __name__ == "__main__":
    rivet_package_bytes = create_package_bytes()

    with open(output_file, "wb") as out_file:
        out_file.write(rivet_package_bytes)

    print(f"Rivet package created at: {output_file}")