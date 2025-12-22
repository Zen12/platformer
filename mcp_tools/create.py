"""
Asset creation utilities.

This script creates new asset files with proper structure and metadata.

IMPORTANT DIRECTORY STRUCTURE RULE:
    All assets MUST be created under assets/resources/ directory.
    Exception: project.yaml goes in assets/ root.

    Correct structure:
        assets/
        ├── project.yaml              # Project configuration (root level)
        └── resources/                # All other assets go here
            ├── shaders/              # Shader files (.vert, .frag)
            ├── materials/            # Material files (.mat)
            ├── scenes/               # Scene files (.scene)
            ├── prefabs/              # Prefab files (.prefab)
            ├── images/               # Image files (.png, .jpg)
            ├── models/               # 3D model files (.fbx, .blend)
            ├── fonts/                # Font files (.ttf)
            └── fsm/                  # FSM files (.fsm)

Usage:
    python3 mcp/create.py project --name "My Game" --resolution 1920 1080 --fps 60
    python3 mcp/create.py project --name "My Game"  # Use defaults
    python3 mcp/create.py fsm --name menu_fsm
    python3 mcp/create.py shader --name my_shader

Environment Variables:
    ASSETS_DIR - Directory where assets will be created (default: assets/resources)
"""

import os
import sys
import uuid
from asset_utils import dict_to_yaml


def create_project_yaml(
    name="New Project",
    resolution_width=1600,
    resolution_height=1200,
    target_fps=30,
    main_fsm_guid=None,
    output_path=None
):
    """
    Create a project.yaml file with the specified configuration.

    Args:
        name (str): Project name
        resolution_width (int): Screen width in pixels
        resolution_height (int): Screen height in pixels
        target_fps (int): Target frames per second
        main_fsm_guid (str): GUID of the main FSM, or None to generate a new one
        output_path (str): Output file path, or None to use ASSETS_DIR/project.yaml

    Returns:
        str: Path to the created file
    """
    # Generate main FSM GUID if not provided
    if main_fsm_guid is None:
        main_fsm_guid = str(uuid.uuid4())

    # Determine output path
    if output_path is None:
        assets_dir = os.getenv('ASSETS_DIR', 'assets')
        output_path = os.path.join(assets_dir, 'project.yaml')

    # Create project data structure
    project_data = {
        'name': name,
        'resolution': [resolution_width, resolution_height],
        'target_fps': target_fps,
        'main_fsm': main_fsm_guid,
    }

    # Write YAML file
    yaml_content = dict_to_yaml(project_data)

    # Ensure parent directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    with open(output_path, 'w') as f:
        f.write(yaml_content)
        f.write('\n')  # Add trailing newline

    return output_path


def create_fsm(
    name="new_fsm",
    output_path=None
):
    """
    Create a new FSM file with default structure and its .meta file.

    Creates an FSM with:
    - 2 nodes (start node and second node)
    - Each node has a log action
    - 1 trigger condition
    - 1 connection between the nodes

    Args:
        name (str): FSM name (used for both FSM name and filename)
        output_path (str): Output file path, or None to use ASSETS_DIR/fsm/{name}.fsm

    Returns:
        tuple: (fsm_path, meta_path, fsm_guid)
    """
    # Determine output path
    if output_path is None:
        assets_dir = os.getenv('ASSETS_DIR', 'assets/resources')
        output_path = os.path.join(assets_dir, 'fsm', f'{name}.fsm')

    # Generate GUIDs
    fsm_guid = str(uuid.uuid4())
    start_node_guid = str(uuid.uuid4())
    second_node_guid = str(uuid.uuid4())
    condition_guid = str(uuid.uuid4())

    # Create FSM data structure
    fsm_data = {
        'name': name,
        'start_node': start_node_guid,
        'nodes': [
            {
                'name': 'Start Node',
                'guid': start_node_guid,
                'actions': [
                    {
                        'type': 'log',
                        'message': 'FSM Started'
                    }
                ]
            },
            {
                'name': 'Second Node',
                'guid': second_node_guid,
                'actions': [
                    {
                        'type': 'log',
                        'message': 'Second Node Reached'
                    }
                ]
            }
        ],
        'conditions': [
            {
                'type': 'trigger_check',
                'guid': condition_guid,
                'trigger_name': 'continue_trigger'
            }
        ],
        'connections': [
            {
                'nodes': [start_node_guid, second_node_guid],
                'conditions': [condition_guid],
                'condition_type': 0
            }
        ]
    }

    # Create metadata
    base_name = os.path.splitext(os.path.basename(output_path))[0]
    meta_data = {
        'name': base_name,
        'guid': fsm_guid,
        'extension': '.fsm',
        'type': 'fsm'
    }

    # Ensure parent directory exists
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    # Write FSM file
    yaml_content = dict_to_yaml(fsm_data)
    with open(output_path, 'w') as f:
        f.write(yaml_content)
        f.write('\n')

    # Write .meta file
    meta_path = output_path + '.meta'
    meta_content = dict_to_yaml(meta_data)
    with open(meta_path, 'w') as f:
        f.write(meta_content)
        f.write('\n')

    return output_path, meta_path, fsm_guid


