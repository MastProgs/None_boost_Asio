#include "pch.h"
#include "AsioServer.h"
#include "Redis.h"
#include "ExceptionHandler.h"
#include "StackWalker.h"

int main()
{
	try
	{
		ExceptionHandler::Inst().initialize(L"Cpp20Server");
		ExceptionHandler::Inst().run();

		if (AsioContext::Inst().PrepareAsioServer() == false)
		{
			// error check
		}
		int* p = nullptr;
		*p = 123;

		if (RedisManager::Inst().Init() == false)
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
		int i;
		std::cin >> i;
	}
}