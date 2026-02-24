#include "Harmony/Properties.h"

#include <glaze/glaze.hpp>
#include <map>
#include <vector>
#include <stdexcept>
#include <fstream>

using StringMap = std::map<std::string, std::string>;

#define PROPERTIES_SUPPORTED_TYPES(VisitorMacro) \
    VisitorMacro(int) \
    VisitorMacro(float) \
    VisitorMacro(double) \
    VisitorMacro(bool) \
    VisitorMacro(std::string) \
    VisitorMacro(std::vector<int>) \
    VisitorMacro(std::vector<std::string>) \
    VisitorMacro(StringMap)

namespace Harmony {

    struct Properties::Impl {
        glz::json_t data = glz::json_t::object_t{};
    };

    Properties::Properties() : pimpl(std::make_unique<Impl>()) {}
    Properties::~Properties() = default;

    Properties::Properties(const Properties& other) : pimpl(std::make_unique<Impl>()) {
        pimpl->data = other.pimpl->data;
    }

    Properties& Properties::operator=(const Properties& other) {
        if (this != &other) {
            pimpl->data = other.pimpl->data;
        }
        return *this;
    }

    void Properties::_set_raw(const std::vector<std::string>& key_path, const void* ptr, std::type_index type) {
        if (key_path.empty()) return;

        glz::json_t* current_node = &pimpl->data;
        for (size_t index = 0; index < key_path.size() - 1; ++index) {
            if (!current_node->is_object()) {
                *current_node = glz::json_t::object_t{};
            }
            current_node = &current_node->get_object()[key_path[index]];
        }

        if (!current_node->is_object()) {
            *current_node = glz::json_t::object_t{};
        }

        glz::json_t& target_node = current_node->get_object()[key_path.back()];

        #define PROPERTIES_SET_CASE(Type) \
            if (type == typeid(Type)) { \
                std::string conversion_buffer; \
                glz::write_json(*static_cast<const Type*>(ptr), conversion_buffer); \
                glz::read_json(target_node, conversion_buffer); \
                return; \
            }

        PROPERTIES_SUPPORTED_TYPES(PROPERTIES_SET_CASE)
        #undef PROPERTIES_SET_CASE

        throw std::runtime_error("Harmony::Properties::set - Unsupported type");
    }

    bool Properties::_get_raw(const std::vector<std::string>& key_path, void* ptr, std::type_index type) const {
        if (key_path.empty()) return false;

        const glz::json_t* current_node = &pimpl->data;
        for (const auto& key : key_path) {
            if (!current_node->is_object()) return false;
            auto& object_map = current_node->get_object();
            auto iterator = object_map.find(key);
            if (iterator == object_map.end()) return false;
            current_node = &iterator->second;
        }

        #define PROPERTIES_GET_CASE(Type) \
            if (type == typeid(Type)) { \
                std::string conversion_buffer; \
                glz::write_json(*current_node, conversion_buffer); \
                Type temporary_result{}; \
                auto parse_error = glz::read_json(temporary_result, conversion_buffer); \
                if (!parse_error) { \
                    *static_cast<Type*>(ptr) = temporary_result; \
                    return true; \
                } \
                return false; \
            }

        PROPERTIES_SUPPORTED_TYPES(PROPERTIES_GET_CASE)
        #undef PROPERTIES_GET_CASE

        return false;
    }

    std::optional<Properties::Keys> Properties::getKeys(const std::vector<std::string>& key_path) const {
        const glz::json_t* current_node = &pimpl->data;
        for (const auto& key : key_path) {
            if (!current_node->is_object()) return std::nullopt;
            auto& object_map = current_node->get_object();
            auto iterator = object_map.find(key);
            if (iterator == object_map.end()) return std::nullopt;
            current_node = &iterator->second;
        }

        if (!current_node->is_object()) return std::nullopt;

        Keys extracted_keys;
        for (const auto& pair : current_node->get_object()) {
            extracted_keys.push_back(pair.first);
        }
        return extracted_keys;
    }

    std::optional<Properties> Properties::getSubProperties(const std::vector<std::string>& key_path) const {
        const glz::json_t* current_node = &pimpl->data;
        for (const auto& key : key_path) {
            if (!current_node->is_object()) return std::nullopt;
            auto& object_map = current_node->get_object();
            auto iterator = object_map.find(key);
            if (iterator == object_map.end()) return std::nullopt;
            current_node = &iterator->second;
        }

        if (!current_node->is_object()) return std::nullopt;

        Properties sub_properties;
        sub_properties.pimpl->data = *current_node;
        return sub_properties;
    }

    void Properties::load(const std::filesystem::path& filepath) {
        std::ifstream file_stream(filepath);
        if (!file_stream.is_open()) return;
        std::string buffer((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());
        glz::read_json(pimpl->data, buffer);
    }

    void Properties::save(const std::filesystem::path& filepath) const {
        std::string buffer;
        glz::write_json(pimpl->data, buffer);
        std::ofstream file_stream(filepath);
        file_stream << buffer;
    }

}