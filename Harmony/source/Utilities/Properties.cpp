#include "Harmony/Utilities/Properties.h"
#include "Harmony/Details/PropertiesRegistry.inc"

#include <glaze/glaze.hpp>
#include <map>
#include <vector>
#include <stdexcept>
#include <fstream>

#include "../../include/Harmony/Core/Logger.h"

namespace Harmony {

    struct Properties::Impl {
        glz::json_t data = glz::json_t::object_t{};
    };

    Properties::Properties() : pimpl(std::make_unique<Impl>()) {}

    Properties::Properties(const std::filesystem::path& filepath) : Properties() {
        load(filepath);
    }
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

    // Traverses the JSON tree along key_path, returning a pointer to the terminal node.
    // Read-only variant: returns nullptr if any step is missing or not an object.
    static const glz::json_t* traverseToNode(const glz::json_t& root, const std::vector<std::string>& key_path)
    {
        const glz::json_t* current_node = &root;
        for (const auto& key : key_path) {
            if (!current_node->is_object()) return nullptr;
            const auto& object_map = current_node->get_object();
            auto iterator = object_map.find(key);
            if (iterator == object_map.end()) return nullptr;
            current_node = &iterator->second;
        }
        return current_node;
    }

    void Properties::_set_raw(const std::vector<std::string>& key_path, const void* ptr, std::type_index type) {
        if (key_path.empty()) return;

        // Walk all but the last key, creating intermediate object nodes as needed.
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

        // Round-trip through JSON text so that any supported C++ type can be
        // stored in the schema-less glaze tree without a type-specific specialization.
        glz::json_t& target_node = current_node->get_object()[key_path.back()];

        #define PROPERTIES_SET_CASE(Type) \
            if (type == typeid(Type)) { \
                std::string conversion_buffer; \
                (void)glz::write_json(*static_cast<const Type*>(ptr), conversion_buffer); \
                (void)glz::read_json(target_node, conversion_buffer); \
                return; \
            }

        PROPERTIES_SUPPORTED_TYPES(PROPERTIES_SET_CASE)
        #undef PROPERTIES_SET_CASE

        throw std::runtime_error("Harmony::Properties::set - Unsupported type");
    }

    bool Properties::_get_raw(const std::vector<std::string>& key_path, void* ptr, std::type_index type) const {
        if (key_path.empty()) return false;

        // Locate the terminal node; return false immediately on any missing step.
        const glz::json_t* current_node = traverseToNode(pimpl->data, key_path);
        if (!current_node) return false;

        // Round-trip the node through JSON text for type-safe deserialization.
        #define PROPERTIES_GET_CASE(Type) \
            if (type == typeid(Type)) { \
                std::string conversion_buffer; \
                (void)glz::write_json(*current_node, conversion_buffer); \
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
        const glz::json_t* current_node = traverseToNode(pimpl->data, key_path);
        if (!current_node || !current_node->is_object()) return std::nullopt;

        Keys extracted_keys;
        for (const auto& pair : current_node->get_object()) {
            extracted_keys.push_back(pair.first);
        }
        return extracted_keys;
    }

    std::optional<Properties> Properties::getSubProperties(const std::vector<std::string>& key_path) const {
        const glz::json_t* current_node = traverseToNode(pimpl->data, key_path);
        if (!current_node || !current_node->is_object()) return std::nullopt;

        Properties sub_properties;
        sub_properties.pimpl->data = *current_node;
        return sub_properties;
    }

    void Properties::load(const std::filesystem::path& filepath) {
        std::ifstream file_stream(filepath);
        if (!file_stream.is_open()) return;
        std::string buffer((std::istreambuf_iterator<char>(file_stream)), std::istreambuf_iterator<char>());
        (void)glz::read_json(pimpl->data, buffer);
    }

    void Properties::save(const std::filesystem::path& filepath) const {
        std::string buffer;
        (void)glz::write_json(pimpl->data, buffer);
        std::ofstream file_stream(filepath);
        file_stream << buffer;
    }

    void Properties::foreach(const Path& key_path, std::function<void(const std::string& key, const Properties& properties)> function) const {
        // Traverse to the target object node, throwing on any invalid step so
        // callers receive a clear diagnostic instead of a silent no-op.
        const glz::json_t* current_node = &pimpl->data;

        for (const auto& step : key_path) {
            if (!current_node->is_object()) {
                throw std::runtime_error("Harmony::Properties::foreach - Invalid Path");
            }

            auto& object_map = current_node->get_object();
            auto it = object_map.find(step);

            if (it == object_map.end()) {
                throw std::runtime_error("Harmony::Properties::foreach - Key not found");
            }

            current_node = &it->second;
        }

        if (!current_node->is_object()) {
            throw std::runtime_error("Harmony::Properties::foreach - Target is not an object");
        }

        for (const auto& [key, value] : current_node->get_object()) {
            Properties sub_properties;
            sub_properties.pimpl->data = value;
            function(key, sub_properties);
        }
    }
}