def create_shader(
    name="new_shader",
    output_path=None
):
    """
    Create a new shader with OpenGL and GLES versions.

    Creates a shader folder containing:
    - gl.vert (OpenGL vertex shader)
    - gl_.frag (OpenGL fragment shader)
    - gles.vert (GLES vertex shader)
    - gles_.frag (GLES fragment shader)
    - .meta files for each shader file

    Note: Fragment shaders use underscore for alphabetical ordering (vert before frag).
    Each shader file has its own unique GUID.

    Args:
        name (str): Shader name (used for folder name)
        output_path (str): Output folder path, or None to use ASSETS_DIR/shaders/{name}

    Returns:
        tuple: (folder_path, gl_vert_guid, gl_frag_guid, gles_vert_guid, gles_frag_guid, files_created)
    """
    # Determine output path
    if output_path is None:
        assets_dir = os.getenv('ASSETS_DIR', 'assets/resources')
        output_path = os.path.join(assets_dir, 'shaders', name)

    # Generate unique GUIDs for each shader file
    gl_vert_guid = str(uuid.uuid4())
    gl_frag_guid = str(uuid.uuid4())
    gles_vert_guid = str(uuid.uuid4())
    gles_frag_guid = str(uuid.uuid4())

    # Ensure folder exists
    os.makedirs(output_path, exist_ok=True)

    # OpenGL vertex shader
    gl_vert_content = """#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
"""

    # OpenGL fragment shader
    gl_frag_content = """#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
"""

    # GLES vertex shader
    gles_vert_content = """#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
"""

    # GLES fragment shader
    gles_frag_content = """#version 300 es
precision mediump float;

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
"""

    # Write shader files and metadata
    files_created = []

    # GL vertex shader
    gl_vert_path = os.path.join(output_path, 'gl.vert')
    with open(gl_vert_path, 'w') as f:
        f.write(gl_vert_content)
    files_created.append(gl_vert_path)

    gl_vert_meta = {
        'name': name,
        'guid': gl_vert_guid,
        'extension': '.vert',
        'type': 'vertex_shader'
    }
    with open(gl_vert_path + '.meta', 'w') as f:
        f.write(dict_to_yaml(gl_vert_meta))
        f.write('\n')
    files_created.append(gl_vert_path + '.meta')

    # GL fragment shader
    gl_frag_path = os.path.join(output_path, 'gl_.frag')
    with open(gl_frag_path, 'w') as f:
        f.write(gl_frag_content)
    files_created.append(gl_frag_path)

    gl_frag_meta = {
        'name': name,
        'guid': gl_frag_guid,
        'extension': '.frag',
        'type': 'fragment_shader'
    }
    with open(gl_frag_path + '.meta', 'w') as f:
        f.write(dict_to_yaml(gl_frag_meta))
        f.write('\n')
    files_created.append(gl_frag_path + '.meta')

    # GLES vertex shader
    gles_vert_path = os.path.join(output_path, 'gles.vert')
    with open(gles_vert_path, 'w') as f:
        f.write(gles_vert_content)
    files_created.append(gles_vert_path)

    gles_vert_meta = {
        'name': name,
        'guid': gles_vert_guid,
        'extension': '.vert',
        'type': 'vertex_shader'
    }
    with open(gles_vert_path + '.meta', 'w') as f:
        f.write(dict_to_yaml(gles_vert_meta))
        f.write('\n')
    files_created.append(gles_vert_path + '.meta')

    # GLES fragment shader
    gles_frag_path = os.path.join(output_path, 'gles_.frag')
    with open(gles_frag_path, 'w') as f:
        f.write(gles_frag_content)
    files_created.append(gles_frag_path)

    gles_frag_meta = {
        'name': name,
        'guid': gles_frag_guid,
        'extension': '.frag',
        'type': 'fragment_shader'
    }
    with open(gles_frag_path + '.meta', 'w') as f:
        f.write(dict_to_yaml(gles_frag_meta))
        f.write('\n')
    files_created.append(gles_frag_path + '.meta')

    return output_path, gl_vert_guid, gl_frag_guid, gles_vert_guid, gles_frag_guid, files_created


