# FSM System (Finite State Machine)

## Overview

The FSM (Finite State Machine) system is the central coordinator in the project architecture. It manages game states, UI flow, and serves as the bridge between UI and game logic.

## Architecture

```
┌─────────────┐
│     UI      │ ◄─── User interactions
└──────┬──────┘
       │
       ▼
┌─────────────┐
│     FSM     │ ◄─── Central coordinator (THIS)
└──────┬──────┘
       │
       ▼
┌─────────────┐
│   Entity/   │ ◄─── Game world
│ Component   │
└─────────────┘
```

**Key Responsibilities:**
- Manage game state transitions (menu → gameplay → pause → exit)
- Load/unload scenes and UI pages
- Handle user input through triggers
- Coordinate between UI and game logic
- Control game flow and lifecycle

## File Structure

FSM files are YAML-based and stored in `assets/resources/fsm/`:

```
assets/resources/fsm/
├── main.fsm           # Main game FSM
├── main.fsm.meta      # Metadata with GUID
└── other_fsm.fsm      # Additional FSMs
```

## FSM File Format

```yaml
name: fsm_name
start_node: "startNodeGuid"
nodes:
  - name: "Node Name"
    guid: "nodeGuid"
    actions: [...]
conditions:
  - type: condition_type
    guid: "conditionGuid"
    # condition-specific fields
connections:
  - nodes: ["fromGuid", "toGuid"]
    conditions: ["conditionGuid1", "conditionGuid2"]
    condition_type: 0  # 0 = All, 1 = AtLeastOne
```

## Nodes

Nodes represent **states** in the state machine. Each node has a name, GUID, and a list of actions.

### Node Structure

```yaml
- name: "Main Menu"
  guid: "mainMenuGuid"
  actions:
    - type: load_ui_page
      page_guid: df04e927-64d5-406b-8170-ddd96f1864a5
    - type: action_trigger_setter_button_listener
      button_id: start-button
      trigger_name: start_game
```

### Node Lifecycle

1. **Enter** - When FSM transitions to this node, all actions' `OnEnter()` methods are called
2. **Update** - Every frame while in this node, all actions' `OnUpdate()` methods are called
3. **Exit** - When FSM transitions away from this node, all actions' `OnExit()` methods are called

## Actions

Actions are behaviors that execute during a node's lifecycle. See [Creating Actions Guide](../.claude/rules/creating-actions.md) for how to create new actions.

### Available Actions

#### load_ui_page
Loads a UI page when entering the state, unloads when exiting.

```yaml
- type: load_ui_page
  page_guid: df04e927-64d5-406b-8170-ddd96f1864a5
```

**Lifecycle:**
- OnEnter: Load and display UI page
- OnExit: Unload UI page

#### load_scene
Loads a game scene.

```yaml
- type: load_scene
  scene_guid: 963d3fa8-8f5c-45aa-beab-1aa887e30816
```

#### action_trigger_setter_button_listener
Registers a button click handler that sets a trigger.

```yaml
- type: action_trigger_setter_button_listener
  button_id: start-button
  trigger_name: start_game
```

**How it works:**
- OnEnter: Registers button listener
- When button clicked: Sets `start_game` trigger to `true`
- OnExit: Unregisters listener

#### action_button_listener
Registers a generic button listener.

```yaml
- type: action_button_listener
  button_id: my-button
```

#### set_system_trigger
Sets a system-level trigger (Exit, Reload, etc.).

```yaml
- type: set_system_trigger
  system_trigger: Exit
```

**Available system triggers:**
- `Exit` - Exit the application
- `Reload` - Reload the current scene

#### log
Logs a message (useful for debugging).

```yaml
- type: log
  message: "Entering main menu"
```

## Conditions

Conditions determine when transitions between nodes should occur. Each condition has a type, GUID, and type-specific parameters.

### Condition Types

#### trigger_check
Checks if a trigger is set to `true`.

```yaml
- type: trigger_check
  guid: "start_condition"
  trigger_name: start_game
```

**How it works:**
- Returns `true` if the trigger `start_game` is set to `true`
- Returns `false` if the trigger is not set or is `false`

**Triggers are set by:**
- Button listeners (`action_trigger_setter_button_listener`)
- Game code
- Other FSM actions

