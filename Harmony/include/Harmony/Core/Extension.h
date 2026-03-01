#pragma once
#include "Harmony/Utilities/Guarded.h"
#include "Harmony/Core/Logger.h"
#include "Harmony/Utilities/Properties.h"

namespace Harmony {
    class Engine;

    // Extension is the base class for all engine modules. It encapsulates a named,
    // stateful lifecycle that the Kernel drives through initialize/update/render/event/finalize.
    // Extensions are designed to be thread-safe through internal mutex protection.
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

        [[nodiscard]] const std::string& getName() const;
        [[nodiscard]] const std::string& getType() const;

    protected:
        virtual void onInitialize(const Properties& properties) = 0;
        virtual void onFinalize() = 0;

        virtual void onUpdate() = 0;
        virtual void onRender() = 0;
        virtual void onEvent() = 0;

        // Logger instance for this extension; pushed onto thread-local context during callbacks.
        std::unique_ptr<Logger> m_logger;

        // Thread-safe state wrapper using reader-writer lock semantics.
        Guarded<State> m_state;

        // Reference to the owning Engine for cross-extension communication.
        Engine& m_engine;

        // Immutable identifiers for this extension.
        const std::string m_name;
        const std::string m_type;

    private:
        // Protects lifecycle transitions to ensure thread-safe state changes.
        std::mutex m_lifecycleMutex;
    };
}
