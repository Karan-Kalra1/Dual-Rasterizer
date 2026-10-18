
# Modern C++ & DirectX 11 Dual Rasterizer

A hybrid **Software (C++)** and **Hardware (DirectX 11)** 3D rasterization engine built in C++ using Microsoft Visual Studio. The project demonstrates the fundamental rendering pipeline by implementing a multi-threaded software rasterizer alongside a feature-matched DirectX 11 hardware pipeline, complete with dynamic runtime toggling, custom lighting, depth buffering, texture sampling, and mesh rendering.

---

## 📸 Key Features

* **Dual-Pipeline Architecture**:
  * **Software Rasterizer**: Pure C++ CPU-based rasterization pipeline featuring triangle rasterization, depth testing (`DepthBuffer`), perspective-correct interpolation, and multi-threading[cite: 3, 4].
  * **Hardware Rasterizer**: Accelerated DirectX 11 GPU pipeline utilizing HLSL vertex and pixel shaders with customizable rasterizer states and texture samplers.
  * **Runtime Switching**: Instant toggle between Software and Hardware execution modes at the press of a key (`F1`).

* **Multi-Threaded C++ Engine**:
  * Multithreaded tile/bounding-box rasterization pass distributing pixel workload across multiple CPU threads using C++ concurrency (`std::thread`).

* **Shading & Lighting Models**:
  * **Software Pipeline (`Software.cpp`)**: Supports Lambert Diffuse, Phong/Blinn-Phong Specular, Tangent-Space Normal Mapping, and customizable shading view modes (Combined, Observed Area, Diffuse Only, Specular Only).
  * **Hardware Pipeline (`Hardware.cpp`)**: HLSL shader passes with support for animated meshes (FireFX/FireMesh blending), diffuse maps, specular maps, glossiness maps, and normal maps.

* **Visualization & Debugging Tools**:
  * Software visualization modes for **Depth Buffer** inspecting and **Triangle Bounding Boxes**.
  * Real-time FPS monitoring and console status display.

---

## 🎮 Interactive Controls

### Mouse Controls

| Input | Action |
| :--- | :--- |
| **RMB + Mouse Move X** | Rotate Camera Yaw |
| **RMB + Mouse Move Y** | Rotate Camera Pitch |
| **LMB + Mouse Move Y** | Move Camera (Local) Forward / Backward |
| **LMB + Mouse Move X** | Rotate Camera Yaw |
| **LMB + RMB + Mouse Move Y** | Move Camera (World) Up / Down |

### Keyboard & Rendering Controls

#### Shared Controls
| Key | Action |
| :--- | :--- |
| **W** / **Up Arrow** | Move Camera (Local) Forward |
| **S** / **Down Arrow** | Move Camera (Local) Backward |
| **A** / **Left Arrow** | Move Camera (Local) Left |
| **D** / **Right Arrow** | Move Camera (Local) Right |
| **LSHIFT** | Boost Camera Movement Speed |
| **F1** | Toggle between DirectX 11 Hardware & C++ Software Rasterizer |
| **F2** | Toggle Mesh/Vehicle Rotation (ON / OFF) |
| **F9** | Cycle Cull Modes (`Back-Face` → `Front-Face` → `None`) |
| **F10** | Toggle Uniform Clear Color (ON / OFF) |
| **F11** | Toggle Print FPS (ON / OFF) |

#### Hardware Mode Controls (DirectX 11)
| Key | Action |
| :--- | :--- |
| **F3** | Toggle FireFX Mesh (ON / OFF)[cite: 3, 4] |
| **F4** | Cycle Texture Filtering Mode (`Point` → `Linear` → `Anisotropic`) |

#### Software Mode Controls (C++ Engine)
| Key | Action |
| :--- | :--- |
| **F5** | Cycle Shading Mode (`Combined` → `Observed Area` → `Diffuse` → `Specular`) |
| **F6** | Toggle Normal Map (ON / OFF) |
| **F7** | Toggle Depth Buffer Visualization (ON / OFF) |
| **F8** | Toggle Bounding Box Visualization (ON / OFF) |

---

## ⚙️ Software Rasterizer Optimization Techniques

1. **Multi-Threaded Tile/Bounding Box Rasterization**:
   Calculates 2D screen-space bounding boxes for primitives and distributes row/tile bounding volumes across worker CPU threads to minimize unnecessary pixel tests.

2. **Early Depth & Back-Face Culling**:
   Performs fast triangle area checks (barycentric cross-products) to reject back-facing geometry early, avoiding perspective division and per-pixel depth buffer writes.

3. **Perspective-Correct Interpolation**:
   Interpolates UV coordinates, surface normals, tangents, and view vectors using inverse-depth ($1/z$) weighting across triangle barycentric coordinates.

4. **Cache-Aligned Buffer Access**:
   Uses contiguous memory buffers for the Color and Depth buffers (`std::vector<float>` / raw array access) to ensure maximum CPU L1/L2 cache hit rates during per-pixel updates.

---

## 🛠️ Repository Structure

```text
├── Main.cpp            # Application entry point, window initialization, and main loop
├── Renderer.h/.cpp     # Pipeline manager (handles context switching between HW/SW)
├── Software.h/.cpp     # Custom multi-threaded C++ software rasterizer pipeline
├── Hardware.h/.cpp     # DirectX 11 hardware rasterizer and D3D device setup
├── Mesh.h/.cpp         # 3D Mesh loading, vertex buffers, and primitive rendering
├── FireMesh.h/.cpp     # Animated/effect mesh representation
├── Texture.h/.cpp      # Texture loading, sampling (Point/Linear), and normal mapping
├── Camera.h            # View matrix transformations, frustum projection, and user input
├── Vertex.h            # Vertex layout definitions (Position, UV, Normal, Tangent)
├── LoadEffects.h       # HLSL shader loading utilities for DirectX 11
└── [Boilerplate / Math / Utilities]