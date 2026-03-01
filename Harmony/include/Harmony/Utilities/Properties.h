#pragma once
#include <string>
#include <vector>
#include <memory>
#include <typeindex>
#include <filesystem>
#include <functional>
#include <optional>

namespace Harmony {

    class Properties {
    public:
        using Keys = std::vector<std::string>;
        using Path = std::vector<std::string>;

        Properties();
        explicit Properties(const std::filesystem::path& filepath);

        ~Properties();

        Properties(const Properties& other);
        Properties& operator=(const Properties& other);

        template<typename Type>
        void set(const Path& key_path, const Type& value);

        template<typename Type>
        std::optional<Type> get(const Path& key_path) const;

        [[nodiscard]] std::optional<Keys> getKeys(const std::vector<std::string>& key_path = {}) const;
        [[nodiscard]] std::optional<Properties> getSubProperties(const std::vector<std::string>& key_path) const;

        void load(const std::filesystem::path& filepath);
        void save(const std::filesystem::path& filepath) const;

        void foreach(const Path& key_path, std::function<void(const std::string& key, const Properties& properties)> function) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pimpl;

        void _set_raw(const std::vector<std::string>& key_path, const void* ptr, std::type_index type);
        bool _get_raw(const std::vector<std::string>& key_path, void* ptr, std::type_index type) const;
    };
}
#include <Harmony/Utilities/Properties.inl>
