# Asset Deletion Rules

## CRITICAL: Always Use MCP Tools for Asset Deletion

When deleting asset files, you MUST use the MCP tools to check for references first. Never manually delete assets without verification.

**See also:** [MCP Tools Documentation](../docs/mcp.md) for complete usage guide.

## Required Workflow

### 1. Check Asset References (REQUIRED)

**Before deleting any asset file**, use the MCP find tool:

```bash
python3 mcp_tools/find.py <asset_file_path>
```

**Interpreting results:**
- **"No references found"** → Safe to delete
- **"Found N references"** → Asset is in use, deletion will break things

### 2. Delete Asset Safely (RECOMMENDED)

Use the MCP delete tool which automatically checks references:

```bash
python3 mcp_tools/delete.py <asset_file_path>
```

**What it does:**
- Checks for references automatically
- Warns if references exist
- Prompts for confirmation if asset is referenced
- Deletes both the asset file AND its `.meta` file

## Example: User Asks to Delete Unused Files

**User request:** "Delete all unused files from the shooter folder"

**✅ Correct approach:**

1. **Check each file for references:**
   ```bash
   for file in assets/resources/models/shooter/*.glb; do
       echo "=== Checking $(basename $file) ==="
       python3 mcp_tools/find.py "$file"
       echo ""
   done
   ```

2. **Delete unreferenced files:**
   ```bash
   # For each file marked "No references found":
   python3 mcp_tools/delete.py assets/resources/models/shooter/file1.glb
   python3 mcp_tools/delete.py assets/resources/models/shooter/file2.glb
   ```

**❌ WRONG approach:**
```bash
# Never do this - doesn't check references!
rm assets/resources/models/shooter/old_*.glb
```

## What NOT to Do

### ❌ Never Manually Delete Without Checking

```bash
# ❌ WRONG - No reference check!
rm assets/resources/models/shooter/model.glb
rm assets/resources/models/shooter/model.glb.meta
```

### ❌ Never Use Wildcards Without Verification

```bash
# ❌ WRONG - Deletes without checking each file!
rm assets/resources/models/shooter/*.fbx
```

### ❌ Never Delete Just the Asset File

```bash
# ❌ WRONG - Leaves orphaned .meta file!
rm assets/resources/models/shooter/model.glb
# Missing: rm assets/resources/models/shooter/model.glb.meta
```

### ❌ Never Delete Just the .meta File

```bash
# ❌ WRONG - Leaves orphaned asset file!
rm assets/resources/models/shooter/model.glb.meta
# Missing: rm assets/resources/models/shooter/model.glb
```

## File Types That Can Reference Assets

The find tool searches these file types for GUID references:

- **Scenes** (`.scene`) - Reference meshes, materials, prefabs, animations
- **Materials** (`.mat`) - Reference shaders, textures
- **Prefabs** (`.prefab`) - Reference meshes, materials
- **FSMs** (`.fsm`) - Reference UI pages, scenes
- **UI Pages** (`.upage`) - Reference images, fonts
- **Other .meta files** - May reference assets in hierarchies

## Exception: Source Files

Source files that don't have `.meta` files (like `.blend`, `.psd`, `.xcf`) can be deleted more freely, but you should still:

1. **Verify they're truly source files** (not exported assets)
2. **Check if exported versions exist** (e.g., `.blend` → `.glb`)
3. **Confirm with user** if unsure about keeping source files

**Example - Blender files:**
```bash
# User says: "except blender files"
# This means: Keep .blend files, they're source files
# Can delete: .glb, .fbx, .png that are exported from .blend
```

## MCP Tools Quick Reference

See [MCP Tools Documentation](../docs/mcp.md) for complete details.

**find.py** - Check asset references
```bash
python3 mcp_tools/find.py assets/resources/models/model.glb
python3 mcp_tools/find.py --guid <guid>  # Search by GUID directly
```

**delete.py** - Safely delete assets
```bash
python3 mcp_tools/delete.py assets/resources/models/model.glb
# Prompts for confirmation if references exist
```

## Summary

1. **ALWAYS use `find.py`** to check references before deleting
2. **PREFER `delete.py`** for actual deletion (safest)
3. **NEVER delete** without checking references first
4. **ALWAYS delete both** asset file AND `.meta` file together
5. **ASK USER** if uncertain about deleting referenced assets

Following these rules prevents breaking asset references and maintains project integrity.