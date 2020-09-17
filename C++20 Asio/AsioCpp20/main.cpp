#include "stdafx.h"
#include "Core/AsioServer.h"

int main()
{
	try
	{
		AsioServer asioServer;

		if (asioServer.PrepareAsioServer() == false)
		{
			// error check
		}

		if (asioServer.StartListen() == false)
		{
			// error check
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}