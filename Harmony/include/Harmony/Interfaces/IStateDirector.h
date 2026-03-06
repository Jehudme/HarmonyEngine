#pragma once
#include "Harmony/Core/Extension.h"

namespace Harmony
{
    class IStateDirector : public Extension
    {
    public:
        HARMONY_EXTENSION_INTERFACE(IStateDirector, "StateStack");

        virtual void switchState(const std::string& state) = 0;
        virtual void pushState(const std::string& state) = 0;
        virtual void popState() = 0;

        virtual void switchScene(const std::string& scene) = 0;
        virtual void pushScene(const std::string& scene) = 0;
        virtual void popScene() = 0;

        virtual std::vector<std::string>  getCurrentSceneStack() = 0;
    };
}
