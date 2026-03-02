#pragma once
#include <atomic>
#include <mutex>
#include "Harmony/Core/Logger.h"
#include "Harmony/Utilities/Properties.h"

namespace Harmony {
    class IKernel;

    class Extension {
    public:
        enum class State { Initialized, Running, Paused, Shutdown };

        explicit Extension(const std::string& name, const std::string& type, IKernel& kernel);
        virtual ~Extension();

        Extension& operator=(const Extension&) = delete;
        Extension(const Extension&) = delete;

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

        std::unique_ptr<Logger> m_logger;
        std::atomic<State> m_state;
        IKernel& m_kernel;

        const std::string m_name;
        const std::string m_type;

    private:
        std::mutex m_lifecycleMutex;
    };
}
