#pragma once
#include <string>
#include <vector>
#include <memory>
#include <typeindex>
#include <filesystem>
#include <functional>
#include <optional>

namespace Harmony {

    // Properties provides a type-safe, JSON-backed configuration system.
    // Values are stored in a schema-less JSON tree and accessed via typed get/set methods.
    // Type conversion is performed through JSON serialization/deserialization round-trips.
    class Properties {
    public:
        using Keys = std::vector<std::string>;
        using Path = std::vector<std::string>;

        Properties();
        explicit Properties(const std::filesystem::path& filepath);

        ~Properties();

        Properties(const Properties& other);
        Properties& operator=(const Properties& other);

        // Sets a value at the specified path, creating intermediate nodes as needed.
        template<typename Type>
        void set(const Path& keyPath, const Type& value);

        // Retrieves a value at the specified path, returning std::nullopt if not found or type mismatch.
        template<typename Type>
        [[nodiscard]] std::optional<Type> get(const Path& keyPath) const;

        // Returns all keys at the specified path, or std::nullopt if path is invalid or not an object.
        [[nodiscard]] std::optional<Keys> getKeys(const std::vector<std::string>& keyPath = {}) const;

        // Returns a new Properties object rooted at the specified path.
        [[nodiscard]] std::optional<Properties> getSubProperties(const std::vector<std::string>& keyPath) const;

        // Loads properties from a JSON file.
        void load(const std::filesystem::path& filepath);

        // Saves properties to a JSON file.
        void save(const std::filesystem::path& filepath) const;

        // Iterates over each child of the object at keyPath, invoking the callback with key and sub-properties.
        void foreach(const Path& keyPath, std::function<void(const std::string& key, const Properties& properties)> callback) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pimpl;

        // Type-erased set: converts value to JSON via round-trip serialization.
        void setRawValue(const std::vector<std::string>& keyPath, const void* valuePtr, std::type_index type);

        // Type-erased get: converts JSON node to typed value via round-trip deserialization.
        bool getRawValue(const std::vector<std::string>& keyPath, void* resultPtr, std::type_index type) const;
    };
}
#include <Harmony/Utilities/Properties.inl>
