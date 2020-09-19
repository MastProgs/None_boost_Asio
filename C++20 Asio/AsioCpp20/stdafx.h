#pragma once

// boost 는 BOOST_ASIO_NO_DEPRECATED - 삭제될 함수에 대해서 사용하지 못하게 막는다
#define ASIO_NO_DEPRECATED

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "ObjectBase/Object.h"
#include "ObjectBase/Singleton.h"