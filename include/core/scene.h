#include <core/object.h>
#include <vector>
#include <glm/vec3.hpp>

class Scene {
	public:
		Scene();
	private:
		std::vector<Object> _objects;
		vec3 _camera;
};