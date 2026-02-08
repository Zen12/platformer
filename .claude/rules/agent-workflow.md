# Agent Workflow - Coordinator Pattern

## CRITICAL: Ask Permission Between Agents

When using the agent pipeline, you MUST ask for user permission before spawning each subsequent agent. Never auto-proceed through the entire pipeline silently.

**Use the pipeline for:**
- "Add a feature..."
- "Implement..."
- "Refactor..."
- "Fix the bug..."
- "Change the code to..."
- Any task requiring code modifications

**Do NOT use pipeline for:**
- Questions about the codebase
- Simple file reads
- Documentation requests
- Explanations

## Overview

The project uses a three-agent pipeline coordinated by main Claude:

```
User Request
     │
     ▼
Main Claude (Coordinator)
     │
     ├──► Task(researcher) ──► Analysis & Solution
     │                              │
     │◄─────────────────────────────┘
     │
     ▼
     Ask Permission ──► User Approval
     │
     ├──► Task(reviewer) ──► Implementation Plan
     │         │
     │◄────────┘
     │
     ▼
     Ask Permission ──► User Approval
     │
     ├──► Task(developer) ──► Code Implementation
     │         │
     │◄────────┘
     │
     ▼
Report to User
```

## How It Works

1. **User** gives a task to main Claude
2. **Main Claude** spawns researcher with the task
3. **Researcher** analyzes and returns solution
4. **Main Claude** presents summary and **asks permission** to continue
5. **After approval**, spawns reviewer with the solution
6. **Reviewer** creates implementation plan
7. **Main Claude** presents plan summary and **asks permission** to continue
8. **After approval**, spawns developer with the plan
9. **Developer** implements and builds
10. **Main Claude** reports results to user

## When to Use This Workflow

Use the pipeline for any code modification task.

**Skip the pipeline only for:**
- Questions/explanations (no code changes)
- Reading files to answer questions
- Documentation-only changes

## Agent Responsibilities

### Researcher
- Analyzes codebase
- Identifies patterns and dependencies
- Recommends solution with trade-offs
- **Does NOT modify files**

### Reviewer
- Reads researcher's solution
- Creates step-by-step implementation plan
- Provides exact code to write
- **Does NOT modify files**

### Developer
- Executes plan exactly as written
- Builds and verifies
- Reports success or failure
- **Does NOT deviate from plan**

## Example Usage

User: "Add a pause menu to the game"

Main Claude:
1. Spawns researcher → gets analysis
2. Presents: "Researcher found X. Recommends Y. Proceed to reviewer?"
3. User: "Yes"
4. Spawns reviewer → gets plan
5. Presents: "Reviewer created plan with 5 steps. Proceed to developer?"
6. User: "Yes"
7. Spawns developer → gets implementation report
8. Reports: "Done. Pause menu implemented. Build: PASS"

**Key:** Always ask permission before spawning the next agent.

## Error Handling

If developer reports build failure:
1. Main Claude reports error to user
2. Asks: "Would you like me to spawn reviewer to fix the plan?"
3. After approval, spawns reviewer with error details
4. Asks permission before re-spawning developer

## Running Agents Standalone

Agents can still be run standalone for specific use cases:

```bash
./run_claude.sh -r  # Researcher only (analysis)
./run_claude.sh -v  # Reviewer only (planning)
./run_claude.sh -d  # Developer only (implementation)
```

For the full coordinated pipeline with permission checkpoints, use main Claude.
