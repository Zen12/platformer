"""
Figma-to-RML conversion pipeline.

Fetches a Figma file via the REST API and generates RmlUi-compatible
.rml, .css, .upage, and .meta files ready for the game engine.

Usage:
    python3 mcp_tools/figma_to_rml.py list --file-key <KEY>
    python3 mcp_tools/figma_to_rml.py preview --file-key <KEY> --page-name main_menu --frame-name "Main Menu"
    python3 mcp_tools/figma_to_rml.py convert --file-key <KEY> --page-name main_menu --frame-name "Main Menu"

Environment Variables:
    FIGMA_TOKEN  - Figma personal access token
    ASSETS_DIR   - Asset output directory (default: assets/resources)
"""

import json
import os
import sys
import time
import uuid
import urllib.request
import urllib.error
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple

sys.path.insert(0, os.path.dirname(__file__))

from asset_utils import dict_to_yaml, ext_check
from figma_config import (
    FigmaConfig, load_config, resolve_tag_and_id, to_kebab_case,
    DEFAULT_MAPPINGS,
)


# ---------------------------------------------------------------------------
# Figma API client
# ---------------------------------------------------------------------------

class FigmaClient:
    """Minimal Figma REST API client using stdlib only."""

    BASE_URL = "https://api.figma.com"

    def __init__(self, token: Optional[str] = None):
        self._token = token or os.getenv("FIGMA_TOKEN", "")
        if not self._token:
            raise ValueError(
                "Figma token required. Set FIGMA_TOKEN env var or pass token param."
            )

    def _request(self, path: str, retries: int = 4) -> dict:
        url = f"{self.BASE_URL}{path}"
        req = urllib.request.Request(url, headers={
            "X-Figma-Token": self._token,
        })
        for attempt in range(retries + 1):
            try:
                with urllib.request.urlopen(req, timeout=60) as resp:
                    return json.loads(resp.read().decode("utf-8"))
            except urllib.error.HTTPError as e:
                if e.code == 429 and attempt < retries:
                    # Exponential backoff: 10s, 30s, 60s, 120s
                    wait = 10 * (3 ** attempt)
                    print(f"Rate limited, waiting {wait}s (attempt {attempt + 1}/{retries})...",
                          file=sys.stderr)
                    time.sleep(wait)
                    continue
                if e.code == 403:
                    raise ValueError("Invalid Figma token (403 Forbidden)") from e
                if e.code == 404:
                    raise ValueError(f"Figma file not found (404): {path}") from e
                raise
        raise RuntimeError("Figma API request failed after retries")

    def get_file(self, file_key: str) -> dict:
        return self._request(f"/v1/files/{file_key}")

    def get_images(self, file_key: str, node_ids: List[str],
                   fmt: str = "png", scale: float = 2.0) -> dict:
        ids_param = ",".join(node_ids)
        return self._request(
            f"/v1/images/{file_key}?ids={ids_param}&format={fmt}&scale={scale}"
        )


# ---------------------------------------------------------------------------
# Figma node tree
# ---------------------------------------------------------------------------

@dataclass
class FigmaNode:
    """Parsed representation of a Figma document node."""

    id: str = ""
    name: str = ""
    node_type: str = ""
    visible: bool = True
    children: List["FigmaNode"] = field(default_factory=list)

    # Bounding box
    x: float = 0.0
    y: float = 0.0
    width: float = 0.0
    height: float = 0.0

    # Style
    fills: list = field(default_factory=list)
    strokes: list = field(default_factory=list)
    stroke_weight: float = 0.0
    corner_radius: float = 0.0
    opacity: float = 1.0
    effects: list = field(default_factory=list)

    # Layout (auto-layout)
    layout_mode: str = ""          # "VERTICAL", "HORIZONTAL", or ""
    padding_top: float = 0.0
    padding_right: float = 0.0
    padding_bottom: float = 0.0
    padding_left: float = 0.0
    item_spacing: float = 0.0
    primary_axis_align: str = ""
    counter_axis_align: str = ""

    # Sizing
    layout_sizing_horizontal: str = ""  # "FIXED", "HUG", "FILL"
    layout_sizing_vertical: str = ""

    # Text
    characters: str = ""
    font_family: str = ""
    font_size: float = 0.0
    font_weight: int = 400
    text_align_horizontal: str = ""
    text_align_vertical: str = ""
    line_height: float = 0.0
    letter_spacing: float = 0.0


