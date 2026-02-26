#pragma once
#include "Harmony/Utilities/Guarded.h"
#include "Harmony/Utilities/Logger.h"
#include "Harmony/Utilities/Properties.h"

namespace Harmony {
    class Engine;

    class Controller {
    public:
        enum class State { Initialized, Running, Paused, Shutdown };

        explicit Controller(Engine& engine);
        virtual ~Controller() = default;

        virtual void Initialize(const Properties& properties);
        virtual void Finalize();

        virtual void update();
        virtual void render();
        virtual void event();

    protected:
        virtual void onInitialize(const Properties& properties) = 0;
        virtual void onFinalize() = 0;

        virtual void onUpdate() = 0;
        virtual void onRender() = 0;
        virtual void onEvent() = 0;

        std::unique_ptr<Logger> logger;
        Guarded<State> state;
        std::mutex mutex;
        Engine& engine;
    };
}
