# Procedural Animation System Research

> Research document for IK-driven, physics-based procedural animation system.
> Date: 2024
> Status: Research phase

## Overview

This document outlines a procedural animation system where **all motion is driven by IK rules and physics**, not keyframes. The core concept is that legs automatically reposition to support the center of mass, and secondary motion (arms, head) uses wave functions or IK targets.

## Core Philosophy

- **No keyframes** - All motion emerges from rules
- **Physics-based** - Center of mass must be supported
- **IK-driven** - All chains solve to targets/constraints
- **Adaptive** - Works on any terrain, any speed
- **Runtime controllable** - Game logic sets targets, system handles motion

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    CENTER OF MASS (CoM)                      │
│                          ●                                   │
│                         /│\                                  │
│                        / │ \                                 │
│              ┌────────/  │  \────────┐                       │
│              │       spine/torso     │  ← IK keeps balanced  │
│              │           │           │                       │
│         left_arm    ┌────┴────┐    right_arm                 │
│         (wave IK)   │   hips  │    (wave IK)                 │
│                     └────┬────┘                              │
│                    ┌─────┴─────┐                             │
│               left_leg       right_leg                       │
│                  │               │     ← IK to ground        │
│                  ▼               ▼                           │
│   ═══════════●═══════════════════●═══════════════            │
│           (foot)    SUPPORT    (foot)                        │
│                     POLYGON                                  │
└─────────────────────────────────────────────────────────────┘

Rule: CoM must stay within support polygon or legs reposition
```

---

## Libraries

| Library | Purpose | WASM Support |
|---------|---------|--------------|
| **[Manifold](https://github.com/elalish/manifold)** | CSG Boolean operations for mesh generation | ✅ |
| **[TinySpline](https://github.com/msteinbeck/tinyspline)** | Bezier/NURBS curve evaluation | ✅ |
| **[tinyexpr](https://github.com/codeplea/tinyexpr)** | Math expression parsing for wave functions | ✅ |

**Note:** No external animation library needed - the engine's existing animation system handles bone transforms. This system generates the transforms procedurally.

---

## YAML Format Specification

### Complete Example

```yaml
name: biped_creature
type: procedural_mesh

# ============================================
# MESH GENERATION (Manifold + TinySpline)
# ============================================
shapes:
  - id: torso
    type: box
    size: [1.0, 1.5, 0.5]
    position: [0.0, 1.25, 0.0]

  - id: leg_profile
    type: bezier_curve
    control_points:
      - [0.15, 0.0, 0.0]
      - [0.12, -0.4, 0.0]
      - [0.1, -0.8, 0.0]
    degree: 3

  # ... more shapes ...

operations:
  - type: union
    a: torso
    b: head
    result: body
  # ... more operations ...

output: final_mesh

# ============================================
# SKELETON
# ============================================
skeleton:
  bones:
    - name: root
      position: [0, 0, 0]
      children:
        - name: hip
          position: [0, 1.0, 0]
          children:
            - name: spine
              position: [0, 0.3, 0]
              children:
                - name: chest
                  position: [0, 0.3, 0]
                  children:
                    - name: neck
                      position: [0, 0.2, 0]
                      children:
                        - name: head
                          position: [0, 0.15, 0]
                    - name: shoulder_L
                      position: [-0.25, 0.1, 0]
                      children:
                        - name: arm_L
                          position: [-0.2, 0, 0]
                          children:
                            - name: forearm_L
                              position: [-0.25, 0, 0]
                              children:
                                - name: hand_L
                                  position: [-0.2, 0, 0]
                    - name: shoulder_R
                      position: [0.25, 0.1, 0]
                      children:
                        - name: arm_R
                          position: [0.2, 0, 0]
                          children:
                            - name: forearm_R
                              position: [0.25, 0, 0]
                              children:
                                - name: hand_R
                                  position: [0.2, 0, 0]
            - name: hip_L
              position: [-0.15, 0, 0]
              children:
                - name: thigh_L
                  position: [0, -0.4, 0]
                  children:
                    - name: shin_L
                      position: [0, -0.4, 0]
                      children:
                        - name: foot_L
                          position: [0, -0.1, 0.05]
            - name: hip_R
              position: [0.15, 0, 0]
              children:
                - name: thigh_R
                  position: [0, -0.4, 0]
                  children:
                    - name: shin_R
                      position: [0, -0.4, 0]
                      children:
                        - name: foot_R
                          position: [0, -0.1, 0.05]

