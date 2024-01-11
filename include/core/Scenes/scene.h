#include <core/object.h>
#include <vector>
#include <glm/vec3.hpp>

class Scene {
	public:
		Scene();
		void renderScene();
		void initScene();
	private:
		std::vector<Object> _objects;
		glm::vec3 _camera;
};