def _parse_node(data: dict) -> FigmaNode:
    """Recursively parse a Figma JSON node into a FigmaNode tree."""
    node = FigmaNode(
        id=data.get("id", ""),
        name=data.get("name", ""),
        node_type=data.get("type", ""),
        visible=data.get("visible", True),
    )

    bbox = data.get("absoluteBoundingBox", {})
    node.x = bbox.get("x", 0.0)
    node.y = bbox.get("y", 0.0)
    node.width = bbox.get("width", 0.0)
    node.height = bbox.get("height", 0.0)

    node.fills = data.get("fills", [])
    node.strokes = data.get("strokes", [])
    node.stroke_weight = data.get("strokeWeight", 0.0)
    node.corner_radius = data.get("cornerRadius", 0.0)
    node.opacity = data.get("opacity", 1.0)
    node.effects = data.get("effects", [])

    node.layout_mode = data.get("layoutMode", "")
    node.padding_top = data.get("paddingTop", 0.0)
    node.padding_right = data.get("paddingRight", 0.0)
    node.padding_bottom = data.get("paddingBottom", 0.0)
    node.padding_left = data.get("paddingLeft", 0.0)
    node.item_spacing = data.get("itemSpacing", 0.0)
    node.primary_axis_align = data.get("primaryAxisAlignItems", "")
    node.counter_axis_align = data.get("counterAxisAlignItems", "")
    node.layout_sizing_horizontal = data.get("layoutSizingHorizontal", "")
    node.layout_sizing_vertical = data.get("layoutSizingVertical", "")

    style = data.get("style", {})
    node.characters = data.get("characters", "")
    node.font_family = style.get("fontFamily", "")
    node.font_size = style.get("fontSize", 0.0)
    node.font_weight = style.get("fontWeight", 400)
    node.text_align_horizontal = style.get("textAlignHorizontal", "")
    node.text_align_vertical = style.get("textAlignVertical", "")
    node.line_height = style.get("lineHeightPx", 0.0)
    node.letter_spacing = style.get("letterSpacing", 0.0)

    for child_data in data.get("children", []):
        node.children.append(_parse_node(child_data))

    return node


def parse_figma_tree(file_data: dict) -> FigmaNode:
    """Parse the top-level Figma file JSON into a FigmaNode tree."""
    document = file_data.get("document", {})
    return _parse_node(document)


def find_frame(root: FigmaNode, frame_name: str) -> Optional[FigmaNode]:
    """Find a frame or page by name anywhere in the node tree."""
    if root.name == frame_name and root.node_type in (
        "FRAME", "COMPONENT", "COMPONENT_SET", "CANVAS"
    ):
        return root
    for child in root.children:
        result = find_frame(child, frame_name)
        if result:
            return result
    return None


def list_frames(root: FigmaNode) -> List[Tuple[str, str]]:
    """List all pages and top-level frames. Returns (page_name, frame_name) pairs."""
    frames = []
    # Root is DOCUMENT, children are pages (CANVAS)
    for page in root.children:
        if page.node_type == "CANVAS":
            # Include the page itself as a convertible target
            frames.append((page.name, page.name + " (entire page)"))
            for child in page.children:
                if child.node_type in ("FRAME", "COMPONENT", "COMPONENT_SET"):
                    frames.append((page.name, child.name))
    return frames


# ---------------------------------------------------------------------------
# Color helpers
# ---------------------------------------------------------------------------

