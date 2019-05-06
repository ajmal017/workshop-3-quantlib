#include "message.h"
#include "json.h"
#include "options_configuration_header.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

namespace wellsfargo {
  namespace workshop {

    namespace {

      const char JSON_KEY_EPOCH[] = "epoch";
      const char JSON_KEY_TICK[] = "tickpr";
      const char JSON_KEY_VOLATILITY[] = "tickvol";
      const char JSON_KEY_TICKER[] = "symbol";

      const std::string KEYS[] = { JSON_KEY_EPOCH, JSON_KEY_TICK, JSON_KEY_VOLATILITY, JSON_KEY_TICKER };
      const char TAG[] = "WF.WORKSHOP.message ";

      const char JSON_KEY_STRIKE_ENUM[] = "strike";
      const char JSON_KEY_PRICE[] = "price";
      const char JSON_KEY_STRIKE_PRICE[] = "strikePrice";
      const char JSON_KEY_PUT_PRICE[] = "putValue";
      const char JSON_KEY_CALL_PRICE[] = "callValue";

      const Tickers g_tickers;

    }

    InputMessage::InputMessage(const std::string& payload)
      : m_epoch(0),
      m_tick_price(0.0),
      m_tick_volatility(0.0),
      m_ticker_symbol() 
    {

#if defined(WELLS_QUANTLIB_DEBUG)
      std::cerr << TAG << "recieved input message" << std::endl; 
#endif
      //auto tr = trim(payload);
      Json value = Json::parse(payload);  //This operation by default throws upon invalid payload

      Json::array_t recs = value["Records"];
      for( auto rec : recs)
      {
        auto sns = rec["Sns"];
        std::string message = sns["Message"];
        Json msg = Json::parse(message);  

        for( auto key : KEYS)
        {
          if( msg.contains(key)) 
          {
            if (key.compare(JSON_KEY_EPOCH) == 0) {
              m_epoch = msg[key];
              continue;
            }

            //AsDouble is not working properly
            if (key.compare(JSON_KEY_TICK) == 0) {
              m_tick_price = std::atof(std::string(msg[key]).c_str());
              continue;
            }

            if (key.compare(JSON_KEY_VOLATILITY) == 0) {
              m_tick_volatility = std::atof(std::string(msg[key]).c_str());
              continue;
            }

            if (key.compare(JSON_KEY_TICKER) == 0) {
              m_ticker_symbol = msg[key];
              continue;
            }
          }
        }
            
        if(m_epoch <= 0 || m_tick_price <= 0 || m_tick_volatility <= 0 || m_ticker_symbol.empty()) {
          std::cerr << "Epoch <" << m_epoch << "> price <" << m_tick_price 
              << "> Vol <" << m_tick_volatility << "> Symbol <" << m_ticker_symbol << ">" << std::endl;
          throw std::runtime_error("Message Parse is not successful");
        }
        break;
      }
    }

    std::ostream& operator<<(std::ostream& os, const StrikeValue& rhs)
    {
      os << std::fixed << std::setprecision(6);
      os << rhs.strikePrice() << ": Put \t:" << rhs.putPrice() << "\tCall \t:" << rhs.callPrice() << "\t";
      return os;
    }

    std::string StrikeValue::payload( const InputMessage& event) const
    {
      const auto strikeIndices = g_tickers.listStrikes(event.symbol());
      const auto maxlen = strikeIndices.size();

      char sbbuffer[8] = {0};
      for(uint32_t idx = 0; idx < maxlen; ++idx) {
          if(strikeIndices[idx] == strikePrice()) {
              snprintf(sbbuffer, sizeof(sbbuffer), "S%d", idx);
              break;
          }
      }

      Json::object_t stval = {
          {JSON_KEY_EPOCH, Json(event.epoch())},
          {JSON_KEY_STRIKE_ENUM, Json(sbbuffer) },
          {JSON_KEY_TICKER, Json(event.symbol())},
          {JSON_KEY_PRICE, Json(event.price()) },
          {JSON_KEY_STRIKE_PRICE, Json(strikePrice()) },
          {JSON_KEY_PUT_PRICE, Json(putPrice()) },
          {JSON_KEY_CALL_PRICE, Json(callPrice()) },
      };

      return Json(stval).dump();
    }
  }
}

