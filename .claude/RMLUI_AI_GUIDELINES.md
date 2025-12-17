# RmlUi Development Guidelines for AI Assistance

This document provides comprehensive guidelines for AI assistants (like Claude) when working with RmlUi RML/CSS files.

## Table of Contents

- [Overview](#overview)
- [Core Concepts](#core-concepts)
- [Key Differences from HTML/CSS](#key-differences-from-htmlcss)
- [Supported CSS3 Features](#supported-css3-features)
- [Document Structure](#document-structure)
- [RML Elements Reference](#rml-elements-reference)
- [Styling Requirements](#styling-requirements)
- [Special RmlUi Features](#special-rmlui-features)
- [RCSS Property Guidelines](#rcss-property-guidelines)
- [Best Practices](#best-practices)
- [Common Pitfalls](#common-pitfalls)
- [Example Documents](#example-documents)
- [Resources](#resources)

---

## Overview

**RmlUi** is a C++ user interface library based on HTML/CSS standards, designed for real-time applications like games. It's lightweight, performant, and has its own layout engine.

### What RmlUi Uses

- **RML (RML Markup Language)**: Similar to XHTML1.0, must be well-formed XML
- **RCSS (RmlUi Cascading Style Sheets)**: Based on CSS2 with select CSS3 features

### Philosophy

- Light-weight and performant
- No default styles (unlike web browsers)
- Specifically designed for real-time applications
- Similar to web standards but optimized for performance

---

## Core Concepts

### Documents, Elements, and Properties

- **Document**: A single RML file representing a UI screen or component
- **Element**: Individual components in the document tree (like HTML elements)
- **Properties**: CSS-like styling attributes applied to elements
- **Context**: A collection of documents (similar to a browser window)

### Rendering Pipeline

1. RML/RCSS files are parsed
2. Element tree is constructed
3. Styles are applied
4. Layout is calculated
5. Vertices, indices, and draw commands are generated
6. Application renders using its own renderer

---

## Key Differences from HTML/CSS

### Critical Differences

| Feature | HTML/CSS | RmlUi |
|---------|----------|-------|
| Root element | `<html>` | `<rml>` |
| XML compliance | Not required | **Required** - all tags must close |
| Default styles | Yes (user agent stylesheet) | **None** - must style everything |
| CSS version | CSS3+ | CSS2 + select CSS3 features |
| Encoding | Various | UTF-8 assumed |
| Namespaces | Supported | Not supported |

### Syntax Requirements

1. **All tags must be closed** (self-closing or with closing tag)
   ```xml
   <!-- Correct -->
   <div>Content</div>
   <br/>
   <img src="image.png"/>
   
   <!-- Incorrect -->
   <div>Content
   <br>
   <img src="image.png">
   ```

2. **Case sensitivity**: Tag names are case-insensitive, but attributes are case-sensitive where noted

3. **Attributes must be quoted**
   ```xml
   <!-- Correct -->
   <div id="myDiv" class="container">
   
   <!-- Incorrect -->
   <div id=myDiv class=container>
   ```

---

## Supported CSS3 Features

RmlUi supports most of CSS2 plus these CSS3 features:

### Fully Supported

- ✅ **Animations and transitions**
- ✅ **Transforms** (2D and 3D with full interpolation)
- ✅ **Flexbox layout**
- ✅ **Media queries**
- ✅ **Border radius**
- ✅ **Box shadows**
- ✅ **Mask images**
- ✅ **Gradients** (linear, radial, conic) - via decorators
- ✅ **Filters and backdrop filters** (blur, drop-shadow, etc.)

### Not Supported or Limited

- ❌ CSS Grid (use Flexbox instead)
- ❌ Many CSS3 selectors
- ❌ CSS Variables (use preprocessors if needed)
- ⚠️ Limited pseudo-classes and pseudo-elements

**Always check the [RCSS Property Index](https://mikke89.github.io/RmlUiDoc/pages/rcss/property_index.html) for complete support details.**

---

## Document Structure

### Basic RML Document

```xml
<rml>
<head>
    <title>Document Title</title>
    <link type="text/rcss" href="style.rcss"/>
    <style>
        /* Inline styles can go here */
        body {
            font-family: Arial;
            font-size: 16px;
        }
    </style>
</head>
<body>
    <!-- Document content goes here -->
    <h1>Hello RmlUi</h1>
    <p>This is a paragraph.</p>
</body>
</rml>
```

### Head Section Elements

#### `<title>`
Defines the document title (often used for window title bars)
```xml
<title>My Game Menu</title>
```

#### `<link>` - External Resources
```xml
<!-- Link to external stylesheet -->
<link type="text/rcss" href="style.rcss"/>

<!-- Link to template -->
<link type="text/template" href="window_template.rml"/>
```

**Link Types:**
- `text/rcss` - RmlUi Style Sheet
- `text/template` - RmlUi Template

#### `<style>` - Inline Styles
```xml
<style>
    .button {
        padding: 10px;
        background-color: #4CAF50;
        color: white;
    }
</style>
```

#### `<script>` - Scripting (requires plugin)
```xml
<!-- External script -->
<script src="script.lua"/>

<!-- Inline script -->
<script>
    -- Lua code here
</script>
```

---

## RML Elements Reference

### Core Structural Elements

```xml
<rml>     <!-- Root element (replaces <html>) -->
<head>    <!-- Document metadata -->
<body>    <!-- Document content -->
<div>     <!-- Generic block container (NO default style) -->
<span>    <!-- Generic inline container (NO default style) -->
```

### Text Elements

```xml
<h1> to <h6>   <!-- Headings (NO default style) -->
<p>            <!-- Paragraph (NO default style) -->
<br/>          <!-- Line break -->
<strong>       <!-- Strong emphasis (NO default style) -->
<em>           <!-- Emphasis (NO default style) -->
```

### Form Elements

```xml
<form>         <!-- Form container -->
<input>        <!-- Input field (various types) -->
<textarea>     <!-- Multi-line text input -->
<select>       <!-- Dropdown select -->
<option>       <!-- Select option -->
<label>        <!-- Form label -->
<progress>     <!-- Progress bar -->
```

#### Input Types
```xml
<input type="text"/>        <!-- Text input -->
<input type="password"/>    <!-- Password input -->
<input type="radio"/>       <!-- Radio button -->
<input type="checkbox"/>    <!-- Checkbox -->
<input type="range"/>       <!-- Slider -->
<input type="submit"/>      <!-- Submit button -->
<input type="button"/>      <!-- Button -->
```

### Special RmlUi Elements

```xml
<handle>       <!-- Draggable handle for moving elements -->
<img>          <!-- Image element -->
<tabset>       <!-- Tab container -->
<tab>          <!-- Individual tab -->
<panel>        <!-- Tab panel content -->
```

### Plugin Elements (require plugins)

```xml
<script>       <!-- Scripting (Lua plugin) -->
<lottie>       <!-- Lottie animations (Lottie plugin) -->
<svg>          <!-- SVG images (SVG plugin) -->
```

### Common Attributes

All elements support these base attributes:

```xml
id="unique_id"           <!-- Unique identifier (Case Sensitive) -->
class="class1 class2"    <!-- Space-separated class names (Case Insensitive) -->
style="prop: value;"     <!-- Inline styles -->
```

---

## Styling Requirements

### Critical: No Default Styles!

**RmlUi provides NO default styling.** This means:

- `<div>` has no display type
- `<h1>` is not bold or large
- `<input>` has no appearance
- Tables have no borders
- Nothing has margins or padding by default

### You Must Define Everything

```css
/* Basic element behavior */
div {
    display: block;
}

span {
    display: inline;
}

h1 {
    display: block;
    font-size: 2em;
    font-weight: bold;
    margin: 0.67em 0;
}

p {
    display: block;
    margin: 1em 0;
}

/* Form elements need complete styling */
input.text {
    display: inline-block;
    background-color: #fff;
    color: #333;
    border: 1px solid #999;
    padding: 5px;
    font-size: 14px;
    box-sizing: border-box;
}

input.text:focus {
    border-color: #4CAF50;
    outline: none;
}
```

### Recommended Base Stylesheet

RmlUi provides a recommended base stylesheet at: [rml.rcss](https://github.com/mikke89/RmlUi/blob/master/Samples/assets/rml.rcss)

Include it as your foundation:
```xml
<link type="text/rcss" href="rml.rcss"/>
```

---

## Special RmlUi Features

### 1. Data Bindings (Model-View-Controller)

Data bindings allow synchronization between application data and UI elements.

#### Setting up Data Bindings

```xml
<body data-model="game_state">
    <!-- Display bound variable -->
    <p>Score: {{player_score}}</p>
    
    <!-- Input bound to variable -->
    <input type="text" data-value="player_name"/>
    
    <!-- Conditional rendering -->
    <div data-if="game_over">
        <h1>Game Over!</h1>
    </div>
    
    <!-- Loop through array -->
    <div data-for="item : inventory">
        <p>{{item.name}}: {{item.quantity}}</p>
    </div>
</body>
```

#### Data Binding Attributes

- `data-model="name"` - Associates element tree with a data model
- `{{variable}}` - Displays variable value
- `data-value="var"` - Two-way binding for form inputs
- `data-if="condition"` - Conditional rendering
- `data-for="item : array"` - Loop rendering
- `data-attr-*="var"` - Bind variable to HTML attribute

### 2. Templates

Templates allow reusable layouts across multiple documents.

#### Defining a Template

**window_template.rml:**
```xml
<template name="window" content="content">
<head>
    <link type="text/rcss" href="window.rcss"/>
</head>
<body>
    <div class="window-frame">
        <div class="title-bar">
            <h1>Window Title</h1>
            <button class="close">X</button>
        </div>
        <div class="content-area" id="content">
            <!-- Content will be injected here -->
        </div>
    </div>
</body>
</template>
```

#### Using a Template

```xml
<rml>
<head>
    <title>My Window</title>
    <link type="text/template" href="window_template.rml"/>
</head>
<body template="window">
    <!-- This content will be placed in the template's #content element -->
    <p>This is the window content.</p>
    <button>Click Me</button>
</body>
</rml>
```

#### Inline Template Injection

```xml
<body>
    <div class="container">
        <template src="window">
            <p>Content injected inline</p>
        </template>
    </div>
</body>
```

### 3. Decorators

Decorators are advanced styling features beyond standard CSS.

#### Built-in Decorators

**Image Decorator:**
```css
.logo {
    decorator: image;
    image-src: logo.png;
    width: 200px;
    height: 100px;
}
```

**Linear Gradient:**
```css
.gradient-bg {
    decorator: linear-gradient;
    linear-gradient-start: 0deg;
    linear-gradient-stop-0: #ff0000;
    linear-gradient-stop-1: #0000ff;
}
```

**Radial Gradient:**
```css
.radial-bg {
    decorator: radial-gradient;
    radial-gradient-shape: circle;
    radial-gradient-stop-0: rgba(255, 255, 0, 1);
    radial-gradient-stop-1: rgba(255, 0, 0, 0);
}
```

**Conic Gradient:**
```css
.conic-bg {
    decorator: conic-gradient;
    conic-gradient-center: 50% 50%;
    conic-gradient-stop-0: #ff0000 0deg;
    conic-gradient-stop-1: #00ff00 120deg;
    conic-gradient-stop-2: #0000ff 240deg;
}
```

**Tiled Box (Nine-Patch):**
```css
.panel {
    decorator: tiled-box;
    tiled-box-image-src: panel.png;
    tiled-box-top: 20px;
    tiled-box-right: 20px;
    tiled-box-bottom: 20px;
    tiled-box-left: 20px;
}
```

**Ninepatch:**
```css
.button {
    decorator: ninepatch;
    ninepatch-image-src: button.png;
    ninepatch-s0: 10px;
    ninepatch-s1: 90px;
    ninepatch-t0: 10px;
    ninepatch-t1: 40px;
}
```

### 4. Sprite Sheets

Efficient texture management with high-DPI support.

**Defining Sprites in RCSS:**
```css
@spritesheet theme {
    src: ui_sprites.png;
    
    button-normal: 0px 0px 100px 32px;
    button-hover:  0px 32px 100px 32px;
    button-active: 0px 64px 100px 32px;
    
    icon-sword:    100px 0px 32px 32px;
    icon-shield:   132px 0px 32px 32px;
}

.button {
    decorator: image;
    image-src: button-normal;
}

.button:hover {
    image-src: button-hover;
}

.icon-sword {
    decorator: image;
    image-src: icon-sword;
    width: 32px;
    height: 32px;
}
```

### 5. Animations and Transitions

#### Transitions
```css
.button {
    background-color: #4CAF50;
    transform: scale(1);
    transition: background-color 0.3s, transform 0.2s;
}

.button:hover {
    background-color: #45a049;
    transform: scale(1.05);
}
```

#### Keyframe Animations
```css
@keyframes pulse {
    0% { transform: scale(1); opacity: 1; }
    50% { transform: scale(1.1); opacity: 0.8; }
    100% { transform: scale(1); opacity: 1; }
}

.notification {
    animation: pulse 2s infinite ease-in-out;
}
```

### 6. Transforms

Full 2D and 3D transform support:

```css
.rotated {
    transform: rotate(45deg);
}

.scaled {
    transform: scale(1.5);
}

.translated {
    transform: translate(50px, 20px);
}

.combined {
    transform: translate(100px, 0) rotate(45deg) scale(1.2);
}

/* 3D transforms */
.cube-face {
    transform: perspective(1000px) rotateY(45deg);
    transform-origin: center center;
}
```

### 7. Filters

```css
.blurred {
    filter: blur(5px);
}

.darkened {
    filter: brightness(0.5);
}

.grayscale {
    filter: grayscale(100%);
}

.drop-shadow {
    filter: drop-shadow(2px 2px 4px #000000);
}

/* Multiple filters */
.fancy {
    filter: blur(2px) brightness(1.2) contrast(1.1);
}

/* Backdrop filter (affects content behind element) */
.glass-panel {
    backdrop-filter: blur(10px) brightness(0.8);
    background-color: rgba(255, 255, 255, 0.2);
}
```

### 8. Localization

Translate text content easily:

```xml
<!-- In RML -->
<p>Welcome</p>

<!-- Translation file (JSON) -->
{
    "Welcome": "Bienvenue"
}
```

Load translations via C++ API.

---

## RCSS Property Guidelines

### Display and Positioning

```css
/* Display types */
display: none | inline | block | inline-block | flex | table | table-row | table-cell;

/* Position types */
position: static | relative | absolute | fixed;

/* Float */
float: none | left | right;

/* Clear */
clear: none | left | right | both;
```

### Box Model

```css
/* Margins (can use em, px, %, auto) */
margin: 10px;
margin: 10px 20px;
margin: 10px 20px 10px 20px;
margin-top: 10px;
margin-right: 20px;
margin-bottom: 10px;
margin-left: 20px;

/* Padding */
padding: 10px;
padding: 10px 20px;
/* ... same pattern as margin */

/* Borders */
border: 2px #ff0000;
border-width: 2px;
border-color: #ff0000;
border-radius: 5px;  /* CSS3 feature */

/* Width and height */
width: 100px | 50% | auto;
height: 100px | 50% | auto;
min-width: 50px;
max-width: 500px;
min-height: 50px;
max-height: 500px;

/* Box sizing */
box-sizing: content-box | border-box;
```

### Typography

```css
/* Font family */
font-family: Arial, sans-serif;

/* Font size (px, em, %, pt) */
font-size: 16px;

/* Font weight */
font-weight: normal | bold | 100-900;

/* Font style */
font-style: normal | italic;

/* Text alignment */
text-align: left | right | center | justify;

/* Text decoration */
text-decoration: none | underline | overline | line-through;

/* Text transform */
text-transform: none | capitalize | uppercase | lowercase;

/* Line height */
line-height: 1.5 | 24px;

/* Letter spacing */
letter-spacing: 1px;

/* Word spacing */
word-spacing: 2px;

/* Color */
color: #ff0000 | rgb(255, 0, 0) | rgba(255, 0, 0, 0.5);
```

### Colors and Backgrounds

```css
/* Color formats */
color: #rgb;
color: #rrggbb;
color: #rrggbbaa;
color: rgb(255, 0, 0);
color: rgba(255, 0, 0, 0.5);

/* Background color */
background-color: #ffffff;
background-color: transparent;

/* Note: background-image is NOT supported */
/* Use decorators for background images */
```

### Flexbox

```css
.flex-container {
    display: flex;
    flex-direction: row | row-reverse | column | column-reverse;
    flex-wrap: nowrap | wrap | wrap-reverse;
    justify-content: flex-start | flex-end | center | space-between | space-around;
    align-items: flex-start | flex-end | center | baseline | stretch;
    align-content: flex-start | flex-end | center | space-between | space-around | stretch;
    gap: 10px;  /* Space between items */
}

.flex-item {
    flex: 1;  /* Shorthand for flex-grow, flex-shrink, flex-basis */
    flex-grow: 1;
    flex-shrink: 1;
    flex-basis: auto | 100px | 50%;
    align-self: auto | flex-start | flex-end | center | baseline | stretch;
    order: 0;  /* Integer value */
}
```

### Z-Index and Stacking

```css
z-index: auto | 0 | 1 | -1;  /* Integer value */
```

### Overflow

```css
overflow: visible | hidden | auto | scroll;
overflow-x: visible | hidden | auto | scroll;
overflow-y: visible | hidden | auto | scroll;
```

### Cursor

```css
cursor: auto | text | pointer | move | resize | progress;
```

### Opacity

```css
opacity: 1;  /* 0.0 to 1.0 */
```

### Visibility

```css
visibility: visible | hidden;
```

### Clip

```css
clip: auto | rect(0, 100px, 100px, 0);
```

---

## Best Practices

### 1. Structure Your Stylesheets

```css
/* ========================================
   Base Styles
   ======================================== */

body {
    font-family: Arial, sans-serif;
    font-size: 16px;
    color: #333;
}

/* ========================================
   Layout Components
   ======================================== */

.container {
    display: block;
    max-width: 1200px;
    margin: 0 auto;
    padding: 20px;
}

/* ========================================
   UI Components
   ======================================== */

.button {
    /* ... */
}
```

### 2. Use Classes Over IDs for Styling

```xml
<!-- Good: Reusable -->
<button class="btn btn-primary">Click Me</button>

<!-- Less flexible: Single use -->
<button id="submit-button">Submit</button>
```

### 3. Follow BEM or Similar Naming Convention

```css
/* Block */
.card {}

/* Element */
.card__header {}
.card__body {}
.card__footer {}

/* Modifier */
.card--featured {}
.button--large {}
```

### 4. Use Semantic HTML-like Elements

```xml
<!-- Good: Clear intent -->
<div class="nav-menu">
    <button class="nav-item">Home</button>
    <button class="nav-item">About</button>
</div>

<!-- Avoid: Unclear purpose -->
<div class="d1">
    <div class="d2">Home</div>
    <div class="d2">About</div>
</div>
```

### 5. Optimize for Performance

```css
/* Avoid expensive operations in frequently updated elements */
.performance-critical {
    /* Prefer */
    transform: translateX(10px);  /* GPU accelerated */
    
    /* Over */
    left: 10px;  /* May trigger layout recalculation */
}
```

### 6. Use Data Bindings for Dynamic Content

```xml
<!-- Instead of manually updating via code -->
<p id="score">Score: 0</p>

<!-- Use data binding -->
<p>Score: {{player_score}}</p>
```

### 7. Create Reusable Templates

```xml
<!-- Create a template for repeated UI patterns -->
<template name="dialog" content="dialog-content">
    <div class="dialog-overlay">
        <div class="dialog-box">
            <div class="dialog-header">
                <h2>{{title}}</h2>
                <button class="close">×</button>
            </div>
            <div class="dialog-body" id="dialog-content">
                <!-- Content injected here -->
            </div>
        </div>
    </div>
</template>
```

### 8. Use Media Queries for Responsive Design

```css
@media (max-width: 800px) {
    .sidebar {
        display: none;
    }
    
    .main-content {
        width: 100%;
    }
}

@media (orientation: portrait) {
    .layout {
        flex-direction: column;
    }
}
```

### 9. Leverage Sprite Sheets

```css
/* More efficient than individual images */
@spritesheet icons {
    src: icons.png;
    icon-play: 0px 0px 32px 32px;
    icon-pause: 32px 0px 32px 32px;
    icon-stop: 64px 0px 32px 32px;
}
```

### 10. Test Across Different Resolutions

```css
/* Use dp unit for resolution-independent sizing */
.button {
    font-size: 16dp;  /* Scales with DPI */
    padding: 8dp 16dp;
}
```

---

## Common Pitfalls

### 1. ❌ Forgetting to Close Tags

```xml
<!-- Wrong -->
<div>Content
<img src="image.png">

<!-- Correct -->
<div>Content</div>
<img src="image.png"/>
```

### 2. ❌ Assuming Default Styles Exist

```xml
<!-- This won't look like a heading without styling -->
<h1>Title</h1>

<!-- Need to explicitly style it -->
<style>
h1 {
    display: block;
    font-size: 2em;
    font-weight: bold;
}
</style>
```

### 3. ❌ Using Unsupported CSS Properties

```css
/* Not supported - check documentation */
.element {
    grid-template-columns: 1fr 1fr;  /* CSS Grid not supported */
    background-image: url(bg.png);    /* Use decorators instead */
    box-shadow: 0 2px 4px #000;       /* Use box-shadow property correctly */
}

/* Correct alternatives */
.element {
    display: flex;  /* Use Flexbox instead of Grid */
    decorator: image;  /* Use decorators for background images */
    box-shadow: 2px 2px 4px #000000;  /* box-shadow is supported */
}
```

### 4. ❌ Incorrect Unit Usage

```css
/* Be careful with units */
.element {
    width: 50;        /* Wrong - missing unit */
    width: 50px;      /* Correct */
    width: 50%;       /* Correct */
    width: 50em;      /* Correct */
}
```

### 5. ❌ Not Using Box-Sizing

```css
/* Without box-sizing, padding and border add to width */
.box {
    width: 100px;
    padding: 10px;
    border: 2px;
    /* Actual width = 100 + 10*2 + 2*2 = 124px */
}

/* Use box-sizing for predictable sizing */
.box {
    box-sizing: border-box;
    width: 100px;
    padding: 10px;
    border: 2px;
    /* Actual width = 100px (padding and border included) */
}
```

### 6. ❌ Inline Styles Without Semicolons

```xml
<!-- Wrong -->
<div style="width: 100px height: 50px">

<!-- Correct -->
<div style="width: 100px; height: 50px;">
```

### 7. ❌ Case Sensitivity Confusion

```xml
<!-- IDs are case-sensitive -->
<div id="MyDiv"></div>

<!-- Classes are case-insensitive -->
<div class="MyClass"></div>  <!-- Same as class="myclass" -->
```

### 8. ❌ Not Handling Form Element States

```css
/* Don't forget interactive states */
input.text {
    background-color: white;
}

/* Add focus state */
input.text:focus {
    border-color: blue;
    outline: none;
}

/* Add disabled state */
input.text:disabled {
    background-color: #e0e0e0;
    color: #999;
}
```

### 9. ❌ Overusing Absolute Positioning

```css
/* This makes layout rigid and hard to maintain */
.element {
    position: absolute;
    left: 100px;
    top: 50px;
}

/* Prefer flexible layouts */
.container {
    display: flex;
    justify-content: center;
    align-items: center;
}
```

### 10. ❌ Forgetting Data Model Declaration

```xml
<!-- Data bindings won't work without data-model -->
<body>
    <p>{{score}}</p>  <!-- Won't display -->
</body>

<!-- Correct -->
<body data-model="game">
    <p>{{score}}</p>  <!-- Will display score from game model -->
</body>
```

---

## Example Documents

### Example 1: Simple Menu

**menu.rml:**
```xml
<rml>
<head>
    <title>Main Menu</title>
    <link type="text/rcss" href="rml.rcss"/>
    <style>
        body {
            width: 400px;
            height: 300px;
            margin: auto;
            padding: 20px;
            background-color: rgba(0, 0, 0, 0.8);
            border-radius: 10px;
        }
        
        h1 {
            display: block;
            color: white;
            font-size: 2em;
            text-align: center;
            margin-bottom: 1em;
        }
        
        .menu-button {
            display: block;
            width: 100%;
            padding: 15px;
            margin: 10px 0;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            font-size: 18px;
            cursor: pointer;
            text-align: center;
            transition: background-color 0.3s;
        }
        
        .menu-button:hover {
            background-color: #45a049;
        }
        
        .menu-button:active {
            background-color: #3d8b40;
        }
    </style>
</head>
<body>
    <h1>Main Menu</h1>
    <button class="menu-button">New Game</button>
    <button class="menu-button">Load Game</button>
    <button class="menu-button">Options</button>
    <button class="menu-button">Exit</button>
</body>
</rml>
```

### Example 2: Flexbox Layout

**dashboard.rml:**
```xml
<rml>
<head>
    <title>Game Dashboard</title>
    <link type="text/rcss" href="rml.rcss"/>
    <style>
        body {
            display: flex;
            flex-direction: column;
            width: 100%;
            height: 100%;
            background-color: #1a1a1a;
            color: white;
            font-family: Arial;
        }
        
        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 20px;
            background-color: #2a2a2a;
            border-bottom: 2px solid #4CAF50;
        }
        
        .header h1 {
            margin: 0;
            font-size: 24px;
        }
        
        .stats {
            display: flex;
            gap: 20px;
        }
        
        .stat {
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        
        .stat-label {
            font-size: 12px;
            color: #999;
        }
        
        .stat-value {
            font-size: 20px;
            font-weight: bold;
            color: #4CAF50;
        }
        
        .main-content {
            display: flex;
            flex: 1;
            gap: 20px;
            padding: 20px;
        }
        
        .sidebar {
            display: flex;
            flex-direction: column;
            width: 200px;
            background-color: #2a2a2a;
            padding: 15px;
            border-radius: 5px;
        }
        
        .content-area {
            flex: 1;
            background-color: #2a2a2a;
            padding: 20px;
            border-radius: 5px;
        }
    </style>
</head>
<body>
    <div class="header">
        <h1>Dashboard</h1>
        <div class="stats">
            <div class="stat">
                <span class="stat-label">Health</span>
                <span class="stat-value">100</span>
            </div>
            <div class="stat">
                <span class="stat-label">Gold</span>
                <span class="stat-value">500</span>
            </div>
            <div class="stat">
                <span class="stat-label">Level</span>
                <span class="stat-value">15</span>
            </div>
        </div>
    </div>
    
    <div class="main-content">
        <div class="sidebar">
            <h2>Menu</h2>
            <button>Inventory</button>
            <button>Skills</button>
            <button>Quests</button>
        </div>
        
        <div class="content-area">
            <h2>Content</h2>
            <p>Main content area goes here...</p>
        </div>
    </div>
</body>
</rml>
```

### Example 3: Data Binding

**scoreboard.rml:**
```xml
<rml>
<head>
    <title>Scoreboard</title>
    <link type="text/rcss" href="rml.rcss"/>
    <style>
        body {
            width: 400px;
            padding: 20px;
            background-color: rgba(0, 0, 0, 0.9);
            color: white;
        }
        
        .score-display {
            display: block;
            font-size: 32px;
            text-align: center;
            margin: 20px 0;
            color: #4CAF50;
        }
        
        .player-list {
            display: flex;
            flex-direction: column;
            gap: 10px;
        }
        
        .player-item {
            display: flex;
            justify-content: space-between;
            padding: 10px;
            background-color: rgba(255, 255, 255, 0.1);
            border-radius: 5px;
        }
        
        .game-over {
            display: block;
            text-align: center;
            padding: 30px;
            background-color: rgba(255, 0, 0, 0.2);
            border: 2px solid red;
            border-radius: 10px;
            margin: 20px 0;
        }
    </style>
</head>
<body data-model="game">
    <h1>Current Game</h1>
    
    <div class="score-display">
        Score: {{current_score}}
    </div>
    
    <div data-if="is_game_over" class="game-over">
        <h2>Game Over!</h2>
        <p>Final Score: {{final_score}}</p>
    </div>
    
    <h2>Players</h2>
    <div class="player-list">
        <div data-for="player : players" class="player-item">
            <span>{{player.name}}</span>
            <span>{{player.score}}</span>
        </div>
    </div>
    
    <input type="text" data-value="player_name" placeholder="Enter name"/>
</body>
</rml>
```

### Example 4: Form with Validation States

**login.rml:**
```xml
<rml>
<head>
    <title>Login</title>
    <link type="text/rcss" href="rml.rcss"/>
    <style>
        body {
            width: 300px;
            padding: 30px;
            margin: auto;
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 8px;
        }
        
        h1 {
            display: block;
            text-align: center;
            color: #333;
            margin-bottom: 1em;
        }
        
        .form-group {
            display: block;
            margin-bottom: 15px;
        }
        
        label {
            display: block;
            margin-bottom: 5px;
            color: #555;
            font-size: 14px;
        }
        
        input.text {
            display: block;
            width: 100%;
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 4px;
            font-size: 14px;
            box-sizing: border-box;
        }
        
        input.text:focus {
            border-color: #4CAF50;
            outline: none;
        }
        
        input.text.error {
            border-color: #f44336;
        }
        
        .error-message {
            display: block;
            color: #f44336;
            font-size: 12px;
            margin-top: 5px;
        }
        
        .submit-button {
            display: block;
            width: 100%;
            padding: 12px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            font-size: 16px;
            cursor: pointer;
            margin-top: 20px;
        }
        
        .submit-button:hover {
            background-color: #45a049;
        }
        
        .submit-button:disabled {
            background-color: #cccccc;
            cursor: not-allowed;
        }
    </style>
</head>
<body>
    <h1>Login</h1>
    
    <form>
        <div class="form-group">
            <label for="username">Username</label>
            <input type="text" id="username" class="text" placeholder="Enter username"/>
            <span class="error-message" style="display: none;">Username is required</span>
        </div>
        
        <div class="form-group">
            <label for="password">Password</label>
            <input type="password" id="password" class="text" placeholder="Enter password"/>
            <span class="error-message" style="display: none;">Password is required</span>
        </div>
        
        <input type="submit" class="submit-button" value="Login"/>
    </form>
</body>
</rml>
```

### Example 5: Animated Components

**notifications.rml:**
```xml
<rml>
<head>
    <title>Notifications</title>
    <link type="text/rcss" href="rml.rcss"/>
    <style>
        @keyframes slideIn {
            0% {
                transform: translateX(400px);
                opacity: 0;
            }
            100% {
                transform: translateX(0);
                opacity: 1;
            }
        }
        
        @keyframes fadeOut {
            0% {
                opacity: 1;
            }
            100% {
                opacity: 0;
            }
        }
        
        body {
            position: fixed;
            top: 20px;
            right: 20px;
            width: 350px;
        }
        
        .notification {
            display: block;
            padding: 15px;
            margin-bottom: 10px;
            background-color: white;
            border-left: 4px solid #4CAF50;
            border-radius: 4px;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.2);
            animation: slideIn 0.3s ease-out;
        }
        
        .notification.success {
            border-left-color: #4CAF50;
        }
        
        .notification.error {
            border-left-color: #f44336;
        }
        
        .notification.warning {
            border-left-color: #ff9800;
        }
        
        .notification.info {
            border-left-color: #2196F3;
        }
        
        .notification-title {
            display: block;
            font-weight: bold;
            margin-bottom: 5px;
            color: #333;
        }
        
        .notification-message {
            display: block;
            color: #666;
            font-size: 14px;
        }
        
        .close-button {
            float: right;
            background: none;
            border: none;
            color: #999;
            cursor: pointer;
            font-size: 20px;
            padding: 0;
            margin-top: -5px;
        }
        
        .close-button:hover {
            color: #333;
        }
    </style>
</head>
<body>
    <div class="notification success">
        <button class="close-button">×</button>
        <span class="notification-title">Success!</span>
        <span class="notification-message">Your changes have been saved.</span>
    </div>
    
    <div class="notification error">
        <button class="close-button">×</button>
        <span class="notification-title">Error</span>
        <span class="notification-message">Failed to connect to server.</span>
    </div>
    
    <div class="notification warning">
        <button class="close-button">×</button>
        <span class="notification-title">Warning</span>
        <span class="notification-message">Your session will expire soon.</span>
    </div>
</body>
</rml>
```

---

## Resources

### Official Documentation

- **Main Documentation**: [https://mikke89.github.io/RmlUiDoc/](https://mikke89.github.io/RmlUiDoc/)
- **GitHub Repository**: [https://github.com/mikke89/RmlUi](https://github.com/mikke89/RmlUi)
- **RML Elements Index**: [https://mikke89.github.io/RmlUiDoc/pages/rml/element_index.html](https://mikke89.github.io/RmlUiDoc/pages/rml/element_index.html)
- **RCSS Property Index**: [https://mikke89.github.io/RmlUiDoc/pages/rcss/property_index.html](https://mikke89.github.io/RmlUiDoc/pages/rcss/property_index.html)
- **Style Guide**: [https://mikke89.github.io/RmlUiDoc/pages/style_guide.html](https://mikke89.github.io/RmlUiDoc/pages/style_guide.html)

### Specific Topics

- **Data Bindings**: [https://mikke89.github.io/RmlUiDoc/pages/data_bindings.html](https://mikke89.github.io/RmlUiDoc/pages/data_bindings.html)
- **Decorators**: [https://mikke89.github.io/RmlUiDoc/pages/rcss/decorators.html](https://mikke89.github.io/RmlUiDoc/pages/rcss/decorators.html)
- **Sprite Sheets**: [https://mikke89.github.io/RmlUiDoc/pages/rcss/sprite_sheets.html](https://mikke89.github.io/RmlUiDoc/pages/rcss/sprite_sheets.html)
- **Animations**: [https://mikke89.github.io/RmlUiDoc/pages/rcss/animations_transitions_transforms.html](https://mikke89.github.io/RmlUiDoc/pages/rcss/animations_transitions_transforms.html)
- **Flexbox**: [https://mikke89.github.io/RmlUiDoc/pages/rcss/flexboxes.html](https://mikke89.github.io/RmlUiDoc/pages/rcss/flexboxes.html)
- **Filters**: [https://mikke89.github.io/RmlUiDoc/pages/rcss/filters.html](https://mikke89.github.io/RmlUiDoc/pages/rcss/filters.html)

### Sample Assets

- **Base Stylesheet (rml.rcss)**: [https://github.com/mikke89/RmlUi/blob/master/Samples/assets/rml.rcss](https://github.com/mikke89/RmlUi/blob/master/Samples/assets/rml.rcss)
- **Sample Applications**: [https://github.com/mikke89/RmlUi/tree/master/Samples](https://github.com/mikke89/RmlUi/tree/master/Samples)

### Reference Standards

- **CSS2 Specification**: [https://www.w3.org/TR/CSS2/](https://www.w3.org/TR/CSS2/)
- **CSS3 Working Draft**: [https://www.w3.org/Style/CSS/current-work](https://www.w3.org/Style/CSS/current-work)

### Community

- **Gitter Chat**: [https://gitter.im/RmlUi/community](https://gitter.im/RmlUi/community)
- **GitHub Issues**: [https://github.com/mikke89/RmlUi/issues](https://github.com/mikke89/RmlUi/issues)
- **GitHub Discussions**: [https://github.com/mikke89/RmlUi/discussions](https://github.com/mikke89/RmlUi/discussions)

---

## Quick Reference Card

### Document Template
```xml
<rml>
<head>
    <title>Title</title>
    <link type="text/rcss" href="style.rcss"/>
</head>
<body>
    <!-- Content -->
</body>
</rml>
```

### Common Elements
- `<div>`, `<span>` - Containers (no default style)
- `<h1>` to `<h6>` - Headings (no default style)
- `<p>` - Paragraph (no default style)
- `<button>`, `<input>`, `<textarea>` - Form controls
- `<img>` - Images

### Essential CSS
```css
body { font-family: Arial; font-size: 16px; }
div { display: block; }
span { display: inline; }
.flex { display: flex; }
```

### Data Binding
- `data-model="name"` - Declare model
- `{{variable}}` - Display value
- `data-value="var"` - Two-way binding
- `data-if="condition"` - Conditional
- `data-for="item : array"` - Loop

### Units
- `px` - Pixels
- `dp` - Density-independent pixels (DPI aware)
- `em` - Relative to font size
- `%` - Percentage
- `#rrggbb` - Colors

---

**Version**: Based on RmlUi master branch (December 2024)  
**License**: MIT License  
**Maintained by**: The RmlUi Community

For questions or contributions, visit the [RmlUi GitHub repository](https://github.com/mikke89/RmlUi).
