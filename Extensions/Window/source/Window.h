#pragma once
#include "Harmony/Interfaces/IWindow.h"

namespace Harmony
{
    class RaylibWindow final : public IWindow
    {
    public:
        HARMONY_EXTENSION_IMPLEMTATION(IWindow, RaylibWindow, "default-raylib-window")

        // Extension overrides
        void onInitialize(const Properties& properties) override;
        void onFinalize() override;
        void onRender() override;
        void onUpdate() override;
        void onEvent() override;

        // Lifecycle & Core Loop
        void processEvents() override;
        void swapBuffers() override;
        void close() override;
        bool shouldClose() const override;

        // Dimensions & Limits
        uint32_t getWidth() const override;
        uint32_t getHeight() const override;
        void setWidth(uint32_t width) override;
        void setHeight(uint32_t height) override;
        void setSize(uint32_t width, uint32_t height) override;
        void setMinSize(uint32_t width, uint32_t height) override;
        void setMaxSize(uint32_t width, uint32_t height) override;
        float getAspectRatio() const override;

        // Position
        int32_t getPositionX() const override;
        int32_t getPositionY() const override;
        void setPosition(int32_t x, int32_t y) override;
        void centerWindow() override;

        // Window States & Modes
        void setWindowMode(WindowMode mode) override;
        WindowMode getWindowMode() const override;
        bool isResizable() const override;
        void setResizable(bool enabled) override;
        bool isMinimized() const override;
        void minimize() override;
        bool isMaximized() const override;
        void maximize() override;
        void restore() override;
        bool isHidden() const override;
        void show() override;
        void hide() override;
        bool isFocused() const override;
        void focus() override;
        void requestAttention() override;
        float getOpacity() const override;
        void setOpacity(float opacity) override;

        // Aesthetics
        std::string getTitle() const override;
        void setTitle(const std::string& title) override;
        void setIcon(const std::filesystem::path& filepath) override;

        // Graphics & Renderer Integration
        bool isVSync() const override;
        void setVSync(bool enabled) override;
        void* getNativeWindow() const override;
        void* getNativeDisplay() const override;
        void* getNativeContext() const override;
        void makeContextCurrent() override;

        // Cursor & OS Integration
        void setCursorMode(CursorMode mode) override;
        CursorMode getCursorMode() const override;
        void setClipboardText(const std::string& text) override;
        std::string getClipboardText() const override;
    };
}