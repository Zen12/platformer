import uuid

# Prepare all lines to write
yaml_tiles = []

for x in range(-10, 11):
    for y in range(-10, 11):
        guid = f"sprite-tile-{uuid.uuid4().hex[:8]}"
        tile = f"""- obj:
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

# Save to file
with open("tiles_generated.yaml", "w") as f:
    f.writelines(yaml_tiles)
