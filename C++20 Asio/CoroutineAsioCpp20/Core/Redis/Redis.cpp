
#include "pch.h"
#include "Redis.h"

#include "Singleton.h"
#include "Format.h"
#include "Datetime.h"

#include <cpp_redis/cpp_redis>

//#include "Rand.h"

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
	return __super::Init() && Init("ip", 6379, "password", 16, ERedisConnectType::Sentinel);
}

bool RedisManager::Init(std::string_view ip, int port, std::string_view password, int connectSize, ERedisConnectType rct)
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
				Logger::Inst().Info(Format("Redis Connect start. Index={}, IP={}, Port={}", i, host, port));
			}
			else if (status == cpp_redis::client::connect_state::dropped) 
			{
				Logger::Inst().Log(ERROR_LOG::REDIS_CONNECTION_FAILED, Format("Redis Index={}, IP={}, Port={}", i, host, port));
			}
		});
	}

	size_t successCnt = 0;
	do
	{
		auto iter = m_redisClinetList
			| std::views::filter([](cpp_redis::client* redis) {
				return redis->is_connected();
				});

		std::vector<cpp_redis::client*> m_successConn{ iter.begin(), iter.end() };
		successCnt = m_successConn.size();
	} while (m_redisClinetList.size() != successCnt);

	if (password.size())
	{
		for (auto& d : m_redisClinetList)
		{
			auto& redis = *d;
			redis.auth(std::string{ password }, [](const cpp_redis::reply& res) {
				if (res.ko())
				{
					Logger::Inst().Log(ERROR_LOG::REDIS_CONNECTION_FAILED, Format("Redis Auth Password Failed"));
				}

				if (res.ok())
				{
					Logger::Inst().Info(Format("Redis Auth Password Success"));
				}
			});
		}
	}

	if (false == Ping())
	{
		Logger::Inst().Log(ERROR_LOG::REDIS_PING_TEST_FAILED);
		return false;
	}

	if (ERedisConnectType::Sentinel == rct)
	{
		// max index size check ( 0 ~ 16 )
		if (connectSize < 0 || 16 < connectSize)
		{
			Logger::Inst().Log(ERROR_LOG::REDIS_INVALID_REQUEST, Format("Redis Connection Type was {} - but index size was: {}", (int)rct, connectSize));
			return false;
		}

		for (int i = 0; i < connectSize; i++)
		{
			auto& redis = *m_redisClinetList[i];
			auto fu = redis.select(i);
			redis.commit();
			auto res = fu.get();
			if (false == res.ok())
			{
				Logger::Inst().Log(ERROR_LOG::REDIS_INVALID_REQUEST, Format("Redis Connection Type was {}, index : {}, redis cmd was SELECT", (int)rct, i));
				return false;
			}
		}

		// test 예시
		/*{
			auto& redis = *m_redisClinetList[8];
			auto fu = redis.set("TEST:hello", "Test");
			redis.commit();
			auto res = fu.get();
		}
		{
			auto& redis = *m_redisClinetList[8];
			auto zadd = redis.send({ "ZADD", "TEST:zadd", "60", "name" });
			redis.commit();
			auto res = zadd.get();
		}
		{
			auto& redis = *m_redisClinetList[8];
			Redis::ZAdd rCmd{ redis };
			rCmd.SetKey("TEST", "zaddTest");

			auto r = Rand::Inst().Dice(10000);
			rCmd.SetParams(r, "testName" + Format("{}", r));
			auto res = rCmd.Run();
			if (res.ok())
			{
				std::cout << "insert Success!!\n";
			}
		}

		{
			auto& redis = *m_redisClinetList[8];
			Redis::ZRange rCmd{ redis };
			rCmd.SetKey("TEST", "zaddTest");
			rCmd.SetParams(0, -1);
			rCmd.SetReverse();
			auto res = rCmd.Run();
			if (res.ok() && res.is_array())
			{
				auto arr = res.as_array();
				for (const auto& rply : arr)
				{
					std::cout << rply.as_string() << " ";
				}
			}
		}

		{
			auto& redis = *m_redisClinetList[8];
			Redis::ZRangeByScore rCmd{ redis };
			rCmd.SetKey("TEST", "zaddTest");
			rCmd.SetParams("-inf", "+inf");
			rCmd.SetWithScores();
			auto res = rCmd.Run();
			if (res.ok() && res.is_array())
			{
				auto arr = res.as_array();
				for (size_t i = 0; i < arr.size(); i++)
				{
					if (i & 1)
					{
						std::cout << atoll(arr[i].as_string().data()) << "\n";
					}
					else
					{
						std::cout << arr[i].as_string() << " ";
					}
				}
			}
		}*/
	}
	else if (ERedisConnectType::Cluster == rct)
	{
		// 미구현
		return false;
	}

	return true;
}

