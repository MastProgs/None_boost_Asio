#pragma once

// used open source cpp_redis
// https://github.com/cpp-redis/cpp_redis
// install - https://github.com/cpp-redis/cpp_redis/wiki/Installation
// 

namespace cpp_redis
{
	class client;
}

class RedisManager : public Singleton<RedisManager>
{
private:
	enum class ERedisConnectType : char
	{
		Sentinel
		, Cluster
	};

public:
	RedisManager();
	virtual ~RedisManager();

	virtual bool Init() final;
	bool Ping();

private:
	bool Init(std::string_view ip, int port, int connectSize, ERedisConnectType rct);

private:
	std::vector<cpp_redis::client*> m_redisClinetList;
};