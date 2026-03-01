#include "RaylibWindow.h"
#include "Modules/Details/RaylibWindowConfig.inc"
#include <raylib.h>
#include <Harmony/Core/Registry.h>
#include <array>
#include <span>

#include "Harmony/Core/Engine.h"
#include "Harmony/Core/ContextLogger.h"

namespace Harmony
{
    RaylibWindow::RaylibWindow(Engine& engine)
        : IWindow("window_raylib", engine) {}

    // ==========================================
    // Extension overrides
    // ==========================================

    void RaylibWindow::onInitialize(const Properties& properties)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());

        // Helper lambda to extract a property with fallback logging.
        // Uses std::array for zero-allocation key path lookups.
        auto getPropertyOrDefault = [this, &properties]<typename T, size_t N>(
            const std::array<std::string_view, N>& keyPath,
            const T& defaultValue,
            const std::string& propertyName) -> T
        {
            if (auto optionalValue = properties.get<T>(std::span<const std::string_view>(keyPath))) {
                return *optionalValue;
            }
            m_logger->warn("RaylibWindow '{}': Property '{}' not found; using default value.", m_name, propertyName);
            return defaultValue;
        };

        int32_t windowWidth = getPropertyOrDefault(std::array<std::string_view, 1>{"width"}, static_cast<int32_t>(DEFAULT_WINDOW_WIDTH), "width");
        int32_t windowHeight = getPropertyOrDefault(std::array<std::string_view, 1>{"height"}, static_cast<int32_t>(DEFAULT_WINDOW_HEIGHT), "height");
        std::string windowTitle = getPropertyOrDefault(std::array<std::string_view, 1>{"title"}, std::string(DEFAULT_WINDOW_TITLE), "title");
        bool vsyncEnabled = getPropertyOrDefault(std::array<std::string_view, 1>{"vsync"}, DEFAULT_WINDOW_VSYNC, "vsync");

        m_logger->info("RaylibWindow '{}': Creating window with dimensions {}x{} and title '{}'.",
                       m_name, windowWidth, windowHeight, windowTitle);

        InitWindow(windowWidth, windowHeight, windowTitle.c_str());
        setVSync(vsyncEnabled);

        m_logger->info("RaylibWindow '{}': Window created successfully. VSync: {}.", m_name, vsyncEnabled ? "enabled" : "disabled");
    }

    void RaylibWindow::onFinalize()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->info("RaylibWindow '{}': Closing window...", m_name);
        CloseWindow();
        m_logger->info("RaylibWindow '{}': Window closed.", m_name);
    }

    void RaylibWindow::onUpdate(float deltaTime)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        // Core engine updates are managed by Kernel; no window-specific update logic needed.
        // deltaTime parameter available for future use by derived classes.
        (void)deltaTime;
    }

    void RaylibWindow::onRender()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        // Raylib requires drawing to be wrapped in Begin/EndDrawing calls for buffer swapping.
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Future render systems will draw here...

        EndDrawing();
    }

    void RaylibWindow::onEvent()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        // Check if the user pressed the close button or the ESC key.
        if (WindowShouldClose()) {
            m_logger->info("RaylibWindow '{}' of type '{}': Window close event detected; signaling engine shutdown.", m_name, m_type);
            m_engine.stop();
        }
    }

    // ==========================================
    // Lifecycle & Core Loop
    // ==========================================

    void RaylibWindow::processEvents()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        // Raylib handles event polling internally during WindowShouldClose().
    }

    void RaylibWindow::swapBuffers()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        // Raylib swaps buffers internally during EndDrawing().
    }

    void RaylibWindow::close()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': close() called.", m_name);
        // Note: This queries close state rather than forcing close.
        WindowShouldClose();
    }

    bool RaylibWindow::shouldClose() const
    {
        return WindowShouldClose();
    }

    // ==========================================
    // Dimensions & Limits
    // ==========================================

    uint32_t RaylibWindow::getWidth() const
    {
        return static_cast<uint32_t>(GetScreenWidth());
    }

    uint32_t RaylibWindow::getHeight() const
    {
        return static_cast<uint32_t>(GetScreenHeight());
    }

    void RaylibWindow::setWidth(uint32_t width)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting width to {}.", m_name, width);
        SetWindowSize(static_cast<int>(width), GetScreenHeight());
    }

    void RaylibWindow::setHeight(uint32_t height)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting height to {}.", m_name, height);
        SetWindowSize(GetScreenWidth(), static_cast<int>(height));
    }

    void RaylibWindow::setSize(uint32_t width, uint32_t height)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting size to {}x{}.", m_name, width, height);
        SetWindowSize(static_cast<int>(width), static_cast<int>(height));
    }

    void RaylibWindow::setMinSize(uint32_t width, uint32_t height)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting minimum size to {}x{}.", m_name, width, height);
        SetWindowMinSize(static_cast<int>(width), static_cast<int>(height));
    }

    void RaylibWindow::setMaxSize(uint32_t width, uint32_t height)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting maximum size to {}x{}.", m_name, width, height);
        SetWindowMaxSize(static_cast<int>(width), static_cast<int>(height));
    }

    float RaylibWindow::getAspectRatio() const
    {
        return static_cast<float>(GetScreenWidth()) / static_cast<float>(GetScreenHeight());
    }

    // ==========================================
    // Position
    // ==========================================

    int32_t RaylibWindow::getPositionX() const
    {
        return static_cast<int32_t>(GetWindowPosition().x);
    }

    int32_t RaylibWindow::getPositionY() const
    {
        return static_cast<int32_t>(GetWindowPosition().y);
    }

    void RaylibWindow::setPosition(int32_t x, int32_t y)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting position to ({}, {}).", m_name, x, y);
        SetWindowPosition(x, y);
    }

    void RaylibWindow::centerWindow()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        const int currentMonitor = GetCurrentMonitor();
        const int monitorWidth = GetMonitorWidth(currentMonitor);
        const int monitorHeight = GetMonitorHeight(currentMonitor);
        const int centeredX = (monitorWidth - GetScreenWidth()) / 2;
        const int centeredY = (monitorHeight - GetScreenHeight()) / 2;
        m_logger->trace("RaylibWindow '{}': Centering window to ({}, {}) on monitor {}.", m_name, centeredX, centeredY, currentMonitor);
        SetWindowPosition(centeredX, centeredY);
    }

    // ==========================================
    // Window States & Modes
    // ==========================================

    void RaylibWindow::setWindowMode(WindowMode mode)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting window mode.", m_name);

        if (mode == WindowMode::Fullscreen) {
            if (!IsWindowFullscreen()) {
                ToggleFullscreen();
            }
        } else if (mode == WindowMode::BorderlessWindowed) {
            if (!IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) {
                ToggleBorderlessWindowed();
            }
        } else {
            // Windowed mode: disable fullscreen and borderless if active.
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
            }
            if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) {
                ToggleBorderlessWindowed();
            }
        }
    }

    WindowMode RaylibWindow::getWindowMode() const
    {
        if (IsWindowFullscreen()) {
            return WindowMode::Fullscreen;
        }
        if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) {
            return WindowMode::BorderlessWindowed;
        }
        return WindowMode::Windowed;
    }

    bool RaylibWindow::isResizable() const
    {
        return IsWindowState(FLAG_WINDOW_RESIZABLE);
    }

    void RaylibWindow::setResizable(bool enabled)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting resizable to {}.", m_name, enabled);
        if (enabled) {
            SetWindowState(FLAG_WINDOW_RESIZABLE);
        } else {
            ClearWindowState(FLAG_WINDOW_RESIZABLE);
        }
    }

    bool RaylibWindow::isMinimized() const
    {
        return IsWindowMinimized();
    }

    void RaylibWindow::minimize()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Minimizing window.", m_name);
        MinimizeWindow();
    }

    bool RaylibWindow::isMaximized() const
    {
        return IsWindowMaximized();
    }

    void RaylibWindow::maximize()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Maximizing window.", m_name);
        MaximizeWindow();
    }

    void RaylibWindow::restore()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Restoring window.", m_name);
        RestoreWindow();
    }

    bool RaylibWindow::isHidden() const
    {
        return IsWindowHidden();
    }

    void RaylibWindow::show()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Showing window.", m_name);
        ClearWindowState(FLAG_WINDOW_HIDDEN);
    }

    void RaylibWindow::hide()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Hiding window.", m_name);
        SetWindowState(FLAG_WINDOW_HIDDEN);
    }

    bool RaylibWindow::isFocused() const
    {
        return IsWindowFocused();
    }

    void RaylibWindow::focus()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Requesting focus.", m_name);
        SetWindowFocused();
    }

    void RaylibWindow::requestAttention()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        // Raylib does not natively support taskbar attention requests.
        m_logger->trace("RaylibWindow '{}': requestAttention() called but not supported by Raylib.", m_name);
    }

    float RaylibWindow::getOpacity() const
    {
        // Raylib lacks a GetWindowOpacity getter; returning 1.0 as fallback.
        return 1.0f;
    }

    void RaylibWindow::setOpacity(float opacity)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting opacity to {}.", m_name, opacity);
        SetWindowOpacity(opacity);
    }

    // ==========================================
    // Aesthetics
    // ==========================================

    std::string RaylibWindow::getTitle() const
    {
        // Raylib lacks a GetWindowTitle getter; returning empty string.
        return "";
    }

    void RaylibWindow::setTitle(const std::string& title)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting title to '{}'.", m_name, title);
        SetWindowTitle(title.c_str());
    }

    void RaylibWindow::setIcon(const std::filesystem::path& filepath)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting icon from '{}'.", m_name, filepath.string());
        Image iconImage = LoadImage(filepath.string().c_str());
        SetWindowIcon(iconImage);
        UnloadImage(iconImage);
    }

    // ==========================================
    // Graphics & Renderer Integration
    // ==========================================

    bool RaylibWindow::isVSync() const
    {
        return IsWindowState(FLAG_VSYNC_HINT);
    }

    void RaylibWindow::setVSync(bool enabled)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting VSync to {}.", m_name, enabled);
        if (enabled) {
            SetWindowState(FLAG_VSYNC_HINT);
        } else {
            ClearWindowState(FLAG_VSYNC_HINT);
        }
    }

    void* RaylibWindow::getNativeWindow() const
    {
        return GetWindowHandle();
    }

    void* RaylibWindow::getNativeDisplay() const
    {
        // Raylib does not expose display handle; returning nullptr.
        return nullptr;
    }

    void* RaylibWindow::getNativeContext() const
    {
        // Raylib does not expose graphics context; returning nullptr.
        return nullptr;
    }

    void RaylibWindow::makeContextCurrent()
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        // Raylib manages context internally; no action required.
    }

    // ==========================================
    // Cursor & OS Integration
    // ==========================================

    void RaylibWindow::setCursorMode(CursorMode mode)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting cursor mode.", m_name);

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

    CursorMode RaylibWindow::getCursorMode() const
    {
        if (IsCursorHidden()) {
            return CursorMode::Hidden;
        }
        return CursorMode::Normal;
    }

    void RaylibWindow::setClipboardText(const std::string& text)
    {
        HARMONY_CONTEXT_LOGGER_GUARD(m_logger.get());
        m_logger->trace("RaylibWindow '{}': Setting clipboard text.", m_name);
        SetClipboardText(text.c_str());
    }

    std::string RaylibWindow::getClipboardText() const
    {
        return ::GetClipboardText();
    }

    // Auto-Register the Window with the Registry
    HARMONY_REGISTER(Extension, RaylibWindow, "window_raylib", Engine&)

} // namespace Harmony