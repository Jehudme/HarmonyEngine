#pragma once
#include "Harmony/Utilities/Guarded.h"
#include "Harmony/Core/Logger.h"
#include "Harmony/Utilities/Properties.h"

namespace Harmony {
    class Engine;

    // Base class for all engine modules. An Extension encapsulates a named,
    // stateful lifecycle that the Kernel drives through initialize/update/render/event/finalize.
    class Extension {
    public:
        enum class State { Initialized, Running, Paused, Shutdown };

        explicit Extension(const std::string& name, const std::string& type, Engine& engine);
        virtual ~Extension();

        Extension& operator=(const Extension&) = delete;
        Extension(const Extension&) = delete;

        Extension& operator=(Extension&&) noexcept = delete;
        Extension(Extension&&) noexcept = delete;

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

        std::unique_ptr<Logger> m_logger;
        Guarded<State> m_state;

        Engine& m_engine;
        const std::string m_name;
        const std::string m_type;

    private:
        std::mutex m_mutex;
    };
}
