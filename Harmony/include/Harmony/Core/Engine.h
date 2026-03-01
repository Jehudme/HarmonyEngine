#pragma once
#include "Service.h"

namespace Harmony
{
    class IKernel;
    class Engine final : public Service
    {
    public:
        explicit Engine(std::unique_ptr<IKernel> kernel);
        ~Engine() override;

        std::unique_ptr<IKernel> kernel;

    protected:
        void onInitialize(const Properties& properties) override;
        void onFinalize() override;
        void onUpdate() override;
        void onRender() override;
        void onEvent() override;

        void onStart() override;
        void onShutdown() override;
        void onPause() override;
        void onResume() override;
    };
}
