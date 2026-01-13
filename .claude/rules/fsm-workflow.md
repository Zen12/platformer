# FSM Workflow Rules

## Overview

These rules govern how to create and modify FSM (Finite State Machine) files. Follow these guidelines when working with FSM definitions.

## When to Modify FSM Files

Modify FSM files when:
- Adding new game states (menu screens, gameplay modes, etc.)
- Adding state transitions (button clicks, trigger events)
- Integrating UI pages with game flow
- Adding game flow logic (pause, resume, exit)

**Do NOT modify FSM files for:**
- Game logic (use entity/component system)
- Visual changes (use UI RML/CSS files)
- Asset changes (use material/scene files)

## File Naming Conventions

```
assets/resources/fsm/
├── main.fsm              # Main game flow FSM
├── menu.fsm              # Menu-specific FSM
├── gameplay.fsm          # Gameplay-specific FSM
└── <purpose>.fsm         # Descriptive, lowercase with underscores
```

## GUID Conventions

### Node GUIDs
Use **descriptive camelCase** strings:

```yaml
# Good
guid: "mainMenu"
guid: "gameplayState"
guid: "pauseMenu"

# Also Acceptable (UUID)
guid: "550e8400-e29b-41d4-a716-446655440000"

# Avoid
guid: "node1"
guid: "state_1"
guid: "n"
```

### Condition GUIDs
Use **descriptive names with prefix**:

```yaml
# Good
guid: "cond_start_game"
guid: "cond_player_died"
guid: "cond_level_complete"

# Avoid
guid: "condition1"
guid: "c1"
```

## Adding a New Node

### Checklist

Before adding a node:
- [ ] Determine the node's purpose (what state does it represent?)
- [ ] Choose a descriptive name and GUID
- [ ] Identify required actions (load UI? load scene? register listeners?)
- [ ] Plan transitions (how to enter? how to exit?)

### Steps

1. **Add to `nodes` list:**

```yaml
- name: "Descriptive Name"
  guid: "descriptiveGuid"
  actions:
    - type: load_ui_page
      page_guid: <ui-page-guid>
```

2. **Add actions in logical order:**
   - Load scene/UI first
   - Register listeners after loading UI
   - Log messages for debugging

3. **Verify all GUIDs exist:**
   - UI page GUIDs must have corresponding `.upage` files
   - Scene GUIDs must have corresponding `.scene` files

## Adding a Connection

### Checklist

Before adding a connection:
- [ ] Both source and destination nodes exist
- [ ] All condition GUIDs are defined in `conditions` list
- [ ] Understand condition type (All vs AtLeastOne)
- [ ] Connection makes logical sense in game flow

### Steps

1. **Define conditions first:**

```yaml
conditions:
  - type: trigger_check
    guid: "cond_my_trigger"
    trigger_name: my_trigger
```

2. **Add connection:**

```yaml
connections:
  - nodes: ["sourceGuid", "destinationGuid"]
    conditions: ["cond_my_trigger"]
    condition_type: 0  # 0 = All, 1 = AtLeastOne
```

3. **Verify:**
   - Source node GUID exists in `nodes`
   - Destination node GUID exists in `nodes`
   - All condition GUIDs exist in `conditions`

## Adding a Condition

### Checklist

- [ ] Choose appropriate condition type (currently only `trigger_check`)
- [ ] Use descriptive GUID with `cond_` prefix
- [ ] Ensure trigger will be set somewhere (button listener, game code, etc.)

### Steps

1. **Add to `conditions` list:**

```yaml
- type: trigger_check
  guid: "cond_my_condition"
  trigger_name: my_trigger
```

2. **Ensure trigger is set:**
   - Via `action_trigger_setter_button_listener` in a node
   - Via game code
   - Via another FSM action

3. **Use in a connection:**
   - Reference the condition GUID in `connections`

## Condition Type Selection

### Use `condition_type: 0` (All/AND)

When **all** conditions must be true:

```yaml
# Example: Require key AND defeated boss to enter next level
- nodes: ["level1", "level2"]
  conditions: ["cond_has_key", "cond_boss_defeated"]
  condition_type: 0
```

**Use cases:**
- Gated progression (need multiple items/flags)
- Complex state transitions
- Safety checks (multiple confirmations)

### Use `condition_type: 1` (AtLeastOne/OR)

When **any one** condition can trigger transition:

