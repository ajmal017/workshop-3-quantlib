#ifdef __WELLSFARGO_WORKSHOP_MESSAGE_H__
#define __WELLSFARGO_WORKSHOP_MESSAGE_H__

#include <string>

namespace wellsfargo {
  namespace workshop {

    enum class Queue : std::int8_t {
      NEAR_PRICE = 0,
      PRICE_MINUS_1_2,
      PRICE_PLUS_1_2,
      PRICE_MINUS_2_5,
      PRICE_PLUS_2_5,
      PRICE_MINUS_5_10,
      PRICE_PLUS_5_10,
      PRICE_MINUS_11,
      PRICE_PLUS_11,
      DEAD_LETTER_QUEUE = 99
    };

    class InputMessage {
      private:
        uint64_t  m_epoch;
        double    m_tick_price;   //This is the input price tick for the underlying equity
        double    m_tick_volatility;  //This is the volatility change when that price tick happened
        Queue     m_queue;

      public:
        InputMessage(const std::string& payload);
    };

    class OutputMessage {
      private:
        uint64_t  m_epoch;
        double    m_strike_price;   //This is the strike price for this this option is valued
        double    m_option_price;   //This is the output option price that is calculated by the engine
                                    //for the given variables
        bool      m_isput;

      public:

        OutputMessage();  //This may take another class as input to create
        std::string& payload() const; // convert this into the JSON string for the next SQS message
    };

  }
}

#endif
