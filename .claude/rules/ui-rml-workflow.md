# UI/RML Workflow Rules

## CRITICAL: All Asset References Must Use GUIDs

**NEVER use path-based references in RML files.** All asset references must use the `guid|` protocol.

## The `guid|` Protocol

The engine's `GuidFileInterface` supports GUID-based file resolution. Use this format for all references:

```
guid|<uuid>
```

### Example

```rml
<!-- WRONG - Path-based reference -->
<link type="text/template" href="../templates/menu_button.rml"/>

<!-- CORRECT - GUID-based reference -->
<link type="text/template" href="guid|604fdc70-a493-4688-b1eb-e32315cba97e"/>
```

## How to Find Asset GUIDs

Every asset has a `.meta` file containing its GUID. To find the GUID for an RML template:

```bash
cat assets/resources/ui/templates/menu_button.rml.meta
```

Output:
```yaml
name: menu_button
guid: 604fdc70-a493-4688-b1eb-e32315cba97e
extension: .rml
type: rml
```

Use the `guid` value with the `guid|` prefix.

## Where to Use `guid|` Protocol

Use `guid|` for ALL asset references in RML files:

| Reference Type | Example |
|----------------|---------|
| Templates | `<link type="text/template" href="guid|..."/>` |
| Images | `<img src="guid|..."/>` |
| External RML | Any `href` attribute referencing RML files |

## Why GUIDs Instead of Paths

1. **Asset tracking** - The engine uses GUIDs to track asset dependencies
2. **Refactoring safety** - Moving files doesn't break references
3. **Consistency** - All other assets (scenes, materials, FSMs) use GUIDs
4. **No relative path issues** - RML loaded via `SetInnerRML()` has no base path context

## Template File Format

RmlUi templates must include `<head>` and `<body>` sections:

```rml
<template name="menu_button" content="label">
<head>
</head>
<body>
    <span id="label"></span>
</body>
</template>
```

**CRITICAL:** Templates without `<head>` and `<body>` will fail to parse.

## Implementation Details

The `GuidFileInterface` class (`src/engine/renderer/guid_file_interface.hpp`) handles GUID resolution. It supports both `guid|` and `guid:` formats (RmlUi internally converts `|` to `:`):

```cpp
std::string ResolveGuidToPath(const std::string& path) const {
    std::string guidString;
    if (path.rfind("guid|", 0) == 0) {
        guidString = path.substr(5);
    } else if (path.rfind("guid:", 0) == 0) {
        guidString = path.substr(5);
    } else {
        return path;
    }
    const Guid guid = Guid::FromString(guidString);
    return assetManager->GetPathFromGuid(guid);
}
```

## Checklist for RML Files

Before committing RML changes:

- [ ] All `href` attributes use `guid|` protocol
- [ ] All `src` attributes for images use `guid|` protocol
- [ ] No relative paths (`../`, `./`) in any references
- [ ] Verified GUIDs exist in corresponding `.meta` files
- [ ] Tested with `./run_debug.sh`

## Related Files

- `src/engine/renderer/guid_file_interface.hpp` - GUID resolution implementation
- `src/engine/renderer/ui_manager.hpp` - UI page loading (uses `LoadDocumentFromMemory()`)
- `src/engine/renderer/ui/ui_page.hpp` - UI page structure
