#include "stdafx.h"
#include "Core/AsioServer.h"

int main()
{
	try
	{
		if (AsioContext::Instance().PrepareAsioServer() == false)
		{
			// error check
		}

		if (AsioContext::Instance().StartIOService() == false)
		{
			// error check
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}