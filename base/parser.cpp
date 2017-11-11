#include "base/parser.h"

#include <string.h>

namespace base {

bool Parser::Cmp(const char* t1, const char* t2)
{
	return strcmp(t1, t2) == 0;
}

}