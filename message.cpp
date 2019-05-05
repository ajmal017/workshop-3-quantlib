#include "message.h"
#include <aws/core/utils/json/JsonSerializer.h>

#include <iostream>
#include <iomanip>
#include <algorithm>

namespace wellsfargo {
  namespace workshop {

    using namespace Aws::Utils::Json;
    namespace {

      const char JSON_KEY_EPOCH[] = "epoch";
      const char JSON_KEY_TICK[] = "tickpr";
      const char JSON_KEY_VOLATILITY[] = "tickvol";
      const char JSON_KEY_TICKER[] = "symbol";


      const char TAG[] = "WF.WORKSHOP.message ";

      //We need this function to trim any extra '\' or '"' characters
      std::string trim( const std::string& payload)
      {
        std::string buffer(payload);
        buffer.erase(std::remove_if(buffer.begin(), buffer.end(), [](char c) { return c == '\\'; }), buffer.end());
        
        size_t stpos = 2;
        stpos = buffer.find("\"{", stpos);
        buffer.erase(buffer.begin() + stpos);
        stpos = buffer.find("}\"", stpos);
        buffer.erase(buffer.begin() + stpos + 1);
        return buffer;
      }

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
      auto tr = trim(payload);
      JsonValue value(tr.c_str());

      if(!value.WasParseSuccessful()) {
        std::cerr << TAG << "Payload parse not successful. Payload <" << tr.c_str() << "> Error <"
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

