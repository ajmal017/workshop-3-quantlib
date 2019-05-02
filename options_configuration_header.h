#ifndef __WELLSFARGO_WORKSHOP_OPTIONSCONFIGURATION_HEADER_H__
#define __WELLSFARGO_WORKSHOP_OPTIONSCONFIGURATION_HEADER_H__

#include <vector>
#include <string>

namespace wellsfargo {
    namespace workshop {

        using StrikePriceTypes = std::vector<float>;
    class Tickers {
        private:
            const std::vector<std::string> m_tickers = { "VIX", "IBM" };
            const StrikePriceTypes m_vixstrikes = { 10.00, 11.00, 12.00, 13.00, 14.00, 15.00, 16.00, 17.00, 18.00, 19.00, 20.00, 21.00, 
                    22.00, 23.00, 24.00, 25.00, 26.00, 27.00, 28.00, 29.00, 30.00, 32.50, 35.00, 37.50, 
                    40.00, 42.50, 45.00, 47.50, 50.00, 55.00, 60.00, 65.00, 70.00, 75.00, 80.00 };
            const StrikePriceTypes m_ibmstrikes = { 60.00, 65.00, 70.00, 75.00, 80.00, 85.00, 90.00, 95.00, 100.00, 105.00, 110.00,  
                    115.00, 120.00, 125.00, 130.00, 135.00, 140.00, 145.00, 150.00, 155.00, 160.00,  
                    165.00, 170.00, 175.00, 180.00};

            const std::vector< StrikePriceTypes > m_strikes = { m_vixstrikes, m_ibmstrikes };

            const std::vector< double > m_endofday_prices = { 14.64, 139.93 };
        public:
            Tickers() = default;
            const std::vector<std::string>& list() const { return m_tickers; }
            const decltype(m_strikes) & listStrikes() const { return m_strikes; }
            const std::vector< double >& endOfDayPrices() const { return m_endofday_prices; }
        };

    }
}


#endif
