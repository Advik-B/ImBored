# Svelte-like C++ UI — Feature & Design Sheet

> A complete feature and design sheet for a retained-mode, GPU-accelerated, Svelte-style reactive UI library in Modern C++ (STL-first). Includes architecture graph, API summary, lifecycle, resource rules, and three example apps: **Hello World**, **Hello Counter**, **Hello Todo**.

---

## 1. One-line summary

Native retained-mode UI engine with GPU acceleration, declarative C++ widget tree, and a Svelte-style reactivity layer (`state`, `derived`, `effect`) that cleanly separates business logic (models & services) from rendering and UX.

Goals: predictable performance, minimal dependencies, STL-first API, composable reactivity, easy-to-read C++ "components" without HTML/JS/CSS.

---

## 2. Key features

- **Retained Mode Widget Tree** (tree of `Widget` objects; measure → layout → paint cycle)  
- **GPU-accelerated rendering**: OpenGL backend initially, batched draw commands, font atlas, vector tessellation (LunaSVG)  
- **Svelte-like Reactivity**: `state<T>`, `derived<T>`, `effect(fn)` — dependency-tracking, lazy `derived`, automatic invalidation of widgets
- **STL-first API**: `std::string`, `std::vector`, `std::span`, `std::unique_ptr`, `std::function` etc.  
- **ViewModel / Binding Layer**: small glue layer mediating domain models and widgets; only this layer may reference domain + UI types.  
- **Resource Managers**: `FontManager`, `TextureManager`, `SvgManager` exposing opaque handles to widgets only. GPU ownership remains in renderer.  
- **Layout engine**: flexbox-inspired layout for rows, columns, spacing, alignment, min/max sizes, scroll containers.  
- **Worker thread pool**: for asset decoding, font rasterization, shaping, and SVG tessellation. Main thread owns widget tree & GPU uploads.  
- **Optional ImGui debug overlay** for devtools & inspectors.  

---

## 3. Architecture graph (text / ASCII)

```
+-----------------------------------------------------------+
| Application Layer (Business logic, services, repos)       |
| - Domain models (no UI includes)                          |
+-----------------------------------------------------------+
              ^                         |
              |                         v
+-----------------------------------------------------------+
| ViewModel / Binding Layer (UI glue)                       |
| - Observables (state/derived/effects)                    |
| - Converts domain to UI primitives (strings, enums)      |
+-----------------------------------------------------------+
              ^                         |
              |                         v
+-----------------------------------------------------------+
| Retained UI Layer                                         |
| - Widget tree (Widget, Container, Controls)              |
| - Layout engine (measure/layout)                         |
| - Style system (POD style structs)                       |
+-----------------------------------------------------------+
              ^                         |
              | emits DrawCmds         v
+-----------------------------------------------------------+
| Compositor & Renderer                                     |
| - Display list -> Batcher -> GPU                          |
| - Font atlas (FreeType + HarfBuzz)                        |
| - Vector tessellation (LunaSVG)                           |
+-----------------------------------------------------------+
              ^                         |
              | requests/loads         v
+-----------------------------------------------------------+
| Platform & Workers                                        |
| - GLFW / native shims (windows, dialogs, clipboard)      |
| - Worker threads for IO, decoding, shaping                |
+-----------------------------------------------------------+
```

---

## 4. Core reactive primitives (Svelte analogues)

All API is header-only friendly and STL-centric.

### `state<T>`
- Holds a value of type `T`. Change with `.set(v)`; read with `.get()`.
- Notifies dependents when `set()` is called.
- Light-weight; subscribable.

### `derived<T>`
- Constructed with a `std::function<T()>` that reads `state`/`derived` values.
- Lazy: recomputed only when `.get()` is called and dependencies are dirty.
- Automatically tracks dependencies during computation.

