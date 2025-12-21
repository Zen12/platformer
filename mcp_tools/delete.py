"""
Asset deletion tool with reference checking.

This script safely deletes asset files by:
1. Reading the asset's GUID from its .meta file
2. Searching for references to that GUID in other assets
3. Warning if the asset is referenced elsewhere
4. Deleting both the asset file and its .meta file

Usage:
 - python3 mcp/delete.py assets/resources/some/file.png
"""

import os
import sys

from find import read_meta_file, find_guid_references


def delete_file_and_meta(file_path):
    """Delete both the asset file and its .meta file."""
    meta_path = file_path + '.meta'

    deleted = []
    failed = []

    # Delete the asset file
    try:
        os.remove(file_path)
        deleted.append(file_path)
        print(f"Deleted: {file_path}")
    except Exception as e:
        failed.append((file_path, str(e)))
        print(f"Failed to delete {file_path}: {e}")

    # Delete the .meta file if it exists
    if os.path.isfile(meta_path):
        try:
            os.remove(meta_path)
            deleted.append(meta_path)
            print(f"Deleted: {meta_path}")
        except Exception as e:
            failed.append((meta_path, str(e)))
            print(f"Failed to delete {meta_path}: {e}")

    return deleted, failed


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 mcp/delete.py <file_path>")
        print("Example: python3 mcp/delete.py assets/resources/images/player.png")
        sys.exit(1)

    file_path = sys.argv[1]

    # Check if file exists
    if not os.path.isfile(file_path):
        print(f"Error: File not found: {file_path}")
        sys.exit(1)

    # Check if .meta file exists
    meta_path = file_path + '.meta'
    if not os.path.isfile(meta_path):
        print(f"Warning: No .meta file found at {meta_path}")
        print("This file may not be a tracked asset.")

        # Ask for confirmation to delete anyway
        response = input("Delete anyway? (y/N): ").strip().lower()
        if response != 'y':
            print("Deletion cancelled.")
            sys.exit(0)

        # Delete just the file
        try:
            os.remove(file_path)
            print(f"Deleted: {file_path}")
        except Exception as e:
            print(f"Failed to delete {file_path}: {e}")
            sys.exit(1)
        sys.exit(0)

    # Read GUID from .meta file
    guid = read_meta_file(meta_path)
    if not guid:
        print(f"Warning: Could not read GUID from {meta_path}")
        print("Proceeding with deletion anyway...")
    else:
        print(f"Asset GUID: {guid}")
        print("Searching for references...")

        # Search for references
        references = find_guid_references(guid)

        if references:
            print(f"\n⚠️  WARNING: Found {len(references)} reference(s) to this asset:")
            print()
            for ref_path, line_num, line_content in references:
                print(f"  {ref_path}:{line_num}")
                print(f"    {line_content}")
                print()

            print("Deleting this asset will break these references!")
            response = input("Continue with deletion? (y/N): ").strip().lower()
            if response != 'y':
                print("Deletion cancelled.")
                sys.exit(0)
        else:
            print("✓ No references found. Safe to delete.")

    # Proceed with deletion
    print()
    deleted, failed = delete_file_and_meta(file_path)

    print()
    if deleted:
        print(f"Successfully deleted {len(deleted)} file(s).")
    if failed:
        print(f"Failed to delete {len(failed)} file(s).")
        sys.exit(1)


if __name__ == "__main__":
    main()
