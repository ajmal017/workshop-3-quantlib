#ifndef __WELLSFARGO_WORKSHOP_OPTIONSCONFIGURATION_HEADER_H__
#define __WELLSFARGO_WORKSHOP_OPTIONSCONFIGURATION_HEADER_H__

#include <vector>
#include <string>
#include <map>

namespace wellsfargo {
    namespace workshop {

    /*
    The strike bucket is a unique concept within this program that is used to maximise the potential of the calculations.
    SB1 is the near price bucket. Just 2 price points on both sides of the END price
    SB2 is  4 strike prices. 2 on each side of the near price bucket.
    SB3 to SB7 are all generic buckets with max size of 6 strike prices each.
    The goal is the get the most looked at strike prices priced faster than the rest of the batch for a given tick.
    Given an underlying with a price of 13.4 and the following strike prices are put into the different strike buckets
                               ||
    10.00, 11.00, 12.00, 13.00, 14.00, 15.00, 16.00, 17.00, 18.00, 19.00, 20.00, 21.00, 22.00, 23.00, 24.00, 25.00, 26.00, 27.00, 28.00, 29.00, 30.00, 32.50, 35.00, 37.50, 40.00, 42.50, 45.00, 47.50, 50.00, 55.00, 60.00, 65.00, 70.00, 75.00, 80.00 
                          SB1     SB1
            SB2    SB2                 SB2     SB2
     SB3                                              SB3    SB3    SB3
                                                                           SB4    SB4    SB4    SB4   SB4    SB4    SB4
                                                                                                                            SB5    SB5    SB5     SB5    SB5    SB5    SB5
                                                                                                                                                                             SB6     SB6    SB6    SB6    SB6   SB6     SB6
                                                                                                                                                                                                                              SB7    SB7    SB7    SB7
    A similar exercise with IBM (EOD price 139.39 )would lead to

                                                                                                                          ||
    60.00, 65.00, 70.00, 75.00, 80.00, 85.00, 90.00, 95.00, 100.00, 105.00, 110.00, 115.00, 120.00, 125.00, 130.00, 135.00, 140.00, 145.00, 150.00, 155.00, 160.00, 165.00, 170.00, 175.00, 180.00 
                                                                                                                      SB1   SB1
                                                                                                      SB2    SB2                     SB2    SB2
                                                                            SB3      SB3    SB3                                                     SB3     SB3      SB3
                                                      SB4    SB4     SB4                                                                                                    SB4     SB4     SB4
           SB5    SB5     SB5    SB5    SB5    SB5
     SB6
    */
    enum class StrikeBucketType : std::int8_t {
      NEAR_PRICE = 0,
      PRICE_RANGE_4_PERBUCKET,
      PRICE_RANGE_6_PERBUCKET
    };

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

            const std::vector< double > m_endofday_prices = { 17.70, 139.93 };
        public:
            Tickers() = default;
            const std::vector<std::string>& list() const { return m_tickers; }
            const StrikePriceTypes& listStrikes(const std::string& symbol) const;
            
            double endOfDayPrices(const std::string& symbol) const;

            const StrikePriceTypes& listStrikesForBucket(const std::string& sb, const std::string& symbol);

        private:

            std::map< std::string, std::vector< StrikePriceTypes >> m_outputAllStrikes;
        };

    }
}


#endif
