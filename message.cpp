
#include "message.h"
#include <JsonSerializer.h>
#include <LogMacros.h>

namespace wellsfargo {
  namespace workshop {

    using namespace Aws::Utils::Json;
    const std::string JSON_KEY_EPOCH("epoch");
    const std::string JSON_KEY_TICK("ticpr");
    const std::string JSON_KEY_VOLATILITY("ticvol");
    const std::string JSON_KEY_QUEUE_ENUM("queno");

    const std::string JSON_KEY_STRIKE_PRICE("spr");
    const std::string JSON_KEY_OPTION_PRICE("optpr");
    const std::string JSON_KEY_IS_PUT("is_put");

    const std::string TAG("message");
    const std::string QueueNames[] = { "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8" };
    const int8_t QueueNamesSize = sizeof(QueueNames)/sizeof(QueueNames[0]);


    InputMessage::InputMessage(const std::string& payload)
      : m_epoch(0),
      m_tick_price(0.0),
      m_tick_volatility(0.0),
      m_queue(DEAD_LETTER_QUEUE) {
        JsonValue value(payload);

        if(!value.WasParseSuccessful()) {
          AWS_LOGSTREAM_ERROR(TAG, "Payload parse not successful. Payload <" << payload << "> Error <"
              << value.errorString() << ">");
          throw std::runtime_error("Payload parse is not successful");
        }

        JsonView view(value);

        m_epoch = view.GetInt64(JSON_KEY_EPOCH);
        m_tick_price = view.GetDouble(JSON_KEY_TICK);
        m_tick_volatility = view.GetDouble(JSON_KEY_VOLATILITY);
        auto quetemp = view.GetString(JSON_KEY_QUEUE_ENUM);
        if(m_epoch <= 0) {
          AWS_LOGSTREAM_ERROR(TAG, "Invalid epoch. Cannot proceed <" << payload << ">");
          throw std::runtime_error("Parse is not successful");
        }
        if(m_tick_price <= 0) {
          AWS_LOGSTREAM_ERROR(TAG, "Invalid price. Cannot proceed <" << payload << ">");
          throw std::runtime_error("Parse is not successful");
        }
        if(m_tick_volatility <= 0) {
          AWS_LOGSTREAM_ERROR(TAG, "Invalid volatility. Cannot proceed <" << payload << ">");
          throw std::runtime_error("Parse is not successful");
        }
        if(quetemp.empty()) {
          AWS_LOGSTREAM_ERROR(TAG, "Queue is empty. Cannot proceed <" << payload << ">");
          throw std::runtime_error("Parse is not successful");
        }

        bool found = false;
        for(int8_t idx = 0; idx < QueueNamesSize; ++idx) {
          if( QueueNames[idx].compare(quetemp) == 0 ) {
            m_queue = Queue { idx };
            found = true;
            break;
          }
        }

        if(!found) {
          AWS_LOGSTREAM_ERROR(TAG, "Invalid Queue. Cannot proceed <" << payload << ">");
          throw std::runtime_error("Parse is not successful");
        }
      }

      
      OutputMessage()
        :m_epoch(0),
        m_strike_price(0.0),
        m_option_price(0.0),
        m_isput(false) {}


  }
}

