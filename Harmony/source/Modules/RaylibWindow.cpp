#include "RaylibWindow.h"
#include "Modules/Details/RaylibWindowConfig.inc"
#include <raylib.h>
#include <Harmony/Core/Registry.h>

#include "Harmony/Core/Engine.h"

namespace Harmony
{
    RaylibWindow::RaylibWindow(Engine& engine)
        : IWindow("window_raylib", engine) {}

    // ==========================================
    // Extension overrides
    // ==========================================

    void RaylibWindow::onInitialize(const Properties& properties)
    {
        int32_t width = DEFAULT_WINDOW_WIDTH;
        if (auto optWidth = properties.get<int32_t>({"width"})) {
            width = *optWidth;
        } else {
            m_logger->warn("Property 'width' not found. Using default: {}", width);
        }

        int32_t height = DEFAULT_WINDOW_HEIGHT;
        if (auto optHeight = properties.get<int32_t>({"height"})) {
            height = *optHeight;
        } else {
            m_logger->warn("Property 'height' not found. Using default: {}", height);
        }

        std::string title = DEFAULT_WINDOW_TITLE;
        if (auto optTitle = properties.get<std::string>({"title"})) {
            title = *optTitle;
        } else {
            m_logger->warn("Property 'title' not found. Using default: {}", title);
        }

        InitWindow(width, height, title.c_str());

        if (auto optVsync = properties.get<bool>({"vsync"})) {
            setVSync(*optVsync);
        } else {
            m_logger->warn("Property 'vsync' not found. Using default: {}", DEFAULT_WINDOW_VSYNC);
            setVSync(DEFAULT_WINDOW_VSYNC);
        }
    }

    void RaylibWindow::onFinalize() { CloseWindow(); }

    void RaylibWindow::onUpdate() {
        // Core engine updates managed by Kernel
    }

    void RaylibWindow::onRender()
    {
        // Raylib requires drawing to be wrapped in these calls to swap buffers
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Future render systems will draw here...

        EndDrawing();
    }

    void RaylibWindow::onEvent() {// Check if the user pressed the close button or the ESC key
        if (WindowShouldClose()) {
            m_logger->info("Window close event detected. Signalling engine shutdown...");
            m_engine.stop();
        }

    }

    // ==========================================
    // Lifecycle & Core Loop
    // ==========================================

    void RaylibWindow::processEvents() {}
    void RaylibWindow::swapBuffers() {}
    void RaylibWindow::close() { WindowShouldClose(); }
    bool RaylibWindow::shouldClose() const { return WindowShouldClose(); }

    // ==========================================
    // Dimensions & Limits
    // ==========================================
    uint32_t RaylibWindow::getWidth() const { return static_cast<uint32_t>(GetScreenWidth()); }
    uint32_t RaylibWindow::getHeight() const { return static_cast<uint32_t>(GetScreenHeight()); }
    void RaylibWindow::setWidth(uint32_t width) { SetWindowSize(static_cast<int>(width), GetScreenHeight()); }
    void RaylibWindow::setHeight(uint32_t height) { SetWindowSize(GetScreenWidth(), static_cast<int>(height)); }
    void RaylibWindow::setSize(uint32_t width, uint32_t height) { SetWindowSize(static_cast<int>(width), static_cast<int>(height)); }
    void RaylibWindow::setMinSize(uint32_t width, uint32_t height) { SetWindowMinSize(static_cast<int>(width), static_cast<int>(height)); }
    void RaylibWindow::setMaxSize(uint32_t width, uint32_t height) { SetWindowMaxSize(static_cast<int>(width), static_cast<int>(height)); }
    float RaylibWindow::getAspectRatio() const { return static_cast<float>(GetScreenWidth()) / static_cast<float>(GetScreenHeight()); }

    // ==========================================
    // Position
    // ==========================================
    int32_t RaylibWindow::getPositionX() const { return static_cast<int32_t>(GetWindowPosition().x); }
    int32_t RaylibWindow::getPositionY() const { return static_cast<int32_t>(GetWindowPosition().y); }
    void RaylibWindow::setPosition(int32_t x, int32_t y) { SetWindowPosition(x, y); }
    void RaylibWindow::centerWindow()
    {
        const int monitor = GetCurrentMonitor();
        const int mx = GetMonitorWidth(monitor);
        const int my = GetMonitorHeight(monitor);
        SetWindowPosition((mx - GetScreenWidth()) / 2, (my - GetScreenHeight()) / 2);
    }

