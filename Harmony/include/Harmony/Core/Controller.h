#pragma once
#include "Harmony/Utilities/Guarded.h"
#include "Harmony/Core/Logger.h"
#include "Harmony/Utilities/Properties.h"

namespace Harmony {
    class Engine;

    class Controller {
    public:
        enum class State { Initialized, Running, Paused, Shutdown };

        explicit Controller(const std::string& name, const std::string type, Engine& engine);
        virtual ~Controller();

        Controller& operator=(const Controller&) = delete;
        Controller(const Controller&) = delete;

        Controller& operator=(Controller&&) noexcept = delete;
        Controller(Controller&&) noexcept = delete;

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
        Guarded<State> state;

        Engine& engine;
        const std::string name;
        const std::string type;

    private:
        std::mutex m_mutex;
    };
}
