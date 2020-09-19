#include "stdafx.h"
#include "Core/AsioServer.h"

int main()
{
	try
	{
		if (AsioServer::Instance().PrepareAsioServer() == false)
		{
			// error check
		}

		if (AsioServer::Instance().StartListen() == false)
		{
			// error check
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}