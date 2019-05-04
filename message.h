#ifndef __WELLSFARGO_WORKSHOP_MESSAGE_H__
#define __WELLSFARGO_WORKSHOP_MESSAGE_H__

#include <string>

namespace wellsfargo {
  namespace workshop {
    class InputMessage {
      private:
        uint64_t    m_epoch;
        double      m_tick_price;   //This is the input price tick for the underlying equity
        double      m_tick_volatility;  //This is the volatility change when that price tick happened
        std::string m_ticker_symbol;

      public:
        InputMessage(const std::string& payload);

        const std::string& symbol() const { return m_ticker_symbol;}
        const uint64_t& epoch() const { return m_epoch;}
        const double& price() const { return m_tick_price;}
        const double& volatility() const { return m_tick_volatility;}

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

            double strikePrice() const { return m_strike_price;}
            double putPrice() const { return m_put_price;}
            double callPrice() const { return m_call_price;}

            void addPutPrice(const double& p) { m_put_price = p;}
            void addCallPrice(const double& p) { m_call_price = p;}

            std::string payload() const;
   };
    
    std::ostream& operator<<(std::ostream& os, const StrikeValue& rhs);
  }
}

#endif
