#ifndef EQEMU_ORIENTED_BOUNDNG_BOX_H
#define EQEMU_ORIENTED_BOUNDNG_BOX_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class OrientedBoundingBox
{
public:
	OrientedBoundingBox() = default;
	OrientedBoundingBox(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, glm::vec3 extents);
	~OrientedBoundingBox() = default;

	bool ContainsPoint(glm::vec3 p) const;
private:
	float min_x, max_x;
	float min_y, max_y;
	float min_z, max_z;
	glm::mat4 transformation;
	glm::mat4 inverted_transformation;
};

#endif
