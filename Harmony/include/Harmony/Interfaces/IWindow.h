#pragma once

#include <string>
#include <cstdint>
#include <Harmony/Utilities/Properties.h>
#include <Harmony/Utilities/Uncopyable.h>

namespace Harmony
{
    class IWindow : private Uncopyable
    {
    public:
        IWindow() = default;
        virtual ~IWindow() = default;

        virtual bool initialize(const Properties& properties) = 0;
        virtual void terminate() = 0;

        virtual void processEvents() = 0;
        virtual void swapBuffers() = 0;

        virtual bool shouldClose() const = 0;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        virtual std::string getTitle() const = 0;
        virtual void* getNativeWindow() const = 0;

        virtual void setVSync(bool enabled) = 0;
        virtual void setTitle(const std::string& title) = 0;
    };
}