### `effect(fn)`
- Runs `fn()` immediately and re-runs it whenever any reactive dependency used within `fn()` changes.
- Intended for side-effects: updating widgets, logging, IO triggers (viewmodel only should enqueue real IO).

### Dependency-tracking model
- Thread-local tracker records which `Node` (derived/effect) is currently computing and subscribes it to any `state` read during that computation.
- Notifications propagate to subscribers which mark themselves dirty (for derived) or schedule effect callbacks.

---

## 5. Public API sketch (minimal, stable surface)

```cpp
namespace ui {

// Basic geometry
struct Dip { float v; };
struct Size { Dip w,h; };
struct Rect { Dip x,y,w,h; };
struct Color { float r,g,b,a; };

// Widget base (public, PIMPL internal)
class Widget {
public:
  virtual Size measure(const Size& constraints) = 0;
  virtual void layout(const Rect& bounds) = 0;
  virtual void paint(PaintContext& ctx) = 0;
  virtual bool on_event(const Event& ev) = 0;

  void mark_dirty_layout();
  void mark_dirty_paint();
};

class Window { /* create, show, set_root */ };
class App { /* create, run, quit */ };

} // ui
```

Also: `state<T>`, `derived<T>`, `effect()` come from `rx::` namespace (reactive layer).

---

## 6. Lifecycle rules & invalidation

- **Measure pass:** compute desired sizes going top-down with constraints. Only runs when layout dirty.  
- **Layout pass:** assigns geometry; children get bounds. Only runs if layout dirty.  
- **Paint pass:** widgets emit `DrawCmd` PODs in paint; only if paint dirty.  
- **Compositor:** consumes `std::vector<DrawCmd>` and batches for GPU; GPU uploads happen on main thread.  

**Invalidation primitives:** `mark_dirty_layout()` triggers measure+layout+paint for subtree. `mark_dirty_paint()` triggers only paint for subtree. Coalesce per-frame.

---

## 7. Resource managers (contracts)

- `FontManager`: exposes `FontHandle load_font(path, size)`, `GlyphRun shape_text(font, text)`. Worker threads rasterize glyphs; main thread inserts into atlas and provides atlas regions.
- `TextureManager`: `TextureHandle load_image(path)`; decode on worker, upload on main thread.
- `SvgManager`: `MeshHandle load_svg(path)`; tessellate on worker, upload mesh on main thread.

Widgets use only handles (opaque integers or small structs). DrawCmd references handles, not raw data.

---

## 8. Style system (no CSS file required)

A `Style` is a POD struct. Widgets accept a `Style` pointer or copy.

```cpp
struct ButtonStyle { Color bg, fg; float padding; float corner_radius; FontHandle font; };
```

Themes are simply collections of style structs. Optionally allow a small compile-time/parsed style language that compiles into these structs.

---

## 9. Threading & async patterns

- **UI thread**: widget tree, layout, paint emission, GPU uploads. Single-threaded for simplicity and predictable semantics.
- **Worker threads**: asset loading, shaping, tessellation. Communicate results via a `MainThreadQueue` (thread-safe) which schedules callbacks executed on the next frame.
- **Best practice**: Worker produces immutable data (mesh, bitmap) and posts `on_main([data]{ resource_manager.register(data); root->mark_dirty_paint(); })`.

---

## 10. Testing strategy

- **Unit tests (no GL)**: layout engine, style merging, reactive primitives, resource manager packer logic. Mock `DrawCmd` outputs.  
- **Snapshot tests**: record `DrawCmd` lists for sample UIs and compare.  
- **Integration (CI)**: run headless GL (or software GL) smoke tests; measure draw call counts.

---

## 11. Example apps

All examples assume the following namespaces:
- UI types: `ui::` (Widget, Button, Label, Window, App, etc.)
- Reactive: `rx::state`, `rx::derived`, `rx::effect`

### A) Hello World

**Description:** simplest app; shows a label in a window.

