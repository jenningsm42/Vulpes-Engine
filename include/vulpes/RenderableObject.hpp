#ifndef _VUL_RENDERABLEOBJECT_HPP
#define _VUL_RENDERABLEOBJECT_HPP

#include "Export.hpp"
#include "Handle.hpp"
#include "Mesh.hpp"
#include "SceneObject.hpp"
#include "Texture.hpp"

namespace vul {
    enum struct RenderableObjectFlags {
        Visible = 1,
        MeshAttached = 2,
        ColorMapAttached = 4,
        NormalMapAttached = 8,
        RoughnessMapAttached = 16,
        MetalMapAttached = 32
    };

    class VEAPI RenderableObject : public SceneObject {
    public:
        RenderableObject(uint32_t id);
        RenderableObject(const RenderableObject&);

        void attachMesh(const Handle<Mesh>&);
        void attachColorMap(const Handle<Texture>&);
        void attachNormalMap(const Handle<Texture>&);
        void attachRoughnessMap(const Handle<Texture>&);
        void attachMetalMap(const Handle<Texture>&);
        void setVisible(bool visible);

        Handle<Mesh> getMesh();
        Handle<Texture> getColorMap();
        Handle<Texture> getNormalMap();
        Handle<Texture> getRoughnessMap();
        Handle<Texture> getMetalMap();
        bool isVisible();

    private:
        Mesh m_mesh;
        Texture m_colorMap; // Analogous with albedo map
        Texture m_normalMap;
        Texture m_roughnessMap;
        Texture m_metalMap;
        uint8_t m_flags; // Ensure space isn't wasted with many bools
    };
}

#endif // _VUL_RENDERABLEOBJECT_HPP
