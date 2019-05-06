#include "options_configuration_header.h"

#include <iostream>

namespace wellsfargo {
    namespace workshop {

        namespace {
            const std::string queuenames[] = { "sb1" /*Anurag*/, "sb2" /*Banu G*/, "sb3"/*Bhanu G*/, "sb4"/*Bhanu S*/, 
                        "sb5"/*Gaurav*/, "sb6" /*Manish*/, "sb7"/*Prasanna*/};
            const uint32_t maxqueuelen = sizeof(queuenames)/ sizeof(queuenames[0]);
        }

        const StrikePriceTypes& Tickers::listStrikes(const std::string& symbol) const
        { 
            for(uint32_t idx = 0; idx < 2; ++idx) {
                if( m_tickers[idx].compare(symbol) == 0) {
                    return m_strikes[idx];
                }
            }
            return StrikePriceTypes(); 
        }

        double Tickers::endOfDayPrices(const std::string& symbol) const
        {
            for(uint32_t idx = 0; idx < 2; ++idx) {
                if( m_tickers[idx].compare(symbol) == 0) {
                    return m_endofday_prices[idx];
                }
            }
            return -1.0;
        }

        const StrikePriceTypes& Tickers::listStrikesForBucket(const std::string& sb, const std::string& symbol)
        {
            uint32_t sbidx = 0;
            bool foundbucket = false;
            for(; sbidx < maxqueuelen; ++sbidx) {
                if( queuenames[sbidx].compare(sb) == 0) {
                    foundbucket = true;
                    break;
                }
            }

            if(!foundbucket) {
                std::cerr << "Found invalid strike bucket <" << sb << ">" << std::endl;
                throw std::runtime_error("Invalid strike bucket. Check your environment variables");
            }

            auto strikesdone = m_outputAllStrikes.find(symbol);
            if (strikesdone != m_outputAllStrikes.end()) {
                return strikesdone->second[sbidx];
            }

            std::vector< StrikePriceTypes> outputAllStrikes;
            outputAllStrikes.reserve(maxqueuelen);

            const StrikePriceTypes& conststrikes = listStrikes(symbol);
            if(conststrikes.empty()) 
            {
                std::cerr << "Found invalid symbol <" << symbol << ">" << std::endl;
                throw std::runtime_error("Invalid symbol. Cannot proceed further");
            }

            auto eodPrice = endOfDayPrices(symbol);
            if( eodPrice <= 0.0) {
                std::cerr << "Found invalid end of day Price for symbol <" << symbol << ">" << std::endl;
                throw std::runtime_error("Invalid EOD Price for symbol. Cannot proceed further");
            }

            StrikePriceTypes strikes(conststrikes); //Make a temporary deletable vector from const
            auto iter = strikes.begin();
            while(iter != strikes.end() && *(iter + 1) < eodPrice) 
                iter++;

            outputAllStrikes.push_back({ *iter, *(iter + 1)});
            iter = strikes.erase(iter);   //erase floor
            iter = strikes.erase(iter);   //erase ceiling
            for(auto idx = 1; idx < maxqueuelen; idx++)
                outputAllStrikes.push_back(StrikePriceTypes());

            bool first = true;
            
            for( auto idx = 1; idx < maxqueuelen; idx++) 
            {
                int maxiterelems = (first ) ? 6 : 8;
                int maxdistance = (first ) ? maxiterelems/2 : maxiterelems;
                if(std::distance(strikes.begin(), iter) > maxdistance) {
                    iter -= maxdistance;
                }
                else {
                    iter = strikes.begin();
                }
                first = false;

                for(int elems = 0; iter != strikes.end() && elems < maxiterelems; elems++) {
                    outputAllStrikes[idx].push_back(*iter);
                    iter = strikes.erase(iter);
                }   
            }
                //All the output strike buckets must be now filled
#if defined(WELLS_QUANTLIB_DEBUG)
            {
                for(auto pidx = 0; pidx < outputAllStrikes.size(); ++pidx)
                {
                    std::cerr << "sb" << pidx + 1 << "==>";
                    for(auto strike : outputAllStrikes[pidx]) {
                        std::cerr << strike << ", ";
                    }
                    std::cerr << std::endl;
                }
            }
#endif
            m_outputAllStrikes.insert(std::make_pair(symbol, outputAllStrikes));
            return m_outputAllStrikes[symbol][sbidx];
        }

    }   //close namespace
}   //close namespace
 