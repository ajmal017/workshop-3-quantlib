#ifndef __WELLSFARGO_WORKSHOP_DB_HANDLER_H__
#define __WELLSFARGO_WORKSHOP_DB_HANDLER_H__

#include "message.h"
#include <vector>
#include <memory>

//forward declare;
namespace Aws { namespace DynamoDB {
    class DynamoDBClient;
}}

namespace wellsfargo {
    namespace workshop {

class DBHandler {
    public:
    DBHandler();
    ~DBHandler();
    void save(const InputMessage& event, const std::vector<StrikeValue>& strikes);
    private:
    std::shared_ptr<Aws::DynamoDB::DynamoDBClient> m_dbclient;
};
    }
}
#endif