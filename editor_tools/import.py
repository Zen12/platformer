"""
This code is write by chatGTP, this is not expected to be maintained
It is only to automate manual work

What it does:
 - Finds all files that doesn't have associated .meta files (similar as in unity engine)
 - create a templated version of it with custom GUID
"""

import os
import uuid



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

def ext_check(value_ext):
    mapping = {
        ".ttf": "font",
        ".png": "image",
        ".jpg": "image",
        ".vert": "vertex-shader",
        ".frag": "fragment-shader",
        ".scene": "scene",
        ".prefab": "prefab",
        ".mat": "material",
        ".atlas-spine": "atlas-spine",
        ".png-spine": "image-spine",
        ".json-spine": "json-spine",
        ".skel-spine": "skel-spine",
        ".engine-spine": "engine-spine"
    }
    return mapping.get(value_ext, "error")

"""TODO: THIS IS HARDCODED. Make it as param or dynamic from project.yaml"""
directory = 'assets/resources'

for root, dirs, files in os.walk(directory):
    # Modify dirs in-place to skip hidden folders and specific names
    dirs[:] = [d for d in dirs if not d.startswith('.') and d != '__pycache__']

    for file in files:
        base_name, ext = os.path.splitext(file)
        if file.startswith('.') or file.endswith('.meta'):
            continue

        yaml_path = os.path.join(root, base_name + ext + '.meta')
        if (os.path.isfile(yaml_path)):
            continue

        print(yaml_path);
        data = {
            'name': base_name,
            'guid': str(uuid.uuid4()),
            'extension': ext,
            'type': ext_check(ext)
        }


        with open(yaml_path, 'w') as f:
            f.write(dict_to_yaml(data))

