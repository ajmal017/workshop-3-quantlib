

#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/logging/LogLevel.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/LogMacros.h>

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
    const std::string JSON_KEY_EPOCH("epoch");
    const std::string JSON_KEY_TICK("ticpr");
    const std::string JSON_KEY_VOLATILITY("ticvol");
    const std::string JSON_KEY_QUEUE_ENUM("queno");

    const std::string JSON_KEY_STRIKE_PRICE("spr");
    const std::string JSON_KEY_OPTION_PRICE("optpr");
    const std::string JSON_KEY_IS_PUT("is_put");

    const char TAG[] = "WF.WORKSHOP.message";
    const std::string QueueNames[] = { "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8" };
    const int8_t QueueNamesSize = sizeof(QueueNames)/sizeof(QueueNames[0]);


    InputMessage::InputMessage(const AwsString& payload)
      : m_epoch(0),
      m_tick_price(0.0),
      m_tick_volatility(0.0),
      m_queue(Queue::DEAD_LETTER_QUEUE) {

        std::cerr << "WF.WORKSHOP recieved input message" << std::endl; 
        JsonValue value(payload);

        if(!value.WasParseSuccessful()) {
          AWS_LOGSTREAM_ERROR(TAG, "Payload parse not successful. Payload <" << payload.c_str() << "> Error <"
              << value.GetErrorMessage() << ">");
          throw std::runtime_error("Payload parse is not successful");
        }

        JsonView view(value);
        /*
        AWS_LOGSTREAM_DEBUG(TAG, "Created Json View" << view);
        m_epoch = view.GetInt64(JSON_KEY_EPOCH);
        if(m_epoch <= 0) {
          AWS_LOGSTREAM_ERROR(TAG, "Invalid epoch. Cannot proceed <" << payload << ">");
          throw std::runtime_error("Parse is not successful");
        }

        m_tick_price = view.GetDouble(JSON_KEY_TICK);
        if(m_tick_price <= 0) {
          AWS_LOGSTREAM_ERROR(TAG, "Invalid price. Cannot proceed <" << payload << ">");
          throw std::runtime_error("Parse is not successful");
        }

        m_tick_volatility = view.GetDouble(JSON_KEY_VOLATILITY);
        if(m_tick_volatility <= 0) {
          AWS_LOGSTREAM_ERROR(TAG, "Invalid volatility. Cannot proceed <" << payload << ">");
          throw std::runtime_error("Parse is not successful");
        }

        auto quetemp = view.GetString(JSON_KEY_QUEUE_ENUM);
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
        */
      }

      
    OutputMessage::OutputMessage()
        :m_epoch(0),
        m_strike_price(0.0),
        m_option_price(0.0),
        m_isput(false) {}


  }
}

