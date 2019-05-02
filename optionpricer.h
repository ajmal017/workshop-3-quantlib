#ifndef __WELLSFARGO_WORKSHOP_OPTIONPRICER_H__
#define __WELLSFARGO_WORKSHOP_OPTIONPRICER_H__

#include "message.h"
#include <vector>

namespace wellsfargo {
    namespace workshop {


class OptionPricer {

    public:
    OptionPricer(const std::string& queue);

    OptionPricer(const StrikeBucket& sb);

    void price(const InputMessage& event);

    const std::vector< StrikeValue>& strikes() const { return m_strikes; }   

    void showPrices();

    private:

    std::vector< StrikeValue> m_strikes;    
};

    }
}

#endif
