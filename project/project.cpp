#include "project/project.h"

namespace ss {

const std::string& ToString(ValueType type) {
	static const std::string strings[] = { "bool", "int", "float", "Vec3" };
	return strings[int(type)];
}

}