#include "project/project.h"

namespace ss {

const std::string& ToString(ValueType type) {
	static const std::string strings[] = { "bool", "int", "float" };
	return strings[int(type)];
}

}