bool RedisManager::Ping()
{
	auto iter = m_redisClinetList
		| std::views::filter([](cpp_redis::client* redis) {
		auto fu = redis->ping();
		redis->commit();
		auto res = fu.get();
		return res.is_string() && (res.as_string() == "PONG");
			});

	std::vector<bool> wasPong{ iter.begin(), iter.end() };
	return m_redisClinetList.size() == wasPong.size();
}

int RedisManager::GetIndex(const RedisContentsType& t)
{
	int idx = 0;
	switch (t)
	{
	default:
		idx = 0;
		break;
	}
	return idx;
}

cpp_redis::client& RedisManager::GetRedis(const RedisContentsType& t)
{
	int i = GetIndex(t);
	if (false == (-1 < i && i < m_redisClinetList.size()))
	{
		return *m_redisClinetList.at(0);
	}
	return *m_redisClinetList.at(i);
}


template<>
inline std::string RedisResult::GetResult() const
{
	return GetString();
}

template<>
inline long long RedisResult::GetResult() const
{
	return GetNumber();
}

template<>
inline int RedisResult::GetResult() const
{
	return (int)GetNumber();
}

template<>
inline std::vector<std::string> RedisResult::GetResult() const
{
	return m_rawRes;
}

template<>
inline std::vector<long long> RedisResult::GetResult() const
{
	std::vector<long long> ret;
	for (const auto& d : m_rawRes)
	{
		ret.emplace_back(atoll(d.data()));
	}

	return ret;
}

template<>
inline std::vector<int> RedisResult::GetResult() const
{
	std::vector<int> ret;
	for (const auto& d : m_rawRes)
	{
		ret.emplace_back(atol(d.data()));
	}

	return ret;
}

template<>
inline std::vector<std::pair<std::string, std::string>> RedisResult::GetResult() const
{
	std::vector<std::pair<std::string, std::string>> ret;
	for (size_t i = 0; i < m_rawRes.size(); i++)
	{
		if (i & 1)
		{
			ret.emplace_back(std::make_pair<std::string, std::string>(std::string{ m_rawRes.at(i - 1) }, std::string{ m_rawRes.at(i) }));
		}
	}

	return ret;
}

template<>
inline std::vector<std::pair<std::string, long long>> RedisResult::GetResult() const
{
	std::vector<std::pair<std::string, long long>> ret;
	for (size_t i = 0; i < m_rawRes.size(); i++)
	{
		if (i & 1)
		{
			ret.emplace_back(std::make_pair<std::string, long long>(std::string{ m_rawRes.at(i - 1) }, atoll(m_rawRes.at(i).data())));
		}
	}

	return ret;
}

template<>
inline std::vector<std::pair<long long, std::string>> RedisResult::GetResult() const
{
	std::vector<std::pair<long long, std::string>> ret;
	for (size_t i = 0; i < m_rawRes.size(); i++)
	{
		if (i & 1)
		{
			ret.emplace_back(std::make_pair<long long, std::string>(atoll(m_rawRes.at(i - 1).data()), std::string{ m_rawRes.at(i) }));
		}
	}

	return ret;
}

