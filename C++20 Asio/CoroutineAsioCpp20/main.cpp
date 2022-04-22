#include "pch.h"
#include "AsioServer.h"

int main()
{
	try
	{
		if (AsioContext::Inst().PrepareAsioServer() == false)
		{
			// error check
		}

		if (AsioContext::Inst().StartIOService() == false)
		{
			// error check
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}