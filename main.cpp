
#include "message.h"
#include "dbhandler.h"
#include "optionpricer.h"
#include "json.h"

#include <aws/lambda-runtime/runtime.h>
#include <iostream>
#include <cstdlib>

const char TAG[] = "WF.WORKSHOP.main ";

namespace wellsfargo {
namespace workshop {

  using namespace aws::lambda_runtime;

  const std::string app_json("application/json");

  invocation_response sendError( const char* errormsg) 
  {
    Json::object_t response( {{"body", Json(errormsg)}, {"statusCode", Json(400)}} );
    return invocation_response::success(Json(response).dump(), app_json);
  }
  
  invocation_response sendSuccess( const char* success) 
  {
    Json::object_t response( {{"body", Json(success)}, {"statusCode", Json(200)}} );
    return invocation_response::success(Json(response).dump(), app_json);
  }

  invocation_response run_local_handler(const invocation_request& req, const std::string& queue)
  //int run_local_handler(const invocation_request& req, const std::string& queue)
  {
#if defined(WELLS_QUANTLIB_DEBUG)
    std::cerr << TAG << "recieved payload" << req.payload << " for queue " << queue << std::endl;
#endif

    InputMessage event(req.payload);
    OptionPricer pricer;
    pricer.price(queue, event);

    DBHandler g_dbh;
    g_dbh.save(event, pricer.strikes());
    //return 0;
    return sendSuccess("Recieved Message");
  }

  } //close namespace
} //close namespace

int main(int argc, char* argv[])
{
  using namespace wellsfargo::workshop;
  try
  {
    std::string m_queue_enum("sb2");

    char* envget = getenv("WF_STRIKE_BUCKET");
    if(envget) {
      m_queue_enum = envget;
    }

    /*
    aws::lambda_runtime::invocation_request req;
    req.payload = "{ \"Records\": [ { \"EventSource\": \"aws:sns\", \"EventVersion\": \"1.0\", \"EventSubscriptionArn\": \"arn:aws:sns:ap-south-1:::-6022-4a28-8a41\", \
    \"Sns\": { \"Type\": \"Notification\", \"MessageId\": \"b9ce43e7-780d--860c-\", \"TopicArn\": \"arn:aws:sns:ap-south-1::\", \"Subject\": null, \
    \"Message\": \"{\\\"symbol\\\":\\\"VIX\\\",\\\"tickpr\\\":\\\"16.34\\\",\\\"lambda\\\":\\\"q0\\\",\\\"epoch\\\":1557042500422,\\\"tickvol\\\":\\\"89.67\\\"}\", \
    \"Timestamp\": \"2019-04-30T19:00:00.944Z\", \"SignatureVersion\": \"1\", \"Signature\": \"/+///==\", \"SigningCertUrl\": \"https://amazonaws.com/SimpleNotificationService.pem\", \"UnsubscribeUrl\": \"https://amazonaws.com/?Action=Unsubscribe&-6ea6f23f6c19\", \"MessageAttributes\": {} } } ] }";
    */    

    auto m_handler_func = [&m_queue_enum ](const aws::lambda_runtime::invocation_request& req) {
      return run_local_handler(req, m_queue_enum);
    };

    aws::lambda_runtime::run_handler(m_handler_func);
    //return run_local_handler(req, m_queue_enum);
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