template<>
inline std::vector<std::pair<long long, long long>> RedisResult::GetResult() const
{
	std::vector<std::pair<long long, long long>> ret;
	for (size_t i = 0; i < m_rawRes.size(); i++)
	{
		if (i & 1)
		{
			ret.emplace_back(std::make_pair<long long, long long>(atoll(m_rawRes.at(i - 1).data()), atoll(m_rawRes.at(i).data())));
		}
	}

	return ret;
}

template<>
inline std::vector<std::pair<std::string, int>> RedisResult::GetResult() const
{
	std::vector<std::pair<std::string, int>> ret;
	for (size_t i = 0; i < m_rawRes.size(); i++)
	{
		if (i & 1)
		{
			ret.emplace_back(std::make_pair<std::string, int>(std::string{ m_rawRes.at(i - 1) }, atol(m_rawRes.at(i).data())));
		}
	}

	return ret;
}

template<>
inline std::vector<std::pair<int, std::string>> RedisResult::GetResult() const
{
	std::vector<std::pair<int, std::string>> ret;
	for (size_t i = 0; i < m_rawRes.size(); i++)
	{
		if (i & 1)
		{
			ret.emplace_back(std::make_pair<int, std::string>(atol(m_rawRes.at(i - 1).data()), std::string{ m_rawRes.at(i) }));
		}
	}

	return ret;
}

template<>
inline std::vector<std::pair<int, int>> RedisResult::GetResult() const
{
	std::vector<std::pair<int, int>> ret;
	for (size_t i = 0; i < m_rawRes.size(); i++)
	{
		if (i & 1)
		{
			ret.emplace_back(std::make_pair<int, int>(atol(m_rawRes.at(i - 1).data()), atol(m_rawRes.at(i).data())));
		}
	}

	return ret;
}


RedisCommand::RedisCommand(cpp_redis::client& redis)
	: m_redis{ redis }
{
}

RedisCommand::RedisCommand(RedisContentsType t)
	: m_redis{ RedisManager::Inst().GetRedis(t) }
{

}

RedisCommand::~RedisCommand()
{
	delete m_reply;
}


std::vector<std::string> RedisCommand::tokenize(std::string_view str, const char delim)
{
	//std::vector<std::string> out;
	//// construct a stream from the string 
	//std::stringstream ss(str);
	//std::string s;
	//while (std::getline(ss, s, delim))
	//{
	//	out.emplace_back(s);
	//}
	//return out;

	/*auto view = str
		| std::ranges::views::split(delim)
		| std::ranges::views::filter([](std::string_view s) { return s != ""; })
		| std::ranges::views::transform([](auto&& elem) {
				return std::string_view(&*elem.begin(), std::ranges::distance(elem));
			});

	std::vector<std::string> out{ view.begin(), view.end() };
	return out;*/

	std::vector<std::string> out;
	auto range = str | std::ranges::views::split(delim);
	for (auto&& view : range)
	{
		if (auto s = std::string_view(&*view.begin(), std::ranges::distance(view)); !s.empty())
		{
			out.emplace_back(s);
		}
	}
	return out;
}

cpp_redis::reply& RedisCommand::RunCommit()
{
	SetCommand();
	auto f = m_redis.send(m_cmd);
	auto tp = std::chrono::system_clock::now() + std::chrono::seconds{ m_timeout };
	m_redis.commit();
	f.wait_until(tp);
	m_reply = new cpp_redis::reply{};
	*m_reply = f.get();
	return *m_reply;
}

void RedisCommand::SetCommand()
{
	if (m_rawCmd.empty())
	{
		m_rawCmd = m_operate + " ";
		if (false == m_key.empty()) { m_rawCmd += m_key + " "; }
		if (false == m_subParams.empty()) { m_rawCmd += m_subParams + " "; }

		if (m_flag_withScores || m_flag_reverse)
		{
			std::string option;
			option = m_flag_withScores ? "WITHSCORES" :
				m_flag_reverse ? "REV" : "";

			m_rawCmd = m_rawCmd + option + " ";
		}
	}

	m_cmd = tokenize(m_rawCmd, ' ');
}