def create_material(
    name="new_material",
    opengl_vertex_guid=None,
    opengl_fragment_guid=None,
    gles_vertex_guid=None,
    gles_fragment_guid=None,
    image_guid=None,
    font_guid=None,
    blend_mode=0,
    is_culling=False,
    output_path=None
):
    """
    Create or update a material file (.mat) with shader references.

    Args:
        name (str): Material name
        opengl_vertex_guid (str): GUID of OpenGL vertex shader
        opengl_fragment_guid (str): GUID of OpenGL fragment shader
        gles_vertex_guid (str): GUID of GLES vertex shader
        gles_fragment_guid (str): GUID of GLES fragment shader
        image_guid (str): Optional GUID of texture image
        font_guid (str): Optional GUID of font
        blend_mode (int): Blend mode (0=none, 1=alpha, etc.)
        is_culling (bool): Enable face culling
        output_path (str): Output file path, or None to use ASSETS_DIR/materials/{name}.mat

    Returns:
        tuple: (mat_path, meta_path, mat_guid)
    """
    # Determine output path
    if output_path is None:
        assets_dir = os.getenv('ASSETS_DIR', 'assets/resources')
        materials_dir = os.path.join(assets_dir, 'materials')
        os.makedirs(materials_dir, exist_ok=True)
        output_path = os.path.join(materials_dir, f"{name}.mat")

    # Check if material already exists to reuse GUID
    mat_guid = None
    meta_path = output_path + '.meta'
    if os.path.exists(meta_path):
        # Read existing GUID from meta file
        try:
            import yaml
            with open(meta_path, 'r') as f:
                meta_content = yaml.safe_load(f)
                if meta_content and 'guid' in meta_content:
                    mat_guid = meta_content['guid']
        except:
            pass

    # Generate new GUID if not found
    if mat_guid is None:
        mat_guid = str(uuid.uuid4())

    # Build material data
    material_data = {
        'name': name,
        'is_culling': is_culling,
        'blend_mode': blend_mode
    }

    # Add shader references
    if opengl_vertex_guid and opengl_fragment_guid:
        material_data['shader_opengl'] = {
            'vertex': opengl_vertex_guid,
            'fragment': opengl_fragment_guid
        }

    if gles_vertex_guid and gles_fragment_guid:
        material_data['shader_gles'] = {
            'vertex': gles_vertex_guid,
            'fragment': gles_fragment_guid
        }

    # Add optional references
    if image_guid:
        material_data['image'] = image_guid

    if font_guid:
        material_data['font'] = font_guid

    # Write material file
    yaml_content = dict_to_yaml(material_data)
    with open(output_path, 'w') as f:
        f.write(yaml_content)
        f.write('\n')

    # Create/update metadata
    meta_data = {
        'name': name,
        'guid': mat_guid,
        'extension': '.mat',
        'type': 'material'
    }

    meta_content = dict_to_yaml(meta_data)
    with open(meta_path, 'w') as f:
        f.write(meta_content)
        f.write('\n')

    return output_path, meta_path, mat_guid


