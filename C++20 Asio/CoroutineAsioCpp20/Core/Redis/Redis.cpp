
#include "pch.h"
#include "Redis.h"

#include "Singleton.h"

#include <cpp_redis/cpp_redis>

RedisManager::RedisManager()
{
}

RedisManager::~RedisManager()
{
	for (auto p : m_redisClinetList)
	{
		delete p;
	}
}

bool RedisManager::Init()
{
	return __super::Init() && Init("172.20.41.44", 6379, 16, ERedisConnectType::Sentinel);
}

bool RedisManager::Init(std::string_view ip, int port, int connectSize, ERedisConnectType rct)
{
	if (0 >= connectSize) { return false; }

	m_redisClinetList.reserve(connectSize);
	for (size_t i = 0; i < connectSize; i++)
	{
		m_redisClinetList.emplace_back(new cpp_redis::client{});
	}

	for (size_t i = 0; i < connectSize; i++)
	{
		auto& redis = *m_redisClinetList[i];
		redis.connect(ip.data(), port, [i](const std::string& host, std::size_t port, cpp_redis::client::connect_state status) {

			if (status == cpp_redis::client::connect_state::ok)
			{
				Logger::Inst().Info(Format("Redis Connect ok. Index={}, IP={}, Port={}", i, host, port));
			}
			else if (status == cpp_redis::client::connect_state::start)
			{
				//Logger::Inst().Info(Format("Redis Connect start. Index={}, IP={}, Port={}", i, host, port));
			}
			else if (status == cpp_redis::client::connect_state::dropped) 
			{
				Logger::Inst().Log(ERROR_LOG::REDIS_CONNECTION_FAILED, Format("Redis Index={}, IP={}, Port={}", i, host, port));
			}
		});
	}

	int successCnt = 0;
	do
	{
		auto iter = m_redisClinetList
			| std::views::filter([](cpp_redis::client* redis) {
				return redis->is_connected();
				});

		std::vector<cpp_redis::client*> m_successConn{ iter.begin(), iter.end() };
		successCnt = m_successConn.size();
	} while (m_redisClinetList.size() != successCnt);

	if (false == Ping())
	{
		Logger::Inst().Log(ERROR_LOG::REDIS_PING_TEST_FAILED);
		return false;
	}

	if (ERedisConnectType::Sentinel == rct)
	{
		// max index size check ( 0 ~ 15 )
		if (connectSize < 0 || 15 < connectSize)
		{
			Logger::Inst().Log(ERROR_LOG::REDIS_INVALID_REQUEST, Format("Redis Connection Type was {} - but index size was: {}", (int)rct, connectSize));
			return false;
		}

		for (size_t i = 0; i < connectSize; i++)
		{
			auto& redis = *m_redisClinetList[i];
			auto res = redis.select(i).get();
			if (false == res.ok())
			{
				Logger::Inst().Log(ERROR_LOG::REDIS_INVALID_REQUEST, Format("Redis Connection Type was {}, index : {}, redis cmd was SELECT", (int)rct, i));
				return false;
			}
		}
	}

	return true;
}

bool RedisManager::Ping()
{
	auto iter = m_redisClinetList
		| std::views::filter([](cpp_redis::client* redis) {
		auto res = redis->ping().get();
		return res.is_string() && (res.as_string() == "pong");
			});

	std::vector<bool> wasPong{ iter.begin(), iter.end() };
	return m_redisClinetList.size() == wasPong.size();
}