```cpp
#include <ImBored/ui.hpp>
#include <ImBored/reactive.hpp>

int main() {
  auto app = ui::App::create({});
  auto win = app->create_window({"Hello", {400,200}});

  auto label = ui::Label::create("Hello, native web-like UI!");
  win->set_root(label);
  app->run();
}
```

Expected behavior: window appears, label shows static text. No reactive primitives used.

---

### B) Hello Counter

**Description:** demonstrates `state`, `effect`, widget callbacks, and invalidation.

```cpp
#include <ImBored/ui.hpp>
#include <ImBored/reactive.hpp>

int main() {
  auto app = ui::App::create({});
  auto win = app->create_window({"Counter", {400,200}});

  auto vm = std::make_shared<struct CounterVM>();
  vm->count = rx::state<int>(0);
  vm->label = rx::derived<std::string>([&]{
    return std::string("Count: ") + std::to_string(vm->count.get());
  });

  auto label = ui::Label::create("");
  auto btn = ui::Button::create("Increment", [vm]{ vm->count.set(vm->count.get() + 1); });

  // wire reactive effect to update label text and invalidate
  rx::effect([=]{
    label->set_text(vm->label.get());
    label->mark_dirty_paint();
  });

  auto root = ui::Column::create({ label, btn });
  win->set_root(root);
  app->run();
}
```

Notes:
- `rx::state` and `rx::derived` power the reactive update. `rx::effect` updates widget state and requests repaint. Domain logic (vm->count set) does not include UI calls.

---

### C) Hello Todo

**Description:** a small todo list app with add/remove, persistent model not shown, demonstrates `state<vector<T>>`, list binding, and derived summaries.

```cpp
#include <ImBored/ui.hpp>
#include <ImBored/reactive.hpp>

struct TodoItem { int id; std::string text; bool done; };

int main() {
  auto app = ui::App::create({});
  auto win = app->create_window({"Todo", {600,400}});

  // ViewModel
  struct TodoVM {
    rx::state<std::vector<TodoItem>> items{std::vector<TodoItem>{}};
    int next_id = 1;
    void add(std::string text) { auto v = items.get(); v.push_back({next_id++, text, false}); items.set(v); }
    void toggle(int id) { auto v = items.get(); for (auto &it: v) if (it.id==id) it.done = !it.done; items.set(v); }
    void remove_done() { auto v = items.get(); v.erase(std::remove_if(v.begin(), v.end(), [](auto &it){ return it.done; }), v.end()); items.set(v); }
  };

  auto vm = std::make_shared<TodoVM>();

  auto input = ui::TextField::create("");
  auto addBtn = ui::Button::create("Add", [vm, input]{ vm->add(input->get_text()); input->set_text(""); input->mark_dirty_paint(); });
  auto listView = ui::ListView::create();

  // effect to keep listView in sync with vm->items
  rx::effect([=]{
    auto items = vm->items.get();
    listView->clear_items();
    for (auto &it: items) {
      auto row = ui::Row::create({
        ui::Checkbox::create(it.done, [vm, id=it.id]{ vm->toggle(id); }),
        ui::Label::create(it.text)
      });
      listView->append_item(row);
    }
    listView->mark_dirty_layout();
  });

  auto rmBtn = ui::Button::create("Remove Done", [vm]{ vm->remove_done(); });

  auto root = ui::Column::create({ ui::Row::create({input, addBtn}), listView, rmBtn });
  win->set_root(root);
  app->run();
}
```

Notes:
- `vm->items` is a `rx::state<std::vector<TodoItem>>`. The effect rebuilds the ListView as the model changes — this is simple and explicit; for larger lists, provide a virtualized `ListView` that accepts an items-provider function to avoid rebuilding UI nodes frequently.

---

## 12. Implementation patterns & best practices

