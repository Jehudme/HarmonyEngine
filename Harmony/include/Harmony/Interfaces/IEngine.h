#pragma once

#include <Harmony/Interfaces/IWindow.h>
#include <Harmony/Core/Service.h>
#include <flecs.h>

namespace Harmony
{
    class IInputManager;
    class IAudioManager;
    class SceneManager;
    class ResourceManager;
    class IRenderer;

    class IEngine : public Service
    {
        IEngine() : Service(*this) {};
        IEngine& getInterface() { return *this; }

    public:
        virtual ~IEngine() override = default;

        virtual IWindow& getWindow() = 0;
        virtual IRenderer& getRenderer() = 0;
        virtual SceneManager& getSceneManager() = 0;
        virtual ResourceManager& getResourceManager() = 0;

        virtual IInputManager& getInputManager() = 0;
        virtual IAudioManager& getAudioManager() = 0;
    };
}