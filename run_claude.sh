#!/bin/bash
# Start Claude Code with project environment configured
# Usage: ./run_claude.sh [ASSETS_DIR]
#
# This script handles:
# - Python virtual environment setup
# - MCP SDK installation
# - Environment configuration
# - Launching Claude Code
#
# Two-project structure:
# 1. Logic project (C++/CMake/Python) - this repository
# 2. Assets project (YAML/.png/etc) - separate repository or directory
#
# Example usage:
#   ./run_claude.sh                              # Use default: ./assets/resources
#   ./run_claude.sh /path/to/game-assets         # Use custom assets directory

# Get the absolute path to the project root (where this script lives)
export PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🚀 Claude Code Setup & Launch${NC}"
echo ""

# Check if Python 3 is installed
if ! command -v python3 &> /dev/null; then
    echo -e "${YELLOW}⚠️  Python 3 is not installed. Please install Python 3.7+ to use MCP features.${NC}"
    exit 1
fi

# Create virtual environment if it doesn't exist
if [ ! -d "${PROJECT_ROOT}/venv" ]; then
    echo -e "${BLUE}📦 Creating Python virtual environment...${NC}"
    python3 -m venv "${PROJECT_ROOT}/venv"
    echo -e "${GREEN}✓ Virtual environment created${NC}"
else
    echo -e "${GREEN}✓ Virtual environment exists${NC}"
fi

# Activate virtual environment
echo -e "${BLUE}🔌 Activating virtual environment...${NC}"
source "${PROJECT_ROOT}/venv/bin/activate"

# Check if mcp[cli] is installed
if ! python -c "import mcp" &> /dev/null; then
    echo -e "${BLUE}📥 Installing MCP SDK...${NC}"
    pip install --quiet "mcp[cli]"
    echo -e "${GREEN}✓ MCP SDK installed${NC}"
else
    echo -e "${GREEN}✓ MCP SDK already installed${NC}"
fi

echo ""

# Configure assets directory for MCP server
# Priority:
#   1. Command-line argument
#   2. ASSETS_DIR environment variable
#   3. Default: ${PROJECT_ROOT}/assets/resources
if [ -n "$1" ]; then
    export ASSETS_DIR="$1"
elif [ -z "$ASSETS_DIR" ]; then
    export ASSETS_DIR="${PROJECT_ROOT}/assets/resources"
fi

# Add project to Python path (for MCP server imports)
export PYTHONPATH="${PROJECT_ROOT}:${PYTHONPATH}"

echo -e "${GREEN}✓ Project environment configured${NC}"
echo -e "  ${BLUE}PROJECT_ROOT:${NC} $PROJECT_ROOT"
echo -e "  ${BLUE}ASSETS_DIR:${NC} $ASSETS_DIR"
echo -e "  ${BLUE}PYTHONPATH:${NC} $PYTHONPATH"
echo ""
echo -e "${GREEN}🎯 Starting Claude Code...${NC}"
echo -e "${YELLOW}⏳ Note: MCP server will initialize in a few seconds...${NC}"
echo ""

# Launch Claude Code
claude