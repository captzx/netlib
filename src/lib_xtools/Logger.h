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
#include <boost/log/utility/manipulators/add_value.hpp>


namespace x {

namespace tool {

namespace src = boost::log::sources;
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;

enum severity_level {
	trivial,
	debug,
	normal,
	notice,
	warning,
	error,
	critical
};

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level);
BOOST_LOG_ATTRIBUTE_KEYWORD(process, "Process", std::string);
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(_g_severity_logger, src::severity_logger<x::tool::severity_level>)

#define _log(_severity_level) BOOST_LOG_SEV(_g_severity_logger::get(), _severity_level)
#define _log_t(_severity_level, _value) BOOST_LOG_SEV(_g_severity_logger::get(), _severity_level) << logging::add_value(tag_attr, _value)
#define _use_log(_1, _2, who, ...) who
#define log(...) _use_log(__VA_ARGS__, _log_t, _log, ...)(__VA_ARGS__)

void global_logger_init(const std::string&); 
void global_logger_set_filter(const logging::filter&);
std::ostream& operator<< (std::ostream& strm, severity_level level);

// filter filter;

} // namespace tool

} // namespace x