# ============================================
# PHYSICS & BALANCE SYSTEM
# ============================================
physics:
  center_of_mass:
    mode: computed  # or "fixed"
    masses:
      - bone: head
        mass: 5.0
      - bone: chest
        mass: 20.0
      - bone: spine
        mass: 15.0
      - bone: hip
        mass: 10.0
      - bone: thigh_L
        mass: 8.0
      - bone: thigh_R
        mass: 8.0
      - bone: shin_L
        mass: 4.0
      - bone: shin_R
        mass: 4.0
      - bone: arm_L
        mass: 3.0
      - bone: arm_R
        mass: 3.0
      - bone: forearm_L
        mass: 2.0
      - bone: forearm_R
        mass: 2.0

  gravity: [0, -9.81, 0]

  ground:
    detection: raycast  # or "plane"
    plane_y: 0.0
    max_step_height: 0.3
    max_slope_angle: 45.0

# ============================================
# IK CHAINS
# ============================================
ik_chains:
  # ──────────────────────────────────────────
  # LEGS - Support the center of mass
  # ──────────────────────────────────────────
  - name: leg_L
    type: support
    bones: [hip_L, thigh_L, shin_L, foot_L]
    end_effector: foot_L

    constraints:
      - joint: thigh_L
        type: hinge
        axis: [1, 0, 0]
        limits: [-120, 10]

      - joint: shin_L
        type: hinge
        axis: [1, 0, 0]
        limits: [0, 150]

      - joint: foot_L
        type: ball
        limits: [-30, 30, -20, 20, -15, 15]

    foot:
      contact_points:
        - offset: [0, 0, 0.1]    # toe
        - offset: [0, 0, -0.08]  # heel
      sticky: true
      align_to_ground: true

  - name: leg_R
    type: support
    bones: [hip_R, thigh_R, shin_R, foot_R]
    end_effector: foot_R
    constraints:
      - joint: thigh_R
        type: hinge
        axis: [1, 0, 0]
        limits: [-120, 10]
      - joint: shin_R
        type: hinge
        axis: [1, 0, 0]
        limits: [0, 150]
      - joint: foot_R
        type: ball
        limits: [-30, 30, -20, 20, -15, 15]
    foot:
      contact_points:
        - offset: [0, 0, 0.1]
        - offset: [0, 0, -0.08]
      sticky: true
      align_to_ground: true

  # ──────────────────────────────────────────
  # SPINE - Keeps torso balanced over support
  # ──────────────────────────────────────────
  - name: spine_chain
    type: balance
    bones: [hip, spine, chest]

    constraints:
      - joint: spine
        type: ball
        limits: [-30, 45, -40, 40, -20, 20]
      - joint: chest
        type: ball
        limits: [-20, 30, -30, 30, -15, 15]

    rules:
      - type: keep_upright
        target_up: [0, 1, 0]
        strength: 0.8

      - type: lean_towards_velocity
        strength: 0.3
        max_lean: 15.0

  # ──────────────────────────────────────────
  # HEAD - Look at target or forward
  # ──────────────────────────────────────────
  - name: head_chain
    type: look_at
    bones: [neck, head]
    end_effector: head

    constraints:
      - joint: neck
        type: ball
        limits: [-40, 40, -60, 60, -20, 20]
      - joint: head
        type: ball
        limits: [-30, 30, -70, 70, -15, 15]

    rules:
      - type: look_at_target
        target: look_target
        speed: 5.0
      - type: default_forward
        direction: [0, 0, 1]
        when: no_target

  # ──────────────────────────────────────────
  # ARMS - Reach or idle wave motion
  # ──────────────────────────────────────────
  - name: arm_L
    type: reach
    bones: [shoulder_L, arm_L, forearm_L, hand_L]
    end_effector: hand_L
    pole_target: elbow_L_hint

    constraints:
      - joint: shoulder_L
        type: ball
        limits: [-45, 160, -30, 90, -90, 90]
      - joint: arm_L
        type: hinge
        axis: [0, 1, 0]
        limits: [0, 145]
      - joint: forearm_L
        type: twist
        axis: [1, 0, 0]
        limits: [-90, 90]

    rules:
      - type: reach_target
        target: left_hand_target
        priority: high
      - type: idle_motion
        when: no_target

  - name: arm_R
    type: reach
    bones: [shoulder_R, arm_R, forearm_R, hand_R]
    end_effector: hand_R
    pole_target: elbow_R_hint
    constraints:
      - joint: shoulder_R
        type: ball
        limits: [-45, 160, -30, 90, -90, 90]
      - joint: arm_R
        type: hinge
        axis: [0, 1, 0]
        limits: [0, 145]
      - joint: forearm_R
        type: twist
        axis: [1, 0, 0]
        limits: [-90, 90]
    rules:
      - type: reach_target
        target: right_hand_target
        priority: high
      - type: idle_motion
        when: no_target

