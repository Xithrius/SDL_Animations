# Systems

This folder contains the core systems that handle game logic, input, timing, and other application-level functionality.

## Current Systems

### InputSystem (`input_system.h/cpp`)

- **Purpose**: Handles all input events and provides input state management
- **Responsibilities**:
  - Processing SDL events
  - Managing input state (key states, mouse position, etc.)
  - Providing clean input interfaces for other systems
  - Handling key bindings and shortcuts
- **Usage**: Other systems can query input state through this system

### TimeSystem (`time_system.h/cpp`)

- **Purpose**: Handles timing, FPS tracking, and time utilities
- **Responsibilities**:
  - FPS tracking and calculation
  - Fixed timestep management
  - Time utilities for the application
  - Performance monitoring
- **Usage**: Provides timing information to other systems and the main loop

### AnimationSystem (`animation_system.h/cpp`)

- **Purpose**: Handles entity animations and movement
- **Responsibilities**:
  - Managing entity animations
  - Handling movement and interpolation
  - Coordinating with the entity manager
  - Providing animation utilities
- **Status**: Currently a placeholder for future animation functionality

## Future Systems

### EventSystem (`event_system.h/cpp`)

- **Purpose**: Handle application-level events
- **Responsibilities**:
  - Manage event queuing and processing
  - Coordinate between different systems
  - Provide event-driven architecture

### PhysicsSystem (`physics_system.h/cpp`)

- **Purpose**: Handle physics simulation
- **Responsibilities**:
  - Collision detection
  - Physics calculations
  - Movement constraints

### AudioSystem (`audio_system.h/cpp`)

- **Purpose**: Handle audio playback and management
- **Responsibilities**:
  - Sound effects
  - Music playback
  - Audio state management

### ParticleSystem (`particle_system.h/cpp`)

- **Purpose**: Handle particle effects
- **Responsibilities**:
  - Particle generation and management
  - Visual effects
  - Performance optimization

## Architecture Principles

1. **Single Responsibility**: Each system has one clear purpose
2. **Dependency Injection**: Systems receive dependencies through constructors
3. **No Circular Dependencies**: Systems can depend on entities and graphics, but not on UI
4. **Clean Interfaces**: Systems provide clear, simple interfaces for other components

## Integration with AppState

Systems are managed by `AppState` and can be accessed by:

- Other systems
- The UI layer
- The main event loop

## Usage Example

```cpp
// In AppState
class AppState {
 private:
  InputSystem inputSystem;
  TimeSystem timeSystem;
  AnimationSystem animationSystem;

 public:
  AppState(Context* context)
    : inputSystem(this)
    , timeSystem()
    , animationSystem(this) {}
};
```
