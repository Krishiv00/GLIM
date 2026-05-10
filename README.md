# GLIM

Raw OpenGL is brutal. Hundreds of lines of boilerplate just to open a window and draw a triangle - shader compilation, buffer binding, stride calculations, manual lifetime management. GLIM wraps all of that into clean, intuitive C++ objects.

No more `glGenBuffers` / `glBindBuffer` / `glBufferData` sequences for every draw call. No more manually tracking buffer lifetimes or computing vertex strides by hand. Just objects that do what you'd expect.

---

## Features

- **Window & context management** - one-line setup with optional fullscreen, vsync, framerate limiting, and 2D/3D mode
- **Shader wrapper** - load vertex/fragment shaders from files, set uniforms via a type-safe templated API
- **VertexArray** - push a layout, append vertices, draw; stride and attribute pointers handled automatically
- **SSBO** - upload arbitrary data to a shader storage buffer in a single call
- **Textures** - `Texture` for CPU-loaded images, `GpuTexture` for GPU render targets
- **Camera2D / Camera3D** - orthographic and FPS-style cameras with VP matrix output
- **Input** - per-frame keyboard (down / pressed / released / typed) and mouse (buttons + delta) state
- **Shapes & collision** - 2D and 3D shape structs with a full symmetric `Intersects()` overload set
- **Audio** - in-memory `Sound` and streamed `Music`, both backed by OpenAL
- **Clock & Profiler** - high-resolution elapsed time and averaged FPS over a configurable interval
- **Vector2 / Vector3** - templated math vectors with the operators you'd expect

---

## Quick Start

### Opening a Window

```cpp
#include "GLIM/Graphics.hpp"

int main() {
    gl::Window window(1280, 720, "My App", gl::Mode::Mode3D);

    while (window.isOpen()) {
        while (const std::optional<gl::Event> event = window.PollEvents());

        window.Clear();
        window.Display();
    }
}
```

### Drawing a Triangle

```cpp
gl::Shader shader("vert.glsl", "frag.glsl");

gl::VertexArray vertices(gl::PrimitiveType::Triangles);

vertices.PushLayout<float>(3); // position
vertices.PushLayout<float>(3); // color

vertices.Append(
    -0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f,
    0.5f, -0.5f, 0.f, 0.f, 1.f, 0.f,
    0.f, 0.5f, 0.f, 0.f, 0.f, 1.f
);

while (window.isOpen()) {
    // ...
    vertices.Draw();
    // ...
}
```

Compare that to the equivalent raw OpenGL - compiling and linking shaders manually, generating and binding a VAO and VBO, specifying `glVertexAttribPointer`, managing `glEnableVertexAttribArray`, computing strides by hand. GLIM compresses all of it.

### 3D Camera

```cpp
gl::Camera3D camera(gl::Vector3f(0.f, 1.f, 3.f));
camera.SetAspect(1280.f / 720.f);

shader.SetUniform("u_Camera", camera);
```

### Uploading an SSBO

```cpp
struct Light {
    gl::Vector3f Position;
    float Radius;
};

std::vector<Light> lights = { ... };

gl::SSBO lightBuffer(0); // binding index 0
lightBuffer.Upload(lights.data(), lights.size() * sizeof(Light));
```

### Input

```cpp
if (gl::Keyboard::IsKeyPressed(gl::Keyboard::Scancode::Escape)) {
    window.Close();
}

gl::Vector2i mousePos = gl::Mouse::GetPosition(window);
```

### FPS Profiler

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

## API Reference

### `gl::Window`

The entry point for any GLIM application. Manages the OpenGL context, event queue, and frame lifecycle.

```cpp
gl::Window window(1280, 720, "Title", gl::Mode::Mode2D);
```

