# 🌌 PhysLab 10/11
### A custom 3D mechanics simulation engine built from scratch in C++ and OpenGL.

![C++](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)
![OpenGL](https://img.shields.io/badge/Graphics-OpenGL%203.3-red.svg)
![GLFW](https://img.shields.io/badge/Windowing-GLFW-orange.svg)

> **A native, thread-safe desktop application designed to visually simulate, track, and mathematically verify classical 10th and 11th-grade mechanics problems without heavy commercial engine dependencies.**


https://github.com/user-attachments/assets/91c972fd-407f-4ec6-b706-e0fff9db228b

## 🚀 Core Features

* **Kinematics Tracking Engine:** Real-time benchmark tracking of ideal, non-gravitational coordinate translation. Verifies actual object coordinates against exact theoretical formulas ($x = x_0 + v_0t + 0.5at^2$).
* **Automated Simulation Bounds:** Precision event-tracking stops the simulation loop automatically the exact moment a user-defined elapsed time or specific 3D position vector target is achieved.
* **Continuous Multi-Axis Wrapping:** Implements fractional overshoot calculation on boundary limits. When an object escapes the visual viewport constraints ($X$, $Y$, or $Z$), it seamlessly wraps back into scene bounds without losing fractional precision.
* **Constraint Dynamics & Momentum:** Simulates multi-body particle mechanics and structural system attachments, featuring custom length limits for **Wires** and dynamic calculations for **Plastic** and **Elastic** collision impulse resolutions.


<img width="694" height="419" alt="image" src="https://github.com/user-attachments/assets/01f889c0-db0a-43f9-be59-9faf47be17b9" />

## 🎯 The Mission: Virtualizing High School Physics
Most educational physics tools are either overly simplistic 2D web apps or buried inside massive, heavy game engines. **PhysLab 10/11** was built from scratch to target the exact mechanics curriculum taught in 10th and 11th-grade physics classrooms:

* **10th Grade Foundations:** Ideal linear kinematics, independent axis translation, and basic Newtonian dynamics ($\Sigma F = ma$).
* **11th Grade Mechanics:** 3D vector calculations, structural constraints (wire systems), and multi-body momentum conservation (elastic vs. plastic collisions).

By avoiding heavy commercial engine frameworks, this project acts as a pure mathematical sandbox where textbook problems can be input, executed, and validated with zero physics engine "fudge factors."

---

## 🛠️ Technical Architecture & Challenges Solved

### 1. Pure Theoretical Benchmark Tracking
Unlike traditional game engines that calculate physics iteratively (which introduces subtle floating-point drift over time), PhysLab running a kinematics test calculates a continuous, exact theoretical trajectory in parallel:

$$\vec{x}(t) = \vec{x}_0 + \vec{v}_0 t + \frac{1}{2}\vec{a} t^2$$

The engine continuously benchmarks the simulated object's properties against these exact kinematic functions, automatically freezing the main execution loop the instant a user's defined position target vector or time threshold is achieved.

### 2. Thread-Safe State Synchronization
To keep the UI responsive and prevent rendering frames from altering physics synchronization, the rendering loops and memory states are cleanly isolated. Critical object mutations (transformations, velocity updates, and force accumulation) are explicitly wrapped in mutual exclusion (`std::mutex`) data protections during state evaluation to ensure total integrity across decoupled application cycles.

### 3. Continuous Multi-Axis Screen Wrapping
To allow for long-running simulation tracking within a constrained 3D viewport, the engine uses custom axis-wrapping logic. Instead of snapping an object rigidly to a boundary edge, it calculates the fractional overshoot:

$$\text{Overshoot} = x_{\text{current}} - x_{\text{boundary}}$$

By applying this fractional offset to the opposite boundary, the object transitions across screen edges seamlessly without losing sub-pixel velocity momentum or clipping mathematically.

---

## 💻 Tech Stack & Frameworks
* **Language:** C++17
* **Graphics API:** OpenGL 3.3 (Core Profile)
* **Windowing & Input:** GLFW
* **OpenGL Loader:** GLAD
* **Mathematics Engine:** GLM (OpenGL Mathematics)

---

## 🚀 Building & Running Locally

### Prerequisites
* **Compiler:** GCC/G++ via MSYS2 (UCRT64 environment recommended)
* **Libraries:** GLFW3, OpenGL32, GDI32, User32, Kernel32 (handled via internal compiler flags)

### Compilation (VS Code)
This project includes a fully dynamic, portable `.vscode/tasks.json` configuration that compiles the project out of whatever workspace folder it is placed in.
1. Clone this repository and open the root folder in VS Code.
2. Press `Ctrl + Shift + B` (Windows) to trigger the build task.
3. The compiler will dynamically parse the source files and generate `main.exe` directly inside your root workspace directory.
