#pragma once
#include "Service.h"

namespace Harmony
{
    class IKernel;

    // Engine is the root Service that owns the Kernel and drives the main game loop.
    // It acts as a Service so that it can be started/stopped/paused independently.
    class Engine final : public Service
    {
    public:
        explicit Engine(std::unique_ptr<IKernel> kernel);
        ~Engine() override;

        // Returns a reference to the kernel for module access.
        IKernel& getKernel() { return *m_kernel; }
        const IKernel& getKernel() const { return *m_kernel; }

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

    private:
        // The kernel orchestrates all registered extensions; owned exclusively by Engine.
        std::unique_ptr<IKernel> m_kernel;
    };
}