def _rgba_to_hex(color: dict) -> str:
    """Convert Figma RGBA (0-1 floats) to #RRGGBB hex string."""
    r = int(color.get("r", 0) * 255)
    g = int(color.get("g", 0) * 255)
    b = int(color.get("b", 0) * 255)
    return f"#{r:02x}{g:02x}{b:02x}"


def _rgba_to_hex_alpha(color: dict) -> Tuple[str, float]:
    """Convert Figma RGBA to (#RRGGBB, alpha) tuple."""
    hex_color = _rgba_to_hex(color)
    alpha = color.get("a", 1.0)
    return hex_color, alpha


def _lighten_hex(hex_color: str, percent: float) -> str:
    """Lighten a hex color by a percentage (0-100)."""
    r = int(hex_color[1:3], 16)
    g = int(hex_color[3:5], 16)
    b = int(hex_color[5:7], 16)
    factor = percent / 100.0
    r = min(255, int(r + (255 - r) * factor))
    g = min(255, int(g + (255 - g) * factor))
    b = min(255, int(b + (255 - b) * factor))
    return f"#{r:02x}{g:02x}{b:02x}"


def _darken_hex(hex_color: str, percent: float) -> str:
    """Darken a hex color by a percentage (0-100)."""
    r = int(hex_color[1:3], 16)
    g = int(hex_color[3:5], 16)
    b = int(hex_color[5:7], 16)
    factor = 1.0 - percent / 100.0
    r = max(0, int(r * factor))
    g = max(0, int(g * factor))
    b = max(0, int(b * factor))
    return f"#{r:02x}{g:02x}{b:02x}"


def _get_solid_fill_color(fills: list) -> Optional[str]:
    """Extract the first visible solid fill color as hex."""
    for fill in fills:
        if fill.get("type") == "SOLID" and fill.get("visible", True):
            return _rgba_to_hex(fill.get("color", {}))
    return None


# ---------------------------------------------------------------------------
# CSS generator
# ---------------------------------------------------------------------------

