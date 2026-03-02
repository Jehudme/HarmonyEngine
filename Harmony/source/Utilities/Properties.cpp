#include "Harmony/Utilities/Properties.h"
#include "Harmony/Details/PropertiesRegistry.inc"

#include <glaze/glaze.hpp>
#include <map>
#include <vector>
#include <fstream>

#include "Harmony/Core/Logger.h"

namespace Harmony {

    struct Properties::Impl {
        glz::json_t data = glz::json_t::object_t{};
        std::string buffer; // Add this to keep the parsed string alive!
    };

    Properties::Properties() : pimpl(std::make_unique<Impl>()) {}

    Properties::Properties(const std::filesystem::path& filepath) : Properties() {
        load(filepath);
    }

    Properties::~Properties() = default;

    Properties::Properties(const Properties& other) : pimpl(std::make_unique<Impl>()) {
        pimpl->data = other.pimpl->data;
        pimpl->buffer = other.pimpl->buffer;
    }

    Properties& Properties::operator=(const Properties& other) {
        if (this != &other) {
            pimpl->data = other.pimpl->data;
            pimpl->buffer = other.pimpl->buffer;
        }
        return *this;
    }

    // ============================================================================
    // Helper: JSON Tree Traversal
    // ============================================================================

    static const glz::json_t* traverseToNode(const glz::json_t& rootNode, std::span<const std::string_view> keyPath)
    {
        const glz::json_t* currentNode = &rootNode;
        for (const auto& pathSegment : keyPath) {
            if (!currentNode->is_object()) {
                return nullptr;
            }
            const auto& objectMap = currentNode->get_object();
            // Convert string_view to string for map lookup
            auto nodeIterator = objectMap.find(std::string(pathSegment));
            if (nodeIterator == objectMap.end()) {
                return nullptr;
            }
            currentNode = &nodeIterator->second;
        }
        return currentNode;
    }

    // ============================================================================
    // Type-Erased Accessors
    // ============================================================================

    void Properties::setRawValue(Path keyPath, const void* valuePtr, std::type_index type) {
        if (keyPath.empty()) {
            Logger::context().error("Properties::setRawValue - Key path is empty. Operation ignored.");
            return;
        }

        // Walk all but the last key, creating intermediate object nodes as needed.
        glz::json_t* currentNode = &pimpl->data;
        for (size_t pathIndex = 0; pathIndex < keyPath.size() - 1; ++pathIndex) {
            if (!currentNode->is_object()) {
                *currentNode = glz::json_t::object_t{};
            }
            currentNode = &currentNode->get_object()[std::string(keyPath[pathIndex])];
        }

        if (!currentNode->is_object()) {
            *currentNode = glz::json_t::object_t{};
        }

        // Round-trip through JSON text so that any supported C++ type can be
        // stored in the schema-less glaze tree without type-specific specialization.
        glz::json_t& targetNode = currentNode->get_object()[std::string(keyPath.back())];

        #define PROPERTIES_SET_CASE(Type) \
            if (type == typeid(Type)) { \
                std::string conversionBuffer; \
                (void)glz::write_json(*static_cast<const Type*>(valuePtr), conversionBuffer); \
                (void)glz::read_json(targetNode, conversionBuffer); \
                return; \
            }

        PROPERTIES_SUPPORTED_TYPES(PROPERTIES_SET_CASE)
        #undef PROPERTIES_SET_CASE

        Logger::context().error("Properties::setRawValue - Unsupported type '{}' for key '{}'; value not set.", type.name(), keyPath.back());
    }

    bool Properties::getRawValue(Path keyPath, void* resultPtr, std::type_index type) const {
        if (keyPath.empty()) {
            Logger::context().error("Properties::getRawValue - Key path is empty.");
            return false;
        }

        // Locate the terminal node; return false immediately on any missing step.
        const glz::json_t* targetNode = traverseToNode(pimpl->data, keyPath);
        if (!targetNode) {
            Logger::context().warn("Properties::getRawValue - Key path not found in properties.");
            return false;
        }

        // Round-trip the node through JSON text for type-safe deserialization.
        #define PROPERTIES_GET_CASE(Type) \
            if (type == typeid(Type)) { \
                std::string conversionBuffer; \
                (void)glz::write_json(*targetNode, conversionBuffer); \
                Type temporaryResult{}; \
                auto parseError = glz::read_json(temporaryResult, conversionBuffer); \
                if (!parseError) { \
                    *static_cast<Type*>(resultPtr) = temporaryResult; \
                    return true; \
                } \
                Logger::context().error("Properties::getRawValue - JSON parse error for type '{}'.", type.name()); \
                return false; \
            }

        PROPERTIES_SUPPORTED_TYPES(PROPERTIES_GET_CASE)
        #undef PROPERTIES_GET_CASE

        Logger::context().error("Properties::getRawValue - Unsupported type requested: '{}'.", type.name());
        return false;
    }

    std::optional<Properties::Keys> Properties::getKeys(Path keyPath) const {
        const glz::json_t* targetNode = traverseToNode(pimpl->data, keyPath);
        if (!targetNode || !targetNode->is_object()) {
            return std::nullopt;
        }

        Keys extractedKeys;
        for (const auto& [key, value] : targetNode->get_object()) {
            extractedKeys.push_back(key);
        }
        return extractedKeys;
    }

    std::optional<Properties> Properties::getSubProperties(Path keyPath) const {
        const glz::json_t* targetNode = traverseToNode(pimpl->data, keyPath);
        if (!targetNode || !targetNode->is_object()) {
            return std::nullopt;
        }

        Properties subProperties;
        subProperties.pimpl->data = *targetNode;
        return subProperties;
    }

    void Properties::load(const std::filesystem::path& filepath) {
        std::ifstream fileStream(filepath);
        if (!fileStream.is_open()) {
            Logger::context().error("Properties::load - Failed to open file: '{}'", filepath.string());
            return;
        }

        // Save the file contents into the pimpl buffer so it lives forever
        pimpl->buffer.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

        // Parse using the persistent buffer
        auto error = glz::read_json(pimpl->data, pimpl->buffer);
        if (error) {
            Logger::context().error("Properties::load - Glaze failed to parse JSON in file: '{}'", filepath.string());
        }
    }

    void Properties::save(const std::filesystem::path& filepath) const {
        std::string jsonOutput;
        (void)glz::write_json(pimpl->data, jsonOutput);
        std::ofstream fileStream(filepath);
        if (!fileStream.is_open()) {
            Logger::context().error("Properties::save - Failed to open or create file: '{}'", filepath.string());
            return;
        }
        fileStream << jsonOutput;
    }

    void Properties::foreach(Path keyPath, std::function<void(const std::string& key, const Properties& properties)> callback) const {
        const glz::json_t* currentNode = &pimpl->data;

        for (const auto& pathSegment : keyPath) {
            if (!currentNode->is_object()) {
                Logger::context().error("Properties::foreach - Key path segment '{}' is not an object.", pathSegment);
                return;
            }

            auto& objectMap = currentNode->get_object();
            auto nodeIterator = objectMap.find(std::string(pathSegment));

            if (nodeIterator == objectMap.end()) {
                Logger::context().error("Properties::foreach - Key '{}' not found in path.", std::string(pathSegment));
                return;
            }

            currentNode = &nodeIterator->second;
        }

        if (!currentNode->is_object()) {
            Logger::context().error("Properties::foreach - Target node at specified path is not an object.");
            return;
        }

        for (const auto& [childKey, childValue] : currentNode->get_object()) {
            Properties childProperties;
            childProperties.pimpl->data = childValue;
            callback(childKey, childProperties);
        }
    }
}