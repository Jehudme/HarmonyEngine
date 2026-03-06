#pragma once
#include <stack>

#include "Harmony/Interfaces/IStateDirector.h"

namespace Harmony
{
    class StateDirector : public Harmony::IStateDirector
    {
    public:
        HARMONY_EXTENSION_IMPLEMTATION(IStateDirector, StateDirector, "default-state-stack")

        void switchState(const std::string& state) override;
        void pushState(const std::string& state) override;
        void popState() override;

        void switchScene(const std::string& scene) override;
        void pushScene(const std::string& scene) override;
        void popScene() override;

    private:
        std::stack<std::stack<std::string>> m_stacks;
    };
} // Harmony
