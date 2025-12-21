"""
Shared utilities for asset metadata management.

This module contains common functions used by both import.py and asset_server.py
to avoid code duplication.
"""

# Mapping of file extensions to asset types
ASSET_TYPE_MAPPING = {
    ".ttf": "font",
    ".png": "image",
    ".jpg": "image",
    ".vert": "vertex_shader",
    ".frag": "fragment_shader",
    ".scene": "scene",
    ".prefab": "prefab",
    ".mat": "material",
    ".css": "css",
    ".rml": "rml",
    ".upage": "ui_page",
    ".fsm": "fsm",
}

# Asset types that can contain GUID references to other assets
ASSET_TYPES_WITH_REFERENCES = {
    "scene",
    "prefab",
    "material",
    "ui_page",
    "fsm",
    "rml",
    "css",
}


def dict_to_yaml(data, indent=0):
    """
    Convert a Python dict to a simple YAML string (supports nested dicts and lists).

    Args:
        data (dict): The data to convert.
        indent (int): Current indentation level (used internally).

    Returns:
        str: YAML-formatted string.
    """
    yaml_lines = []
    for key, value in data.items():
        prefix = '  ' * indent + str(key) + ':'
        if isinstance(value, dict):
            yaml_lines.append(prefix)
            yaml_lines.append(dict_to_yaml(value, indent + 1))
        elif isinstance(value, list):
            yaml_lines.append(prefix)
            for item in value:
                if isinstance(item, dict):
                    yaml_lines.append('  ' * (indent + 1) + '-')
                    yaml_lines.append(dict_to_yaml(item, indent + 2))
                else:
                    yaml_lines.append('  ' * (indent + 1) + '- ' + str(item))
        else:
            yaml_lines.append(prefix + ' ' + str(value))
    return '\n'.join(yaml_lines)


def get_asset_extensions_with_references():
    """
    Get list of asset file extensions that can contain GUID references.

    Returns:
        list: List of file extensions (e.g., ['.scene', '.prefab', '.mat'])
    """
    return [ext for ext, asset_type in ASSET_TYPE_MAPPING.items()
            if asset_type in ASSET_TYPES_WITH_REFERENCES]


def ext_check(value_ext):
    """
    Map file extension to asset type.

    Args:
        value_ext (str): File extension (e.g., ".png", ".ttf")

    Returns:
        str: Asset type (e.g., "image", "font") or "unknown" if not recognized
    """
    return ASSET_TYPE_MAPPING.get(value_ext, "unknown")
