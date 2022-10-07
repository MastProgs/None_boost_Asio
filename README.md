# C++20 Asio Server

## 서론
boost 라이브러리를 사용하지 않은 단일 asio 서버 통신 라이브러리에, C++20 베이스의 코어 서버이며, 실무에서 직접 구현한 코드들을 테스트하고 검증한 구조와 알고리즘 위주로 작성.
필요한 경우 오픈소스를 활용하여 작성된 경우가 있고, 각 오픈소스는 소스코드의 github 주소들이 존재.

## 환경설정
* Visual Studio 2022
* C++20

## 오픈소스
* cpp_redis ( https://github.com/cpp-redis/cpp_redis )
  * thread safe ( using mutex )
  * can be async ( using future )
* nlohmann json ( https://github.com/nlohmann/json )
* httplib ( https://github.com/yhirose/cpp-httplib )
  * blocking
* better enums ( https://github.com/aantron/better-enums )

## 코드 설명

### Core
대부분의 핵심 기능들은 여기에 구현하여 가져다 쓰는중

#### [Asio](https://github.com/MastProgs/None_boost_Asio/tree/master/C%2B%2B20%20Asio/CoroutineAsioCpp20/Core/Asio)
* asio 을 기반으로 네트워크 통신의 가장 핵심적인 역할을 담당
* boost 문법을 제거하고, std 표준 문법으로 재작성
* asyncronous
* thread safe

##### AsioServer
* Singleton
* io_context 객체를 직접 다루는 핸들러
* 기본적인 IOCP 함수들 같은 IO 서비스를 담당
* Post 함수를 통한 비통기 처리 가능
* Acceptor Handling ( listen control )
* Thread list control

##### AsioAcceptor
* need shared pointer
* need io_context reference
* 접속한 클라이언트를 핸들링
  * async_accept 수행을 동한 AsioClient handling
* Acceptor 상속을 통한, 다양한 연결 지원
* 하나의 io_context 에서 여러 Acceptor 를 활용하려면, 상위 AsioServer 가 핸들링 해주는것을 상정하여 구조를 설계
* 다른 io_context 별 컨트롤이 필요하다면, AsioServer 를 따로 만들어서 handling 해주는 것을 추천

##### AsioClient
* Winsock 의 SOCKET 객체와 같은 기본적인 Send Recv 를 지원하는 클라이언트 객체
* Send Recv 는 asyncronous
* 데이터의 input param 값은 string_view 객체이나, 데이터를 강제 캐스팅 하여 string 버퍼에 담아주는 형태에 불과
* Packet delimiter 를 통한 데이터 끝을 구분하는 구조라 클라이언트 측에서도 Packet delimiter 가 같아야 함
  * 다른 커스텀이 필요하다면, Send Recv 부분을 다른 형태로 커스텀하여 사용 가능
  * asio 의 write, read 함수는 다양한 형식을 지원
* Acceptor 에서 어떠한 Client 형태로 랩핑할지를 결정

### [Coroutine](https://github.com/MastProgs/None_boost_Asio/tree/master/C%2B%2B20%20Asio/CoroutineAsioCpp20/Core/Coroutine)
코루틴을 통해 더 효율적인 비동기 처리를 할 수 있으며, C++20 이 지원이 되어야 정상적으로 처리 가능

* Task 객체를 통해 함수에서 스위칭 되는 시점의 상태 값을 Task 객체로 받음
  * RTask
    * Return task 의 의미
    * 함수로 부터 결과값을 co_yield, co_return 받아야 하는 경우 쓰는 객체
    * 템플릿을 통해 value 값이 임시 저장됨
    * 복사 생성을 추천
    * reference 및 pointer 같은 값은 추천하지 않음 ( 원본 객체가 살아있을 보장이 없음 )
  * VTask
    * Void task 의 의미
    * 특별히 결과값을 받을 필요 없을때 사용
* coroutine handle 을 기본적으로 가지고 있으며, suspend 가 수행 될 때마다, AsioContext Post 함수로 수행을 넘겨줌
* AwaitPost 를 통해 handle
```c++
// coroutine 은 아래와 같은 형태로 사용 가능
VTask Logger::StoreDB(std::string_view log)
{
    /* ... */
    DB.Send(log);
    co_await AwaitPost{};
    auto result = DB.Recv();
    /* ... */
}
```

### [Logger](https://github.com/MastProgs/None_boost_Asio/tree/master/C%2B%2B20%20Asio/CoroutineAsioCpp20/Core/Logger)
로그 관련 처리를 하기 위한 클래스. 기본적으로 std::format 을 사용하므로, C++17 이상 필요. Better Enums 오픈소스를 활용중

#### LogManager
에러 로그 정보들을 사전에 map 구조의 error number, detail info 로 가지고 있고, 에러가 발생하는 시점에 관련 에러코드를 이 LogManager 에서 찾아서 활용

* Singleton
* not thread safe
  * 서버 초기화 시점에 error log 정보를 추가하는 것을 추천
  * 서버가 올라간 상황에서는 std::mutex m_lock 을 통해 접근 관리를 해주어야 함
* Logger 클래스에서만 접근하여 관련 정보를 참조로 불러옴

#### Logger
실질적으로 log 내용을 작성하는 객체, file out 이나 DB store 작업이 필요하다면, print 함수에서 커스텀 시켜주면 된다. 기본은 std::cout 으로 cmd 창 출력

* Singleton
* not thread safe

#### [Redis](https://github.com/MastProgs/None_boost_Asio/tree/master/C%2B%2B20%20Asio/CoroutineAsioCpp20/Core/Redis)
cpp_redis 오픈소스를 랩핑하여 활용한 Redis 객체, 다양한 범위 연산을 하기위해 ranges 를 활용하기에 C++20 필요.

##### RedisManager
cpp_redis 클라이언트 객체를 관리하고 처리하는 클래스. cpp_redis 자체가 비동기 지원을 하며, thread safe.

* Singleton
* Sentinal 형태로 사용하는 경우, vector 를 통해 미리 각 redis index 를 select 해두고 활용하는 구조
  * GetIndex 로 컨텐츠에 해당하는 redis index 를 select 한 cpp_redis::client 객체를 가져오고 GetRedis 를 통해 객체를 reference 로 전달
* not thread safe
  * 최초 서버 시점에 초기화를 진행해주고, 그 이후에 만든 객체를 통해서만 활용하는 것으로 설계

##### RedisCommand
이 객체는 redis 명령어에만 관여하며, 실질적으로 명령어 생성 & 구조 변경을 하여 cpp_redis::client 객체에 전달하는 역할

* need cpp_redis::client
* key hash 구분자를 ':' 를 사용
  * SetKey 함수를 통해 다양한 hash key 를 자동으로 처리
* param value 들을 추가로 넣어주어서 사용

```c++
Redis::Zadd zadd { RedisContentsType::MAX };
zadd.SetKey("User", 326, "th"); // key = "User:326:th"
zadd.SetParams(10);
zadd.Run();
```

##### RedisResult
RedisCommand 객체를 통해서 실행한 결과가 cpp_redis::reply 객체로 반환되는데, 이 reply 객체를 랩핑하여 데이터를 훨씬 쉽고 간편하게 다루기 위한 클래스

* 단일 데이터를 받는다면 GetString(), GetNumber() 를 통해 결과를 받을 수 있음
* IsError(), IsNull() 을 통해 결과의 에러 여부 확인 가능
* Value Score 형태의 리스트로 반환을 받아와야 하는 경우 GetResult<>() 활용 가능
  * 템플릿에서 변환하고자 하는 형태의 데이터를 입력하면, 캐스팅 시켜줌
  * 지원 가능한 형변환
    * std::string
    * long long
    * int
    * std::vector<std::string>
    * std::vector<long long>
    * std::vector<int>
    * std::vector<std::pair<std::string, std::string>>
    * std::vector<std::pair<std::string, long long>>
    * std::vector<std::pair<long long, std::string>>
    * std::vector<std::pair<long long, long long>>
    * std::vector<std::pair<std::string, int>>
    * std::vector<std::pair<int, std::string>>
    * std::vector<std::pair<int, int>> 

```c++
Redis::ZRevRank zrevrank{ RedisContentsType::MAX }; 
/* ... */

zrevrank.SetWithScores();
zrevrank.Run();
 
auto userRankList = zrevrank.GetResult<std::vector<std::pair<std::string, long long>>>();
for ( const auto& iter : userRankList )
{
   const auto& name = iter.first;
   const auto& rank = iter.second; 
}
```
 
### [Common](https://github.com/MastProgs/None_boost_Asio/tree/master/C%2B%2B20%20Asio/CoroutineAsioCpp20/Common)
공통적으로 자주 활용하게 되는 코드의 경우 여기에 구현

* Datetime
  * 시간 관련 다루는 객체
* Enum
  * 공통으로 쓰이는 enum 정의
* Random
  * std random engine 을 활용한 랜덤 함수
* String
  * std::vformat 함수를 한 번 더 랩핑
  * ToStr 을 통해 다양한 문자열 형태 변환
    * Unicode <-> Multibytes
    * std::string <-> std::wstring 
 
### [OpenSource](https://github.com/MastProgs/None_boost_Asio/tree/master/C%2B%2B20%20Asio/CoroutineAsioCpp20/OpenSource)
대부분 오픈 소스들을 가져와 활용하게 되는 곳
 
* Dump
  * StackWalker
    * 이 소스를 통해서 크래시 났을 경우, call stack 들을 출력 가능
    * 상속받아서, 파일 입출력 형태로도 활용 가능
  * ExceptionHandler
    * Minidump 파일 (.dmp) 를 남기는 코드
* httplib
* json
