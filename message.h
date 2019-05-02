#ifndef __WELLSFARGO_WORKSHOP_MESSAGE_H__
#define __WELLSFARGO_WORKSHOP_MESSAGE_H__

#include <string>

namespace wellsfargo {
  namespace workshop {

    enum class StrikeBucket : std::int8_t {
      NEAR_PRICE_5 = 0,
      PRICE_RANGE_5_15,
      PRICE_RANGE_15_30,
      PRICE_RANGE_30_50,
      PRICE_RANGE_50_75,
      PRICE_RANGE_75_100,
      DEAD_LETTER_QUEUE = 99
    };

    class InputMessage {
      private:
        uint64_t    m_epoch;
        double      m_tick_price;   //This is the input price tick for the underlying equity
        double      m_tick_volatility;  //This is the volatility change when that price tick happened
        std::string m_ticker_symbol;

      public:
        InputMessage(const std::string& payload);
    };

    std::ostream& operator<<(std::ostream& os, const InputMessage& rhs);

    class StrikeValue {
        private:
            float m_strike_price;
            double m_put_price;
            double m_call_price;
 
        public:
            StrikeValue(const decltype(m_strike_price) & sp) //Store a strike price
                : m_strike_price(sp),
                m_put_price(0.0),
                m_call_price(0.0){}

            void addPutPrice(const double& p) { m_put_price = p;}
            void addCallPrice(const double& p) { m_call_price = p;}

            std::string payload() const;
   };
    
    std::ostream& operator<<(std::ostream& os, const StrikeValue& rhs);
  }
}

#endif