class CssGenerator:
    """Generates RmlUi CSS from a FigmaNode tree."""

    def __init__(self, frame: FigmaNode, config: FigmaConfig):
        self._frame = frame
        self._config = config
        self._rules: List[str] = []
        self._selector_counts: Dict[str, int] = {}

    def generate(self) -> str:
        """Generate complete CSS content."""
        # Body rule
        self._rules.append(
            "body {\n"
            "    width: 100%;\n"
            "    height: 100%;\n"
            "    font-family: Arial;\n"
            "}\n"
        )

        self._process_node(self._frame, is_root=True)
        return "\n".join(self._rules)

    def _find_text_child(self, node: FigmaNode) -> Optional[FigmaNode]:
        """Find the first TEXT child node (for extracting text style from buttons)."""
        if node.node_type != "TEXT":
            for child in node.children:
                if child.node_type == "TEXT":
                    return child
        return None

    def _is_template_element(self, node: FigmaNode) -> bool:
        """Check if a node maps to a template element."""
        _, _, tmpl = resolve_tag_and_id(
            node.name, node.node_type, node.font_size, self._config.mappings
        )
        return tmpl is not None

    def _make_selector(self, tag: str, element_id: Optional[str]) -> str:
        if element_id:
            return f"#{element_id}"
        return tag

    def _process_node(self, node: FigmaNode, is_root: bool = False,
                      parent: Optional[FigmaNode] = None,
                      child_index: int = 0, sibling_count: int = 0):
        if not node.visible:
            return

        if is_root:
            # Process children of root frame only
            for i, child in enumerate(node.children):
                self._process_node(child, parent=node,
                                   child_index=i, sibling_count=len(node.children))
            return

        # Skip RECTANGLE and TEXT children inside COMPONENT/INSTANCE parents
        # (background shapes and labels are handled by the parent element's styling)
        if parent and parent.node_type in ("COMPONENT", "INSTANCE"):
            if node.node_type in ("RECTANGLE", "TEXT"):
                return

        tag, element_id, _ = resolve_tag_and_id(
            node.name, node.node_type, node.font_size, self._config.mappings
        )
        selector = self._make_selector(tag, element_id)
        is_button = tag in ("button", "menu_button")

        props = []

        # Position and size
        if parent and not parent.layout_mode:
            # Absolute positioning within parent
            rel_x = node.x - parent.x
            rel_y = node.y - parent.y

            # Centering detection: if child is horizontally centered (within 5dp)
            parent_center = parent.width / 2.0
            child_center = rel_x + node.width / 2.0
            if abs(parent_center - child_center) < 5.0:
                props.append("position: absolute")
                props.append("left: 50%")
                props.append(f"margin-left: -{node.width / 2.0:.0f}dp")
            else:
                props.append("position: absolute")
                props.append(f"left: {rel_x:.0f}dp")

            props.append(f"top: {rel_y:.0f}dp")

        # Width and height
        if node.layout_sizing_horizontal == "FILL":
            props.append("width: 100%")
        elif node.width > 0:
            props.append(f"width: {node.width:.0f}dp")

        if node.layout_sizing_vertical == "FILL":
            props.append("height: 100%")
        elif node.height > 0 and node.node_type != "TEXT":
            props.append(f"height: {node.height:.0f}dp")

        # Background color (for non-text nodes)
        if node.node_type != "TEXT":
            bg_color = _get_solid_fill_color(node.fills)
            if bg_color:
                props.append(f"background-color: {bg_color}")

        # Text color and font properties
        text_node = node if node.node_type == "TEXT" else self._find_text_child(node)
        if text_node:
            text_color = _get_solid_fill_color(text_node.fills)
            if text_color:
                props.append(f"color: {text_color}")
            if text_node.font_size > 0:
                props.append(f"font-size: {text_node.font_size:.0f}dp")
            if text_node.font_weight >= 700:
                props.append("font-weight: bold")
            if text_node.text_align_horizontal:
                align = text_node.text_align_horizontal.lower()
                if align in ("left", "center", "right", "justified"):
                    if align == "justified":
                        align = "justify"
                    props.append(f"text-align: {align}")

        # Border
        if node.strokes and node.stroke_weight > 0:
            stroke_color = _get_solid_fill_color(node.strokes)
            if stroke_color:
                props.append(f"border-width: {node.stroke_weight:.0f}dp")
                props.append(f"border-color: {stroke_color}")

        # Border radius
        if node.corner_radius > 0:
            props.append(f"border-radius: {node.corner_radius:.0f}dp")

        # Ellipse special case
        if node.node_type == "ELLIPSE":
            props.append("border-radius: 50%")

        # Padding (from auto-layout)
        if node.layout_mode:
            pad_parts = []
            pt = node.padding_top
            pr = node.padding_right
            pb = node.padding_bottom
            pl = node.padding_left
            if pt == pr == pb == pl and pt > 0:
                props.append(f"padding: {pt:.0f}dp")
            elif pt > 0 or pr > 0 or pb > 0 or pl > 0:
                props.append(f"padding: {pt:.0f}dp {pr:.0f}dp {pb:.0f}dp {pl:.0f}dp")

        # Opacity
        if node.opacity < 1.0:
            props.append(f"opacity: {node.opacity:.2f}")

        # Spacing for auto-layout children
        if parent and parent.layout_mode and parent.item_spacing > 0:
            is_last = (child_index == sibling_count - 1)
            if not is_last:
                if parent.layout_mode == "VERTICAL":
                    props.append(f"margin-bottom: {parent.item_spacing:.0f}dp")
                elif parent.layout_mode == "HORIZONTAL":
                    props.append(f"margin-right: {parent.item_spacing:.0f}dp")

        # Unsupported effects as comments
        skipped = []
        for effect in node.effects:
            effect_type = effect.get("type", "")
            if effect_type in ("DROP_SHADOW", "INNER_SHADOW", "LAYER_BLUR", "BACKGROUND_BLUR"):
                skipped.append(effect_type)

        # Emit rule
        if props:
            rule_lines = [f"{selector} {{"]
            for prop in props:
                rule_lines.append(f"    {prop};")
            if skipped:
                rule_lines.append(f"    /* Unsupported Figma effects: {', '.join(skipped)} */")
            rule_lines.append("}")
            self._rules.append("\n".join(rule_lines))
        elif skipped:
            self._rules.append(
                f"{selector} {{\n"
                f"    /* Unsupported Figma effects: {', '.join(skipped)} */\n"
                f"}}"
            )

        # Auto-generate hover/active for button-like elements
        if is_button:
            bg_color = _get_solid_fill_color(node.fills)
            if bg_color:
                hover_color = _lighten_hex(bg_color, 12)
                active_color = _darken_hex(bg_color, 10)
                self._rules.append(
                    f"{selector}:hover {{\n"
                    f"    background-color: {hover_color};\n"
                    f"}}"
                )
                self._rules.append(
                    f"{selector}:active {{\n"
                    f"    background-color: {active_color};\n"
                    f"}}"
                )

        # Skip children of template elements (text is inlined as content)
        if is_button and self._is_template_element(node):
            return

        # Process children
        for i, child in enumerate(node.children):
            self._process_node(child, parent=node,
                               child_index=i, sibling_count=len(node.children))


