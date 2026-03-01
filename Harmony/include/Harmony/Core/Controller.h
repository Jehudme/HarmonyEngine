#pragma once
#include "Harmony/Utilities/Guarded.h"
#include "Harmony/Utilities/Logger.h"
#include "Harmony/Utilities/Properties.h"
#include "Harmony/Utilities/Uncopyable.h"

namespace Harmony {
    class Engine;

    class Controller : private Uncopyable {
    public:
        enum class State { Initialized, Running, Paused, Shutdown };

        explicit Controller(const std::string& name, const std::string type, Engine& engine);
        virtual ~Controller();

        virtual void initialize(const Properties& properties);
        virtual void finalize();

        virtual void update();
        virtual void render();
        virtual void event();

        const std::string& getName() const;
        const std::string& getType() const;

    protected:
        virtual void onInitialize(const Properties& properties) = 0;
        virtual void onFinalize() = 0;

        virtual void onUpdate() = 0;
        virtual void onRender() = 0;
        virtual void onEvent() = 0;

        std::unique_ptr<Logger> logger;
        Guarded<State> state = Guarded<State>(State::Shutdown);

        Engine& engine;
        const std::string name;
        const std::string type;

    private:
        std::mutex m_mutex;
    };
}
