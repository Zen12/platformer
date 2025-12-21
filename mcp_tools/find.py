"""
Asset reference finder.

This script finds all references to an asset by searching for its GUID
in other asset files.

The script uses the ASSETS_DIR environment variable to determine where to search.
If not set, defaults to 'assets/resources'.

Usage:
 - python3 mcp/find.py assets/resources/images/player.png
 - python3 mcp/find.py --guid f22681f4-45d1-4542-a838-99f854890c18

 - ASSETS_DIR=/custom/path python3 mcp/find.py assets/resources/images/player.png
"""

import os
import sys
from asset_utils import get_asset_extensions_with_references

# Get assets directory from environment, or use default
ASSETS_DIR = os.getenv('ASSETS_DIR', 'assets/resources')


def read_meta_file(meta_path):
    """
    Read GUID from a .meta file.

    Args:
        meta_path (str): Path to the .meta file

    Returns:
        str: GUID string, or None if not found
    """
    try:
        with open(meta_path, 'r') as f:
            content = f.read()
            for line in content.split('\n'):
                if line.startswith('guid:'):
                    return line.split(':', 1)[1].strip()
        return None
    except Exception as e:
        print(f"Error reading .meta file: {e}", file=sys.stderr)
        return None


def find_guid_references(guid, search_dir=None):
    """
    Search for GUID references in asset files.

    Args:
        guid (str): The GUID to search for
        search_dir (str): Directory to search in (default: ASSETS_DIR from environment)

    Returns:
        list: List of (file_path, line_number, line_content) tuples where the GUID is found
    """
    if search_dir is None:
        search_dir = ASSETS_DIR

    references = []

    # File types that can contain GUID references
    asset_extensions = get_asset_extensions_with_references()

    for root, dirs, files in os.walk(search_dir):
        # Skip hidden folders
        dirs[:] = [d for d in dirs if not d.startswith('.') and d != '__pycache__']

        for file in files:
            # Check if file has an extension we care about
            if not any(file.endswith(ext) for ext in asset_extensions):
                continue

            file_path = os.path.join(root, file)

            try:
                with open(file_path, 'r') as f:
                    lines = f.readlines()
                    for line_num, line in enumerate(lines, 1):
                        if guid in line:
                            references.append((file_path, line_num, line.strip()))
            except Exception as e:
                print(f"Warning: Could not read {file_path}: {e}", file=sys.stderr)

    return references


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 mcp/find.py <file_path>")
        print("   or: python3 mcp/find.py --guid <guid>")
        print()
        print("Examples:")
        print("  python3 mcp/find.py assets/resources/images/player.png")
        print("  python3 mcp/find.py --guid f22681f4-45d1-4542-a838-99f854890c18")
        sys.exit(1)

    # Check if using --guid flag
    if sys.argv[1] == '--guid':
        if len(sys.argv) < 3:
            print("Error: --guid flag requires a GUID argument")
            sys.exit(1)
        guid = sys.argv[2]
        print(f"Searching for GUID: {guid}")
    else:
        # Treat as file path
        file_path = sys.argv[1]

        if not os.path.isfile(file_path):
            print(f"Error: File not found: {file_path}")
            sys.exit(1)

        # Read GUID from .meta file
        meta_path = file_path + '.meta'
        if not os.path.isfile(meta_path):
            print(f"Error: No .meta file found at {meta_path}")
            sys.exit(1)

        guid = read_meta_file(meta_path)
        if not guid:
            print(f"Error: Could not read GUID from {meta_path}")
            sys.exit(1)

        print(f"Asset: {file_path}")
        print(f"GUID: {guid}")

    # Search for references
    print("Searching for references...")
    print()

    references = find_guid_references(guid)

    if references:
        print(f"Found {len(references)} reference(s):")
        print()
        for ref_path, line_num, line_content in references:
            print(f"  {ref_path}:{line_num}")
            print(f"    {line_content}")
            print()
    else:
        print("✓ No references found.")
        print("This asset is not being used anywhere.")


if __name__ == "__main__":
    main()
