#pragma once

#include <string>
#include <cstdint>
#include <tuple>

#include <Harmony/Utilities/Properties.h>
#include <Harmony/Utilities/Uncopyable.h>
#include "Harmony/Core/Controller.h"

namespace Harmony
{
    class Engine;

    // Defines how the mouse cursor interacts with the window
    enum class CursorMode {
        Normal,   // Visible and behaves normally
        Hidden,   // Invisible but moves freely
        Locked    // Invisible and locked to the center (Ideal for 3D FPS cameras)
    };

    // Defines the display mode of the window
    enum class WindowMode {
        Windowed,
        Fullscreen,
        BorderlessWindowed
    };

    class IWindow : public Controller
    {
    public:
        explicit IWindow(Engine& engine) : Controller(engine) {}
        ~IWindow() override = default;

        // ==========================================
        // Lifecycle & Core Loop
        // ==========================================
        virtual bool initialize(const Properties& properties) = 0;
        virtual void terminate() = 0;

        virtual void processEvents() = 0;
        virtual void swapBuffers() = 0;

        virtual void close() = 0; // Forcefully asks the window to close
        virtual bool shouldClose() const = 0;

        // ==========================================
        // Dimensions & Limits
        // ==========================================
        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        virtual void setWidth(uint32_t width) = 0;
        virtual void setHeight(uint32_t height) = 0;
        virtual void setSize(uint32_t width, uint32_t height) = 0;

        virtual void setMinSize(uint32_t width, uint32_t height) = 0;
        virtual void setMaxSize(uint32_t width, uint32_t height) = 0;
        virtual float getAspectRatio() const = 0;

        // ==========================================
        // Position
        // ==========================================
        virtual int32_t getPositionX() const = 0;
        virtual int32_t getPositionY() const = 0;
        virtual void setPosition(int32_t x, int32_t y) = 0;
        virtual void centerWindow() = 0;

        // ==========================================
        // Window States & Modes
        // ==========================================
        virtual void setWindowMode(WindowMode mode) = 0;
        virtual WindowMode getWindowMode() const = 0;

        virtual bool isResizable() const = 0;
        virtual void setResizable(bool enabled) = 0;

        virtual bool isMinimized() const = 0;
        virtual void minimize() = 0;

        virtual bool isMaximized() const = 0;
        virtual void maximize() = 0;

        virtual void restore() = 0; // Restores from minimized/maximized state

        virtual bool isHidden() const = 0;
        virtual void show() = 0;
        virtual void hide() = 0;

        virtual bool isFocused() const = 0;
        virtual void focus() = 0;
        virtual void requestAttention() = 0; // Flashes taskbar icon

        virtual float getOpacity() const = 0;
        virtual void setOpacity(float opacity) = 0;

        // ==========================================
        // Aesthetics
        // ==========================================
        virtual std::string getTitle() const = 0;
        virtual void setTitle(const std::string& title) = 0;
        virtual void setIcon(const std::filesystem::path& filepath) = 0;

        // ==========================================
        // Graphics & Renderer Integration
        // ==========================================
        virtual bool isVSync() const = 0;
        virtual void setVSync(bool enabled) = 0;

        // Core OS handles required for cross-platform graphics API initialization
        virtual void* getNativeWindow() const = 0;  // e.g., HWND, NSWindow, xcb_window_t
        virtual void* getNativeDisplay() const = 0; // e.g., HDC, xcb_connection_t
        virtual void* getNativeContext() const = 0; // e.g., HGLRC, GLXContext
        virtual void makeContextCurrent() = 0;      // Required for OpenGL multi-threading

        // ==========================================
        // Cursor & OS Integration
        // ==========================================
        virtual void setCursorMode(CursorMode mode) = 0;
        virtual CursorMode getCursorMode() const = 0;

        // Clipboard is intrinsically tied to the windowing system on most OS's
        virtual void setClipboardText(const std::string& text) = 0;
        virtual std::string getClipboardText() const = 0;
    };
}