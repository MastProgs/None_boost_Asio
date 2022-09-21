#pragma once

namespace cpp_redis
{
	class client;
}

class RedisManager : public Singleton<RedisManager>
{
public:
	RedisManager();
	virtual ~RedisManager();

	virtual bool Init() final;
	bool Ping();

private:
	bool Init(std::string_view ip, int port, int connectSize);

private:
	std::vector<cpp_redis::client*> m_redisClinetList;
};