### Future Condition Types

Additional condition types can be added by:
1. Creating a condition class (inherits from `Condition`)
2. Implementing `IsSuccess()` method
3. Registering in `FsmController`

See `src/engine/fsm/condition/` for examples.

## Connections

Connections define **transitions** between nodes. A connection specifies:
- Source and destination nodes (by GUID)
- Conditions that must be met
- How conditions are evaluated (All vs AtLeastOne)

### Connection Structure

```yaml
connections:
  - nodes: ["fromNodeGuid", "toNodeGuid"]
    conditions: ["condition1Guid", "condition2Guid"]
    condition_type: 0
```

### Condition Types

The `condition_type` field determines how multiple conditions are evaluated:

#### All (0) - AND Logic

**ALL** conditions must be true to transition.

```yaml
connections:
  - nodes: ["nodeA", "nodeB"]
    conditions: ["trigger1", "trigger2", "trigger3"]
    condition_type: 0  # All must be true
```

**Use case:** Transition when player has key AND opened door AND defeated boss.

#### AtLeastOne (1) - OR Logic

**AT LEAST ONE** condition must be true to transition.

```yaml
connections:
  - nodes: ["nodeA", "nodeB"]
    conditions: ["trigger1", "trigger2", "trigger3"]
    condition_type: 1  # Any one can be true
```

**Use case:** Transition when player clicks "Exit" OR presses ESC OR dies.

### Multiple Connections from Same Node

A node can have multiple outgoing connections. The **first** connection whose conditions are met will trigger.

```yaml
connections:
  - nodes: ["menu", "game"]
    conditions: ["start_clicked"]
    condition_type: 0
  - nodes: ["menu", "settings"]
    conditions: ["settings_clicked"]
    condition_type: 0
  - nodes: ["menu", "exit"]
    conditions: ["exit_clicked"]
    condition_type: 0
```

**Order matters:** Connections are evaluated in the order they appear in the YAML file.

## Complete Example

Here's a complete FSM for a simple game flow:

```yaml
name: game_fsm
start_node: "mainMenu"

nodes:
  # Main Menu State
  - name: "Main Menu"
    guid: "mainMenu"
    actions:
      - type: load_ui_page
        page_guid: menu-page-guid
      - type: action_trigger_setter_button_listener
        button_id: start-button
        trigger_name: start_game
      - type: action_trigger_setter_button_listener
        button_id: exit-button
        trigger_name: exit_game

  # Gameplay State
  - name: "Gameplay"
    guid: "gameplay"
    actions:
      - type: load_scene
        scene_guid: level1-scene-guid
      - type: load_ui_page
        page_guid: hud-page-guid
      - type: action_trigger_setter_button_listener
        button_id: pause-button
        trigger_name: pause_game

  # Pause State
  - name: "Pause Menu"
    guid: "pauseMenu"
    actions:
      - type: load_ui_page
        page_guid: pause-page-guid
      - type: action_trigger_setter_button_listener
        button_id: resume-button
        trigger_name: resume_game
      - type: action_trigger_setter_button_listener
        button_id: quit-button
        trigger_name: quit_to_menu

  # Exit State
  - name: "Exit"
    guid: "exitState"
    actions:
      - type: set_system_trigger
        system_trigger: Exit

conditions:
  # Menu transitions
  - type: trigger_check
    guid: "cond_start"
    trigger_name: start_game
  - type: trigger_check
    guid: "cond_exit"
    trigger_name: exit_game

  # Gameplay transitions
  - type: trigger_check
    guid: "cond_pause"
    trigger_name: pause_game

  # Pause menu transitions
  - type: trigger_check
    guid: "cond_resume"
    trigger_name: resume_game
  - type: trigger_check
    guid: "cond_quit"
    trigger_name: quit_to_menu

connections:
  # From Main Menu
  - nodes: ["mainMenu", "gameplay"]
    conditions: ["cond_start"]
    condition_type: 0

  - nodes: ["mainMenu", "exitState"]
    conditions: ["cond_exit"]
    condition_type: 0

  # From Gameplay
  - nodes: ["gameplay", "pauseMenu"]
    conditions: ["cond_pause"]
    condition_type: 0

  # From Pause Menu
  - nodes: ["pauseMenu", "gameplay"]
    conditions: ["cond_resume"]
    condition_type: 0

  - nodes: ["pauseMenu", "mainMenu"]
    conditions: ["cond_quit"]
    condition_type: 0
```

