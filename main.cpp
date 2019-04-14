
#include <aws/lambda-runtime/runtime.h>
#include <LogMacros.h>

#include "OptionParser.h"
#include "message.h"
#include <stringstream>

namespace wellsfargo {
namespace workshop {

  const std::string TAG("main");

  using namespace aws::lambda_runtime;

  invocation_response sendError( const char* errormsg) {
    JsonValue response;
    response.WithString("body", errormsg).WithInteger("statusCode", 400);
    auto const apig_response = response.View().WriteCompact();
    return invocation_response::success(apig_response, "application/json");
  }
  
  invocation_response sendSuccess( const char* success) {
    JsonValue response;
    response.WithString("body", success).WithInteger("statusCode", 200);
    auto const apig_response = response.View().WriteCompact();
    return invocation_response::success(apig_response, "application/json");
  }

  invocation_response run_local_handler(const invocation_request& req, int batchsize, int queueno)
  {
    AWS_LOGSTREAM_DEBUG(TAG, "received payload: " << req.payload);
    try {
      InputMessage event(req.payload);

    const criteria cr(eventJson);
    if (cr.error_msg) {
      AWS_LOGSTREAM_ERROR(TAG, "Validation failed. " << cr.error_msg);
      return sendError(cr.error_msg.c_str());
    }

    return sendSuccess(successmsg.str().c_str());
  }

  std::function<std::shared_ptr<Aws::Utils::Logging::LogSystemInterface>()> GetConsoleLoggerFactory()
  {
      return [] {
          return Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(
              "console_logger", Aws::Utils::Logging::LogLevel::Trace);
      };
  }
  }
}

int main(int argc, char* argv[])
{
  using namespace Aws;
  SDKOptions awsoptions;

  int m_batchsize = 0, m_queue_enum = 0;
  bool m_debug = false;

  {
    using namespace optparse;

    OptionParser parse = OptionParser()
      .usage("Pricer task for AWS Usage only")
      .version("1.0.0")
      .description("This pricer task will price Equity Options on AWS cloud");

    parse.add_option("-b", "--batchsize")
      .dest("batchsize")
      .set_default(10)
      .help("Number of queue messages to consume per instantiation");

    parse.add_option("-d", "--debug").action("store_false").dest("debug");
    parse.add_option("-e", "--enum")
      .dest("queue_enum")
      .help("Mandatory Argument! The queue enum/topic for filtering messages");
    
    Values& options = parse.parse_args(argc, argv);
    
    m_batchsize = std::atoi(options["batchsize"]);
    m_queue_enum = std::atoi(options["queue_enum"]);

    m_debug = (options["debug"].compare("true") == 0);
  }

  awsoptions.loggingOptions.logLevel = (m_debug) ? 
                                          Utils::Logging::LogLevel::Debug : 
                                          Utils::Logging::LogLevel::Warn;

  options.loggingOptions.logger_create_fn = GetConsoleLoggerFactory();

  InitAPI(awsoptions);

  auto m_handler_func = [&m_batchsize, &m_queue_enum](const aws::lambda_runtime::invocation_request& req) {
    return run_local_handler(req, m_batchsize, m_queue_enum);
  };

  aws::lambda_runtime::run_handler(m_handler_func);

  ShutdownAPI(awsoptions);
  return 0;
}