- **Prefer small derived values** (strings/indices) over recomputing large structures in `derived`. Use manual batching where necessary.  
- **Effects should not do heavy work** — they should mutate UI or schedule IO via services. Use worker threads for heavy tasks.  
- **Widgets should emit `DrawCmd` only**: no GL calls from widgets. Keep GPU ownership in renderer.  
- **Keep `state<T>` coarse for model-level data, fine-grained for UI state** (selection indexes, input contents). Avoid massive `state<vector<T>>` churn; use specialized methods on viewmodels to perform batched updates.

---

## 13. Sprint plan (first 8 sprints, 1 week each)

1. Repo scaffold, CMake, App/Window basic. (GLFW + OpenGL)  
2. Reactive core (`state`, `derived`, `effect`) with unit tests.  
3. Renderer skeleton: DrawCmd model & textured quad shader.  
4. Widget base, simple Label and Button. Invalidation system.  
5. Text pipeline: FreeType + HarfBuzz + FontManager + atlas.  
6. ListView + ScrollView + layout engine (flex).  
7. SvgManager (LunaSVG) + icon support; batching & compositor caching.  
8. Demo apps: Hello World, Counter, Todo; CI + snapshot tests.

---

## 14. Risks & mitigations

- **Reactive cycles**: avoid or detect cycles between derived/effect; provide cycle detection with runtime assertion for debug builds.  
- **Worker-main races**: ensure main-thread registration of resources and post callbacks to main loop.  
- **Atlas fragmentation**: implement multi-atlas fallback and a defragmentation policy.  
- **Large lists**: provide virtualization for large lists; avoid full UI rebuilds inside effects.

---

## 15. Next deliverables I can produce immediately

- Production-ready `reactive.hpp` header with thread-safety and refcounting.  
- `include/ui.hpp` public API header for the widget surface.  
- Starter repo skeleton (CMake + single-window + minimal widget) as a zip.  
- Working counter & todo demo as runnable code (single-file projects).  

Tell me which one to produce next and I’ll output it right away.

---

*End of Feature & Design Sheet.*



## 8. Class-Based Component Example (Qt-like API)

Below is a full example showing how to structure UI code using **C++ classes**, similar to Qt’s `QObject` + `QWidget` style, but using our reactive system.

### Example: `CounterWindow` Class

```cpp
class CounterViewModel {
public:
    rx::state<int> count = 0;

    rx::derived<std::string> label = [&] {
        return "Count = " + std::to_string(count.get());
    };

    void increment() {
        count.set(count.get() + 1);
    }
};

class CounterWindow : public ui::Window {
public:
    std::shared_ptr<CounterViewModel> vm;

    ui::Label* lbl;
    ui::Button* btn;

    CounterWindow() {
        vm = std::make_shared<CounterViewModel>();

        // Root layout
        auto column = add_child<ui::Column>();

        lbl = column->add_child<ui::Label>("Initializing...");
        btn = column->add_child<ui::Button>("Add");

        // Bind button click to viewmodel
        btn->on_click([this] {
            vm->increment();
        });

        // Reactive binding of label text
        rx::effect([this] {
            lbl->set_text(vm->label.get());
            lbl->mark_dirty_paint();
        });
    }
};
```

### Usage

```cpp
int main() {
    ui::App app;
    CounterWindow win;
    win.show();
    return app.run();
}
```

### Key Features Demonstrated
- **Qt-like class-based design** (`CounterWindow`, `CounterViewModel`)
- ViewModel containing pure reactive state
- UI construction done in constructor (like Qt)
- Signals → Slots equivalent via reactive `$effect` bindings
- Clean separation: UI never stores logic beyond simple wiring
- No virtual DOM — all direct retained-mode widgets

This example mirrors typical Qt patterns:
- Qt: `connect(button, &QPushButton::clicked, this, &MyWindow::increment);`
- Ours: `btn->on_click([this]{ vm->increment(); });`

But adds Svelte-like reactivity for the rendering updates.

Note: The examples use snake_case, but ignore that. we should always write it in camelCase