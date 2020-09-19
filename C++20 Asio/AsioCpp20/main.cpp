#include "stdafx.h"
#include "Core/AsioServer.h"

int main()
{
	try
	{
		if (ThisServerIO::Instance().PrepareAsioServer() == false)
		{
			// error check
		}

		if (ThisServerIO::Instance().StartListen() == false)
		{
			// error check
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}