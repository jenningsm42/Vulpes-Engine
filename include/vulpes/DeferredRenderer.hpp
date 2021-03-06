#ifndef _VUL_DEFERREDRENDERER_HPP
#define _VUL_DEFERREDRENDERER_HPP

#include "Export.hpp"
#include "GBuffer.hpp"
#include "Handle.hpp"
#include "Mesh.hpp"
#include "RenderTarget.hpp"
#include "Renderer.hpp"
#include "ResourceLoader.hpp"
#include "Shader.hpp"

namespace vul {
    enum struct DeferredGeometryUniformLocations {
        ViewMatrix = 0,
        ProjectionMatrix = 4,
        ModelMatrix = 8,
        NormalMatrix = 12,
        Near = 15,
        BoneStateArray = 16,
        ColorMap = 781,
        NormalMap = 782,
        RoughnessMap = 783,
        MetalMap = 784
    };

    enum struct DeferredLightUniformLocations {
        ColorBuffer = 0,
        NormalBuffer = 1,
        DepthBuffer = 2,
        MiscBuffer = 3,
        Near = 4,
        Far = 5,
        InverseViewMatrix = 6,
        EnvironmentMap = 9,
        DiffuseEnvironmentMap = 10,
        EnvironmentMipMaps = 11,
        EnvironmentLUT = 12,
        LightArray = 13
    };

    class VEAPI DeferredRenderer : public Renderer {
    public:
        DeferredRenderer(ResourceLoader&);
        ~DeferredRenderer();

        void setCamera(Camera&) override;
        void setActiveEnvironment(Handle<Texture>&);
        void setActiveDiffuseEnvironment(Handle<Texture>&);

        void render() override;
        void render(RenderTarget* renderTarget);

        void setWireframeMode(bool) override;
        void setClearColor(float r, float g, float b, float a = 1.f);

    private:
        GBuffer m_gbuffer;
        Handle<Shader> m_geometryShader;
        Handle<Shader> m_lightShader;
        Handle<Texture> m_defaultColorMap;
        Handle<Texture> m_defaultNormalMap;
        Handle<Texture> m_defaultRoughnessMap;
        Handle<Texture> m_defaultMetalMap;
        Handle<Texture> m_environmentMap;
        Handle<Texture> m_diffuseEnvironmentMap;
        Handle<Texture> m_environmentLUT;
        Mesh m_quadMesh;
        float m_color[4];
        bool m_wireframe;

        void geometryPass();
        void lightPass(RenderTarget* renderTarget);

        void createQuad();
    };
}

#endif // _VUL_DEFERREDRENDERER_HPP
