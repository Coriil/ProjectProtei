#ifndef MYLOGGER_H
#define MYLOGGER_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/keywords/auto_flush.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace logging = boost::log;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace src = boost::log::sources;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::severity_logger_mt<logging::trivial::severity_level>)


class MyLogger
{
public:
    MyLogger(){};
    void initLogging()
    {
        boost::log::add_common_attributes();
        boost::log::add_file_log(
         "log.txt",
        boost::log::keywords::format = "[%TimeStamp%][%Severity%]: - [%Message%]",
        boost::log::keywords::auto_flush = true);
    }

};

#endif // MYLOGGER_H
