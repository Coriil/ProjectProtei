#ifndef MYLOGGER_H
#define MYLOGGER_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace src = boost::log::sources;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::severity_logger_mt<logging::trivial::severity_level>)


class MyLogger
{
public:
    MyLogger();
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