def main():
    """Command-line interface for asset creation."""
    if len(sys.argv) < 2:
        print("Usage: python3 mcp/create.py <asset_type> [options]")
        print()
        print("Asset types:")
        print("  project    - Create project.yaml file")
        print("  fsm        - Create FSM file with default structure")
        print("  shader     - Create shader folder with GL and GLES versions")
        print()
        print("Examples:")
        print("  python3 mcp/create.py project --name 'My Game' --resolution 1920 1080 --fps 60")
        print("  python3 mcp/create.py fsm --name my_menu")
        print("  python3 mcp/create.py shader --name my_shader")
        sys.exit(1)

    asset_type = sys.argv[1]

    if asset_type == "project":
        # Parse command-line arguments
        name = "New Project"
        resolution_width = 1600
        resolution_height = 1200
        target_fps = 30
        main_fsm_guid = None
        output_path = None

        i = 2
        while i < len(sys.argv):
            arg = sys.argv[i]

            if arg == '--name':
                if i + 1 < len(sys.argv):
                    name = sys.argv[i + 1]
                    i += 2
                else:
                    print("Error: --name requires a value")
                    sys.exit(1)

            elif arg == '--resolution':
                if i + 2 < len(sys.argv):
                    try:
                        resolution_width = int(sys.argv[i + 1])
                        resolution_height = int(sys.argv[i + 2])
                        i += 3
                    except ValueError:
                        print("Error: --resolution requires two integer values")
                        sys.exit(1)
                else:
                    print("Error: --resolution requires two values (width height)")
                    sys.exit(1)

            elif arg == '--fps':
                if i + 1 < len(sys.argv):
                    try:
                        target_fps = int(sys.argv[i + 1])
                        i += 2
                    except ValueError:
                        print("Error: --fps requires an integer value")
                        sys.exit(1)
                else:
                    print("Error: --fps requires a value")
                    sys.exit(1)

            elif arg == '--main-fsm':
                if i + 1 < len(sys.argv):
                    main_fsm_guid = sys.argv[i + 1]
                    i += 2
                else:
                    print("Error: --main-fsm requires a GUID value")
                    sys.exit(1)

            elif arg == '--output':
                if i + 1 < len(sys.argv):
                    output_path = sys.argv[i + 1]
                    i += 2
                else:
                    print("Error: --output requires a path")
                    sys.exit(1)

            else:
                print(f"Error: Unknown argument '{arg}'")
                sys.exit(1)

        # Create project.yaml
        output = create_project_yaml(
            name=name,
            resolution_width=resolution_width,
            resolution_height=resolution_height,
            target_fps=target_fps,
            main_fsm_guid=main_fsm_guid,
            output_path=output_path
        )

        print(f"✓ Created project file: {output}")
        print()
        print("Project configuration:")
        print(f"  Name: {name}")
        print(f"  Resolution: {resolution_width}x{resolution_height}")
        print(f"  Target FPS: {target_fps}")
        if main_fsm_guid:
            print(f"  Main FSM: {main_fsm_guid}")

    elif asset_type == "fsm":
        # Parse command-line arguments
        name = "new_fsm"
        output_path = None

        i = 2
        while i < len(sys.argv):
            arg = sys.argv[i]

            if arg == '--name':
                if i + 1 < len(sys.argv):
                    name = sys.argv[i + 1]
                    i += 2
                else:
                    print("Error: --name requires a value")
                    sys.exit(1)

            elif arg == '--output':
                if i + 1 < len(sys.argv):
                    output_path = sys.argv[i + 1]
                    i += 2
                else:
                    print("Error: --output requires a path")
                    sys.exit(1)

            else:
                print(f"Error: Unknown argument '{arg}'")
                sys.exit(1)

        # Create FSM
        fsm_path, meta_path, fsm_guid = create_fsm(
            name=name,
            output_path=output_path
        )

        print(f"✓ Created FSM file: {fsm_path}")
        print(f"✓ Created metadata: {meta_path}")
        print()
        print("FSM configuration:")
        print(f"  Name: {name}")
        print(f"  GUID: {fsm_guid}")
        print(f"  Nodes: 2 (Start Node, Second Node)")
        print(f"  Conditions: 1 (trigger_check: continue_trigger)")
        print(f"  Connections: 1")

    elif asset_type == "shader":
        # Parse command-line arguments
        name = "new_shader"
        output_path = None

        i = 2
        while i < len(sys.argv):
            arg = sys.argv[i]

            if arg == '--name':
                if i + 1 < len(sys.argv):
                    name = sys.argv[i + 1]
                    i += 2
                else:
                    print("Error: --name requires a value")
                    sys.exit(1)

            elif arg == '--output':
                if i + 1 < len(sys.argv):
                    output_path = sys.argv[i + 1]
                    i += 2
                else:
                    print("Error: --output requires a path")
                    sys.exit(1)

            else:
                print(f"Error: Unknown argument '{arg}'")
                sys.exit(1)

        # Create shader
        folder_path, gl_vert_guid, gl_frag_guid, gles_vert_guid, gles_frag_guid, files = create_shader(
            name=name,
            output_path=output_path
        )

        print(f"✓ Created shader folder: {folder_path}")
        print()
        print("Files created:")
        for file in files:
            print(f"  - {file}")
        print()
        print("Shader configuration:")
        print(f"  Name: {name}")
        print(f"  GL Vertex Shader GUID: {gl_vert_guid}")
        print(f"  GL Fragment Shader GUID: {gl_frag_guid}")
        print(f"  GLES Vertex Shader GUID: {gles_vert_guid}")
        print(f"  GLES Fragment Shader GUID: {gles_frag_guid}")

    else:
        print(f"Error: Unknown asset type '{asset_type}'")
        print("Supported types: project, fsm, shader")
        sys.exit(1)


if __name__ == "__main__":
    main()