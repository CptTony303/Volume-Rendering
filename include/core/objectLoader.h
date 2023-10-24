#include <fstream>
#include <core/object.h>

class ObjectLoader {
public:
	int load(std::string filePath, int type, Object* file);
};