# ---------------------------------------------------------------------------
# RML generator
# ---------------------------------------------------------------------------

class RmlGenerator:
    """Generates RmlUi RML markup from a FigmaNode tree."""

    def __init__(self, frame: FigmaNode, config: FigmaConfig):
        self._frame = frame
        self._config = config
        self._template_guids: List[str] = []

    @property
    def template_guids(self) -> List[str]:
        return list(self._template_guids)

    def generate(self) -> str:
        """Generate complete RML content."""
        # Collect template GUIDs first
        self._collect_templates(self._frame)

        head_lines = []
        for guid in self._template_guids:
            head_lines.append(
                f'    <link type="text/template" href="guid|{guid}"/>'
            )
        # RmlUi requires font-family set via inline style for font loading
        head_lines.append(
            '    <style>\n'
            '        body {\n'
            '            font-family: Arial;\n'
            '        }\n'
            '    </style>'
        )

        body_content = self._render_children(self._frame, indent=1)

        head_section = "\n".join(head_lines)

        return (
            f"<rml>\n"
            f"<head>\n{head_section}\n</head>\n"
            f"<body>\n"
            f"{body_content}\n"
            f"</body>\n"
            f"</rml>"
        )

    def _collect_templates(self, node: FigmaNode):
        """Recursively collect all template GUIDs needed."""
        _, _, template_guid = resolve_tag_and_id(
            node.name, node.node_type, node.font_size, self._config.mappings
        )
        if template_guid and template_guid not in self._template_guids:
            self._template_guids.append(template_guid)
        for child in node.children:
            self._collect_templates(child)

    def _render_node(self, node: FigmaNode, indent: int,
                     parent: Optional[FigmaNode] = None) -> str:
        if not node.visible:
            return ""

        # Skip RECTANGLE children inside COMPONENT parents (background shapes)
        if node.node_type == "RECTANGLE" and parent and parent.node_type in ("COMPONENT", "INSTANCE"):
            return ""

        tag, element_id, template_guid = resolve_tag_and_id(
            node.name, node.node_type, node.font_size, self._config.mappings
        )

        prefix = "    " * indent
        id_attr = f' id="{element_id}"' if element_id else ""

        # TEXT nodes: emit tag with text content, no children
        if node.node_type == "TEXT":
            text = node.characters.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
            return f"{prefix}<{tag}{id_attr}>{text}</{tag}>"

        # Self-closing tags
        if tag in ("hr", "img", "input", "progress"):
            return f"{prefix}<{tag}{id_attr}/>"

        # Template elements (e.g., menu_button): extract text from children as content
        if template_guid:
            text = self._extract_text(node)
            if text:
                return f"{prefix}<{tag}{id_attr}>{text}</{tag}>"
            return f"{prefix}<{tag}{id_attr}></{tag}>"

        # Button-like elements: extract text as inline content
        if tag in ("button", "menu_button"):
            text = self._extract_text(node)
            if text:
                return f"{prefix}<{tag}{id_attr}>{text}</{tag}>"
            return f"{prefix}<{tag}{id_attr}></{tag}>"

        # Container nodes
        children_rml = self._render_children(node, indent + 1)
        if children_rml:
            return (
                f"{prefix}<{tag}{id_attr}>\n"
                f"{children_rml}\n"
                f"{prefix}</{tag}>"
            )
        else:
            return f"{prefix}<{tag}{id_attr}></{tag}>"

    def _extract_text(self, node: FigmaNode) -> str:
        """Recursively extract text content from TEXT children."""
        if node.node_type == "TEXT" and node.characters:
            return node.characters.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
        for child in node.children:
            text = self._extract_text(child)
            if text:
                return text
        return ""

    def _render_children(self, parent: FigmaNode, indent: int) -> str:
        parts = []
        for child in parent.children:
            rendered = self._render_node(child, indent, parent=parent)
            if rendered:
                parts.append(rendered)
        return "\n".join(parts)


