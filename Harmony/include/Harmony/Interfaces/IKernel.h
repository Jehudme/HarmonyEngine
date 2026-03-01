#pragma once
#include <Harmony/Utilities/Properties.h>

#include "Harmony/Utilities/Uncopyable.h"

namespace Harmony
{
    class IKernel : private Uncopyable
    {
    public:
        explicit IKernel() = default;
        virtual ~IKernel() = default;

        virtual void initialize(const Properties& properties);
    };
}
