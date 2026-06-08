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
