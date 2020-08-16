#ifndef SHAPE2D_FACTORY_H
#define SHAPE2D_FACTORY_H

#include "Mesh.h"
#include <glm/fwd.hpp>
#include <memory>

#include "Entity.h"

#include_next <vector>
#include_next <glm/glm.hpp>

using namespace glm;

class Shape2DFactory
{
public:
    Shape2DFactory();

    static Mesh* Line(const vec3& point1, const vec3& point2);
    static Mesh* Lines(const std::vector<vec3>& points);
    static const std::vector<vec3>& RectPoints(const vec3& offset, const vec2& size);
    static Mesh* LineRect(const vec3& offset, const vec2& size);
    static Mesh* SolidRect(const vec3& offset, const vec2& size);
    static Mesh* LinePolygon(const std::vector<vec3>& points);
    static Mesh* SolidPolygon(const std::vector<vec3>& points);

private:
    static Mesh* RectMesh(const vec3& offset, const vec2& size);
    static std::vector<vec3> points_;
};

#endif
