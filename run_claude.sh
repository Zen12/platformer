#!/bin/bash
# Start Claude Code with project environment configured
# Usage: ./run_claude.sh [OPTIONS] [ASSETS_DIR]
#
# Agent Options:
#   (default)           Coordinator - auto-delegates to researcher/reviewer/developer
#   -r, --researcher    Researcher only (analysis, no delegation)
#   -v, --reviewer      Reviewer only (planning, no delegation)
#   -d, --developer     Developer only (implementation, no delegation)
#   -u, --ui-designer   UI designer only (RML/CSS, no delegation)
#   -a, --tech-art      Tech Art only (Blender/FBX to GLB, export scripts)
#   -g, --generic       General purpose Claude (no agent profile, no auto-delegation)
#
# Session Options:
#   -s, --session [NAME]  Use a named session (auto-generates if no name given)
#   -c, --continue        Continue the most recent session
#
# Other Options:
#   -h, --help          Show this help message
#
# Recommended workflow:
#   Just run ./run_claude.sh (coordinator mode)
#   The coordinator auto-delegates: researcher -> reviewer -> developer
#   No manual agent switching needed!
#
# Example usage:
#   ./run_claude.sh                              # Coordinator (recommended)
#   ./run_claude.sh -r                           # Researcher only
#   ./run_claude.sh -v                           # Reviewer only
#   ./run_claude.sh -d                           # Developer only
#   ./run_claude.sh -g                           # Generic (general purpose)
#   ./run_claude.sh -s my-feature                # Named session
#   ./run_claude.sh -c                           # Continue last session

# Get the absolute path to the project root (where this script lives)
export PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Set terminal tab title
set_terminal_title() {
    echo -ne "\033]0;$1\007"
}

# Generate a session name using timestamp
generate_session_name() {
    echo "session-$(date +%Y%m%d-%H%M%S)"
}

# Parse command-line arguments
AGENT_TYPE=""
ASSETS_ARG=""
SESSION_NAME=""
CONTINUE_SESSION=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -r|--researcher)
            AGENT_TYPE="researcher"
            shift
            ;;
        -v|--reviewer)
            AGENT_TYPE="reviewer"
            shift
            ;;
        -d|--developer)
            AGENT_TYPE="developer"
            shift
            ;;
        -u|--ui-designer)
            AGENT_TYPE="ui-designer"
            shift
            ;;
        -a|--tech-art)
            AGENT_TYPE="tech-art"
            shift
            ;;
        -g|--generic)
            AGENT_TYPE="generic"
            shift
            ;;
        -s|--session)
            # Check if next arg exists and doesn't start with -
            if [[ -n "$2" && ! "$2" =~ ^- ]]; then
                SESSION_NAME="$2"
                shift 2
            else
                SESSION_NAME="__AUTO_GENERATE__"
                shift
            fi
            ;;
        -c|--continue)
            CONTINUE_SESSION=true
            shift
            ;;
        -h|--help)
            echo "Usage: ./run_claude.sh [OPTIONS] [ASSETS_DIR]"
            echo ""
            echo "Agent Options:"
            echo "  (default)           Coordinator - auto-delegates to agents"
            echo "  -r, --researcher    Researcher only (analysis)"
            echo "  -v, --reviewer      Reviewer only (planning)"
            echo "  -d, --developer     Developer only (implementation)"
            echo "  -u, --ui-designer   UI designer only (RML/CSS)"
            echo "  -a, --tech-art      Tech Art (Blender/FBX export scripts)"
            echo "  -g, --generic       General purpose (no agent, no auto-delegation)"
            echo ""
            echo "Session Options:"
            echo "  -s, --session [NAME]  Use a named session (auto-generates if no name)"
            echo "  -c, --continue        Continue the most recent session"
            echo ""
            echo "Other Options:"
            echo "  -h, --help          Show this help message"
            echo ""
            echo "Recommended: Just run ./run_claude.sh"
            echo "Coordinator auto-delegates: researcher -> reviewer -> developer"
            echo ""
            echo "Examples:"
            echo "  ./run_claude.sh                     # Coordinator (recommended)"
            echo "  ./run_claude.sh -s my-feature       # Named session"
            echo "  ./run_claude.sh -c                  # Continue last session"
            exit 0
            ;;
        *)
            ASSETS_ARG="$1"
            shift
            ;;
    esac
done

echo -e "${BLUE}Claude Code Setup & Launch${NC}"
echo ""

# Check if Python 3 is installed
if ! command -v python &> /dev/null; then
    echo -e "${YELLOW}Warning: Python 3 is not installed. Please install Python 3.7+ to use MCP features.${NC}"
    exit 1
fi

# Create virtual environment if it doesn't exist
if [ ! -d "${PROJECT_ROOT}/venv" ]; then
    echo -e "${BLUE}Creating Python virtual environment...${NC}"
    python -m venv "${PROJECT_ROOT}/venv"
    echo -e "${GREEN}Virtual environment created${NC}"
