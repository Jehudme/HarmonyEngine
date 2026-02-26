#include <iostream>
#include <string>
#include <Harmony/Systems/Registry.h>

/**
 * Test Interface
 */
class IBase {
public:
    virtual ~IBase() = default;
    virtual void speak() = 0;
};

/**
 * Concrete Implementation
 */
class Derived : public IBase {
public:
    Derived(const std::string& message) : m_message(message) {}

    void speak() override {
        std::cout << "Derived says: " << m_message << std::endl;
    }

private:
    std::string m_message;
};

int main() {
    try {
        // 1. Save a factory that takes a std::string argument
        // Matches your Registry::save implementation
        Harmony::Registry::save<IBase, Derived, std::string>("tester");

        // 2. Attempt a duplicate save to test your error handling
        // This should trigger the logger.error and throw an exception
        // Harmony::Registry::save<IBase, Derived, std::string>("tester");

        // 3. Create the object using the variadic factory
        // Use create() with the identifier and matching constructor arguments
        std::unique_ptr<IBase> instance = Harmony::Registry::create<IBase>("tester", std::string("Hello Harmony!"));

        if (instance) {
            instance->speak();
        }

        // 4. Test freeing the factory
        // Removes the mapping from the static m_registry
        Harmony::Registry::free<IBase, std::string>("tester");

        // 5. Verify it is gone
        auto failedInstance = Harmony::Registry::create<IBase>("tester", std::string("Should fail"));
        if (!failedInstance) {
            std::cout << "Registry successfully freed the factory." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught expected exception: " << e.what() << std::endl;
    }

    return 0;
}