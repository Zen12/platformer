# UI System

## Overview

The project uses **RmlUi** (formerly libRocket) for the UI system. The UI is intentionally separated from the 3D/entity world and operates at the **FSM (Finite State Machine) level**. This architectural decision ensures clean separation of concerns - UI does not directly interact with entity components.

## Architecture

### Separation of Concerns

```
┌─────────────┐
│     UI      │ ◄─── RmlUi framework
└──────┬──────┘
       │
       ▼
┌─────────────┐
│     FSM     │ ◄─── State machine controls everything
└──────┬──────┘
       │
       ▼
┌─────────────┐
│   Entity/   │ ◄─── Game world
│ Component   │
└─────────────┘
```

**Key Principles:**
- UI does **not** directly interact with entity/component system
- FSM acts as the **coordinator** between UI and game logic
- UI triggers flow through FSM actions and triggers
- Future: UI and game logic will communicate via **signal bus**

### Why This Design?

1. **Clean separation**: UI is not part of the 3D world
2. **Testability**: FSM logic can be tested independently
3. **Flexibility**: UI can be completely replaced without touching game logic
4. **Maintainability**: Clear boundaries between systems

## File Structure

All UI assets are located in `assets/resources/ui/`:

```
assets/resources/ui/
├── page_name.rml           # RML markup (HTML-like)
├── page_name.rml.meta      # RML metadata with GUID
├── styles.css              # CSS styling
├── styles.css.meta         # CSS metadata with GUID
├── page_name.upage         # UI page definition
└── page_name.upage.meta    # UI page metadata
```

## UI Page Structure (.upage)

UI pages are definition files that combine RML, CSS, material, and font references:

```yaml
rml: 83929e12-fe39-41dd-97c5-d63dd7f03964      # GUID of .rml file
css: b47f55b0-2e85-43a9-9bf3-3ed8265c62d5      # GUID of .css file
material: 48389756-b5e5-4e57-a44c-e85a030304c8 # GUID of material
font: 682a2b0c-eeac-48f0-ab67-d2312a971cec     # GUID of font
```

All references use **GUIDs** for rename-safe asset management.

## RML Files (.rml)

RML files use HTML-like syntax with RmlUi-specific features:

```rml
<rml>
<body>
    <h1>Hello world</h1>
    <img src="guid://126fb991-fe1c-45e0-ae0c-1649a2c65ae3" width="100" height="100"/>
    <br/>
    <button id="hello-button">Exit</button>
</body>
</rml>
```

### GUID-Based Image Loading

Images are referenced using the `guid://` protocol:

```rml
<img src="guid://126fb991-fe1c-45e0-ae0c-1649a2c65ae3" width="100" height="100"/>
```

**Why GUIDs?**
- Rename-safe: Image files can be renamed without breaking references
- Consistent with other asset references (materials, scenes, etc.)
- Find all references with MCP tools: `check_asset_references()`

### Element IDs

Elements that need interaction (buttons, inputs) require unique IDs:

```rml
<button id="hello-button">Exit</button>
<input id="username-input" type="text"/>
```

IDs are used by FSM actions to attach listeners and trigger handlers.

## CSS Styling (.css)

