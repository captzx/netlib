#include "common.h"
#include "Logger.h"


std::ostream& operator<< (std::ostream& strm, severity_level level)
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

void global_logger_init() {
	logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
	logging::core::get()->add_global_attribute("Process", attrs::current_process_name());

	typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

	sink->set_formatter(expr::stream
		<< expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S ")
		<< process
		<< std::setiosflags(std::ios::right) << std::setw(10) << severity << ": "
		<< expr::smessage);

	sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("sample.log"));
	logging::core::get()->add_sink(sink);
}

void test_logger() {
	log(debug) << "debug" << 1 << "debug debug";
	log(normal) << "normal" << 2 << " normal normal";
	log(notice) << "notice" << 3 << " notice notice";
	log(warning) << "warning" << 4 << " warning warning";
	log(error) << "error error" << 5 << " error";
	log(critical) << "critical" << 6 << " critical critical";
}