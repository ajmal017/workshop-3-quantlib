#ifndef __WELLSFARGO_WORKSHOP_DB_HANDLER_H__
#define __WELLSFARGO_WORKSHOP_DB_HANDLER_H__

#include "message.h"
#include <vector>

namespace wellsfargo {
    namespace workshop {

class DBHandler {
    public:
    DBHandler();

    void save(const InputMessage& event, const std::vector<StrikeValue>& strikes);
};
    }
}
#endif