#pragma once
#include <Harmony/Utilities/Properties.h>

#include "Harmony/Core/Controller.h"
#include "Harmony/Utilities/Uncopyable.h"

namespace Harmony
{
    class IKernel : private Uncopyable
    {
    public:
        explicit IKernel() = default;
        virtual ~IKernel() = default;

        virtual void initialize(Engine& engine, const Properties& properties) = 0;
        virtual void finalize() = 0;

        virtual void update() = 0;
        virtual void render() = 0;
        virtual void event() = 0;

        virtual Controller& controller(const std::string& type) = 0;

        template<typename ControllerType>
        ControllerType& controller()
        {
            static_assert(std::is_base_of_v<Controller, ControllerType>,
                  "T must derive from Controller to be used in Controller<T>()");

            return controller(ControllerType::getType());
        }
    };

};