```yaml
# Example: Exit game on button click OR ESC key OR alt-F4
- nodes: ["menu", "exit"]
  conditions: ["cond_exit_button", "cond_esc_key", "cond_alt_f4"]
  condition_type: 1
```

**Use cases:**
- Multiple ways to achieve same result
- Fallback mechanisms
- User choice (any option works)

## Action Order

Actions in a node execute in order. Follow this pattern:

```yaml
actions:
  # 1. Load resources first
  - type: load_scene
    scene_guid: <guid>
  - type: load_ui_page
    page_guid: <guid>

  # 2. Register listeners after loading UI
  - type: action_trigger_setter_button_listener
    button_id: my-button
    trigger_name: my_trigger

  # 3. Logging last (for debugging)
  - type: log
    message: "Entered state"
```

**Why this order?**
- UI must be loaded before registering listeners
- Scene should load before UI (background first)
- Logs help debug if earlier actions fail

## Integration with UI

When adding UI-related FSM actions:

### Pattern 1: Load UI Page

```yaml
- name: "Menu State"
  guid: "menuState"
  actions:
    - type: load_ui_page
      page_guid: <upage-guid>
```

**Automatic cleanup:** UI page is unloaded when exiting the state.

### Pattern 2: Button Listener with Trigger

```yaml
- name: "Menu State"
  guid: "menuState"
  actions:
    - type: load_ui_page
      page_guid: <upage-guid>
    - type: action_trigger_setter_button_listener
      button_id: start-button  # Must match RML button ID
      trigger_name: start_game
```

**Button ID must match:** The `button_id` must exactly match the `id` attribute in the RML file.

### Pattern 3: Condition for Trigger

```yaml
conditions:
  - type: trigger_check
    guid: "cond_start"
    trigger_name: start_game  # Must match trigger_name above
```

**Trigger name must match:** The `trigger_name` must exactly match between action and condition.

## Validation Checklist

Before committing FSM changes:

### File Structure
- [ ] FSM file is in `assets/resources/fsm/`
- [ ] `.meta` file exists with GUID
- [ ] YAML syntax is valid (no tabs, proper indentation)

### Nodes
- [ ] All node GUIDs are unique within the FSM
- [ ] `start_node` references an existing node GUID
- [ ] All action types are valid and spelled correctly
- [ ] All action parameters reference existing assets (GUIDs)

### Conditions
- [ ] All condition GUIDs are unique
- [ ] All condition types are valid (`trigger_check`)
- [ ] All trigger names are set somewhere

### Connections
- [ ] All node GUIDs in connections exist in `nodes`
- [ ] All condition GUIDs in connections exist in `conditions`
- [ ] `condition_type` is 0 or 1
- [ ] Connections make logical sense (no orphaned states)

### Testing
- [ ] Run `./run_debug.sh` to verify no errors
- [ ] Test all state transitions in-game
- [ ] Verify UI loads/unloads correctly
- [ ] Verify triggers fire as expected

## Common Patterns

### Pattern: Simple Menu Flow

```yaml
name: menu_fsm
start_node: "mainMenu"

nodes:
  - name: "Main Menu"
    guid: "mainMenu"
    actions:
      - type: load_ui_page
        page_guid: <menu-page>
      - type: action_trigger_setter_button_listener
        button_id: start-button
        trigger_name: start_game

  - name: "Game"
    guid: "gameState"
    actions:
      - type: load_scene
        scene_guid: <game-scene>

conditions:
  - type: trigger_check
    guid: "cond_start"
    trigger_name: start_game

connections:
  - nodes: ["mainMenu", "gameState"]
    conditions: ["cond_start"]
    condition_type: 0
```

### Pattern: Pause Menu

```yaml
# Add to existing gameplay FSM

nodes:
  - name: "Gameplay"
    guid: "gameplay"
    actions:
      - type: load_scene
        scene_guid: <scene>
      - type: action_trigger_setter_button_listener
        button_id: pause-button
        trigger_name: pause

  - name: "Paused"
    guid: "paused"
    actions:
      - type: load_ui_page
        page_guid: <pause-menu>
      - type: action_trigger_setter_button_listener
        button_id: resume-button
        trigger_name: resume

conditions:
  - type: trigger_check
    guid: "cond_pause"
    trigger_name: pause
  - type: trigger_check
    guid: "cond_resume"
    trigger_name: resume

connections:
  - nodes: ["gameplay", "paused"]
    conditions: ["cond_pause"]
    condition_type: 0
  - nodes: ["paused", "gameplay"]
    conditions: ["cond_resume"]
    condition_type: 0
```

