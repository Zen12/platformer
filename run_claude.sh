#!/bin/bash
# Start Claude Code with project environment configured
# Usage: ./run_claude.sh [OPTIONS] [ASSETS_DIR]
#
# Agent Options:
#   -r, --researcher    Launch with researcher agent (analysis, creates solutions)
#   -v, --reviewer      Launch with reviewer agent (creates implementation plans)
#   -d, --developer     Launch with developer agent (implements plans)
#
# Session Options:
#   -s, --session [NAME]  Use a named session (auto-generates if no name given)
#   -c, --continue        Continue the most recent session
#
# Other Options:
#   -h, --help          Show this help message
#
# This script handles:
# - Python virtual environment setup
# - MCP SDK installation
# - Environment configuration
# - Agent selection (researcher, reviewer, developer, or generic)
# - Session management (named sessions, continue previous)
# - Launching Claude Code
#
# Two-project structure:
# 1. Logic project (C++/CMake/Python) - this repository
# 2. Assets project (YAML/.png/etc) - separate repository or directory
#
# Example usage:
#   ./run_claude.sh                              # Interactive agent selection
#   ./run_claude.sh -r                           # Researcher (analysis, solutions)
#   ./run_claude.sh -v                           # Reviewer (implementation plans)
#   ./run_claude.sh -d                           # Developer (implements plans)
#   ./run_claude.sh -s                           # Auto-generate session name
#   ./run_claude.sh -s my-feature                # Use named session "my-feature"
#   ./run_claude.sh -c                           # Continue last session
#   ./run_claude.sh -r -s                        # Researcher with auto session
#   ./run_claude.sh /path/to/game-assets         # Use custom assets directory

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
            echo "  -r, --researcher    Launch with researcher agent (analysis, solutions)"
            echo "  -v, --reviewer      Launch with reviewer agent (implementation plans)"
            echo "  -d, --developer     Launch with developer agent (implements plans)"
            echo ""
            echo "Session Options:"
            echo "  -s, --session [NAME]  Use a named session (auto-generates if no name)"
            echo "  -c, --continue        Continue the most recent session"
            echo ""
            echo "Other Options:"
            echo "  -h, --help          Show this help message"
            echo ""
            echo "Workflow: researcher -> reviewer -> developer"
            echo ""
            echo "Examples:"
            echo "  ./run_claude.sh -r                  # Researcher agent"
            echo "  ./run_claude.sh -s                  # Auto-generate session name"
            echo "  ./run_claude.sh -s my-feature       # Named session"
            echo "  ./run_claude.sh -c                  # Continue last session"
            echo "  ./run_claude.sh -r -s               # Researcher with auto session"
            echo ""
            echo "If no option is specified, you'll be prompted to choose an agent."
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
if ! command -v python3 &> /dev/null; then
    echo -e "${YELLOW}Warning: Python 3 is not installed. Please install Python 3.7+ to use MCP features.${NC}"
    exit 1
fi

# Create virtual environment if it doesn't exist
if [ ! -d "${PROJECT_ROOT}/venv" ]; then
    echo -e "${BLUE}Creating Python virtual environment...${NC}"
    python3 -m venv "${PROJECT_ROOT}/venv"
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
    echo "  1) Researcher (default - analysis, creates solutions)"
    echo "  2) Reviewer (creates implementation plans)"
    echo "  3) Developer (implements plans exactly)"
    echo "  4) Generic (full capabilities)"
    echo ""
    echo -e "${YELLOW}Workflow: Researcher -> Reviewer -> Developer${NC}"
    echo ""
    read -p "Enter choice [1]: " choice
    case $choice in
        1|"")
            AGENT_TYPE="researcher"
            ;;
        2)
            AGENT_TYPE="reviewer"
            ;;
        3)
            AGENT_TYPE="developer"
            ;;
        *)
            AGENT_TYPE=""
            ;;
    esac
    echo ""
fi

if [ -n "$AGENT_TYPE" ]; then
    echo -e "${GREEN}Agent: ${AGENT_TYPE}${NC}"
    # Set terminal title based on agent type
    case $AGENT_TYPE in
        researcher) set_terminal_title "Researcher" ;;
        reviewer) set_terminal_title "Reviewer" ;;
        developer) set_terminal_title "Developer" ;;
    esac
else
    set_terminal_title "Claude"
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

if [ -n "$AGENT_TYPE" ]; then
    CLAUDE_CMD="$CLAUDE_CMD --agent $AGENT_TYPE"
fi

if [ -n "$SESSION_NAME" ]; then
    CLAUDE_CMD="$CLAUDE_CMD --resume $SESSION_NAME"
elif [ "$CONTINUE_SESSION" = true ]; then
    CLAUDE_CMD="$CLAUDE_CMD --continue"
fi

# Launch Claude Code
eval $CLAUDE_CMD