| Method | Description |
|---|---|
| `Window(w, h, title, mode, style, state)` | Create and open a window |
| `Create(w, h, title, mode, style, state)` | Create on a default-constructed window |
| `PollEvents()` | Returns `std::optional<Event>`; call in a loop |
| `Clear(color, clearDepth)` | Clear the framebuffer |
| `Display()` | Swap buffers; applies framerate limiter if set |
| `Close()` | Signal the window to close |
| `isOpen()` | Returns `false` once closed |
| `SetFramerateLimit(fps)` | Cap framerate; `0` = unlimited |
| `SetVsync(bool)` | Toggle vsync |
| `SetFullscreen(bool)` | Toggle fullscreen, restoring previous windowed size |
| `SetTitle(string)` | Update the window title |
| `SetMetadata(string)` | Update window metadata (e.g. FPS counter) |
| `SetMouseVisible(bool)` | Show or hide the OS cursor |
| `SetKeyRepeatEnabled(bool)` | Enable or suppress key-repeat events |
| `GetSize()` | Returns `Vector2u` |
| `GetPosition()` | Returns `Vector2i` |

**Modes:** `gl::Mode::Mode2D`, `gl::Mode::Mode3D`  
**Styles:** `gl::Style::Default`, `gl::Style::Close`  
**States:** `gl::State::Windowed`, `gl::State::Fullscreen`

---

### `gl::Event`

A type-safe variant wrapping all possible OS events, retrieved from `Window::PollEvents()`.

```cpp
while (auto event = window.PollEvents()) {
    if (const auto* key = event->getIf<gl::Event::KeyPressed>()) {
        // key->Scancode, key->ModShift, key->ModControl, ...
    }
}
```

| Event Type | Fields |
|---|---|
| `Resized` | `Size` (`Vector2u`) |
| `KeyPressed` / `KeyReleased` | `Scancode`, `ModAlt`, `ModControl`, `ModShift`, `ModSystem` |
| `TextEntered` | `Unicode` (`char32_t`) |
| `MouseButtonPressed` / `MouseButtonReleased` | `Button`, `Position` (`Vector2i`) |
| `MouseWheelScrolled` | `Delta` (`float`), `Position` |
| `MouseMoved` | `Position`, `Delta` (`Vector2i`) |
| `MouseEntered` / `MouseLeft` | - |
| `FocusGained` / `FocusLost` | - |

---

### `gl::Keyboard` / `gl::Mouse`

Static input classes for polling state directly, outside the event loop.

```cpp
if (gl::Keyboard::IsKeyPressed(gl::Keyboard::Scancode::W)) { ... }
if (gl::Mouse::IsButtonPressed(gl::Mouse::Button::Left)) { ... }

gl::Vector2i pos = gl::Mouse::GetPosition(window);
gl::Mouse::SetPosition(window, {640, 360});
```

**Common scancodes:** `W A S D`, `Up Down Left Right`, `Space`, `Escape`, `Enter`, `LShift`, `LControl`, `F1`–`F25`, `KP0`–`KP9`  
**Mouse buttons:** `Left`, `Right`, `Middle`, `Button4`–`Button8`

---

### `gl::Shader`

Loads and owns a GLSL program. Uniforms are set via a type-safe templated API.

```cpp
gl::Shader shader("vert.glsl", "frag.glsl");

shader.SetUniform<float>("u_Time", 1.f);
shader.SetUniform<glm::vec3>("u_Color", color);
shader.SetUniform("u_Camera", camera);
```

| Method | Description |
|---|---|
| `Shader(vert, frag)` | Load and compile from file paths |
| `LoadFromFile(vert, frag)` | Reload shaders at runtime; returns `bool` |
| `Use()` | Bind the program |
| `SetUniform<T>(name, args...)` | Set any uniform by type and name |
| `SetUniform(name, camera)` | Upload a camera's VP matrix |
| `GetId()` | Returns the raw GL program handle |

---

### `gl::VertexArray`

CPU-side vertex buffer with automatic stride and attribute management.

```cpp
gl::VertexArray vertices(gl::PrimitiveType::Triangles);

vertices.PushLayout<float>(3); // position
vertices.PushLayout<float>(2); // tex pos

vertices.Append(
    0.f, 0.f, 0.f, 0.f, 0.f,
    1.f, 0.f, 0.f, 1.f, 0.f,
    0.f, 1.f, 0.f, 0.f, 1.f
);

vertices.Draw();
```

