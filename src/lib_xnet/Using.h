#pragma once

#include <xtools/Common.h>
#include <xtools/Logger.h>
#include <xtools/Type.h>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using SocketPtr = std::shared_ptr<tcp::socket>;

using boost::system::error_code;
