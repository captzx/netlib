#include <boost/log/trivial.hpp>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp> // add_file_log
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#include <fstream>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>

#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>

#include <boost/log/attributes/attribute.hpp>
#include <iomanip>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>


#include <boost/locale/generator.hpp>


namespace src = boost::log::sources;
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;

enum severity_level
{
	trace,
	debug,
	info,
	normal,
	notification,
	warning,
	error,
	critical
};


BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)
//BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)
//BOOST_LOG_ATTRIBUTE_KEYWORD(timeline, "Timeline", attrs::timer::value_type)

void init() {
	// initializes a logging sink that stores log records into a text file.
	// aux function, simplify some common initialization procedures, like sink and commonly used attributes registration.
	
	logging::add_file_log(
		keywords::file_name = "sample_%N.log",
		keywords::rotation_size = 10 * 1024 * 1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = (
			expr::stream
				<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
				<< ": <" << logging::trivial::severity
				<< "> " << expr::smessage
			)
	); 

	// 1. logging core
	// logging::trivial::severity keyword is a placeholder that identifies the severity attribute value in the template expressions
	// This value is expected to have name "Severity" and type severity_level
	// The placeholder along with the ordering operator creates a function object that will be called by the logging core to filter log records.
	// logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);

	// custom functions as filters
	// bool(logging::attribute_value_set const& attrs);
}

// The operator puts a human-friendly representation of the severity level to the stream
std::ostream& operator<< (std::ostream& strm, severity_level level)
{
	static const char* strings[] =
	{
		"trace",
		"debug",
		"info",
		"normal",
		"notification",
		"warning",
		"error",
		"critical"
	};

	if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
		strm << strings[level];
	else
		strm << static_cast<int>(level);

	return strm;
}

void init2() {
	// 2. logging sink
	// sink frontend is responsible for various common tasks for all sinks
	// sink backend implements everything specific to the sink
	typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

	logging::formatter fmt = expr::stream
		<< std::setw(6) << std::setfill('0') << line_id << std::setfill(' ')
		<< ": <" << severity << ">\t"
		<< expr::if_(expr::has_attr(tag_attr))
		[
			expr::stream << "[" << tag_attr << "] "
		]
	<< expr::smessage;

	// 5. format
	sink->set_formatter
	(
		//expr::stream
		//// line id will be written in hex, 8-digits, zero-filled
		//<< std::hex << std::setw(8) << std::setfill('0') << expr::attr< unsigned int >("LineID")
		//<< ": <" << logging::trivial::severity
		//<< "> " << expr::smessage

		/*expr::format("%1%: <%2%> %3%")
		% line_id
		% logging::trivial::severity
		% expr::smessage*/

		fmt
	);

	// The sink will perform character code conversion as needed, according to the locale set with imbue()
	std::locale loc = boost::locale::generator()("en_US.UTF-8");
	sink->imbue(loc);

	// Custom formatting functions
	// void (logging::record_view const& rec, logging::basic_formatting_ostream< CharT >& strm);

	// add a stream to write log to
	sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("sample.log"));

	// We have to provide an empty deleter to avoid destroying the global stream object
	boost::shared_ptr<std::ostream> stream(&std::clog, boost::null_deleter());
	sink->locked_backend()->add_stream(stream);

	// register the sink in the loggin core
	logging::core::get()->add_sink(sink);
}

// BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(mylogger, src::severity_logger<severity_level>)


int main()
{
	init2();
	
	// 4. attributes
	// scope of attribute: source-specific, thread-specific and global
	// When registered, an attribute is given a unique name
	// When a log record is made, attribute values from these three sets are joined into a single set and passed to sinks
	// logging::add_common_attributes(); // aux function, then "LineID", "TimeStamp", "ProcessID" and "ThreadID" are registered globally

	boost::shared_ptr<logging::core> core = logging::core::get();
	core->add_global_attribute("LineID", attrs::counter<unsigned int>(1));
	core->add_global_attribute("TimeStamp", attrs::local_clock()); 
	core->add_global_attribute("Scope", attrs::named_scope());

	// 3.sources 
	// registered anywhere, interact directly with the logging core
	// (thread-safe & non-thread-safe)
	// default and copy-constructible and support swapping
	// different logger, different features, can be combined

	// src::logger_mt& lg = mylogger::get();

	BOOST_LOG_NAMED_SCOPE("named_scope_logging");
	// using namespace logging::trivial;
	src::severity_logger<severity_level> slg;
	slg.add_attribute("Tag", attrs::constant< std::string >("My tag value"));
	logging::record rec = slg.open_record(keywords::severity = info); // construct record
	if (rec) {
		logging::record_ostream strm(rec);
		strm << "hello world";
		strm.flush();
		slg.push_record(boost::move(rec)); // complete record
	}

	BOOST_LOG_SEV(slg, trace) << "sss";
	BOOST_LOG_SEV(slg, debug) << "sss";
	BOOST_LOG_SEV(slg, info) << "sss";

	return 0;
}
