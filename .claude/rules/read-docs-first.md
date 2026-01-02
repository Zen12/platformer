# Read Documentation First Rule

## CRITICAL: Always Read Relevant Documentation Before Starting

Before implementing any feature, fixing bugs, or performing tasks, you MUST read the relevant documentation to understand existing tools, workflows, and best practices.

## Required Documentation Check

### 1. Check for Existing Documentation

Before starting any task, check if documentation exists:

```bash
# List all documentation
ls -la docs/

# Search for relevant docs
grep -r "<topic>" docs/
```

### 2. Read Project Rules

**Always read** `.claude/rules/` before making changes:

```bash
ls -la .claude/rules/
```

**Common rules to check:**
- `coding-style.md` - Before writing C++ code
- `fsm-workflow.md` - Before modifying FSM files
- `asset-deletion.md` - Before deleting assets
- `asset-creation.md` - Before creating assets
- `creating-actions.md` - Before adding FSM actions

### 3. Read Technical Documentation

**Check `docs/` for:**
- `mcp.md` - MCP tools and workflows
- `blender.md` - Blender integration and export workflows
- Other technical documentation

## Task-Specific Documentation

### Before Deleting Assets
**MUST READ:**
- `docs/mcp.md` - Learn about find/delete tools
- `.claude/rules/asset-deletion.md` - Deletion workflow

**Why:** Understand MCP tools exist and how to use them before manually deleting files.

### Before Creating Assets
**MUST READ:**
- `.claude/rules/asset-creation.md` - Asset creation guidelines
- `docs/mcp.md` - MCP creation tools

**Why:** Learn about GIMP for 2D, Blender for 3D, and MCP metadata tools.

### Before Modifying FSM Files
**MUST READ:**
- `.claude/rules/fsm-workflow.md` - FSM modification rules
- `docs/fsm-system.md` - FSM architecture (if exists)

**Why:** Understand FSM structure, node GUIDs, connection types, and validation.

### Before Writing C++ Code
**MUST READ:**
- `.claude/rules/coding-style.md` - Code style and patterns
- Component factory pattern documentation
- Build requirements

**Why:** Follow existing patterns, naming conventions, and build workflow.

### Before Exporting 3D Models
**MUST READ:**
- `docs/blender.md` - Blender export workflows
- `mcp_tools/export_to_glb.py` - Script usage

**Why:** Understand GLB export, MCP tools, and proper workflow.

## How to Read Documentation

### 1. Use Read Tool (Preferred)

```python
# Read entire file
Read(file_path="/Users/.../docs/mcp.md")

# Read specific section (if large)
Read(file_path="/Users/.../docs/mcp.md", offset=0, limit=100)
```

### 2. Search for Keywords

```bash
# Find relevant docs by keyword
grep -r "asset deletion" docs/
grep -r "MCP tools" docs/
grep -r "export" docs/
```

### 3. Check Rule Files

```bash
# List all rules
ls .claude/rules/

# Read specific rule
cat .claude/rules/asset-deletion.md
```

## Anti-Patterns (What NOT to Do)

### ❌ Don't Assume You Know

**WRONG:**
```
User: "Delete unused files from shooter folder"
Claude: *manually checks files and deletes with rm*
```

**Why wrong:** Didn't read `docs/mcp.md` to learn about find/delete tools.

**RIGHT:**
```
User: "Delete unused files from shooter folder"
Claude: *reads docs/mcp.md*
Claude: *learns about python3 mcp_tools/find.py*
Claude: *uses MCP tools to check references*
```

### ❌ Don't Recreate Existing Documentation

**WRONG:**
```
User: "Add a rule about X"
Claude: *creates new comprehensive documentation without checking if it exists*
```

**Why wrong:** Duplicates existing docs, creates inconsistency.

**RIGHT:**
```
User: "Add a rule about X"
Claude: *reads docs/ and .claude/rules/ first*
Claude: *finds existing docs/x.md*
Claude: *updates existing doc or creates concise rule referencing it*
```

### ❌ Don't Ignore Existing Tools

**WRONG:**
```
User: "Check if this file is used"
Claude: *manually searches with grep/find*
```

**Why wrong:** Didn't read docs to learn about `python3 mcp_tools/find.py`.

**RIGHT:**
```
User: "Check if this file is used"
Claude: *reads docs/mcp.md first*
Claude: *uses python3 mcp_tools/find.py*
```

## Documentation Reading Workflow

```
1. User Request
   ↓
2. Identify Task Type (delete, create, modify, etc.)
   ↓
3. Read Relevant Documentation
   - .claude/rules/<task>.md
   - docs/<topic>.md
   ↓
4. Check for Existing Tools/Scripts
   - mcp_tools/
   - scripts/
   ↓
5. Implement Using Best Practices
   ↓
6. Build and Test
```

## Example: Correct Workflow

**User request:** "Export my Blender model to GLB for the game"

**✅ Correct approach:**

1. **Check for docs:**
   ```bash
   ls docs/*blender* docs/*export*
   # Found: docs/blender.md
   ```

2. **Read documentation:**
   ```python
   Read(file_path="docs/blender.md")
   # Learn about MCP server method and standalone script
   ```

3. **Check for scripts:**
   ```bash
   ls mcp_tools/*export*
   # Found: mcp_tools/export_to_glb.py
   ```

4. **Use existing tools:**
   ```python
   # Use MCP server method (recommended in docs)
   export_to_glb(input_blend="...", output_glb="...")
   ```

**❌ WRONG approach:**

1. User request
2. Immediately write Blender script from scratch
3. Don't check if tools exist
4. Duplicate functionality

## Documentation Locations

### Project Rules (Always Read First)
- `.claude/rules/` - Workflow rules, coding style, patterns

### Technical Documentation
- `docs/` - Architecture, workflows, user guides

### MCP Tools
- `mcp_tools/` - Tool implementations (read docstrings)
- `docs/mcp.md` - MCP tool usage guide

### Code Documentation
- Component factory pattern files
- FSM system headers
- Animation system headers

## When Documentation is Missing

If documentation doesn't exist for a topic:

1. **Explore the codebase** to understand current patterns
2. **Ask user for clarification** if uncertain
3. **Document your implementation** for future reference
4. **Create concise documentation** in appropriate location

But **NEVER assume** documentation doesn't exist without checking first!

## Summary

**Before ANY task:**

1. ✅ Read `.claude/rules/<topic>.md` if it exists
2. ✅ Read `docs/<topic>.md` if it exists
3. ✅ Check `mcp_tools/` for existing tools
4. ✅ Search for keywords if unsure: `grep -r "keyword" docs/`
5. ✅ Use existing tools and follow existing patterns

**Benefits:**
- Avoid duplicating functionality
- Use existing tools correctly
- Follow project conventions
- Faster, better implementation

**Remember:** Documentation was written to help you. Use it!
