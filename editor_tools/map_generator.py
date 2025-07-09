import uuid

def generate_sprite_tiles_yaml(x_min, x_max, y_min, y_max):
    yaml_tiles = []

    for x in range(x_min, x_max + 1):
        for y in range(y_min, y_max + 1):
            guid = f"sprite-tile-{uuid.uuid4().hex[:8]}"
            tile = f"""
  - obj:
    - guid: {guid}
    - components:
      - component:
        - type: transform
        - data:
          - position:
            - x: {float(x)}
            - y: {float(y)}
            - z: 0.0
          - rotation:
            - x: 0.0
            - y: 0.0
            - z: 0.0
          - scale:
            - x: 1.0
            - y: 1.0
            - z: 1.0
      - component:
        - type: sprite_renderer
        - data:
          - material: f22681f4-45d1-4542-a838-99f854890c18
          - image: 3d0040e7-ee77-477b-b7d8-863fd45ac38c
      - component:
        - type: rigidbody2d
        - data:
          - isDynamic: false
      - component:
        - type: box_collider
        - data:
          - size:
            - x: 1.0
            - y: 1.0
            - z: 0.0
"""
            yaml_tiles.append(tile)

    return yaml_tiles


# Example usage
if __name__ == "__main__":
    #box
    max_x = 8;
    min_x = -8;

    max_y = 7;
    min_y = -7;

    tiles = generate_sprite_tiles_yaml(min_x, min_x, min_y, max_y)
    tiles += generate_sprite_tiles_yaml(max_x, max_x, min_y, max_y)
    tiles += generate_sprite_tiles_yaml(min_x+1, max_x - 1, min_y, min_y)
    tiles += generate_sprite_tiles_yaml(min_x+1, max_x -1, max_y, max_y)

    with open("assets/resources/scenes/main_scene.scene", "a") as f:
        for tile in tiles:
            f.write(tile)
            if not tile.endswith("\n"):
                f.write("\n")

