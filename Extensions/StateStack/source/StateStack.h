#pragma once
#include <stack>

#include "Harmony/Interfaces/IStateStack.h"

namespace Harmony
{
    class StateStack : public Harmony::IStateStack
    {
    public:
        HARMONY_EXTENSION_IMPLEMTATION(IStateStack, StateStack, "state_stack")

        void switchState(const std::string& state);
        void pushState(const std::string& state);
        void popState();

        void switchScene(const std::string& scene);
        void pushScene(const std::string& scene);
        void popScene();

        std::vector<std::string> retrieveSceneStack();

    private:
        std::stack<std::stack<std::string>> m_sceneStack;
    };
} // Harmony
