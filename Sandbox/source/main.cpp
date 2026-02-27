#include <iostream>
#include <string>
#include "../../Harmony/include/Harmony/Core/Registry.h"

// ==========================================
// 1. Dummy Classes for Testing
// ==========================================
class Entity
{
public:
    virtual ~Entity() = default;
    virtual void debugPrint() const = 0;
};

class Player : public Entity
{
public:
    Player() { std::cout << "   [Allocated] Player created.\n"; }
    ~Player() override { std::cout << "   [Destroyed] Player destroyed.\n"; }

    void debugPrint() const override {
        std::cout << "   -> I am the Player.\n";
    }
};

class Enemy : public Entity
{
    int m_health;
    float m_speed;
public:
    Enemy(int health, float speed) : m_health(health), m_speed(speed) {
        std::cout << "   [Allocated] Enemy created (HP: " << m_health << ", Speed: " << m_speed << ").\n";
    }
    ~Enemy() override { std::cout << "   [Destroyed] Enemy destroyed.\n"; }

    void debugPrint() const override {
        std::cout << "   -> I am an Enemy.\n";
    }
};

class NPC : public Entity
{
public:
    NPC() { std::cout << "   [Allocated] NPC created.\n"; }
    ~NPC() override { std::cout << "   [Destroyed] NPC destroyed.\n"; }
    void debugPrint() const override { std::cout << "   -> I am an NPC.\n"; }
};

// ==========================================
// 2. Automatic Registration (Runs BEFORE main)
// ==========================================
// Note: We don't register NPC here to test manual registration later.

HARMONY_REGISTER(Entity, Player, "Hero")
HARMONY_REGISTER(Entity, Enemy, "Orc", int, float)


// ==========================================
// 3. The Main Test Loop
// ==========================================
int main()
{
    std::cout << "\n--- Harmony Engine Registry Test ---\n\n";

    // --- Test A: Successful Creation ---
    std::cout << "[Test A] Creating registered objects...\n";
    {
        auto player = Harmony::Registry::create<Entity>("Hero");
        if (player) player->debugPrint();

        auto orc = Harmony::Registry::create<Entity>("Orc", 100, 2.5f);
        if (orc) orc->debugPrint();
    } // Objects go out of scope here and should be destroyed.
    std::cout << "\n";

    // --- Test B: Error Handling (Wrong Arguments) ---
    std::cout << "[Test B] Trying to create 'Orc' with wrong arguments...\n";
    // Orc expects (int, float). Passing (int, int) or just (int) should fail gracefully.
    auto badOrc = Harmony::Registry::create<Entity>("Orc", 100, 50);
    if (!badOrc) {
        std::cout << "   -> Success: Registry rejected the invalid arguments.\n";
    }
    std::cout << "\n";

    // --- Test C: Error Handling (Unregistered Name) ---
    std::cout << "[Test C] Trying to create an unregistered object...\n";
    auto ghost = Harmony::Registry::create<Entity>("Ghost");
    if (!ghost) {
        std::cout << "   -> Success: Registry returned nullptr for unknown name.\n";
    }
    std::cout << "\n";

    // --- Test D: Manual Registration & Unregistration ---
    std::cout << "[Test D] Manual Registration of 'TownGuard'...\n";
    HARMONY_REGISTER_MANUAL(Entity, NPC, "TownGuard");

    auto guard = Harmony::Registry::create<Entity>("TownGuard");
    if (guard) guard->debugPrint();

    std::cout << "Freeing 'TownGuard'...\n";
    HARMONY_UNREGISTER_MANUAL("TownGuard");

    auto guardAfterFree = Harmony::Registry::create<Entity>("TownGuard");
    if (!guardAfterFree) {
        std::cout << "   -> Success: TownGuard could not be created after being freed.\n";
    }
    std::cout << "\n--- End of Test ---\n";

    return 0;
}