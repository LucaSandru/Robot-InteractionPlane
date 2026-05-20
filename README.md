# Robot Interaction Arena

A 3D visualization system built with OpenGL that simulates three autonomous robots navigating and interacting in a procedurally generated terrain environment.

## Overview

This project demonstrates real-time 3D graphics rendering using FreeGLUT and OpenGL. It features three distinct robots that move along predefined circular paths on a wavy terrain surface, with dynamic skeletal animation and interactive camera controls.

## Features

- **Three Independent Robots**: Each robot has unique movement speeds, paths, and behaviors
- **Skeletal Animation**: Realistic walking and waving arm animations driven by sine functions
- **Procedurally Generated Terrain**: A wavy surface created using sine and cosine functions
- **Dynamic Path Visualization**: Color-coded circular paths for each robot
- **Real-time Rendering**: Smooth 60 FPS animation with interactive controls
- **Camera System**: Adjustable perspective with multiple axes rendering for reference

## Technical Architecture

### Core Components

**Data Structures**:
- `Vec3`: Basic 3D vector with x, y, z components
- `RobotState`: Contains position, movement parameters, heading, and animation state

**Terrain System**:
- `surfaceHeight()`: Generates terrain height using a combination of sine and cosine waves
- `surfaceNormal()`: Calculates surface normals for lighting calculations
- Terrain adapts between -5 to 5 on X and Z axes with smooth height variations

**Path System**:
- Three distinct elliptical paths defined by parametric equations
- `evaluatePath()`: Converts path parameter (0-1) to 3D coordinates
- Each path offset by different angles (0°, 85°, 170°) for visual separation

### Robot Structure

Each robot is composed of:
- **Torso**: Central blue body (2.2 x 3.0 x 1.2 units)
- **Head**: Tan colored cube with facial features (eyes and mouth)
- **Arms**: Upper arms (red) and forearms (orange) with joint spheres
- **Legs**: Upper legs (green) and lower legs (gray) with foot plates
- **Joints**: Shiny gray spheres connecting body segments

### Animation System

**Walking Animation**:
- Driven by `sin(animationTime * 4.0f)` for natural gait
- Upper and lower legs move oppositely for realistic movement
- Arms swing in sync with leg motion on one side, opposite on the other

**Waving Animation**:
- Robot 0 continuously waves its right arm
- Right forearm rotates using `sin(3.0f * animationTime)`
- Right shoulder locked at -65° when waving

**Movement**:
- Robots follow elliptical paths at variable speeds (0.8x to 1.2x)
- Heading automatically calculated based on forward path direction
- Y-position adjusted to follow terrain surface

## Build Requirements

- C++ compiler with C++11 support
- FreeGLUT library
- OpenGL libraries
- Standard math library (cmath)

### Compilation

```bash
g++ -o robot_arena main.cpp -lglut -lGL -lm
```

## Controls

- **W**: Toggle animation on/off
- **+**: Increase animation speed by 0.2x
- **-**: Decrease animation speed by 0.2x (minimum 0.2x)
- **ESC**: Exit application

## Window Configuration

- Default resolution: 1000 x 700 pixels
- Perspective view with 60-degree field of view
- Clear color: Dark blue-gray (#1A1E28)

## Robot Specifications

**Robot 0**:
- Path: Small ellipse (3.5 x 1.5 units)
- Speed: 1.0x (baseline)
- Initial heading: 20°
- Behavior: Walking + Waving

**Robot 1**:
- Path: Medium ellipse (3.8 x 2.0 units, phase offset 85°)
- Speed: 0.8x (slower)
- Initial heading: -20°
- Behavior: Walking only

**Robot 2**:
- Path: Larger ellipse (2.5 x 3.0 units, phase offset 170°)
- Speed: 1.2x (faster)
- Initial heading: -45°
- Behavior: Walking only

## Visual Elements

**Color Scheme**:
- Path 0 (Robot 0): Yellow
- Path 1 (Robot 1): Orange
- Path 2 (Robot 2): Cyan
- Terrain: Green
- Reference axes: Red (X), Green (Y), Blue (Z)

**Lighting**:
- Smooth shading enabled
- Lighting system disabled for flat material rendering
- Depth testing enabled for proper occlusion

## Implementation Details

### Rendering Pipeline

1. **Initialization**: Set up three robot states with position, speed, and animation parameters
2. **Update Phase**: Calculate new robot positions along paths based on elapsed time
3. **Rendering Phase**: Transform matrix stack for robot hierarchy and terrain rendering
4. **Display**: Swap buffers for smooth animation

### Mathematics

- Path generation: Parametric ellipses with phase offsets
- Heading calculation: arctangent of path direction vector
- Animation timing: Global `animationTime` variable scaled by user-controlled `animationSpeed`
- Terrain: Composite sine/cosine function for natural undulation

## Future Development

- Collision detection and response between robots
- Interactive behavior states and decision-making
- Material system implementation (shiny, plastic, matte surfaces)
- Robot interaction mechanics and communication
- Terrain physics simulation

## File Structure

- `main.cpp`: Complete implementation (655 lines)
  - Global state variables and structures (lines 5-34)
  - Terrain and path generation (lines 37-66)
  - Initialization (lines 69-118)
  - Camera setup (lines 121-147)
  - Drawing functions for components and robots (lines 183-511)
  - Animation and update logic (lines 514-636)
  - Main entry point and event handlers (lines 638-654)

## Performance Notes

- 60 FPS target (16ms timer interval)
- Triangle strip rendering for efficient terrain mesh
- Sphere subdivision: 15-16 vertices for smooth joints
- Scaling factor of 0.35x applied to robots for proportional terrain fit

## Author Notes

This is Milestone 1 of the Robot Interaction Arena project. The foundation establishes the 3D rendering pipeline, multi-robot animation system, and interactive camera controls. Subsequent milestones will introduce collision detection, behavior states, and advanced interaction mechanics.
