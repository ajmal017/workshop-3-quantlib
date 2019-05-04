#ifndef __WELLSFARGO_WORKSHOP_OPTIONPRICER_H__
#define __WELLSFARGO_WORKSHOP_OPTIONPRICER_H__

#include "message.h"
#include "options_configuration_header.h"
#include <vector>

namespace wellsfargo {
    namespace workshop {


class OptionPricer {

    public:
    OptionPricer(const std::string& queue);

    void price(const InputMessage& event);

    const std::vector< StrikeValue>& strikes() const { return m_strikes; }   

    void showPrices();

    private:

    std::vector< StrikeValue> m_strikes;   
    Tickers m_config;
    StrikeBucket m_bucket; 
};

    }
}

#endif
