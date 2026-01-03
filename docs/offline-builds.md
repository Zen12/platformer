# Offline Build Support

## Overview

The project now supports building without an internet connection. CMake's FetchContent will check if dependencies are already cached before attempting to download them.

## Build Modes

### 1. Online Mode (Default)
**Script:** `./run_debug.sh`

**Behavior:**
- Downloads missing dependencies from Git repositories
- **Skips update checks** for already cached dependencies (faster!)
- Uses cached dependencies if available (no re-download)
- Ideal for normal development

**CMake flag:** `FETCHCONTENT_UPDATES_DISCONNECTED=ON` (automatic)

### 2. Offline Mode (Fully Disconnected)
**Script:** `./run_offline.sh`

**Behavior:**
- Uses **only cached dependencies**
- **Fails if any dependency is not cached**
- Never accesses the internet
- Ideal for: offline work, CI environments, air-gapped systems

**CMake flag:** `-DOFFLINE_BUILD=ON`

## Usage

### First-time Setup (Download Dependencies)
```bash
# Download all dependencies (requires internet)
./run_debug.sh
```

This populates the cache in `bin/debug/desktop/_deps/`.

### Subsequent Builds (Fast, uses cache)
```bash
# Normal builds use cache automatically (no re-download)
./run_debug.sh

# Or use fully offline mode
./run_offline.sh
```

### Manual CMake Usage

**Online with cache preference:**
```bash
cmake -H. -Bbin/debug/desktop -DCMAKE_BUILD_TYPE=Debug
cmake --build bin/debug/desktop/
```

**Fully offline:**
```bash
cmake -H. -Bbin/debug/desktop -DCMAKE_BUILD_TYPE=Debug -DOFFLINE_BUILD=ON
cmake --build bin/debug/desktop/
```

## Verifying Cached Dependencies

To check if dependencies are cached:

```bash
ls -la bin/debug/desktop/_deps/

# Should show directories like:
# entt-src/
# yaml-cpp-src/
# rmlui-src/
# glm-src/
# freetype-src/
# assimp-src/
# glfw-src/
# glew-src/
```

## Checking Package Versions

Each dependency is locked to a specific commit/tag in the Find*.cmake files:

- **EnTT:** `cmake/FindEnTT.cmake` → `GIT_TAG v3.14.0`
- **yaml-cpp:** `cmake/FindYamlCpp.cmake` → `GIT_TAG a83cd31548b19d50f3f983b069dceb4f4d50756d`
- **RmlUi:** `cmake/FindRmlUi.cmake` → `GIT_TAG 6.0`
- **GLM, Freetype, Assimp, etc.:** See respective `cmake/Find*.cmake` files

To verify a cached package is on the correct commit:

```bash
# Example: Check EnTT version
cd bin/debug/desktop/_deps/entt-src
git log -1 --oneline
# Should show: v3.14.0 or the commit specified in FindEnTT.cmake

# Example: Check yaml-cpp commit
cd bin/debug/desktop/_deps/yaml-cpp-src
git log -1 --format="%H"
# Should show: a83cd31548b19d50f3f983b069dceb4f4d50756d
```

## How It Works

### FetchContent Cache Behavior

CMake's FetchContent downloads dependencies to `bin/debug/desktop/_deps/` and reuses them:

1. **First build:** Downloads all dependencies from Git
2. **Subsequent builds:**
   - **Online mode:** Checks cache first, skips update checks (fast!)
   - **Offline mode:** Uses cache exclusively, fails if missing

### What Gets Cached

- Full Git repositories with history
- Exact commit/tag specified in Find*.cmake files
- Built binaries (for libraries that need compilation)

### Cache Location

- **Desktop Debug:** `bin/debug/desktop/_deps/`
- **Desktop Release:** `bin/release/desktop/_deps/` (separate cache)
- **Web builds:** `bin/web/_deps/` (separate cache)

## Cleaning the Cache

To force fresh downloads:

```bash
# Remove entire build directory
rm -rf bin/debug/desktop/

# Or just remove _deps folder
rm -rf bin/debug/desktop/_deps/

# Then rebuild
./run_debug.sh
```

## CI/Offline Workflows

### Pre-cache Dependencies for Offline Work

```bash
# On a machine with internet:
./run_debug.sh  # Downloads all dependencies

# Copy the _deps folder to offline machine:
tar -czf deps-cache.tar.gz bin/debug/desktop/_deps/
# Transfer deps-cache.tar.gz to offline machine

# On offline machine:
tar -xzf deps-cache.tar.gz
./run_offline.sh  # Uses cached dependencies
```

### Docker/CI Builds

```dockerfile
# Example Dockerfile with cached dependencies
FROM ubuntu:22.04

# Install CMake, compilers, etc.
# ...

# Copy project
COPY . /project
WORKDIR /project

# Download dependencies (online step)
RUN ./run_debug.sh

# Now builds can be offline
CMD ["./run_offline.sh"]
```

## Troubleshooting

### "Failed to populate" Error

**Symptom:**
```
CMake Error: Failed to populate entt
```

**Cause:** Offline mode enabled but dependency not cached

**Solution:**
```bash
# Run online build first
./run_debug.sh

# Then try offline build
./run_offline.sh
```

### Wrong Package Version

**Symptom:** Package is cached but on wrong commit

**Solution:**
```bash
# Delete the specific package cache
rm -rf bin/debug/desktop/_deps/entt-*

# Or clean all
rm -rf bin/debug/desktop/_deps/

# Rebuild
./run_debug.sh
```

### Cache Corruption

**Symptom:** Build fails with Git errors

**Solution:**
```bash
# Clean and rebuild
rm -rf bin/debug/desktop/
./run_debug.sh
```

## Summary

| Mode | Script | Internet Required | Uses Cache | Speed |
|------|--------|-------------------|------------|-------|
| **Online (default)** | `./run_debug.sh` | Only for missing deps | Yes | Fast |
| **Offline** | `./run_offline.sh` | Never | Yes (required) | Fastest |

**Recommendation:** Use default online mode for normal development. It automatically uses cached dependencies and is fast. Use offline mode only when internet is unavailable or for fully air-gapped builds.
