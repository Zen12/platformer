#!/usr/bin/env python3
"""
Rename an asset file and its metadata, preserving the GUID.
"""

import os
import sys
import yaml
import shutil

def rename_asset(old_path: str, new_path: str):
    """
    Rename an asset file and its .meta file, preserving the GUID.

    Args:
        old_path: Original file path
        new_path: New file path
    """
    # Check if old file exists
    if not os.path.exists(old_path):
        print(f"ERROR: File '{old_path}' does not exist")
        return False

    # Check if old .meta exists
    old_meta_path = f"{old_path}.meta"
    if not os.path.exists(old_meta_path):
        print(f"ERROR: Metadata file '{old_meta_path}' does not exist")
        return False

    # Check if new file already exists
    if os.path.exists(new_path):
        print(f"ERROR: Target file '{new_path}' already exists")
        return False

    new_meta_path = f"{new_path}.meta"
    if os.path.exists(new_meta_path):
        print(f"ERROR: Target metadata file '{new_meta_path}' already exists")
        return False

    # Read the old metadata
    with open(old_meta_path, 'r') as f:
        meta = yaml.safe_load(f)

    # Get the new name (filename without path and extension)
    new_name = os.path.splitext(os.path.basename(new_path))[0]

    # Update the name field in metadata (preserve GUID and other fields)
    meta['name'] = new_name

    # Rename the main file
    print(f"Renaming: {old_path} -> {new_path}")
    shutil.move(old_path, new_path)

    # Write new metadata file
    print(f"Renaming metadata: {old_meta_path} -> {new_meta_path}")
    with open(new_meta_path, 'w') as f:
        yaml.dump(meta, f, default_flow_style=False, sort_keys=False)

    # Remove old metadata file
    os.remove(old_meta_path)

    print(f"✓ Successfully renamed asset")
    print(f"  Name: {meta['name']}")
    print(f"  GUID: {meta['guid']} (preserved)")
    print(f"  Type: {meta['type']}")

    return True

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 rename_asset.py <old_path> <new_path>")
        print("Example: python3 rename_asset.py assets/file.fsm assets/new_file.fsm")
        sys.exit(1)

    old_path = sys.argv[1]
    new_path = sys.argv[2]

    success = rename_asset(old_path, new_path)
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
