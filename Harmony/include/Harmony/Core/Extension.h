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

        virtual void initialize(const Properties& properties);
        virtual void finalize();

        virtual void update();
        virtual void render();
        virtual void event();

        [[nodiscard]] virtual const std::string& getName() const = 0;
        [[nodiscard]] virtual const std::string& getType() const = 0;

    protected:
        virtual void onInitialize(const Properties& properties);
        virtual void onFinalize();

        virtual void onUpdate();
        virtual void onRender();
        virtual void onEvent();

        Logger m_logger;
        std::atomic<State> m_state;
        IKernel& m_kernel;

    private:
        std::mutex m_lifecycleMutex;
    };

}

#define HARMONY_REGISTER_EXTENSION(CLASS) \
    HARMONY_REGISTER(Harmony::Extension, CLASS, CLASS::GET_NAME(), Harmony::IKernel&)

#define HARMONY_EXTENSION_INTERFACE(CLASS, TYPE)\
    explicit CLASS(const std::string& name, Harmony::IKernel& kernel) : Extension(name, GET_TYPE(), kernel) {}\
    static const std::string& GET_TYPE() { static std::string type =TYPE; return type; } \
    const std::string& getType() const override { return GET_TYPE(); } \

#define HARMONY_EXTENSION_IMPLEMTATION(INTERFACE, CLASS, NAME) \
    explicit CLASS(Harmony::IKernel& kernel) : INTERFACE(NAME, kernel) {}\
    static const std::string& GET_NAME() { static std::string name =NAME; return name; } \
    const std::string& getName() const override { return GET_NAME(); } \