## FSM Flow Visualization

```
┌─────────────┐
│ Main Menu   │
│ (start)     │
└──────┬──────┘
       │
       ├─── start_game ──────────┐
       │                         ▼
       │                    ┌─────────────┐
       │                    │  Gameplay   │
       │                    └──────┬──────┘
       │                           │
       │                           │ pause_game
       │                           ▼
       │                    ┌─────────────┐
       │                    │ Pause Menu  │
       │                    └──────┬──────┘
       │                           │
       │                           ├─── resume_game ───┘
       │                           │
       │                           └─── quit_to_menu ───┐
       │                                                 │
       └─── exit_game ──────────┐                       │
                                 ▼                       │
                          ┌─────────────┐               │
                          │    Exit     │ ◄─────────────┘
                          └─────────────┘
```

## Creating a New FSM

### Using MCP Tool

```python
# Via Claude Code
"Create a new FSM called 'game_fsm'"
```

### Using Command Line

```bash
python3 mcp_tools/create.py fsm --name game_fsm
```

This creates:
- `assets/resources/fsm/game_fsm.fsm` - FSM definition
- `assets/resources/fsm/game_fsm.fsm.meta` - Metadata with GUID

### Manual Creation

1. Create `assets/resources/fsm/your_fsm.fsm`
2. Add basic structure:
```yaml
name: your_fsm
start_node: "firstNode"
nodes: []
conditions: []
connections: []
```
3. Generate metadata: `python3 mcp_tools/import.py assets/resources/fsm/your_fsm.fsm`

## Modifying an Existing FSM

### Adding a Node

1. Add to `nodes` list:
```yaml
- name: "New State"
  guid: "newStateGuid"
  actions:
    - type: log
      message: "Entered new state"
```

2. **GUID Tip:** Use descriptive strings or UUIDs. Must be unique within the FSM.

### Adding a Condition

1. Add to `conditions` list:
```yaml
- type: trigger_check
  guid: "new_condition"
  trigger_name: my_trigger
```

### Adding a Connection

1. Ensure both nodes exist
2. Ensure all condition GUIDs exist
3. Add to `connections` list:
```yaml
- nodes: ["sourceGuid", "destinationGuid"]
  conditions: ["conditionGuid"]
  condition_type: 0
```

### Example: Adding a Settings Menu

**Step 1: Add Settings Node**
```yaml
- name: "Settings"
  guid: "settingsNode"
  actions:
    - type: load_ui_page
      page_guid: settings-page-guid
    - type: action_trigger_setter_button_listener
      button_id: back-button
      trigger_name: back_to_menu
```

**Step 2: Add Button Listener to Main Menu**
```yaml
# In Main Menu node, add:
- type: action_trigger_setter_button_listener
  button_id: settings-button
  trigger_name: open_settings
```

**Step 3: Add Conditions**
```yaml
- type: trigger_check
  guid: "cond_open_settings"
  trigger_name: open_settings
- type: trigger_check
  guid: "cond_back_to_menu"
  trigger_name: back_to_menu
```

**Step 4: Add Connections**
```yaml
# Main Menu → Settings
- nodes: ["mainMenu", "settingsNode"]
  conditions: ["cond_open_settings"]
  condition_type: 0

# Settings → Main Menu
- nodes: ["settingsNode", "mainMenu"]
  conditions: ["cond_back_to_menu"]
  condition_type: 0
```

## Integration with UI System

FSM actions control the UI system. See [UI System Documentation](ui-system.md) for details.

**Typical Pattern:**
1. FSM loads UI page
2. FSM registers button listeners
3. User clicks button → trigger is set
4. FSM evaluates conditions → transitions to new state
5. FSM unloads old UI page, loads new UI page

## Triggers

Triggers are boolean flags that conditions check. They are managed by the FSM controller.

### Trigger Lifecycle

```
1. Button clicked
   └─> action_trigger_setter_button_listener sets trigger = true

2. FSM Update() checks connections
   └─> Evaluates conditions
       └─> trigger_check reads trigger value

3. If conditions met: Transition to new state
   └─> Triggers are CLEARED when changing state
```