else
    echo -e "${GREEN}Virtual environment exists${NC}"
fi

# Activate virtual environment
echo -e "${BLUE}Activating virtual environment...${NC}"
source "${PROJECT_ROOT}/venv/bin/activate"

# Check if mcp[cli] is installed
if ! python -c "import mcp" &> /dev/null; then
    echo -e "${BLUE}Installing MCP SDK...${NC}"
    pip install --quiet "mcp[cli]"
    echo -e "${GREEN}MCP SDK installed${NC}"
else
    echo -e "${GREEN}MCP SDK already installed${NC}"
fi

echo ""

# Configure assets directory for MCP server
# Priority:
#   1. Command-line argument
#   2. ASSETS_DIR environment variable
#   3. Default: ${PROJECT_ROOT}/assets
if [ -n "$ASSETS_ARG" ]; then
    export ASSETS_DIR="$ASSETS_ARG"
elif [ -z "$ASSETS_DIR" ]; then
    export ASSETS_DIR="${PROJECT_ROOT}/assets"
fi

# Agent selection (interactive if not specified via flag)
if [ -z "$AGENT_TYPE" ]; then
    echo -e "${BLUE}Select agent type:${NC}"
    echo "  1) Coordinator (default - auto-delegates to researcher/reviewer/developer)"
    echo "  2) Researcher (analysis only)"
    echo "  3) Reviewer (planning only)"
    echo "  4) Developer (implementation only)"
    echo "  5) UI Designer (RML/CSS only)"
    echo "  6) Tech Art (Blender/FBX export scripts)"
    echo "  7) Generic (general purpose, no agent profile)"
    echo ""
    echo -e "${YELLOW}Recommended: Coordinator auto-manages the full pipeline${NC}"
    echo ""
    read -p "Enter choice [1]: " choice
    case $choice in
        1|"")
            AGENT_TYPE=""  # No agent = coordinator with Task tool
            ;;
        2)
            AGENT_TYPE="researcher"
            ;;
        3)
            AGENT_TYPE="reviewer"
            ;;
        4)
            AGENT_TYPE="developer"
            ;;
        5)
            AGENT_TYPE="ui-designer"
            ;;
        6)
            AGENT_TYPE="tech-art"
            ;;
        7)
            AGENT_TYPE="generic"
            ;;
    esac
    echo ""
fi

if [ "$AGENT_TYPE" = "generic" ]; then
    echo -e "${GREEN}Agent: Generic (general purpose)${NC}"
    set_terminal_title "Claude"
elif [ -n "$AGENT_TYPE" ]; then
    echo -e "${GREEN}Agent: ${AGENT_TYPE}${NC}"
    # Set terminal title based on agent type
    case $AGENT_TYPE in
        researcher) set_terminal_title "Researcher" ;;
        reviewer) set_terminal_title "Reviewer" ;;
        developer) set_terminal_title "Developer" ;;
        ui-designer) set_terminal_title "UI Designer" ;;
        tech-art) set_terminal_title "Tech Art" ;;
    esac
else
    echo -e "${GREEN}Agent: Coordinator (auto-delegates)${NC}"
    set_terminal_title "Coordinator"
fi

# Add project to Python path (for MCP server imports)
export PYTHONPATH="${PROJECT_ROOT}:${PYTHONPATH}"

echo -e "${GREEN}Project environment configured${NC}"
echo -e "  ${BLUE}PROJECT_ROOT:${NC} $PROJECT_ROOT"
echo -e "  ${BLUE}ASSETS_DIR:${NC} $ASSETS_DIR"
echo -e "  ${BLUE}PYTHONPATH:${NC} $PYTHONPATH"

# Auto-generate session name if requested
if [ "$SESSION_NAME" = "__AUTO_GENERATE__" ]; then
    SESSION_NAME=$(generate_session_name)
fi

# Session status output
if [ -n "$SESSION_NAME" ]; then
    echo -e "  ${BLUE}SESSION:${NC} $SESSION_NAME"
elif [ "$CONTINUE_SESSION" = true ]; then
    echo -e "  ${BLUE}SESSION:${NC} (continuing last)"
fi

echo ""
echo -e "${GREEN}Starting Claude Code...${NC}"
echo -e "${YELLOW}Note: MCP server will initialize in a few seconds...${NC}"
echo ""

# Build claude command dynamically
CLAUDE_CMD="claude"

# Add agent flag only for specific agents (not for coordinator or generic)
if [ -n "$AGENT_TYPE" ] && [ "$AGENT_TYPE" != "generic" ]; then
    CLAUDE_CMD="$CLAUDE_CMD --agent $AGENT_TYPE"
fi

if [ -n "$SESSION_NAME" ]; then
    CLAUDE_CMD="$CLAUDE_CMD --resume $SESSION_NAME"
elif [ "$CONTINUE_SESSION" = true ]; then
    CLAUDE_CMD="$CLAUDE_CMD --continue"
fi

# Launch Claude Code
eval $CLAUDE_CMD
