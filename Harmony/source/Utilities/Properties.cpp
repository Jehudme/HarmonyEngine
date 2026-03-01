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

    // ============================================================================
    // Helper: JSON Tree Traversal
    // ============================================================================

    // Traverses the JSON tree along keyPath, returning a pointer to the terminal node.
    // Returns nullptr if any step is missing or the node is not an object.
    static const glz::json_t* traverseToNode(const glz::json_t& rootNode, const std::vector<std::string>& keyPath)
    {
        const glz::json_t* currentNode = &rootNode;
        for (const auto& pathSegment : keyPath) {
            if (!currentNode->is_object()) {
                return nullptr;
            }
            const auto& objectMap = currentNode->get_object();
            auto nodeIterator = objectMap.find(pathSegment);
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

    void Properties::setRawValue(const std::vector<std::string>& keyPath, const void* valuePtr, std::type_index type) {
        if (keyPath.empty()) {
            return;
        }

        // Walk all but the last key, creating intermediate object nodes as needed.
        glz::json_t* currentNode = &pimpl->data;
        for (size_t pathIndex = 0; pathIndex < keyPath.size() - 1; ++pathIndex) {
            if (!currentNode->is_object()) {
                *currentNode = glz::json_t::object_t{};
            }
            currentNode = &currentNode->get_object()[keyPath[pathIndex]];
        }

        if (!currentNode->is_object()) {
            *currentNode = glz::json_t::object_t{};
        }

        // Round-trip through JSON text so that any supported C++ type can be
        // stored in the schema-less glaze tree without type-specific specialization.
        glz::json_t& targetNode = currentNode->get_object()[keyPath.back()];

        #define PROPERTIES_SET_CASE(Type) \
            if (type == typeid(Type)) { \
                std::string conversionBuffer; \
                (void)glz::write_json(*static_cast<const Type*>(valuePtr), conversionBuffer); \
                (void)glz::read_json(targetNode, conversionBuffer); \
                return; \
            }

        PROPERTIES_SUPPORTED_TYPES(PROPERTIES_SET_CASE)
        #undef PROPERTIES_SET_CASE

        throw std::runtime_error("Properties::set - Unsupported type for serialization.");
    }

    bool Properties::getRawValue(const std::vector<std::string>& keyPath, void* resultPtr, std::type_index type) const {
        if (keyPath.empty()) {
            return false;
        }

        // Locate the terminal node; return false immediately on any missing step.
        const glz::json_t* targetNode = traverseToNode(pimpl->data, keyPath);
        if (!targetNode) {
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
                return false; \
            }

        PROPERTIES_SUPPORTED_TYPES(PROPERTIES_GET_CASE)
        #undef PROPERTIES_GET_CASE

        return false;
    }

    std::optional<Properties::Keys> Properties::getKeys(const std::vector<std::string>& keyPath) const {
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

    std::optional<Properties> Properties::getSubProperties(const std::vector<std::string>& keyPath) const {
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
            return;
        }
        std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
        (void)glz::read_json(pimpl->data, fileContent);
    }

    void Properties::save(const std::filesystem::path& filepath) const {
        std::string jsonOutput;
        (void)glz::write_json(pimpl->data, jsonOutput);
        std::ofstream fileStream(filepath);
        fileStream << jsonOutput;
    }

    void Properties::foreach(const Path& keyPath, std::function<void(const std::string& key, const Properties& properties)> callback) const {
        // Traverse to the target object node, throwing on any invalid step so
        // callers receive a clear diagnostic instead of a silent no-op.
        const glz::json_t* currentNode = &pimpl->data;

        for (const auto& pathSegment : keyPath) {
            if (!currentNode->is_object()) {
                throw std::runtime_error("Properties::foreach - Path segment '" + pathSegment + "' is not an object.");
            }

            auto& objectMap = currentNode->get_object();
            auto nodeIterator = objectMap.find(pathSegment);

            if (nodeIterator == objectMap.end()) {
                throw std::runtime_error("Properties::foreach - Key '" + pathSegment + "' not found in path.");
            }

            currentNode = &nodeIterator->second;
        }

        if (!currentNode->is_object()) {
            throw std::runtime_error("Properties::foreach - Target node at specified path is not an object.");
        }

        for (const auto& [childKey, childValue] : currentNode->get_object()) {
            Properties childProperties;
            childProperties.pimpl->data = childValue;
            callback(childKey, childProperties);
        }
    }
}
