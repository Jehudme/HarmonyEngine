#pragma once
#include <Harmony/Interfaces/IEngine.h>

namespace Harmony
{
    class IKernel;

    class Engine : public IEngine
    {
    public:
        explicit Engine(IKernel& kernel);
        ~Engine() override = default;
        Engine& operator=(const IEngine&) = delete;
        Engine(const IEngine&) = delete;

        Engine& operator=(Engine&&) noexcept = delete;
        Engine(Engine&&) noexcept = delete;

        void start() override;
        void run() override;

        void stop() override;
        void pause() override;
        void resume() override;

    private:
        void onInitialize(const Properties& properties) override;
        void onFinalize() override;
        void onRender() override;
        void onUpdate() override;
        void onEvent() override;

    private:
        std::unique_ptr<IKernel> m_kernel;
    };

}