Standard CSS is used for styling. Refer to [RmlUi CSS documentation](https://mikke89.github.io/RmlUiDoc/pages/rcss.html) for full syntax.

Example:

```css
body {
    font-family: Arial;
    font-size: 18px;
    color: #02475e;
    text-align: center;
    padding: 2em 1em;
}

button {
    background-color: #f6470a;
    color: white;
    padding: 12px 24px;
    font-size: 1em;
    cursor: pointer;
}

button:hover {
    background-color: #d63e08;
}

#hello-button {
    position: absolute;
    top: 40px;
    right: 80px;
}
```

**CSS Features:**
- Standard CSS selectors (tag, class, ID)
- Pseudo-classes (`:hover`, `:active`, `:focus`)
- Box model (padding, margin, border)
- Positioning (absolute, relative)
- See [RmlUi RCSS](https://mikke89.github.io/RmlUiDoc/pages/rcss.html) for full reference

## FSM Integration

UI is controlled entirely through FSM actions. The FSM loads/unloads pages and registers button listeners.

### Loading a UI Page

Use the `load_ui_page` action in FSM:

```yaml
nodes:
- name: "Main Menu"
  guid: "mainMenuNode"
  actions:
    - type: load_ui_page
      page_guid: df04e927-64d5-406b-8170-ddd96f1864a5
```

When the FSM enters this state:
- UI page is loaded
- RML is parsed and rendered
- CSS is applied
- Page becomes visible

### Unloading UI Pages

UI pages are automatically unloaded when the FSM exits the state (handled by `UiPageAction::OnExit()`).

### Button Listeners

Register button click handlers that set triggers:

```yaml
nodes:
- name: "Main Menu"
  guid: "mainMenuNode"
  actions:
    - type: load_ui_page
      page_guid: df04e927-64d5-406b-8170-ddd96f1864a5
    - type: action_trigger_setter_button_listener
      button_id: hello-button
      trigger_name: is_close
```

**How it works:**
1. FSM enters state and loads UI page
2. Button listener is registered for `hello-button`
3. When user clicks the button, trigger `is_close` is set to `true`
4. FSM evaluates conditions and transitions to next state

### Complete Example

```yaml
name: main_fsm
start_node: "menuNode"
nodes:
- name: "Main Menu"
  guid: "menuNode"
  actions:
    - type: load_scene
      scene_guid: 963d3fa8-8f5c-45aa-beab-1aa887e30816
    - type: load_ui_page
      page_guid: df04e927-64d5-406b-8170-ddd96f1864a5
    - type: action_trigger_setter_button_listener
      button_id: start-button
      trigger_name: start_game
    - type: action_trigger_setter_button_listener
      button_id: exit-button
      trigger_name: exit_game

- name: "Game Running"
  guid: "gameNode"
  actions:
    - type: load_scene
      scene_guid: 12345678-abcd-efgh-ijkl-mnopqrstuvwx

- name: "Exit Game"
  guid: "exitNode"
  actions:
    - type: set_system_trigger
      system_trigger: Exit

conditions:
  - type: trigger_check
    guid: "start_condition"
    trigger_name: start_game
  - type: trigger_check
    guid: "exit_condition"
    trigger_name: exit_game

connections:
  - nodes: ["menuNode", "gameNode"]
    conditions: ["start_condition"]
    condition_type: 0
  - nodes: ["menuNode", "exitNode"]
    conditions: ["exit_condition"]
    condition_type: 0
```

**Flow:**
1. FSM starts at "Main Menu" state
2. Scene and UI page load
3. Button listeners registered
4. User clicks "Start" → `start_game` trigger set → FSM transitions to "Game Running"
5. User clicks "Exit" → `exit_game` trigger set → FSM transitions to "Exit Game"

## Available FSM Actions

### load_ui_page
Loads a UI page when entering FSM state, unloads when exiting.

```yaml
- type: load_ui_page
  page_guid: df04e927-64d5-406b-8170-ddd96f1864a5
```

### action_trigger_setter_button_listener
Registers a button click handler that sets a trigger.

```yaml
- type: action_trigger_setter_button_listener
  button_id: my-button        # Must match ID in RML
  trigger_name: my_trigger    # Trigger to set on click
```

**Lifecycle:**
- **OnEnter**: Registers button handler
- **OnExit**: Unregisters handler (cleanup)

### Future Actions (Planned)
- Input field listeners
- Checkbox/radio button listeners
- List selection listeners
- Form submission handlers

## Creating New UI Elements

For detailed information on creating RML elements, see the official [RmlUi documentation](https://mikke89.github.io/RmlUiDoc/).

### Common Elements

```rml
<!-- Text -->
<h1>Heading</h1>
<p>Paragraph text</p>

<!-- Images (GUID-based) -->
<img src="guid://your-image-guid" width="100" height="100"/>

<!-- Buttons -->
<button id="my-button">Click Me</button>

<!-- Text Input -->
<input type="text" id="username" placeholder="Enter name"/>

<!-- Containers -->
<div class="container">
    <p>Content here</p>
</div>
```

### Element IDs

Always add `id` attributes to interactive elements:

```rml
<button id="start-button">Start Game</button>
<button id="settings-button">Settings</button>
<button id="quit-button">Quit</button>
```

These IDs are used in FSM actions to attach listeners.

## Data Bindings (Work in Progress)

Data bindings are currently in development and should be **ignored** for now. Future updates will enable:
- Two-way data binding between UI and game state
- Automatic UI updates when data changes
- Template-based dynamic UI generation

## Signal Bus Integration (Planned)

Future architecture will include a signal bus for communication:

```
┌─────────────┐
│     UI      │ ───┐
└─────────────┘    │
                   ▼
               ┌────────────┐
               │ Signal Bus │
               └────────────┘
                   ▲
┌─────────────┐    │
│   Entity/   │ ───┘
│ Component   │
└─────────────┘
```

This will allow:
- UI to emit signals (e.g., "player clicked start")
- Game systems to emit signals (e.g., "health changed")
- Decoupled communication without direct dependencies

## Best Practices

### 1. Use GUID References
Always reference images and assets by GUID, not file paths:

```rml
<!-- Good -->
<img src="guid://126fb991-fe1c-45e0-ae0c-1649a2c65ae3"/>

<!-- Bad -->
<img src="../images/player.png"/>
```

### 2. Unique Element IDs
Ensure all interactive elements have unique IDs across the entire UI page:

```rml
<!-- Good -->
<button id="main-menu-start">Start</button>
<button id="main-menu-quit">Quit</button>

<!-- Bad (potential ID conflicts) -->
<button id="button">Start</button>
<button id="button">Quit</button>
```

### 3. Keep UI Logic in FSM
Don't try to handle game logic in RML or CSS. All logic belongs in FSM:

```yaml
# Good - Logic in FSM
- type: action_trigger_setter_button_listener
  button_id: attack-button
  trigger_name: player_attack
```

### 4. Semantic HTML
Use semantic RML elements for better structure:

```rml
<!-- Good -->
<div class="menu">
    <h1>Main Menu</h1>
    <button id="start">Start Game</button>
</div>

<!-- Avoid -->
<div>
    <div class="title">Main Menu</div>
    <div class="button" id="start">Start Game</div>
</div>
```

### 5. Organize Styles
Group related CSS rules and use comments for sections:

```css
/* Layout */
body {
    width: 100%;
    height: 100%;
}

/* Typography */
h1 {
    font-size: 2em;
    color: #f6470a;
}

/* Buttons */
button {
    padding: 12px 24px;
    background-color: #f6470a;
}
```

## Workflow: Creating a New UI Page

1. **Create RML file** in `assets/resources/ui/my_page.rml`
2. **Create CSS file** in `assets/resources/ui/my_page.css`
3. **Generate metadata** using MCP: `generate_asset_metadata()`
4. **Create .upage file** with GUIDs:
   ```yaml
   rml: <rml-file-guid>
   css: <css-file-guid>
   material: <material-guid>
   font: <font-guid>
   ```
5. **Generate .upage metadata**: `generate_asset_metadata()`
6. **Add FSM action** to load the page:
   ```yaml
   - type: load_ui_page
     page_guid: <upage-guid>
   ```
7. **Test**: Run the game and verify UI loads correctly

## Troubleshooting

### UI Page Not Loading

**Check:**
- `.upage` file has correct GUIDs
- RML and CSS `.meta` files exist
- FSM action uses correct page GUID
- No syntax errors in RML/CSS

### Button Not Responding

**Check:**
- Button has unique `id` attribute in RML
- FSM action uses correct `button_id`
- Listener is registered in the same FSM node that loads the UI
- No JavaScript/event errors in logs

### Images Not Displaying

**Check:**
- Using `guid://` protocol, not file paths
- Image GUID is correct (check `.meta` file)
- Image file exists in `assets/resources/images/`
- Image has `.meta` file with correct GUID

### Styling Not Applied

**Check:**
- CSS GUID in `.upage` is correct
- CSS syntax is valid (check RmlUi docs)
- CSS selectors match RML structure
- No conflicting styles with higher specificity

## Resources

- [RmlUi Official Documentation](https://mikke89.github.io/RmlUiDoc/)
- [RmlUi RCSS (CSS) Reference](https://mikke89.github.io/RmlUiDoc/pages/rcss.html)
- [RmlUi RML (HTML) Reference](https://mikke89.github.io/RmlUiDoc/pages/rml.html)
- [RmlUi GitHub](https://github.com/mikke89/RmlUi)

## Related Documentation

- [FSM System](creating-actions.md) - Creating FSM actions
- [Material System](material.md) - Material creation and GUID references
- [Asset Creation](../../../.claude/rules/asset-creation.md) - Asset management and MCP tools