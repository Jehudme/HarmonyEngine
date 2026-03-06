#pragma once
#include "IKernel.h"

namespace Harmony
{
    class IScene
    {
    public:
        virtual void onCreate(const Properties& properties);
        virtual void onDelete();

        void createEntity(const std::string& name);
        void deleteEntity(const std::string& name);
        void deleteEntity(EntityID id);

    };
}
