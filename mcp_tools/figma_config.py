"""
Figma-to-RML configuration and naming conventions.

Defines element mappings that control how Figma layer names
are converted to RML tags and IDs.
"""

import os
import re
from dataclasses import dataclass, field
from typing import List, Optional


@dataclass
class ElementMapping:
    """Maps a Figma layer name pattern to an RML element.

    Args:
        pattern: fnmatch-style glob pattern to match against layer names.
        tag: RML tag name to emit (e.g., "button", "div", "h1").
        extract_id: If True, extract an element ID from the layer name.
        id_transform: How to transform the extracted ID ("kebab-case" or "none").
        template: Optional template GUID to include via <link> in <head>.
    """
    pattern: str
    tag: str
    extract_id: bool = False
    id_transform: str = "kebab-case"
    template: Optional[str] = None


@dataclass
class FigmaConfig:
    """Configuration for Figma-to-RML conversion.

    Attributes:
        mappings: List of ElementMapping rules, checked in order.
        default_material_guid: GUID of the default UI material.
        default_font_guid: GUID of the default UI font.
        assets_dir: Base assets directory path.
    """
    mappings: List[ElementMapping] = field(default_factory=list)
    default_material_guid: str = "48389756-b5e5-4e57-a44c-e85a030304c8"
    default_font_guid: str = "682a2b0c-eeac-48f0-ab67-d2312a971cec"
    assets_dir: str = field(default_factory=lambda: os.getenv("ASSETS_DIR", "assets/resources"))


# Built-in default mappings (standard set)
DEFAULT_MAPPINGS = [
    ElementMapping("Button/*", "button", extract_id=True, id_transform="kebab-case"),
    ElementMapping("MenuButton/*", "menu_button", extract_id=True, id_transform="kebab-case",
                   template="604fdc70-a493-4688-b1eb-e32315cba97e"),
    ElementMapping("Header", "h1", extract_id=False),
    ElementMapping("Icon/*", "img", extract_id=True, id_transform="kebab-case"),
    ElementMapping("Input/*", "input", extract_id=True, id_transform="kebab-case"),
]

# Figma node types mapped to default RML tags
TYPE_FALLBACKS = {
    "FRAME": "div",
    "GROUP": "div",
    "RECTANGLE": "div",
    "COMPONENT": "div",
    "INSTANCE": "div",
    "ELLIPSE": "div",
    "LINE": "hr",
}

# Text size thresholds for heading tags
TEXT_SIZE_THRESHOLDS = [
    (48, "h1"),
    (32, "h2"),
    (24, "h3"),
]


def to_kebab_case(name: str) -> str:
    """Convert a display name to kebab-case ID.

    Examples:
        "Start Button" -> "start-button"
        "MainMenu" -> "main-menu"
        "fps_display" -> "fps-display"
    """
    # Insert hyphens before uppercase letters in camelCase/PascalCase
    s = re.sub(r'([a-z0-9])([A-Z])', r'\1-\2', name)
    # Replace spaces and underscores with hyphens
    s = re.sub(r'[\s_]+', '-', s)
    # Lowercase and strip leading/trailing hyphens
    return s.lower().strip('-')


def resolve_tag_and_id(name: str, node_type: str, font_size: float,
                       mappings: List[ElementMapping]):
    """Resolve a Figma node to an RML tag, element ID, and optional template GUID.

    Checks mappings in order using fnmatch. Falls back to type-based defaults.

    Args:
        name: Figma layer name.
        node_type: Figma node type (FRAME, TEXT, etc.).
        font_size: Font size in px (only relevant for TEXT nodes).
        mappings: List of ElementMapping rules.

    Returns:
        Tuple of (tag, element_id_or_None, template_guid_or_None).
    """
    import fnmatch

    for mapping in mappings:
        if fnmatch.fnmatch(name, mapping.pattern):
            element_id = None
            if mapping.extract_id:
                # Extract the part after the last "/"
                parts = name.rsplit("/", 1)
                raw_id = parts[-1] if len(parts) > 1 else name
                if mapping.id_transform == "kebab-case":
                    element_id = to_kebab_case(raw_id)
                else:
                    element_id = raw_id
            return mapping.tag, element_id, mapping.template

    # Type-based fallback
    if node_type == "TEXT":
        for threshold, tag in TEXT_SIZE_THRESHOLDS:
            if font_size >= threshold:
                return tag, None, None
        return "p", None, None

    tag = TYPE_FALLBACKS.get(node_type, "div")
    return tag, None, None


def load_config(config_path: Optional[str] = None) -> FigmaConfig:
    """Load FigmaConfig, optionally merging a YAML config file.

    If config_path is provided and the file exists, mappings from it
    are prepended to the default mappings (higher priority).

    Args:
        config_path: Optional path to figma_naming.yaml.

    Returns:
        FigmaConfig with resolved mappings.
    """
    config = FigmaConfig(mappings=list(DEFAULT_MAPPINGS))

    if config_path and os.path.isfile(config_path):
        try:
            import yaml
            with open(config_path, 'r') as f:
                data = yaml.safe_load(f)

            if data and isinstance(data, dict):
                extra_mappings = []
                for entry in data.get("mappings", []):
                    extra_mappings.append(ElementMapping(
                        pattern=entry.get("pattern", "*"),
                        tag=entry.get("tag", "div"),
                        extract_id=entry.get("extract_id", False),
                        id_transform=entry.get("id_transform", "kebab-case"),
                        template=entry.get("template"),
                    ))
                # Prepend custom mappings so they take priority
                config.mappings = extra_mappings + config.mappings

                if "default_material_guid" in data:
                    config.default_material_guid = data["default_material_guid"]
                if "default_font_guid" in data:
                    config.default_font_guid = data["default_font_guid"]
        except ImportError:
            pass  # yaml not available, use defaults only

    return config
