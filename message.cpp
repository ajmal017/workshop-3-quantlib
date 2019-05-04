#include "message.h"
#include <aws/core/utils/json/JsonSerializer.h>

#include <iostream>
#include <iomanip>

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
    const char JSON_KEY_TICK[] = "tickpr";
    const char JSON_KEY_VOLATILITY[] = "tickvol";
    const char JSON_KEY_TICKER[] = "symbol";

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
      auto recs = view.GetArray("Records");
      auto snsrec = recs[0].GetObject("Sns");
      auto message = snsrec.GetObject("Message");
      for(auto key : message.GetAllObjects()) {
        if (key.first.compare(JSON_KEY_EPOCH) == 0) {
          m_epoch = key.second.AsInt64();
          continue;
        }

        //AsDouble is not working properly
        if (key.first.compare(JSON_KEY_TICK) == 0) {
          m_tick_price = std::atof(key.second.AsString().c_str());
          continue;
        }

        if (key.first.compare(JSON_KEY_VOLATILITY) == 0) {
          m_tick_volatility = std::atof(key.second.AsString().c_str());
          continue;
        }

        if (key.first.compare(JSON_KEY_TICKER) == 0) {
          m_ticker_symbol = key.second.AsString().c_str();
          continue;
        }
      }
      
      if(m_epoch <= 0 || m_tick_price <= 0 || m_tick_volatility <= 0 || m_ticker_symbol.empty()) {
        std::cerr << "Epoch <" << m_epoch << "> price <" << m_tick_price 
            << "> Vol <" << m_tick_volatility << "> Symbol <" << m_ticker_symbol << ">" << std::endl;
        throw std::runtime_error("Message Parse is not successful");
      }
    }

    std::ostream& operator<<(std::ostream& os, const StrikeValue& rhs)
    {
      std::setprecision(6);
      os << rhs.strikePrice() << ": Put \t:" << rhs.putPrice() << "\tCall \t:" << rhs.callPrice() << "\t";
      return os;
    }
  }
}

