#pragma once

#include <xtools/Common.h>
#include <xtools/Logger.h>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using SocketPtr = std::shared_ptr<tcp::socket>;