# ============================================
# LOCOMOTION RULES
# ============================================
locomotion:
  balance:
    support_polygon:
      mode: convex_hull
      margin: 0.05

    stability_threshold: 0.1

    on_unstable:
      - type: lean_torso
        max_compensation: 15
      - type: step

  stepping:
    trigger:
      - condition: com_outside_support
        margin: 0.05
      - condition: foot_too_far
        max_distance: 0.8
      - condition: foot_too_close
        min_distance: 0.15
      - condition: velocity_requires_step
        speed_threshold: 0.3

    selection:
      prefer: furthest_from_target
      never_both: true

    placement:
      mode: predictive
      prediction_time: 0.3
      lateral_offset: 0.2
      forward_offset: 0.0
      align_to_velocity: true
      avoid_obstacles: true

    trajectory:
      type: arc
      height: 0.15
      duration: 0.25
      ease: ease_in_out
      toe_lift: 30
      plant_angle: 0

  gaits:
    - name: stand
      speed_range: [0, 0.1]
      step_frequency: 0

    - name: walk
      speed_range: [0.1, 1.5]
      step_frequency: auto
      stride_length: 0.6
      double_support_ratio: 0.3

    - name: run
      speed_range: [1.5, 5.0]
      step_frequency: auto
      stride_length: 1.0
      flight_phase: true
      lean_forward: 20

# ============================================
# WAVE FUNCTIONS (Secondary Motion)
# ============================================
wave_functions:
  - name: arm_idle
    apply_to: [arm_L, arm_R]
    when: no_reach_target
    channels:
      - bone: shoulder_L
        rotation:
          z: "sin(t * 0.8) * 3"
      - bone: shoulder_R
        rotation:
          z: "sin(t * 0.8 + PI) * 3"
      - bone: hand_L
        rotation:
          x: "sin(t * 1.2) * 5"
          z: "cos(t * 0.9) * 3"
      - bone: hand_R
        rotation:
          x: "sin(t * 1.2 + 0.5) * 5"
          z: "cos(t * 0.9 + 0.5) * 3"

  - name: breathing
    apply_to: [spine_chain]
    mode: additive
    channels:
      - bone: chest
        scale:
          x: "1 + sin(t * 2) * 0.01"
          y: "1 + sin(t * 2) * 0.01"
          z: "1 + sin(t * 2) * 0.015"
        rotation:
          x: "sin(t * 2) * 1.5"

  - name: head_micro_motion
    apply_to: [head_chain]
    mode: additive
    when: no_look_target
    channels:
      - bone: head
        rotation:
          x: "noise(t * 0.5) * 2"
          y: "noise(t * 0.7 + 100) * 3"

