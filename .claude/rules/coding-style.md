# Coding Style Rules

## General C++ Guidelines
- Use modern C++17 features where appropriate
- Prefer smart pointers over raw pointers
- Use const correctness consistently
- Use noexcept for simple setter methods
- Follow RAII principles for resource management

## Naming Conventions
- Use PascalCase for class names (e.g., `CharacterController`, `RenderObject`)
- Use PascalCase for method name (e.g., `UpdatePosition`)
- Use _camalCase for private variables (e.g., `_currentPosition`)
- Use ALL_CAPS for constants and enums

## Code Organization
- Keep header files (.hpp) and implementation files (.cpp) separate
- Group related functionality into classes

## Comments and Documentation
- Write clear, no comments

## Error Handling
- Use exceptions for exceptional cases
- Validate input parameters
- Handle edge cases explicitly

## Performance
- Minimize allocations in game loop
- Cache frequently accessed values
- Profile before optimizing

## Build and Run
- Use `./run_debug.sh` to build and run the project in debug mode
- Always test changes by running the project after implementation