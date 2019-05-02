

#include <aws/core/utils/json/JsonSerializer.h>

#include <message.h>


namespace wellsfargo {
  namespace workshop {

    /*
    { "Records": 
    [ 
      { 
        "EventSource": "aws:sns",
        "EventVersion": "1.0",
        "EventSubscriptionArn": "arn:aws:sns:ap-south-1:661710984818:TickVolQueue:7335915d-6022-4a28-8a41-6ea6f23f6c19",
        "Sns": { "Type": "Notification",
        "MessageId": "b9ce43e7-780d-5a32-860c-16a5e57db623",
        "TopicArn": "arn:aws:sns:ap-south-1:661710984818:TickVolQueue",
        "Subject": null,
        "Message": "\"Just dont fail\"",
        "Timestamp": "2019-04-30T19:00:00.944Z",
        "SignatureVersion": "1",
        "Signature": "+/+++/+/+///==",
        "SigningCertUrl": "https://sns.ap-south-1.amazonaws.com/SimpleNotificationService-6aad65c2f9911b05cd53efda11f913f9.pem",
        "UnsubscribeUrl": "https://sns.ap-south-1.amazonaws.com/?Action=Unsubscribe&SubscriptionArn=arn:aws:sns:ap-south-1:661710984818:TickVolQueue:7335915d-6022-4a28-8a41-6ea6f23f6c19",
        "MessageAttributes": {} } } ] }
     *
     * 
     */


    using namespace Aws::Utils::Json;
    const char JSON_KEY_EPOCH[] = "epoch";
    const char JSON_KEY_TICK[] = "ticpr";
    const char JSON_KEY_VOLATILITY[] = "ticvol";
    const char JSON_KEY_TICKER[] = "ticker";

    const char JSON_KEY_STRIKE_PRICE[] = "spr";
    const char JSON_KEY_OPTION_PRICE[] = "optpr";
    const char JSON_KEY_IS_PUT[] = "is_put";

    const char TAG[] = "WF.WORKSHOP.message ";

    InputMessage::InputMessage(const std::string& payload)
      : m_epoch(0),
      m_tick_price(0.0),
      m_tick_volatility(0.0),
      m_ticker_symbol() 
    {

#if defined(WELLS_QUANTLIB_DEBUG)
      std::cerr << TAG << "recieved input message" << std::endl; 
#endif
      JsonValue value(payload.c_str());

      if(!value.WasParseSuccessful()) {
        std::cerr << TAG << "Payload parse not successful. Payload <" << payload.c_str() << "> Error <"
            << value.GetErrorMessage() << ">" << std::endl;
        throw std::runtime_error("Payload parse is not successful");
      }

      JsonView view(value);
      
      m_epoch = view.GetInt64(JSON_KEY_EPOCH);
      if(m_epoch <= 0) {
        std::cerr << TAG << "Invalid epoch. Cannot proceed <" << payload << ">" << std::endl;
        throw std::runtime_error("Parse is not successful");
      }

      m_tick_price = view.GetDouble(JSON_KEY_TICK);
      if(m_tick_price <= 0) {
        std::cerr << TAG << "Invalid price. Cannot proceed <" << payload << ">" << std::endl;
        throw std::runtime_error("Parse is not successful");
      }

      m_tick_volatility = view.GetDouble(JSON_KEY_VOLATILITY);
      if(m_tick_volatility <= 0) {
        std::cerr << TAG << "Invalid volatility. Cannot proceed <" << payload << ">" << std::endl;
        throw std::runtime_error("Parse is not successful");
      }

      m_ticker_symbol = view.GetString(JSON_KEY_TICKER).c_str();
      if(m_ticker_symbol.empty()) {
        std::cerr << TAG << "Invalid ticker symbol. Cannot proceed <" << payload << ">" << std::endl;
        throw std::runtime_error("Parse is not successful");
      }
    }



  }
}