# ---------------------------------------------------------------------------
# Asset writer
# ---------------------------------------------------------------------------

class AssetWriter:
    """Writes generated RML/CSS/UPAGE files with metadata."""

    def __init__(self, config: FigmaConfig):
        self._config = config

    def write(self, page_name: str, rml_content: str, css_content: str) -> dict:
        """Write all asset files for a UI page.

        Args:
            page_name: Name for the UI page (used as directory and file name).
            rml_content: Generated RML markup.
            css_content: Generated CSS styles.

        Returns:
            Dict with keys: dir, rml_path, css_path, upage_path, rml_guid, css_guid, upage_guid
        """
        output_dir = os.path.join(self._config.assets_dir, "ui", page_name)
        os.makedirs(output_dir, exist_ok=True)

        rml_guid = str(uuid.uuid4())
        css_guid = str(uuid.uuid4())
        upage_guid = str(uuid.uuid4())

        # Write .rml
        rml_path = os.path.join(output_dir, f"{page_name}.rml")
        with open(rml_path, "w") as f:
            f.write(rml_content)

        rml_meta = {
            "name": page_name,
            "guid": rml_guid,
            "extension": ".rml",
            "type": "rml",
        }
        with open(rml_path + ".meta", "w") as f:
            f.write(dict_to_yaml(rml_meta))
            f.write("\n")

        # Write .css
        css_path = os.path.join(output_dir, f"{page_name}.css")
        with open(css_path, "w") as f:
            f.write(css_content)

        css_meta = {
            "name": page_name,
            "guid": css_guid,
            "extension": ".css",
            "type": "css",
        }
        with open(css_path + ".meta", "w") as f:
            f.write(dict_to_yaml(css_meta))
            f.write("\n")

        # Write .upage
        upage_content = (
            f"rml: {rml_guid}\n"
            f"css: {css_guid}\n"
            f"material: {self._config.default_material_guid}\n"
            f"font: {self._config.default_font_guid}\n"
        )
        upage_path = os.path.join(output_dir, f"{page_name}.upage")
        with open(upage_path, "w") as f:
            f.write(upage_content)

        upage_meta = {
            "name": page_name,
            "guid": upage_guid,
            "extension": ".upage",
            "type": "ui_page",
        }
        with open(upage_path + ".meta", "w") as f:
            f.write(dict_to_yaml(upage_meta))
            f.write("\n")

        return {
            "dir": output_dir,
            "rml_path": rml_path,
            "css_path": css_path,
            "upage_path": upage_path,
            "rml_guid": rml_guid,
            "css_guid": css_guid,
            "upage_guid": upage_guid,
        }