# ============================================
# RUNTIME TARGETS
# ============================================
runtime_variables:
  - name: look_target
    type: vec3
    default: null

  - name: left_hand_target
    type: vec3
    default: null

  - name: right_hand_target
    type: vec3
    default: null

  - name: movement_velocity
    type: vec3
    default: [0, 0, 0]

  - name: desired_facing
    type: quat
    default: identity
```

---

## IK Chain Types

| Type | Behavior |
|------|----------|
| support | Keeps end effector on ground, repositions to maintain CoM balance |
| balance | Adjusts chain to keep CoM over support polygon |
| look_at | Points end effector toward target position |
| reach | Moves end effector to target, respects constraints |
| swing | Passive swing (like arms during walk) based on body motion |

---

## Constraint Types

| Constraint | Description | Parameters |
|------------|-------------|------------|
| hinge | Rotate around single axis | axis, limits: [min, max] |
| ball | Rotate freely within cone | limits: [pitch_min, pitch_max, yaw_min, yaw_max, roll_min, roll_max] |
| twist | Twist along bone axis | axis, limits: [min, max] |
| fixed | No rotation allowed | - |

---

## Expression Language

For wave functions, the following expressions are supported:

### Variables

```
t           # current time (seconds)
phase       # normalized time (0 to 1) if duration defined
PI          # 3.14159...
TAU         # 2 * PI
```

### Functions

```
sin(x), cos(x), tan(x)
abs(x), floor(x), ceil(x)
min(a, b), max(a, b), clamp(x, min, max)
pow(x, n), sqrt(x)
lerp(a, b, t)
smoothstep(a, b, t)
noise(t)              # Perlin noise
noise(t, frequency)   # with frequency control
```

### Examples

```
"sin(t * 2.0) * 5.0"                    # oscillate ±5 degrees
"lerp(-10, 10, phase)"                  # linear -10 to 10 over duration
"sin(t * 3) * 5 + noise(t * 2) * 2"     # oscillate + organic variation
"smoothstep(0, 1, phase) * 90"          # smooth 0 to 90 degrees
```

---

## Stepping Algorithm

```
1. CHECK if step needed:
   - CoM approaching edge of support polygon?
   - Foot too far from hip (overstretched)?
   - Movement velocity requires repositioning?

2. SELECT which foot:
   - Pick foot furthest from where we're going
   - Never lift both feet (unless running with flight phase)

3. COMPUTE target position:
   - Predict where CoM will be in N seconds
   - Place foot to support that position
   - Offset laterally for stability
   - Raycast to find actual ground position

4. EXECUTE step:
   - Lift foot along arc trajectory
   - Rotate toe up during swing
   - Plant foot, align to ground normal
   - Transfer weight
```

---

## Engine Implementation

### Core Classes

```cpp
// src/engine/procedural/procedural_ik_system.hpp
class ProceduralIKSystem {
public:
    void LoadFromPMesh(const std::string& path);
    void Update(float deltaTime);

    // Runtime control (called by game logic)
    void SetLookTarget(const glm::vec3& target);
    void SetLookTarget(std::nullptr_t); // clear
    void SetReachTarget(const std::string& chain, const glm::vec3& target);
    void SetVelocity(const glm::vec3& velocity);

    // Output
    void ApplyToSkeleton(Skeleton& skeleton);

private:
    // Physics state
    CenterOfMass _com;
    std::vector<Foot> _feet;
    SupportPolygon _support;

    // IK chains
    std::vector<IKChain> _chains;

    // Active steps
    std::vector<StepRequest> _activeSteps;

    // Wave functions
    std::vector<WaveFunction> _waveFunctions;
    float _time = 0;

