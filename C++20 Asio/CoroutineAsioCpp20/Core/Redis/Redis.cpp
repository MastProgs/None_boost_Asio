
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

		// test ¿¹½Ã
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

RedisCommand::RedisCommand(cpp_redis::client& redis)
	: m_redis{ redis }
{
	m_reply = new cpp_redis::reply{};
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

	auto view = str
		| std::ranges::views::split(delim)
		| std::ranges::views::filter([](std::string_view s) { return s != ""; })
		| std::ranges::views::transform([](auto&& elem) {
				return std::string_view(&*elem.begin(), std::ranges::distance(elem));
			});

	std::vector<std::string> out{ view.begin(), view.end() };
	return out;
}

void RedisCommand::SetCommand()
{
	std::string fullCmd = m_operate + " ";
	if (false == m_key.empty()) { fullCmd = fullCmd + m_key + " "; }
	fullCmd += m_subParams + " ";
	if (m_flag_withScores || m_flag_reverse)
	{
		std::string option;
		option = m_flag_withScores ? "WITHSCORES" :
			m_flag_reverse ? "REV" : "";

		fullCmd = fullCmd + option + " ";
	}

	m_cmd = tokenize(fullCmd, ' ');
}

void RedisCommand::SetRawCommand(std::string_view cmds)
{
	m_cmd = tokenize(cmds, ' ');
}

template <typename Elem>
std::string RedisCommand::MakeKey(const Elem& s)
{
	return Format("{}", s);
}

template <typename Elem, typename ... Strings>
std::string RedisCommand::MakeKey(const Elem& s, Strings... forms)
{
	return std::string{ Format("{}", s) + ":" + MakeKey(forms...) };
}

template <typename Elem>
std::string RedisCommand::MakeParams(const Elem& s)
{
	return Format("{}", s);
}

template <typename Elem, typename ... Strings>
std::string RedisCommand::MakeParams(const Elem& s, Strings... forms)
{
	return std::string{ Format("{}", s) + " " + MakeParams(forms...) };
}

template<typename ...ARGS>
void RedisCommand::SetKey(const ARGS & ...args)
{
	m_key = MakeKey(args...);
}

template<typename ...ARGS>
void RedisCommand::SetParams(const ARGS & ...args)
{
	m_subParams = MakeParams(args...);
}

void RedisCommand::SetWithScores(bool b)
{
	m_flag_withScores = b;
}

void RedisCommand::SetReverse(bool b)
{
	m_flag_reverse = b;
}


cpp_redis::reply& RedisCommand::Run()
{
	SetCommand();
	auto f = m_redis.send(m_cmd);
	auto tp = std::chrono::system_clock::now() + std::chrono::seconds{ m_timeout };
	m_redis.commit();
	f.wait_until(tp);
	*m_reply = f.get();
	return *m_reply;
}
