
#include "base/base.h"

#include <iostream>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	try
	{
	}
	catch (std::exception& e) {
		std::cerr << e.what() << "\n";
	}

    return 0;
}

