# GLIM

> Raw OpenGL is brutal. Hundreds of lines of boilerplate just to open a window and draw a triangle - shader compilation, buffer binding, stride calculations, manual lifetime management. GLIM exists to make that disappear.

GLIM is a C++ framework built on top of OpenGL that abstracts the painful parts into clean, intuitive objects. No more wrestling with `glGenBuffers` / `glBindBuffer` / `glBufferData` sequences for every draw call. No more manually tracking buffer lifetimes or computing vertex strides by hand. Just objects that do what you'd expect.

---

## Features

- **Window & context management** - create a window in one line, with optional fullscreen, vsync, and framerate limiting
- **Shader wrapper** - load vertex / fragment shaders from files, set uniforms via a type-safe templated API
- **VertexArray** - push a layout, append vertices, draw. Stride and attribute pointers handled automatically
- **SSBO** - upload arbitrary data to a shader storage buffer in a single call
- **Textures** - `Texture` for CPU-loaded images, `GpuTexture` for render targets
- **Camera3D** - FPS-style 3D camera with VP matrix, pitch clamping, and forward/right vectors
- **Input** - per-frame keyboard (down / pressed / released / typed) and mouse (buttons + delta) state
- **Shapes & collision** - 2D and 3D shape structs with a full `Intersects()` overload set
- **Clock & Profiler** - high-resolution elapsed time and averaged FPS over a configurable interval
- **Vector2 / Vector3** - templated math vectors with the operators you'd expect

---

## Quick Start

### Opening a Window

```cpp
#include "GLIM/Graphics.hpp"

int main() {
    gl::Window window(1280, 720, "My App");

    while (window.isOpen()) {
        window.PollEvents();
        window.Clear();
        window.Display();
    }

    return 0;
}
```

### Drawing a Triangle

```cpp
gl::Window window(1280, 720, "Triangle");

gl::Shader shader;
if (!shader.LoadFromFile("vert.glsl", "frag.glsl")) {
    return 1;
}

gl::VertexArray vertices(gl::PrimitiveType::Triangles);
vertices.PushLayout<float>(3); // position

vertices.Append(
    -0.5f, -0.5f, 0.f,
    0.5f, -0.5f, 0.f,
    0.f, 0.5f, 0.f
);

while (window.isOpen()) {
    window.PollEvents();
    window.Clear();

    shader.Use();
    vertices.Draw();

    window.Display();
}
```

Compare that to the equivalent raw OpenGL - compiling and linking shaders manually, generating and binding a VAO and VBO, specifying `glVertexAttribPointer`, managing `glEnableVertexAttribArray`, computing strides... GLIM compresses all of it.

### 3D Camera

```cpp
gl::Camera3D camera;
camera.SetAspect(1280.f / 720.f);
camera.SetPosition({0.f, 1.f, 3.f});

shader.SetUniform("u_Camera", camera);
```

### Uploading an SSBO

```cpp
struct Light { glm::vec3 position; float radius; };
std::vector<Light> lights = { ... };

gl::SSBO lightBuffer(0); // binding index 0
lightBuffer.Upload(lights.data(), lights.size() * sizeof(Light));
```

### Input

```cpp
if (gl::Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE)) {
    window.Close();
}

glm::vec2 mouseDelta = {
    (float)gl::Mouse::GetDelta().x,
    (float)gl::Mouse::GetDelta().y
};

camera.Rotate(mouseDelta * sensitivity);
```

### Profiler

```cpp
gl::Profiler profiler(1.f); // update every second

while (window.isOpen()) {
    if (profiler.Tick()) {
        window.SetMetadata("FPS: " + std::to_string(profiler.GetAverageFPS()));
    }

    // ...
}
```

---

## API Overview

### `gl::Window`

| Method | Description |
|---|---|
| `Window(w, h, title, style, state)` | Create and open a window |
| `PollEvents()` | Process OS events, update input state |
| `Clear(color, clearDepth)` | Clear the framebuffer |
| `Display()` | Swap buffers (+ framerate limiter if set) |
| `SetFramerateLimit(fps)` | Cap framerate; `0` = unlimited |
| `SetVsync(bool)` | Toggle vsync |
| `SetFullscreen(bool)` | Toggle fullscreen, preserves previous windowed size |
| `SetView(view)` | Set the OpenGL viewport region |
| `SetResizeCallback(fn)` | Callback fired on window resize |

### `gl::Shader`

| Method | Description |
|---|---|
| `LoadFromFile(vert, frag)` | Compile and link from file paths |
| `Use()` | Bind the shader program |
| `SetUniform<T>(name, ...)` | Type-safe uniform upload (float, int, uint, vec3, mat4) |
| `SetUniform(name, camera)` | Upload a camera's VP matrix |

### `gl::VertexArray`

| Method | Description |
|---|---|
| `PushLayout<T>(count, normalized)` | Add an attribute (e.g. `PushLayout<float>(3)` for vec3) |
| `Append(args...)` | Append data |
| `Clear()` | Discard all vertex data |
| `Upload()` | Force GPU upload (called automatically by `Draw`) |
| `Draw()` | Upload if dirty, bind, and draw |

### `gl::Camera3D`

| Method | Description |
|---|---|
| `SetPosition(vec3)` / `Move(delta)` | Absolute or relative position |
| `SetRotation(vec2)` / `Rotate(delta)` | Yaw/pitch in radians; pitch is clamped to ±89° |
| `SetFov(degrees)` | Vertical field of view |
| `SetAspect(float)` | Aspect ratio |
| `SetClipping(near, far)` | Clip planes |
| `GetVP()` | Combined view-projection matrix |
| `GetForward()` / `GetRight()` | Oriented direction vectors |

### `gl::Keyboard`

```cpp
Keyboard::IsKeyDown(key)      // held this frame
Keyboard::IsKeyPressed(key)   // went down this frame
Keyboard::IsKeyReleased(key)  // went up this frame
Keyboard::IsKeyTyped(key)     // pressed or held with repeat
```

### `gl::Mouse`

```cpp
Mouse::GetPosition()           // Vector2<double> screen position
Mouse::GetDelta()              // movement since last frame
Mouse::IsButtonPressed(btn)
Mouse::SetPosition(window, pos)
```

---

## Shapes & Collision

GLIM includes 2D and 3D shape types and a comprehensive `Intersects()` overload set covering every shape pair:

**2D:** `Rectangle`, `Circle`, `Triangle`, `Polygon2D`, `Line2D`, `Point2D`  
**3D:** `Cuboid`, `Sphere`, `Cone`, `Cylinder`, `Polygon3D`, `Line3D`, `Point3D`

```cpp
gl::Circle ball{{100.f, 200.f}, 32.f};
gl::Rectangle wall{{0.f, 0.f}, {800.f, 20.f}};

if (gl::Intersects(ball, wall)) {
    // resolve collision
}
```

All overloads are symmetric - `Intersects(a, b)` and `Intersects(b, a)` both exist.

---

## Dependencies

- [GLEW](https://glew.sourceforge.net/)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm)

---

## Building

Add include directory:

```
GLIM/include/
```

Link library:

```
LXXFLAGS := -lglim -lopengl32 -lglew32 -lglfw3
```