| Method | Description |
|---|---|
| `PushLayout<T>(count, normalized)` | Add a vertex attribute of type `T` with `count` components |
| `Append(args...)` | Append one vertex worth of data |
| `Draw()` | Upload if dirty, then draw |
| `Clear()` | Erase all vertex data |
| `Upload()` | Force upload to the GPU |
| `GetVertexCount()` | Returns number of vertices |
| `SetPrimitiveType(type)` | Change draw mode |

**Primitive types:** `Points`, `Lines`, `LineStrip`, `Triangles`, `TriangleStrip`, `TriangleFan`

---

### `gl::SSBO`

Uploads arbitrary data to a Shader Storage Buffer Object at a given binding index.

```cpp
gl::SSBO ssbo(0);
ssbo.Upload(data.data(), data.size() * sizeof(MyStruct));
```

| Method | Description |
|---|---|
| `SSBO(index)` | Construct bound to a binding index |
| `Upload(ptr, size)` | (Re)allocate and upload raw bytes |

---

### `gl::Texture` / `gl::GpuTexture`

`Texture` loads an image from disk. `GpuTexture` is a GPU-side render target created programmatically.

```cpp
gl::Texture tex;
tex.LoadFromFile("sprite.png");
tex.Bind();

gl::GpuTexture target(0); // image unit 0
target.Create(1280, 720);
target.Bind();
```

| Method | `Texture` | `GpuTexture` |
|---|---|---|
| `LoadFromFile(path)` | ✓ (returns `bool`) | - |
| `Create(w, h)` | - | ✓ |
| `Bind()` | ✓ | ✓ |
| `Clear(color)` | ✓ | ✓ |
| `SetSmooth(bool)` | ✓ | ✓ |
| `SetRepeated(bool)` | ✓ | ✓ |

---

### `gl::Camera2D` / `gl::Camera3D`

Both expose `GetVP()` returning a `glm::mat4` for use as a shader uniform.

#### Camera2D

```cpp
gl::Camera2D cam({0.f, 0.f}, windowSize);
cam.Move({10.f, 0.f});
cam.SetZoom(2.f);
shader.SetUniform("u_Camera", cam);
```

| Method | Description |
|---|---|
| `Camera2D(position, size)` | Construct with world position and viewport size |
| `Move(delta)` | Translate by delta |
| `Rotate(delta)` | Rotate by delta (radians) |
| `Zoom(delta)` | Add to zoom factor |
| `SetPosition / SetSize / SetRotation / SetZoom` | Direct setters |
| `GetPosition / GetSize / GetRotation / GetZoom` | Getters |

Zoom is clamped to `[0.01, 100.0]`.

#### Camera3D

```cpp
gl::Camera3D cam({0.f, 1.f, 3.f}, {0.f, 0.f});
cam.SetAspect(1280.f / 720.f);
cam.SetFov(60);
shader.SetUniform("u_Camera", cam);
```

| Method | Description |
|---|---|
| `Camera3D(position, rotation)` | Construct at world position with yaw/pitch |
| `Move(delta)` | Translate in world space |
| `Rotate(delta)` | Yaw/pitch delta (radians); pitch clamped to ±89° |
| `SetPosition / SetRotation` | Direct setters |
| `SetFov(degrees)` | Field of view |
| `SetAspect(float)` | Viewport aspect ratio |
| `SetClipping(near, far)` | Clipping planes |
| `GetForward / GetFront / GetRight` | Direction vectors |

---

### `gl::Vector2<T>` / `gl::Vector3<T>`

Templated math vectors. Common aliases: `Vector2f`, `Vector2i`, `Vector2u`, `Vector3f`, `Vector3i`, `Vector3u`.

```cpp
gl::Vector2f a{1.f, 0.f};
gl::Vector2f b{0.f, 1.f};

float d = a.Dot(b);
float c = a.Cross(b);
float len = a.Length();
gl::Vector2f n = a.Normalize();
```

