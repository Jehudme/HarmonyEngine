#include "RaylibWindow.h"

#include <raylib.h>
#include <Harmony/Core/Registry.h>

namespace Harmony
{
    RaylibWindow::RaylibWindow(Engine& engine)
        : IWindow("RaylibWindow", engine) {}

    // ==========================================
    // Controller overrides
    // ==========================================

    void RaylibWindow::onInitialize(const Properties& properties)
    {
        const int width  = properties.get<int>({"width"}).value_or(800);
        const int height = properties.get<int>({"height"}).value_or(600);
        m_title = properties.get<std::string>({"title"}).value_or("Harmony Engine");

        InitWindow(width, height, m_title.c_str());
    }

    void RaylibWindow::onFinalize()
    {
        CloseWindow();
    }

    void RaylibWindow::onUpdate() {}
    void RaylibWindow::onRender()
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
    }
    void RaylibWindow::onEvent() {}

    // ==========================================
    // Lifecycle & Core Loop
    // ==========================================

    void RaylibWindow::processEvents() {}

    void RaylibWindow::swapBuffers()
    {
        EndDrawing();
    }

    void RaylibWindow::close()
    {
        m_shouldClose = true;
    }

    bool RaylibWindow::shouldClose() const
    {
        return m_shouldClose || WindowShouldClose();
    }

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
    void RaylibWindow::setResizable(bool enabled)
    {
        if (enabled) SetWindowState(FLAG_WINDOW_RESIZABLE);
        else ClearWindowState(FLAG_WINDOW_RESIZABLE);
    }

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
    void RaylibWindow::requestAttention() {}

    float RaylibWindow::getOpacity() const { return m_opacity; }
    void RaylibWindow::setOpacity(float opacity)
    {
        m_opacity = opacity;
        SetWindowOpacity(opacity);
    }

    // ==========================================
    // Aesthetics
    // ==========================================

    std::string RaylibWindow::getTitle() const { return m_title; }
    void RaylibWindow::setTitle(const std::string& title)
    {
        m_title = title;
        SetWindowTitle(title.c_str());
    }

    void RaylibWindow::setIcon(const std::filesystem::path& filepath)
    {
        Image icon = LoadImage(filepath.string().c_str());
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    // ==========================================
    // Graphics & Renderer Integration
    // ==========================================

    bool RaylibWindow::isVSync() const { return m_vsync; }
    void RaylibWindow::setVSync(bool enabled)
    {
        m_vsync = enabled;
        if (enabled) SetWindowState(FLAG_VSYNC_HINT);
        else ClearWindowState(FLAG_VSYNC_HINT);
    }

    void* RaylibWindow::getNativeWindow() const { return nullptr; }
    void* RaylibWindow::getNativeDisplay() const { return nullptr; }
    void* RaylibWindow::getNativeContext() const { return nullptr; }
    void RaylibWindow::makeContextCurrent() {}

    // ==========================================
    // Cursor & OS Integration
    // ==========================================

    void RaylibWindow::setCursorMode(CursorMode mode)
    {
        m_cursorMode = mode;
        switch (mode) {
            case CursorMode::Normal:
                ShowCursor();
                EnableCursor();
                break;
            case CursorMode::Hidden:
                HideCursor();
                EnableCursor();
                break;
            case CursorMode::Locked:
                HideCursor();
                DisableCursor();
                break;
        }
    }

    CursorMode RaylibWindow::getCursorMode() const { return m_cursorMode; }

    void RaylibWindow::setClipboardText(const std::string& text)
    {
        SetClipboardText(text.c_str());
    }

    std::string RaylibWindow::getClipboardText() const
    {
        return ::GetClipboardText();
    }

} // namespace Harmony

namespace Harmony {
    HARMONY_REGISTER(Controller, RaylibWindow, "window_raylib", Engine&)
}