# ---------------------------------------------------------------------------
# High-level conversion functions
# ---------------------------------------------------------------------------

def convert_frame(file_key: str, frame_name: str, page_name: str,
                  token: Optional[str] = None,
                  config_path: Optional[str] = None,
                  write_files: bool = True) -> dict:
    """Full conversion pipeline: fetch Figma file, generate RML/CSS, write assets.

    Args:
        file_key: Figma file key (from the URL).
        frame_name: Name of the frame to convert.
        page_name: Output page name (used for directory and filenames).
        token: Figma API token (or reads from FIGMA_TOKEN env var).
        config_path: Optional path to figma_naming.yaml.
        write_files: If True, write files to disk. If False, return content only.

    Returns:
        Dict with generated content and file paths (if written).
    """
    config = load_config(config_path)
    client = FigmaClient(token=token)

    file_data = client.get_file(file_key)
    tree = parse_figma_tree(file_data)
    frame = find_frame(tree, frame_name)

    if frame is None:
        available = list_frames(tree)
        available_str = "\n".join(f"  [{page}] {name}" for page, name in available)
        raise ValueError(
            f"Frame '{frame_name}' not found.\n"
            f"Available frames:\n{available_str}"
        )

    css_gen = CssGenerator(frame, config)
    css_content = css_gen.generate()

    rml_gen = RmlGenerator(frame, config)
    rml_content = rml_gen.generate()

    result = {
        "rml_content": rml_content,
        "css_content": css_content,
        "frame_name": frame_name,
        "page_name": page_name,
        "template_guids": rml_gen.template_guids,
    }

    if write_files:
        writer = AssetWriter(config)
        file_info = writer.write(page_name, rml_content, css_content)
        result.update(file_info)

    return result


def list_figma_frames_api(file_key: str,
                          token: Optional[str] = None) -> List[Tuple[str, str]]:
    """Fetch a Figma file and list all top-level frames.

    Args:
        file_key: Figma file key.
        token: Figma API token.

    Returns:
        List of (page_name, frame_name) tuples.
    """
    client = FigmaClient(token=token)
    file_data = client.get_file(file_key)
    tree = parse_figma_tree(file_data)
    return list_frames(tree)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def _print_usage():
    print("Figma-to-RML Conversion Pipeline")
    print()
    print("Usage:")
    print("  python3 mcp_tools/figma_to_rml.py list --file-key <KEY>")
    print("  python3 mcp_tools/figma_to_rml.py preview --file-key <KEY> --page-name <NAME> --frame-name <FRAME>")
    print("  python3 mcp_tools/figma_to_rml.py convert --file-key <KEY> --page-name <NAME> --frame-name <FRAME>")
    print()
    print("Commands:")
    print("  list     List all frames in a Figma file")
    print("  preview  Preview generated RML/CSS without writing files")
    print("  convert  Convert and write all asset files")
    print()
    print("Options:")
    print("  --file-key <KEY>      Figma file key (from URL)")
    print("  --page-name <NAME>    Output page name (for preview/convert)")
    print("  --frame-name <FRAME>  Figma frame name to convert")
    print("  --token <TOKEN>       Figma API token (or set FIGMA_TOKEN env var)")
    print("  --config <PATH>       Path to figma_naming.yaml (optional)")