**Operators:** `+`, `-`, `*` (scalar), `/`, `+=`, `-=`, `*=`, `/=`, `==`, `!=`, unary `-`  
**Methods:** `Dot`, `Cross`, `Length`, `LengthSquared`, `Normalize`

---

### `gl::Color`

RGBA color with float components in `[0, 1]`.

```cpp
gl::Color c(1.f, 0.5f, 0.f); // RGB, alpha = 1
gl::Color c(1.f, 0.5f, 0.f, 0.5f); // RGBA

window.Clear(gl::Color::Black);
```

**Predefined:** `Black`, `White`, `Red`, `Green`, `Blue`, `Yellow`, `Magenta`, `Cyan`, `Transparent`

---

### `gl::Clock`

High-resolution timer backed by `std::chrono::steady_clock`.

```cpp
gl::Clock clock;

float dt = clock.Restart(); // seconds since last restart
float elapsed = clock.GetElapsedTime();
```

---

### `gl::Profiler`

Tracks average FPS over a configurable interval.

```cpp
gl::Profiler profiler(1.f);

while (window.isOpen()) {
    if (profiler.Tick()) {
        window.SetMetadata("FPS: " + std::to_string(profiler.GetAverageFPS()));
    }
}
```

---

### `gl::VideoMode`

Query display capabilities.

```cpp
gl::VideoMode desktop = gl::VideoMode::GetDesktopMode();

// desktop.Size -> Vector2u
// desktop.RefreshRate -> unsigned int
```

---

### Shapes & Collision

2D and 3D shape structs with a symmetric `Intersects()` overload for every pair.

**2D:** `Rectangle`, `Circle`, `Triangle`, `Polygon2D`, `Line2D`, `Point2D`  
**3D:** `Cuboid`, `Sphere`, `Cone`, `Cylinder`, `Polygon3D`, `Line3D`, `Point3D`

```cpp
gl::Circle ball{{100.f, 200.f}, 32.f};
gl::Rectangle wall{{0.f, 0.f}, {800.f, 20.f}};

if (gl::Intersects(ball, wall)) { /* resolve */ }
```

Both argument orders are supported - `Intersects(a, b)` and `Intersects(b, a)`.

---

### Audio

#### `gl::AudioDevice`

Singleton that owns the OpenAL context and handles hot-plug device changes. Instantiate once, before any `Sound` or `Music`, and keep it alive for the duration of your app.

```cpp
gl::AudioDevice audio;
```

#### `gl::Sound`

Fully in-memory audio for short effects. Supports WAV and MP3. Move-constructible and move-assignable.

```cpp
gl::Sound sfx("click.wav");
sfx.SetVolume(0.8f);
sfx.Play();
```

#### `gl::Music`

Streamed audio for long tracks. Buffers in a background thread to keep memory overhead low.

```cpp
gl::Music bgm("theme.mp3");
bgm.SetLoop(true);
bgm.Play();
```

#### Shared `AudioSource` API

| Method | Description |
|---|---|
| `Play()` | Start playback from the beginning |
| `Pause()` | Pause at current position |
| `Resume()` | Resume from paused position |
| `Stop()` | Stop and reset position |
| `SetVolume(float)` | `0.0`–`1.0+` |
| `SetPitch(float)` | Playback speed multiplier |
| `SetLoop(bool)` | Loop playback |
| `IsPlaying / IsPaused / IsStopped` | State queries |

---

## Building

Add to your include path:

```
GLIM/include/
```

Link flags:

```
LXXFLAGS := -lglim -lopengl32 -lglew32 -lglfw3 -lOpenAL32
```

### Dependencies

| Library | Purpose |
|---|---|
| [GLEW](https://glew.sourceforge.net/) | OpenGL extension loading |
| [GLFW](https://www.glfw.org/) | Window and input |
| [GLM](https://github.com/g-truc/glm) | Math |
| [stb_image](https://github.com/nothings/stb) | Image loading |
| [OpenAL](https://www.openal.org/) | Audio |
| [minimp3](https://github.com/lieff/minimp3) | MP3 decoding |