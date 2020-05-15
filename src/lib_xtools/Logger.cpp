#include "Logger.h"

#include <fstream>

using namespace x::tool;

std::ostream& x::tool::operator<< (std::ostream& strm, severity_level level)
{
	static const char* strings[] =
	{
		"DEBUG",
		"NORMAL",
		"NOITCE",
		"WARRING",
		"ERROR",
		"CRITICAL"
	};

	if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
		strm << strings[level];
	else
		strm << static_cast<int>(level);

	return strm;
}

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level);
BOOST_LOG_ATTRIBUTE_KEYWORD(process, "Process", std::string);

void x::tool::global_logger_init(const std::string& file) {
	logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
	logging::core::get()->add_global_attribute("Process", attrs::current_process_name());

	boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();
	// backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
	backend->add_stream(boost::shared_ptr<std::ostream>(new std::ofstream(file.c_str(), std::ios_base::app)));

	backend->auto_flush(true);

	boost::shared_ptr<sink_t> sink = boost::make_shared<sink_t>(backend, true);
	
	sink->set_formatter(expr::stream
		<< expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S ")
		<< process
		<< std::setiosflags(std::ios::right) << std::setw(10) << severity << ": "
		<< expr::if_(expr::has_attr("Tag"))[expr::stream << expr::attr< std::string >("Tag")]
		<< expr::smessage);

	logging::core::get()->add_sink(sink);

	log(normal) << "logger initialize ... success!";
}

void x::tool::global_logger_setlevel(severity_level level) {
	logging::core::get()->set_filter(severity >= level);
}