def _parse_args(args: List[str]) -> dict:
    """Parse CLI arguments manually (following create.py pattern)."""
    parsed = {}
    i = 0
    while i < len(args):
        arg = args[i]
        if arg == "--file-key":
            if i + 1 < len(args):
                parsed["file_key"] = args[i + 1]
                i += 2
            else:
                print("Error: --file-key requires a value")
                sys.exit(1)
        elif arg == "--page-name":
            if i + 1 < len(args):
                parsed["page_name"] = args[i + 1]
                i += 2
            else:
                print("Error: --page-name requires a value")
                sys.exit(1)
        elif arg == "--frame-name":
            if i + 1 < len(args):
                parsed["frame_name"] = args[i + 1]
                i += 2
            else:
                print("Error: --frame-name requires a value")
                sys.exit(1)
        elif arg == "--token":
            if i + 1 < len(args):
                parsed["token"] = args[i + 1]
                i += 2
            else:
                print("Error: --token requires a value")
                sys.exit(1)
        elif arg == "--config":
            if i + 1 < len(args):
                parsed["config"] = args[i + 1]
                i += 2
            else:
                print("Error: --config requires a value")
                sys.exit(1)
        else:
            print(f"Error: Unknown argument '{arg}'")
            sys.exit(1)
    return parsed


def main():
    if len(sys.argv) < 2:
        _print_usage()
        sys.exit(1)

    command = sys.argv[1]
    args = _parse_args(sys.argv[2:])

    if command == "list":
        file_key = args.get("file_key")
        if not file_key:
            print("Error: --file-key is required")
            sys.exit(1)

        token = args.get("token")
        frames = list_figma_frames_api(file_key, token=token)

        if not frames:
            print("No frames found in the Figma file.")
        else:
            print(f"Found {len(frames)} frame(s):")
            print()
            current_page = None
            for page_name, frame_name in frames:
                if page_name != current_page:
                    print(f"  Page: {page_name}")
                    current_page = page_name
                print(f"    - {frame_name}")

    elif command == "preview":
        file_key = args.get("file_key")
        page_name = args.get("page_name")
        frame_name = args.get("frame_name")

        if not file_key:
            print("Error: --file-key is required")
            sys.exit(1)
        if not page_name:
            print("Error: --page-name is required")
            sys.exit(1)
        if not frame_name:
            print("Error: --frame-name is required")
            sys.exit(1)

        token = args.get("token")
        config_path = args.get("config")

        result = convert_frame(
            file_key, frame_name, page_name,
            token=token, config_path=config_path, write_files=False
        )

        print("=" * 60)
        print(f"  RML Preview: {page_name}.rml")
        print("=" * 60)
        print(result["rml_content"])
        print()
        print("=" * 60)
        print(f"  CSS Preview: {page_name}.css")
        print("=" * 60)
        print(result["css_content"])
        print()
        if result["template_guids"]:
            print(f"Template GUIDs required: {', '.join(result['template_guids'])}")

    elif command == "convert":
        file_key = args.get("file_key")
        page_name = args.get("page_name")
        frame_name = args.get("frame_name")

        if not file_key:
            print("Error: --file-key is required")
            sys.exit(1)
        if not page_name:
            print("Error: --page-name is required")
            sys.exit(1)
        if not frame_name:
            print("Error: --frame-name is required")
            sys.exit(1)

        token = args.get("token")
        config_path = args.get("config")

        result = convert_frame(
            file_key, frame_name, page_name,
            token=token, config_path=config_path, write_files=True
        )

        print(f"Conversion complete!")
        print()
        print(f"Output directory: {result['dir']}")
        print(f"Files created:")
        print(f"  - {result['rml_path']}")
        print(f"  - {result['rml_path']}.meta")
        print(f"  - {result['css_path']}")
        print(f"  - {result['css_path']}.meta")
        print(f"  - {result['upage_path']}")
        print(f"  - {result['upage_path']}.meta")
        print()
        print(f"UPAGE GUID: {result['upage_guid']}")
        print(f"Use this GUID in your FSM to load the UI page.")

    else:
        print(f"Error: Unknown command '{command}'")
        print()
        _print_usage()
        sys.exit(1)


if __name__ == "__main__":
    main()