    // Core methods
    glm::vec3 ComputeCenterOfMass();
    SupportPolygon ComputeSupportPolygon();
    bool IsStable(const glm::vec3& com, const SupportPolygon& support);
    void TriggerStep(Foot& foot, const glm::vec3& targetPos);

    // IK solvers
    void SolveFABRIK(IKChain& chain, const glm::vec3& target);
    void SolveCCD(IKChain& chain, const glm::vec3& target);
    void EnforceConstraints(IKChain& chain);
};
```

### Update Loop

```cpp
void ProceduralIKSystem::Update(float deltaTime) {
    _time += deltaTime;

    // 1. Compute center of mass from bone positions + masses
    glm::vec3 com = ComputeCenterOfMass();

    // 2. Get support polygon from grounded feet
    SupportPolygon support = ComputeSupportPolygon();

    // 3. Check stability
    bool stable = IsStable(com, support);

    // 4. If unstable, trigger stepping or leaning
    if (!stable) {
        if (CanCompensateWithLean(com, support)) {
            ApplyTorsoLean(com, support);
        } else {
            Foot& footToMove = SelectFootToMove();
            glm::vec3 targetPos = PredictFootPlacement();
            TriggerStep(footToMove, targetPos);
        }
    }

    // 5. Update stepping feet (in-flight trajectory)
    for (auto& step : _activeSteps) {
        step.Update(deltaTime);
        if (step.IsComplete()) {
            step.foot->SetGrounded(true);
        }
    }
    _activeSteps.erase(
        std::remove_if(_activeSteps.begin(), _activeSteps.end(),
            [](const StepRequest& s) { return s.IsComplete(); }),
        _activeSteps.end()
    );

    // 6. Solve IK for all chains
    for (auto& chain : _chains) {
        switch (chain.type) {
            case IKType::Support:
                SolveFootIK(chain);
                break;
            case IKType::Balance:
                SolveBalanceIK(chain, com, support);
                break;
            case IKType::LookAt:
                if (_lookTarget.has_value()) {
                    SolveLookAtIK(chain, *_lookTarget);
                }
                break;
            case IKType::Reach:
                if (chain.target.has_value()) {
                    SolveReachIK(chain, *chain.target);
                }
                break;
        }
        EnforceConstraints(chain);
    }

    // 7. Apply wave functions (additive secondary motion)
    for (auto& wave : _waveFunctions) {
        if (wave.ShouldApply()) {
            ApplyWaveFunction(wave, _time);
        }
    }
}
```

---

## Files to Create

### Engine (C++)

| File | Description |
|------|-------------|
| src/engine/procedural/pmesh_loader.hpp | Load .pmesh YAML files |
| src/engine/procedural/procedural_ik_system.hpp | Main IK + physics system |
| src/engine/procedural/ik_chain.hpp | IK chain data structure |
| src/engine/procedural/ik_solver.hpp | FABRIK/CCD solvers |
| src/engine/procedural/support_polygon.hpp | Balance/support calculations |
| src/engine/procedural/step_controller.hpp | Stepping logic |
| src/engine/procedural/wave_function.hpp | Expression-based motion |
| src/engine/procedural/expression_parser.hpp | Parse math expressions |

### MCP Tools

| File | Description |
|------|-------------|
| mcp_tools/server/pmesh_server.py | Save/validate .pmesh files |

---

## References

- Rain World - Procedural creature animation
- Overgrowth - Physics-based character animation
- Boston Dynamics - Real robotics balance/locomotion
- https://github.com/elalish/manifold
- https://github.com/msteinbeck/tinyspline
- https://github.com/codeplea/tinyexpr

---

## TODO

- [ ] Implement FABRIK IK solver
- [ ] Implement support polygon calculation
- [ ] Implement stepping controller
- [ ] Implement wave function parser
- [ ] Create MCP server for .pmesh
- [ ] Integrate with existing animation system
- [ ] Test with simple biped
- [ ] Add quadruped support