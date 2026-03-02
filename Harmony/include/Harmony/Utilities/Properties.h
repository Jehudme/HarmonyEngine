#pragma once
#include <string>
#include <string_view>
#include <span>
#include <vector>
#include <memory>
#include <typeindex>
#include <filesystem>
#include <functional>
#include <optional>
#include <initializer_list>

namespace Harmony {

    // Properties provides a type-safe, JSON-backed configuration system.
    // Values are stored in a schema-less JSON tree and accessed via typed get/set methods.
    // Type conversion is performed through JSON serialization/deserialization round-trips.
    class Properties {
    public:
        using Keys = std::vector<std::string>;

        // Path accepts both std::initializer_list<std::string_view> (for convenient inline
        // usage like {"key1", "key2"}) and std::span<const std::string_view> (for backward
        // compatibility with existing code passing arrays/spans).
        // NOTE: Path is a non-owning view. When using initializer_list construction,
        // the backing storage only lives until the end of the full expression.
        // Use Path only as a function argument, not as a stored member.
        struct Path {
            std::span<const std::string_view> m_span;

            Path() = default;
            Path(std::initializer_list<std::string_view> il) : m_span(il.begin(), il.size()) {}
            Path(std::span<const std::string_view> sp) : m_span(sp) {}

            auto begin() const { return m_span.begin(); }
            auto end() const { return m_span.end(); }
            std::size_t size() const { return m_span.size(); }
            bool empty() const { return m_span.empty(); }
            const std::string_view& operator[](std::size_t i) const { return m_span[i]; }
            const std::string_view& back() const { return m_span.back(); }

            operator std::span<const std::string_view>() const { return m_span; }
        };

        Properties();
        explicit Properties(const std::filesystem::path& filepath);

        ~Properties();

        Properties(const Properties& other);
        Properties& operator=(const Properties& other);

        // Sets a value at the specified path, creating intermediate nodes as needed.
        template<typename Type>
        void set(Path keyPath, const Type& value);

        // Retrieves a value at the specified path, returning std::nullopt if not found or type mismatch.
        template<typename Type>
        [[nodiscard]] std::optional<Type> get(Path keyPath) const;

        // Returns all keys at the specified path, or std::nullopt if path is invalid or not an object.
        [[nodiscard]] std::optional<Keys> getKeys(Path keyPath = {}) const;

        // Returns a new Properties object rooted at the specified path.
        [[nodiscard]] std::optional<Properties> getSubProperties(Path keyPath) const;

        // Loads properties from a JSON file.
        void load(const std::filesystem::path& filepath);

        // Saves properties to a JSON file.
        void save(const std::filesystem::path& filepath) const;

        // Iterates over each child of the object at keyPath, invoking the callback with key and sub-properties.
        void foreach(Path keyPath, std::function<void(const std::string& key, const Properties& properties)> callback) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pimpl;

        // Type-erased set: converts value to JSON via round-trip serialization.
        void setRawValue(Path keyPath, const void* valuePtr, std::type_index type);

        // Type-erased get: converts JSON node to typed value via round-trip deserialization.
        bool getRawValue(Path keyPath, void* resultPtr, std::type_index type) const;
    };
}
#include <Harmony/Utilities/Properties.inl>
