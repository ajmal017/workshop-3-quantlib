
#include "message.h"
#include "dbhandler.h"
#include "optionpricer.h"

#include <aws/lambda-runtime/runtime.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/Aws.h>
#include <iostream>
#include <cstdlib>

const char TAG[] = "WF.WORKSHOP.main ";

namespace wellsfargo {
namespace workshop {

  using namespace aws::lambda_runtime;
  using namespace Aws::Utils::Json;

  const std::string app_json("application/json");

  namespace {
    DBHandler g_dbh;
  }

  invocation_response sendError( const char* errormsg) 
  {
    JsonValue response;
    response.WithString("body", errormsg).WithInteger("statusCode", 400);
    auto const apig_response = response.View().WriteCompact();
    std::string api_resp(apig_response.c_str(), apig_response.size());
    return invocation_response::success(api_resp, app_json);
  }
  
  invocation_response sendSuccess( const char* success) 
  {
    JsonValue response;
    response.WithString("body", success).WithInteger("statusCode", 200);
    auto const apig_response = response.View().WriteCompact();
    std::string api_resp(apig_response.c_str(), apig_response.size());
    return invocation_response::success(api_resp, app_json);
  }

  invocation_response run_local_handler(const invocation_request& req, const std::string& queue)
  {
#if defined(WELLS_QUANTLIB_DEBUG)
    std::cerr << TAG << "recieved payload" << req.payload << " for queue " << queue << std::endl;
#endif

    InputMessage event(req.payload);
    OptionPricer pricer;
    pricer.price(queue, event);

    g_dbh.save(event, pricer.strikes());
    //return 0;
    return sendSuccess("Recieved Message");
  }

  } //close namespace
} //close namespace

int main(int argc, char* argv[])
{
  using namespace wellsfargo::workshop;
  Aws::SDKOptions awsoptions;

  try
  {
    std::string m_queue_enum("sb3");

    char* envget = getenv("WF_STRIKE_BUCKET");
    if(envget) {
      m_queue_enum = envget;
    }
    /*
    aws::lambda_runtime::invocation_request req;
    req.payload = "{ \"Records\": [ { \"EventSource\": \"aws:sns\", \"EventVersion\": \"1.0\", \"EventSubscriptionArn\": \"arn:aws:sns:ap-south-1:::-6022-4a28-8a41\", \
    \"Sns\": { \"Type\": \"Notification\", \"MessageId\": \"b9ce43e7-780d--860c-\", \"TopicArn\": \"arn:aws:sns:ap-south-1::\", \"Subject\": null, \
    \"Message\": {\"tickpr\" : \"12.87\", \"tickvol\": \"4.56\", \"symbol\": \"VIX\", \"epoch\": 12345678}, \
    \"Timestamp\": \"2019-04-30T19:00:00.944Z\", \"SignatureVersion\": \"1\", \"Signature\": \"/+///==\", \"SigningCertUrl\": \"https://amazonaws.com/SimpleNotificationService.pem\", \"UnsubscribeUrl\": \"https://amazonaws.com/?Action=Unsubscribe&-6ea6f23f6c19\", \"MessageAttributes\": {} } } ] }";
    OptionPricer optp;
    */

    Aws::InitAPI(awsoptions);

    auto m_handler_func = [&m_queue_enum ](const aws::lambda_runtime::invocation_request& req) {
      return run_local_handler(req, m_queue_enum);
    };

    aws::lambda_runtime::run_handler(m_handler_func);
    Aws::ShutdownAPI(awsoptions);

  }
  catch(const std::exception &ae) {
    std::cerr << "Caught Exception " << ae.what() << std::endl;
    return 1;
  }
  catch(...) {
    std::cerr << "Unknown exception" << std::endl;
    return 1;
  }
  return 0;
}