**Important:** Triggers are **automatically cleared** when the FSM changes state. This prevents accidental re-triggering.

### System Triggers

System triggers are special triggers that control the application:

```yaml
- type: set_system_trigger
  system_trigger: Exit  # or Reload
```

**Available System Triggers:**
- `Exit` - Quit the application
- `Reload` - Reload the current scene

System triggers are checked by the engine and trigger application-level behavior.

## Best Practices

### 1. Descriptive Node Names
```yaml
# Good
- name: "Main Menu"
  guid: "mainMenu"

# Bad
- name: "Node1"
  guid: "n1"
```

### 2. Descriptive GUIDs
```yaml
# Good
guid: "mainMenu"
guid: "cond_start_game"

# Also Good (UUID)
guid: "550e8400-e29b-41d4-a716-446655440000"

# Bad
guid: "node1"
guid: "c1"
```

### 3. Organize Conditions by Purpose
```yaml
conditions:
  # Menu navigation
  - type: trigger_check
    guid: "cond_start"
    trigger_name: start_game
  - type: trigger_check
    guid: "cond_settings"
    trigger_name: open_settings

  # Gameplay
  - type: trigger_check
    guid: "cond_pause"
    trigger_name: pause_game
```

### 4. Document Complex Transitions
```yaml
connections:
  # Player can only enter level 2 if they have the key AND defeated boss
  - nodes: ["level1", "level2"]
    conditions: ["has_key", "boss_defeated"]
    condition_type: 0  # All must be true
```

### 5. One FSM per Flow
Don't mix unrelated game flows in a single FSM. Create separate FSMs for:
- Main menu flow
- Gameplay flow
- Settings flow
- etc.

### 6. Use Logging for Debug
```yaml
- name: "Debug State"
  guid: "debugState"
  actions:
    - type: log
      message: "Entered debug state"
```

## Troubleshooting

### FSM Not Transitioning

**Check:**
- Condition GUIDs match between `conditions` and `connections`
- Triggers are being set (add log actions)
- Condition type is correct (0 = All, 1 = AtLeastOne)
- Node GUIDs are correct in connections

### Action Not Executing

**Check:**
- Action type is spelled correctly
- Action parameters are valid (GUIDs exist)
- Action is in the correct node
- FSM is actually entering the node (add log action)

### UI Not Loading

**Check:**
- `load_ui_page` uses correct page GUID
- Page GUID exists and has valid .upage file
- FSM is entering the node (add log action)
- No errors in console

### Button Listener Not Working

**Check:**
- Button ID in RML matches `button_id` in action
- Listener is registered in the same node that loads the UI
- FSM is in the correct state
- Button element exists in RML

### Trigger Not Firing

**Check:**
- Trigger name matches between action and condition
- Triggers are cleared when changing state (expected behavior)
- Button listener is registered before button is clicked

## Technical Details

### FSM Controller Implementation

The FSM controller (`src/engine/fsm/fsm_controller.hpp`) manages:
- State transitions
- Action execution
- Condition evaluation
- Trigger management

**Update Loop:**
1. If not entered, call `EnterAll()` on current node
2. Call `UpdateAll()` on current node
3. Find connections from current state
4. Evaluate conditions based on `condition_type`
5. If conditions met, transition to next state

### Condition Evaluation

**All (AND):**
```cpp
for each condition {
    if (!condition.IsSuccess()) {
        return; // Fail - don't transition
    }
}
// All passed - transition
```

**AtLeastOne (OR):**
```cpp
bool anySuccess = false;
for each condition {
    if (condition.IsSuccess()) {
        anySuccess = true;
        break; // Found one - can transition
    }
}
if (anySuccess) {
    // Transition
}
```

## Related Documentation

- [Creating Actions Guide](../.claude/rules/creating-actions.md) - How to create new FSM actions
- [UI System](ui-system.md) - UI integration with FSM
- [Material System](material.md) - Asset GUID system
- [MCP Tools](mcp.md) - Asset creation and management

## Resources

- FSM Controller: `src/engine/fsm/fsm_controller.hpp`
- Actions: `src/engine/fsm/node/action/`
- Conditions: `src/engine/fsm/condition/`
- Connections: `src/engine/fsm/connection/`
