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

#### Asio
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

### Coroutine
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

### Logger
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

