

#include <aws/core/utils/json/JsonSerializer.h>

#include <message.h>


namespace wellsfargo {
  namespace workshop {

    /*
     *  This is the sample SNS message. We should be looking for our "Message" within the Records[0].Sns.Message
     * {
          "Records": [
          {
            "EventSource": "aws:sns",
            "EventVersion": "1.0",
            "EventSubscriptionArn": "arn:.......",
            "Sns": {
              "Type": "Notification",
              "MessageId": "ce4a1d7c-c50d-51ea-bfe8-4dc1078795fe",
              "TopicArn": "arn:.......",
              "Subject": null,
              "Message": "test queue",
              "Timestamp": "2016-12-04T07:05:46.709Z",
              "SignatureVersion": "1",
              "Signature": "<mysighere>",
              "SigningCertUrl": "<pem url here>",
              "UnsubscribeUrl": "<unsub url here>",
              "MessageAttributes": {}
            }
            }
          ]
          } 
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

      m_ticker_symbol = view.GetString(JSON_KEY_TICKER);
      if(m_ticker_symbol.empty()) {
        std::cerr << TAG << "Invalid ticker symbol. Cannot proceed <" << payload << ">" << std::endl;
        throw std::runtime_error("Parse is not successful");
      }
    }

    OutputMessage::OutputMessage()
        :m_epoch(0),
        m_strike_price(0.0),
        m_option_price(0.0),
        m_isput(false) { }


  }
}

