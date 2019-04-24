#ifndef _VUL_SCENEOBJECT_HPP
#define _VUL_SCENEOBJECT_HPP

#include "Export.hpp"
#include "Transformation.hpp"

namespace vul {
    enum struct SceneObjectType {
        Renderable,
        PointLight,
        Other
    };

    class VEAPI SceneObject {
    public:
        SceneObject(uint32_t id, SceneObjectType type);
        SceneObject(const SceneObject&);

        uint32_t getID() const;
        SceneObjectType getType() const;
        Transformation& getTransformation();

    protected:
        uint32_t m_id;
        SceneObjectType m_type;
        Transformation m_transformation;
    };
}

#endif // _VUL_SCENEOBJECT_HPP
