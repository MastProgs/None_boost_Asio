#pragma once

// used open source cpp_redis
// https://github.com/cpp-redis/cpp_redis
// install - https://github.com/cpp-redis/cpp_redis/wiki/Installation
// 

namespace cpp_redis
{
	class client;
	class reply;
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

public:
	cpp_redis::client& GetRedis(int i = 0);

private:
	bool Init(std::string_view ip, int port, std::string_view password, int connectSize, ERedisConnectType rct);

private:
	std::vector<cpp_redis::client*> m_redisClinetList;
};

class RedisCommand
{
public:
	explicit RedisCommand(cpp_redis::client& redis);
	virtual ~RedisCommand();

	template<typename ... ARGS>
	void SetKey(const ARGS&... args);

	template<typename ... ARGS>
	void SetParams(const ARGS&... args);

	void SetWithScores(bool b = true);
	void SetReverse(bool b = true);

	cpp_redis::reply& Run();

protected:
	std::vector<std::string> tokenize(std::string_view str, const char delim);

	template <typename Elem, typename ... Strings>
	std::string MakeKey(const Elem& s, Strings... forms);
	template <typename Elem>
	std::string MakeKey(const Elem& s);

	template <typename Elem, typename ... Strings>
	std::string MakeParams(const Elem& s, Strings... forms);
	template <typename Elem>
	std::string MakeParams(const Elem& s);

private:
	void SetCommand();
	void SetRawCommand(std::string_view cmds);

protected:
	std::string m_operate;
	std::string m_key;
	std::string m_subParams;

	bool m_flag_withScores = false;
	bool m_flag_reverse = false;
	bool m_flag_byScore = false;

	std::vector<std::string> m_cmd;
	cpp_redis::client& m_redis;
	cpp_redis::reply* m_reply;
	int m_timeout = 10;
};

#define DEFINE_REDIS_CMD(x) class x : public RedisCommand \
{ \
public: \
	explicit x(cpp_redis::client & redis) \
		: RedisCommand{ redis } \
	{ \
		m_operate = tokenize(typeid(this).name(), ' ').at(1); \
		if (std::string::npos != m_operate.find(':')) { m_operate = tokenize(m_operate, ':').at(1); } \
	}; \
}


namespace Redis
{
	DEFINE_REDIS_CMD(ZAdd);
	DEFINE_REDIS_CMD(ZRange);
	DEFINE_REDIS_CMD(ZRevRange);
	DEFINE_REDIS_CMD(ZRangeByScore);
	DEFINE_REDIS_CMD(ZRevRangeByScore);
}
