#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include <glad/gl.h>
#endif

#include <algorithm>
#include <memory>
#include <cmath>

#include <demo/canvas.hpp>
#include <demo/input.hpp>
#include <demo/demo.hpp>

namespace {

class OpenglCanvas : public demo::Canvas {

private:
    demo::Vec2 size_;
    demo::Color fill_;
    demo::Color stroke_;
    bool no_fill_ = false;
    bool no_stroke_ = false;

public:
    OpenglCanvas(demo::Vec2 size)
        : size_(size) {}

    demo::Vec2 size() const override { return size_; }

    void fill(demo::Color c) override { no_fill_ = false; fill_ = c; }
    void stroke(demo::Color c) override { no_stroke_ = false; stroke_ = c; }
    void no_fill() override { no_fill_ = true; }
    void no_stroke() override { no_stroke_ = true; }

    void clear() override {
        if (no_fill_) return;

        glClearColor(fill_.r / 255.0f, fill_.g / 255.0f, fill_.b / 255.0f, fill_.a / 255.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void line(demo::Vec2 a, demo::Vec2 b) override {
        if (no_stroke_) return;
    }

    void circle(demo::Vec2 center, float radius) override {
        if (no_fill_) return;
    }
};

struct HostData {
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context;
    std::unique_ptr<OpenglCanvas> canvas;
    std::unique_ptr<demo::Demo> demo; // fetched from create_demo()
    demo::InputData inputdata;
    Uint64 last_ns;
};

}

SDL_AppResult SDL_AppInit(void** state, int argc, char** argv) {
    HostData* hostdata = new HostData;
    *state = hostdata;

    hostdata->demo = demo::create_demo(); // this is how we'll call the demo-specific update functions;
    const demo::Vec2 size = hostdata->demo->preferred_size();
    const char* title = hostdata->demo->title();

    if (!SDL_Init(SDL_INIT_VIDEO))
        return SDL_APP_FAILURE;

    #ifdef __EMSCRIPTEN__
        const char* canvas_selector = (argc > 1) ? argv[1] : "#canvas";

        SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR, canvas_selector);
        SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, canvas_selector + 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);   // ES 3.0 ≈ WebGL2
    #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    #endif

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    hostdata->window = SDL_CreateWindow(title, int(size.x), int(size.y), SDL_WINDOW_OPENGL);
    hostdata->gl_context = SDL_GL_CreateContext(hostdata->window);
    SDL_GL_SetSwapInterval(1);

    #ifndef __EMSCRIPTEN__
        if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
            SDL_Log("gladLoadGL failed");
            return SDL_APP_FAILURE;
        }
    #endif

    hostdata->canvas = std::make_unique<OpenglCanvas>(size);
    hostdata->last_ns = SDL_GetTicksNS();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* state, SDL_AppResult) {
    auto* hostdata = static_cast<HostData*>(state);
    if (!hostdata) return;
    
    if (hostdata->window)   SDL_DestroyWindow(hostdata->window);

    delete hostdata;
}

SDL_AppResult SDL_AppIterate(void* state) {
    HostData* hostdata = static_cast<HostData*>(state);

    const Uint64 now = SDL_GetTicksNS();
    double dt = std::min(double(now - hostdata->last_ns) / 1e9, 0.1);
    hostdata->last_ns = now;

    hostdata->demo->update(dt, hostdata->inputdata);
    hostdata->demo->draw(*hostdata->canvas);
    
    SDL_GL_SwapWindow(hostdata->window);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* state, SDL_Event* e) {
    HostData* hostdata = static_cast<HostData*>(state);
    switch (e->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_MOUSE_MOTION:
            hostdata->inputdata.pointer = {e->motion.x, e->motion.y};
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (e->button.button == SDL_BUTTON_LEFT)
                hostdata->inputdata.pointer_held =
                    (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            break;
    }
    return SDL_APP_CONTINUE;
}