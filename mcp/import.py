"""
This code is write by chatGTP, this is not expected to be maintained
It is only to automate manual work

What it does:
 - Finds all files that doesn't have associated .meta files (similar as in unity engine)
 - create a templated version of it with custom GUID

The script uses the ASSETS_DIR environment variable to determine the default directory.
If not set, defaults to 'assets/resources'.

Usage:
 - python3 mcp/import.py              # Import everything from ASSETS_DIR (default: assets/resources)
 - python3 mcp/import.py some/folder  # Import only that folder
 - python3 mcp/import.py some/file.png # Import only that file

 - ASSETS_DIR=/custom/path python3 mcp/import.py  # Use custom default directory
"""

import os
import sys
import uuid

from asset_utils import dict_to_yaml, ext_check

# Get assets directory from environment, or use default
ASSETS_DIR = os.getenv('ASSETS_DIR', 'assets/resources')


def create_meta_file(file_path):
    """Create a .meta file for a single file"""
    base_name, ext = os.path.splitext(os.path.basename(file_path))

    if ext == '.meta' or ext == '':
        return

    yaml_path = file_path + '.meta'
    if os.path.isfile(yaml_path):
        return

    print(yaml_path)
    data = {
        'name': base_name,
        'guid': str(uuid.uuid4()),
        'extension': ext,
        'type': ext_check(ext)
    }

    with open(yaml_path, 'w') as f:
        f.write(dict_to_yaml(data))


def process_directory(directory):
    """Process all files in a directory recursively"""
    for root, dirs, files in os.walk(directory):
        dirs[:] = [d for d in dirs if not d.startswith('.') and d != '__pycache__']

        for file in files:
            if file.startswith('.') or file.endswith('.meta'):
                continue

            file_path = os.path.join(root, file)
            create_meta_file(file_path)


def main():
    if len(sys.argv) == 1:
        target = ASSETS_DIR
        if not os.path.exists(target):
            print(f"Default directory '{target}' not found")
            return
        print(f"Processing entire tree from: {target}")
        process_directory(target)
    else:
        target = sys.argv[1]

        if os.path.isfile(target):
            print(f"Processing file: {target}")
            create_meta_file(target)
        elif os.path.isdir(target):
            print(f"Processing directory: {target}")
            process_directory(target)
        else:
            print(f"Error: '{target}' is not a valid file or directory")


if __name__ == "__main__":
    main()

