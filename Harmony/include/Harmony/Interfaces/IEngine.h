#pragma once
#include "IKernel.h"
#include "Harmony/Core/Extension.h"

namespace Harmony
{
    class IEngine : public Extension
    {
    public:
        explicit IEngine(const std::string& name, IKernel& kernel) : Extension(name, "engine", kernel) {}
        virtual ~IEngine() = default;

        IEngine& operator=(const IEngine&) = delete;
        IEngine(const IEngine&) = delete;

        IEngine& operator=(IEngine&&) noexcept = delete;
        IEngine(IEngine&&) noexcept = delete;

        virtual void start() = 0;
        virtual void run() = 0;

        virtual void stop() = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;

    };

    struct IEngineContext
    {
    public:
        explicit IEngineContext(IEngine* ptr) : ptr(ptr) {}
        ~IEngineContext() = default;
        IEngine* ptr;
    };
}