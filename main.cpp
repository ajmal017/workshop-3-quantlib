
#include "message.h"

#include "OptionParser.h"

#include <aws/lambda-runtime/runtime.h>
#include <aws/sns/SNSRequest.h>
#include <aws/sns/model/SubscribeRequest.h>
#include <aws/sns/model/SubscribeResult.h>
#include <aws/sns/SNSClient.h>

#include <aws/core/Aws.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/LogMacros.h>
#include <aws/core/utils/json/JsonSerializer.h>

#include <aws/core/platform/Environment.h>
#include <aws/core/client/ClientConfiguration.h>

const char TAG[] = "main";

namespace wellsfargo {
namespace workshop {

  using namespace aws::lambda_runtime;
  using namespace Aws::Utils::Json;

    const std::string app_json("application/json");
    invocation_response sendError( const char* errormsg) {
      JsonValue response;
      response.WithString("body", errormsg).WithInteger("statusCode", 400);
      auto const apig_response = response.View().WriteCompact();
      std::string api_resp(apig_response.c_str(), apig_response.size());
      return invocation_response::success(api_resp, app_json);
    }
    
    invocation_response sendSuccess( const char* success) {
      JsonValue response;
      response.WithString("body", success).WithInteger("statusCode", 200);
      auto const apig_response = response.View().WriteCompact();
      std::string api_resp(apig_response.c_str(), apig_response.size());
      return invocation_response::success(api_resp, app_json);
    }

    invocation_response run_local_handler(const invocation_request& req, const std::string& queue)
    {
      AWS_LOGSTREAM_DEBUG(TAG, "received payload: " << req.payload);
      InputMessage event(AwsString(req.payload.c_str(), req.payload.size()));

      return sendSuccess("Recieved Message");
    }

    std::function<std::shared_ptr<Aws::Utils::Logging::LogSystemInterface>()> GetConsoleLoggerFactory()
    {
      return [] {
          return Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(
              "console_logger", Aws::Utils::Logging::LogLevel::Trace);
      };
    }

  }
} //close namespace

int main(int argc, char* argv[])
{
  using namespace Aws;
  using namespace wellsfargo::workshop;
  SDKOptions awsoptions;

  Aws::String m_topicname, m_lambda;
  std::string m_queue_enum;
  bool m_debug = false;

  {
    using namespace optparse;

    OptionParser parse = OptionParser()
      .usage("Pricer task for AWS Usage only")
      .version("1.0.0")
      .description("This pricer task will price Equity Options on AWS cloud");

    parse.add_option("-t", "--topicname")
      .dest("topicname")
      .help("Topic ARN to subscribe to for tickvol prices\n" \
          " For e.g.: arn:aws:sns:ap-south-1:661710984818:TickVolQueue:66e8ba22-b0b5-45a4-aeb2-9fb605259a56" );

    parse.add_option("-l", "--lambda")
      .dest("lambda")
      .help("Lambda ARN that was generated after deploying this task to AWS\n" \
          " For e.g.: arn:aws:lambda:ap-south-1:661710984818:function:workshop-demo" );

    parse.add_option("-d", "--debug").action("store_false").dest("debug");
    parse.add_option("-e", "--enum")
      .dest("queue_enum")
      .help("Mandatory Argument! The topic name for processing right strike prices");
    
    Values& options = parse.parse_args(argc, argv);
    
    m_topicname = Aws::String(options["topicname"].c_str(), options["topicname"].size());
    m_lambda = Aws::String(options["lambda"].c_str(), options["lambda"].size());
    m_queue_enum = options["queue_enum"];
    m_debug = (options["debug"].compare("true") == 0);

    std::cerr << "Initializing task with parameters: " << std::endl
      << "   Topic:  " << m_topicname << std::endl
      << "   Lambda: " << m_lambda << std::endl
      << "   Queue:  " << m_queue_enum << std::endl
      << "   Debug:  " << ((m_debug) ? "True" : "False") << std::endl;
  }

  awsoptions.loggingOptions.logLevel = (m_debug) ? 
                                          Utils::Logging::LogLevel::Debug : 
                                          Utils::Logging::LogLevel::Warn;

  awsoptions.loggingOptions.logger_create_fn = GetConsoleLoggerFactory();

  InitAPI(awsoptions);

  auto m_handler_func = [&m_queue_enum](const aws::lambda_runtime::invocation_request& req) 
  {
    return run_local_handler(req, m_queue_enum);
  };

  Aws::SNS::SNSClient sns;
  Aws::String protocol = "lamdba";
  Aws::String topic_arn = m_topicname;
  Aws::String endpoint = m_lambda;

  Aws::SNS::Model::SubscribeRequest s_req;
  s_req.SetTopicArn(topic_arn);
  s_req.SetProtocol(protocol);
  s_req.SetEndpoint(endpoint);

  auto s_out = sns.Subscribe(s_req);

  if (s_out.IsSuccess())
  {
    AWS_LOGSTREAM_DEBUG(TAG, "Subscribed successfully");
    auto res = s_out.GetResult();
    //Aws::OStringStream output;
    //res.GetResponseMetadata().OutputToStream( output );
    //AWS_LOGSTREAM_DEBUG(TAG, output.str() );
  }
  else
  {
    std::cerr << "Error while subscribing " << s_out.GetError().GetMessage()
      << std::endl;
  }

  aws::lambda_runtime::run_handler(m_handler_func);

  ShutdownAPI(awsoptions);
  return 0;
}