    // ==========================================
    // Window States & Modes
    // ==========================================
    void RaylibWindow::setWindowMode(WindowMode mode)
    {
        if (mode == WindowMode::Fullscreen) {
            if (!IsWindowFullscreen()) ToggleFullscreen();
        } else if (mode == WindowMode::BorderlessWindowed) {
            if (!IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) ToggleBorderlessWindowed();
        } else {
            if (IsWindowFullscreen()) ToggleFullscreen();
            if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) ToggleBorderlessWindowed();
        }
    }

    WindowMode RaylibWindow::getWindowMode() const
    {
        if (IsWindowFullscreen()) return WindowMode::Fullscreen;
        if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) return WindowMode::BorderlessWindowed;
        return WindowMode::Windowed;
    }

    bool RaylibWindow::isResizable() const { return IsWindowState(FLAG_WINDOW_RESIZABLE); }
    void RaylibWindow::setResizable(bool enabled) { if (enabled) SetWindowState(FLAG_WINDOW_RESIZABLE); else ClearWindowState(FLAG_WINDOW_RESIZABLE); }
    bool RaylibWindow::isMinimized() const { return IsWindowMinimized(); }
    void RaylibWindow::minimize() { MinimizeWindow(); }
    bool RaylibWindow::isMaximized() const { return IsWindowMaximized(); }
    void RaylibWindow::maximize() { MaximizeWindow(); }
    void RaylibWindow::restore() { RestoreWindow(); }
    bool RaylibWindow::isHidden() const { return IsWindowHidden(); }
    void RaylibWindow::show() { ClearWindowState(FLAG_WINDOW_HIDDEN); }
    void RaylibWindow::hide() { SetWindowState(FLAG_WINDOW_HIDDEN); }
    bool RaylibWindow::isFocused() const { return IsWindowFocused(); }
    void RaylibWindow::focus() { SetWindowFocused(); }
    void RaylibWindow::requestAttention() {} // Unsupported natively by Raylib

    float RaylibWindow::getOpacity() const { return 1.0f; /* Raylib lacks GetWindowOpacity getter */ }
    void RaylibWindow::setOpacity(float opacity) { SetWindowOpacity(opacity); }

    // ==========================================
    // Aesthetics
    // ==========================================
    std::string RaylibWindow::getTitle() const { return ""; /* Raylib lacks GetWindowTitle getter */ }
    void RaylibWindow::setTitle(const std::string& title) { SetWindowTitle(title.c_str()); }
    void RaylibWindow::setIcon(const std::filesystem::path& filepath)
    {
        Image icon = LoadImage(filepath.string().c_str());
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    // ==========================================
    // Graphics & Renderer Integration
    // ==========================================
    bool RaylibWindow::isVSync() const { return IsWindowState(FLAG_VSYNC_HINT); }
    void RaylibWindow::setVSync(bool enabled) { if (enabled) SetWindowState(FLAG_VSYNC_HINT); else ClearWindowState(FLAG_VSYNC_HINT); }
    void* RaylibWindow::getNativeWindow() const { return GetWindowHandle(); }
    void* RaylibWindow::getNativeDisplay() const { return nullptr; }
    void* RaylibWindow::getNativeContext() const { return nullptr; }
    void RaylibWindow::makeContextCurrent() {}

    // ==========================================
    // Cursor & OS Integration
    // ==========================================
    void RaylibWindow::setCursorMode(CursorMode mode)
    {
        switch (mode) {
            case CursorMode::Normal: ShowCursor(); EnableCursor(); break;
            case CursorMode::Hidden: HideCursor(); EnableCursor(); break;
            case CursorMode::Locked: HideCursor(); DisableCursor(); break;
        }
    }

    CursorMode RaylibWindow::getCursorMode() const
    {
        if (IsCursorHidden()) return CursorMode::Hidden;
        return CursorMode::Normal;
    }

    void RaylibWindow::setClipboardText(const std::string& text) { SetClipboardText(text.c_str()); }
    std::string RaylibWindow::getClipboardText() const { return ::GetClipboardText(); }

    // Auto-Register the Window with the Registry
    HARMONY_REGISTER(Extension, RaylibWindow, "window_raylib", Engine&)

} // namespace Harmony