void RedisCommand::SetRawCommand(std::string_view cmds)
{
	m_cmd = tokenize(cmds, ' ');
}

template<typename Elem, typename ...Strings>
void RedisCommand::SetKey(const Elem& s, const Strings & ...params)
{
	m_key = MakeKey(s, params...);
}

template<typename Elem>
void RedisCommand::SetKey(const Elem& s)
{
	m_key = MakeKey(s);
}

template<typename Elem, typename ...Strings>
void RedisCommand::SetParams(const Elem& s, const Strings & ...params)
{
	m_subParams = MakeParams(s, params...);
}

template<typename Elem>
void RedisCommand::SetParams(const Elem& s)
{
	m_subParams = MakeParams(s);
}

template <typename Elem>
void RedisCommand::SetParams(const std::vector<Elem>& s)
{
	m_subParams = MakeParams(s);
}

template <typename Elem, typename Value>
void RedisCommand::SetParams(const std::vector<Elem>& s, const std::vector<Value>& v)
{
	m_subParams = MakeParams(s, v);
}

template <typename Elem>
std::string RedisCommand::MakeParams(const Elem& s)
{
	return std::format("{}", s);
}

template <typename Elem, typename ... Strings>
std::string RedisCommand::MakeParams(const Elem& s, const Strings&... forms)
{
	return std::string{ std::format("{}", s) + " " + MakeParams(forms...) };
}

template <typename Elem>
std::string RedisCommand::MakeParams(const std::vector<Elem>& s)
{
	std::string params = "";
	for (const auto& d : s)
	{
		params += std::format("{} ", d);
	}

	return params;
}

template <typename Elem, typename Value>
std::string RedisCommand::MakeParams(const std::vector<Elem>& s, const std::vector<Value>& v)
{
	std::string params = "";
	if (s.size() != v.size()) { return params; }

	for (size_t i = 0; i < s.size(); ++i)
	{
		params += std::format("{} ", s.at(i));
		params += std::format("{} ", v.at(i));
	}

	return params;
}

template<typename T>
RedisCommand::ValueType RedisCommand::CheckType()
{
	auto v = std::string_view{ typeid(T).name() };
	size_t pos = v.find("string");
	if (pos != std::string::npos) { return ValueType::STRING; }

	pos = v.find("int");
	if (pos != std::string::npos) { return ValueType::INT; }

	return ValueType::ETC;
}

template <typename Elem>
std::string RedisCommand::MakeKey(const Elem& s)
{
	return std::format("{}", s);
}

template <typename Elem, typename ... Strings>
std::string RedisCommand::MakeKey(const Elem& s, const Strings&... forms)
{
	return std::string{ std::format("{}", s) + ":" + MakeKey(forms...) };
}

void RedisCommand::SetWithScores(bool b)
{
	m_flag_withScores = b;
}

void RedisCommand::SetReverse(bool b)
{
	m_flag_reverse = b;
}


RedisResult RedisCommand::Run()
{
	RunCommit();

	if (nullptr != m_reply)
	{
		auto& rep = *m_reply;

		if (rep.ko()) { return RedisResult{ true, rep.error() }; }
		if (rep.is_null()) { return RedisResult{}; }

		if (rep.ok())
		{
			if (rep.is_array())
			{
				std::vector<std::string> res;

				for (const auto& d : rep.as_array())
				{
					res.emplace_back(d.as_string());
				}

				return RedisResult{ res };
			}
			else if (rep.is_string())
			{
				return RedisResult{ rep.as_string() };
			}
			else if (rep.is_integer())
			{
				return RedisResult{ rep.as_integer() };
			}
		}
	}

	return RedisResult{ true, std::string{ "m_reply was nullptr"} };
}
