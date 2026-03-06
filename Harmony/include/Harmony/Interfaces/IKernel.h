#pragma once
#include "IScene.h"
#include "Harmony/Utilities/Properties.h"
#include "Harmony/Core/Extension.h"

namespace Harmony
{
    using uuid = uint32_t;

    struct IComponent;
    struct IFlag;

    class IKernel : public Extension
    {
    public:
        HARMONY_EXTENSION_INTERFACE(IKernel, "kernal");

        virtual void initialize(const Properties& properties) = 0;
        virtual void finalize() = 0;

        ///////////////////////////////////////////////////////////////////////
        /// Scene Management
        ///////////////////////////////////////////////////////////////////////

        virtual void createScene(const std::string& name) = 0;
        virtual void deleteScene(const std::string& name) = 0;

        virtual void setSceneActive(const std::string& name, bool active) = 0;
        virtual void getSceneActive(const std::string& name) const = 0;

        ///////////////////////////////////////////////////////////////////////
        /// Entity management
        //////////////////////////////////////////////////////////////////////

        virtual uuid createEntity(const std::string& name, const std::string& scene) = 0;
        virtual void deleteEntity(const std::string& name, const std::string& scene) = 0;
        virtual void deleteEntity(uuid id) = 0;

        virtual void attachEntity(uuid children, uuid parent) = 0;
        virtual void detachEntity(uuid children) = 0;

        virtual void enableEntityUpdate(uuid id, bool enabled) = 0;
        virtual void enableEntityRender(uuid id, bool enabled) = 0;

        virtual IComponent* setEntityComponent(uuid id, std::unique_ptr<IComponent> component) = 0;
        virtual IComponent* getEntityComponent(uuid id, const std::string& componentName) = 0;

        template<typename ComponentType, typename... Args>
        IComponent* setEntityComponent(uuid id, Args&&... args);
        template<typename ComponentType>
        IComponent* getEntityComponent(uuid id);

        virtual bool setEntityFlag(uuid id, const std::string& flag, bool value) = 0;
        virtual bool getEntityFlag(uuid id, const std::string& flag) const = 0;

        //////////////////////////////////////////////////////////////////////
        /// Extension management
        //////////////////////////////////////////////////////////////////////

        virtual void createExtension(const std::string& name) = 0;
        virtual void deleteExtension(const std::string& name) = 0;
        virtual void updateExtensions() = 0;

        virtual Extension* extension(const std::string& type) = 0;

        template<typename ExtensionType>
        ExtensionType* extension();

        ///////////////////////////////////////////////////////////////////////
        /// Storage Management
        ///////////////////////////////////////////////////////////////////////

        virtual const Properties& properties(Properties::Path path) const = 0;
    };
}

#include "Harmony/Interfaces/IKernel.inl"

#define HARMONY_KERNAL_IMPLEMENTATION(CLASS, NAME)                                \
    explicit CLASS(Harmony::IKernel& kernel) : IKernel(NAME, *this) {}                      \
    static const std::string& GET_NAME() { static std::string name =NAME; return name; }    \
    const std::string& getName() const override { return GET_NAME(); }                      \