### Pattern: Exit Game

```yaml
- name: "Exit State"
  guid: "exitState"
  actions:
    - type: set_system_trigger
      trigger_type: Exit
```

**Note:** This is a terminal state - no connections should lead away from it.

## Things to Avoid

### 1. Circular Dependencies Without Exit

```yaml
# Bad - no way to exit this loop
- nodes: ["a", "b"]
  conditions: ["cond1"]
  condition_type: 0
- nodes: ["b", "a"]
  conditions: ["cond2"]
  condition_type: 0
```

**Fix:** Add an exit condition or connection to another state.

### 2. Orphaned States

```yaml
# Bad - node "settingsState" has no incoming connections
nodes:
  - name: "Main Menu"
    guid: "mainMenu"
  - name: "Settings"
    guid: "settingsState"  # No way to get here!
```

**Fix:** Add a connection from another state to this state.

### 3. Invalid GUIDs in References

```yaml
# Bad - page GUID doesn't exist
- type: load_ui_page
  page_guid: nonexistent-guid
```

**Fix:** Verify all GUIDs exist before using them. Use `check_asset_references()` MCP tool.

### 4. Mismatched Trigger Names

```yaml
# Bad - trigger names don't match
actions:
  - type: action_trigger_setter_button_listener
    trigger_name: start_game

conditions:
  - type: trigger_check
    trigger_name: startGame  # Different name!
```

**Fix:** Ensure exact match between trigger names (case-sensitive).

### 5. Button Listener Before UI Load

```yaml
# Bad - registering listener before UI exists
actions:
  - type: action_trigger_setter_button_listener
    button_id: my-button
  - type: load_ui_page  # UI loaded too late!
    page_guid: <guid>
```

**Fix:** Load UI first, then register listeners.

## Testing FSM Changes

### Always Test After Modifying FSM

1. **Build and run:**
   ```bash
   ./run_debug.sh
   ```

2. **Test all transitions:**
   - Click all buttons
   - Verify state changes
   - Check UI loads/unloads

3. **Check logs:**
   - Look for errors
   - Verify actions execute
   - Confirm triggers fire

4. **Edge cases:**
   - Rapid button clicking
   - Multiple transitions
   - Back-and-forth navigation

## Debugging FSM Issues

### Add Logging

```yaml
- name: "Debug State"
  guid: "debugState"
  actions:
    - type: log
      message: "Entering debug state"
    - type: load_ui_page
      page_guid: <guid>
    - type: log
      message: "UI page loaded"
```

### Check Trigger Values

Add temporary log actions to verify triggers:

```yaml
- type: action_trigger_setter_button_listener
  button_id: my-button
  trigger_name: my_trigger
- type: log
  message: "Registered trigger: my_trigger"
```

### Verify Condition Evaluation

Check that conditions are being evaluated:

```yaml
conditions:
  - type: trigger_check
    guid: "cond_test"
    trigger_name: test_trigger
```

Then check logs to see if condition passes or fails.

## Creating New FSM Files

### Use MCP Tool

```bash
python3 mcp_tools/create.py fsm --name my_fsm
```

This creates:
- `assets/resources/fsm/my_fsm.fsm`
- `assets/resources/fsm/my_fsm.fsm.meta`

### Manual Creation

1. Create file: `assets/resources/fsm/my_fsm.fsm`
2. Add basic structure:
   ```yaml
   name: my_fsm
   start_node: "firstNode"
   nodes: []
   conditions: []
   connections: []
   ```
3. Generate metadata:
   ```bash
   python3 mcp_tools/import.py assets/resources/fsm/my_fsm.fsm
   ```

## Build Requirement

**CRITICAL:** After ANY FSM modification, ALWAYS run `./run_debug.sh` to verify changes work correctly. This applies even to small changes like adding a single condition or connection.

## Related Documentation

- [FSM System Documentation](../../docs/fsm-system.md) - Comprehensive FSM reference
- [Creating Actions Guide](creating-actions.md) - How to create new FSM actions
- [UI System](../../docs/ui-system.md) - UI integration with FSM
- [Coding Style](coding-style.md) - General coding conventions
