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

BETTER_ENUM(RedisContentsType, char
	, MAX
);

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
	int GetIndex(const RedisContentsType& t);

public:
	cpp_redis::client& GetRedis(const RedisContentsType& t);

private:
	bool Init(std::string_view ip, int port, std::string_view password, int connectSize, ERedisConnectType rct);

private:
	std::vector<cpp_redis::client*> m_redisClinetList;
};

class RedisResult
{
public:
	explicit RedisResult(bool isError = false, const std::string& errMsg = "") : isError{ isError }, m_errMsg{ errMsg } {};
	explicit RedisResult(const std::vector<std::string>& res, bool isError = false) : m_rawRes{ res }, isError{ isError }, isNull{ false } {};
	explicit RedisResult(const long long& res, bool isError = false) : m_intSingleRes{ res }, isError{ isError }, isNull{ false } {};
	explicit RedisResult(const std::string& res, bool isError = false) : isError{ isError }, isNull{ false } { m_rawRes.emplace_back(res); };

	~RedisResult() {};

	bool IsError() const { return isError; }
	bool IsNull() const { return isNull || IsError(); }

	template<typename ResultType>
	ResultType GetResult() const;

	std::string GetString() const { return m_rawRes.at(0); }
	long long GetNumber() const
	{
		if (m_intSingleRes != MIN_SCORE) return m_intSingleRes;
		if (m_rawRes.size() == 1) return atoll(m_rawRes.at(0).data());
		return MIN_SCORE;
	}

	std::string GetErrorMsg() const { return m_errMsg; }

	static const long long MIN_SCORE = INT64_MIN;

private:
	bool isError = false;
	bool isNull = true;
	long long m_intSingleRes = INT64_MIN;
	std::vector<std::string> m_rawRes;
	std::string m_errMsg;
};

class RedisCommand
{
public:
	explicit RedisCommand(cpp_redis::client& redis);
	explicit RedisCommand(RedisContentsType t);
	virtual ~RedisCommand();

	void SetWithScores(bool b = true);
	void SetReverse(bool b = true);

	template <typename Elem, typename ... Strings>
	void SetKey(const Elem& s, const Strings&... forms);
	template <typename Elem>
	void SetKey(const Elem& s);

	template <typename Elem, typename ... Strings>
	void SetParams(const Elem& s, const Strings&... forms);
	template <typename Elem>
	void SetParams(const Elem& s);
	template <typename Elem>
	void SetParams(const std::vector<Elem>& s);
	template <typename Elem, typename Value>
	void SetParams(const std::vector<Elem>& s, const std::vector<Value>& v);

	RedisResult Run();

	std::string GetKey() const { return m_key; }
	std::string GetRawCmd() const { return m_rawCmd; }

protected:
	std::vector<std::string> tokenize(std::string_view str, const char delim);

	template <typename Elem, typename ... Strings>
	std::string MakeKey(const Elem& s, const Strings&... forms);
	template <typename Elem>
	std::string MakeKey(const Elem& s);

	template <typename Elem, typename ... Strings>
	std::string MakeParams(const Elem& s, const Strings&... forms);
	template <typename Elem>
	std::string MakeParams(const Elem& s);
	template <typename Elem>
	std::string MakeParams(const std::vector<Elem>& s);
	template <typename Elem, typename Value>
	std::string MakeParams(const std::vector<Elem>& s, const std::vector<Value>& v);

private:
	cpp_redis::reply& RunCommit();

	void SetCommand();
	void SetRawCommand(std::string_view cmds);

private:
	enum ValueType : char
	{
		INT
		, STRING
		, ETC
	};

	template <typename T>
	ValueType CheckType();

protected:
	std::string m_operate;
	std::string m_key;
	std::string m_subParams;
	std::string m_rawCmd;
	std::vector<std::string> m_cmd;
protected:

	bool m_flag_withScores = false;
	bool m_flag_reverse = false;
	bool m_flag_byScore = false;

	cpp_redis::client& m_redis;
	cpp_redis::reply* m_reply = nullptr;
	int m_timeout = 10;

public:

};

#define DEFINE_REDIS_CMD_COMMON_BODY(x) public: \
	explicit x(cpp_redis::client& redis) \
		: RedisCommand{ redis } \
	{ \
		m_operate = tokenize(typeid(this).name(), ' ').at(1); \
		if (std::string::npos != m_operate.find(':')) { m_operate = tokenize(m_operate, ':').at(1); } \
	}; \
	explicit x(RedisContentsType t) \
		: RedisCommand{ t } \
	{ \
		m_operate = tokenize(typeid(this).name(), ' ').at(1); \
		if (std::string::npos != m_operate.find(':')) { m_operate = tokenize(m_operate, ':').at(1); } \
	}; 


#define DEFINE_REDIS_CMD(x) class x : public RedisCommand \
{ \
	DEFINE_REDIS_CMD_COMMON_BODY(x) \
}


namespace Redis
{
	// Sorted Sets
	DEFINE_REDIS_CMD(ZAdd);
	DEFINE_REDIS_CMD(ZCard);
	DEFINE_REDIS_CMD(ZRem);
	DEFINE_REDIS_CMD(ZRange);
	DEFINE_REDIS_CMD(ZRank);
	DEFINE_REDIS_CMD(ZRevRank);
	DEFINE_REDIS_CMD(ZRevRange);
	DEFINE_REDIS_CMD(ZRangeByScore);
	DEFINE_REDIS_CMD(ZRevRangeByScore);
	DEFINE_REDIS_CMD(ZScore);

	// Hash
	DEFINE_REDIS_CMD(HDel);
	DEFINE_REDIS_CMD(HExists);
	DEFINE_REDIS_CMD(HGet);
	DEFINE_REDIS_CMD(HGetAll);
	DEFINE_REDIS_CMD(HKeys);
	DEFINE_REDIS_CMD(HMGet);
	DEFINE_REDIS_CMD(HSet);
	DEFINE_REDIS_CMD(HMSet);
	DEFINE_REDIS_CMD(HMSetNx);
	DEFINE_REDIS_CMD(HLen);
	DEFINE_REDIS_CMD(HIncrBy);

	class ZIncrBy : public RedisCommand
	{
		DEFINE_REDIS_CMD_COMMON_BODY(ZIncrBy);

	};
}
