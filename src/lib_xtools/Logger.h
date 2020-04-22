#pragma once 

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp> // add_file_log
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>

#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>

#include <boost/log/attributes/attribute.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>


#include <boost/locale/generator.hpp>
#include <boost/log/attributes/current_process_name.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include <boost/log/sinks/async_frontend.hpp>


namespace x {

namespace tool {

namespace src = boost::log::sources;
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;

enum severity_level {
	debug,
	normal,
	notice,
	warning,
	error,
	critical
};

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(_g_severity_logger, src::severity_logger<x::tool::severity_level>)
#define log(_severity_level) BOOST_LOG_SEV(_g_severity_logger::get(), _severity_level)

typedef sinks::asynchronous_sink<sinks::text_ostream_backend> sink_t;
void global_logger_init(const std::string&); 

std::ostream& operator<< (std::ostream& strm, severity_level level);

} // namespace tool

} // namespace x