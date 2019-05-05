#include "dbhandler.h"
#include "options_configuration_header.h"
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h> 
#include <aws/core/auth/AWSCredentials.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/AttributeValue.h>
#include <aws/dynamodb/model/PutItemRequest.h>
#include <aws/dynamodb/model/PutItemResult.h>

#include <iostream>
#include <algorithm>

namespace wellsfargo {
    namespace workshop {
        namespace {
            const char access_key_id[] = "AKIARCADW6K2H3UMRRN4";
            const char secret_access_key[] = "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";
            const char tablename[] = "arn:aws:dynamodb:ap-south-1:073022239412:table/PricerTable";

            const char JSON_KEY_EPOCH[] = "epoch";
            const char JSON_KEY_STRIKE_ENUM[] = "strike";
            const char JSON_KEY_TICKER[] = "symbol";
            const char JSON_KEY_PRICE[] = "price";
            const char JSON_KEY_STRIKE_PRICE[] = "strikePrice";
            const char JSON_KEY_PUT_PRICE[] = "putValue";
            const char JSON_KEY_CALL_PRICE[] = "callValue";

            const char DOUBLE_FORMAT[] = "%3.6f";
            const Tickers g_tickers;
        }
        DBHandler::DBHandler() {
            Aws::Client::ClientConfiguration clientConfig;
            clientConfig.region = "ap-south-1";
            Aws::Auth::AWSCredentials credentials(access_key_id, secret_access_key);
            m_dbclient = std::make_shared<Aws::DynamoDB::DynamoDBClient>(credentials, clientConfig);
        }

        void DBHandler::save(const InputMessage& event, const std::vector<StrikeValue> &strikes) 
        {
            using namespace Aws::Utils::Json;
            using namespace Aws::DynamoDB::Model;
            Aws::DynamoDB::Model::PutItemRequest pir;
            pir.SetTableName(tablename);
            
            const auto strikeIndices = g_tickers.listStrikes(event.symbol());
            const auto maxlen = strikeIndices.size();

            for( auto st : strikes)
            {
                char sbbuffer[16] = {0};
                Aws::Map< Aws::String, AttributeValue > dict;
                AttributeValue attvalue;

                snprintf(sbbuffer, sizeof(sbbuffer), "%ld", event.epoch());
                attvalue.SetN(sbbuffer);
                dict.insert(std::make_pair(JSON_KEY_EPOCH, attvalue));

                for(auto idx = 0; idx < maxlen; ++idx) {
                    if(strikeIndices[idx] == st.strikePrice()) {
                        snprintf(sbbuffer, sizeof(sbbuffer), "S%d", idx);
                        attvalue.SetN(sbbuffer);
                        dict.insert(std::make_pair(JSON_KEY_STRIKE_ENUM, attvalue));
                        break;
                    }
                }
                
                attvalue.SetS(event.symbol().c_str());
                dict.insert(std::make_pair(JSON_KEY_TICKER, attvalue));

                snprintf(sbbuffer, sizeof(sbbuffer), DOUBLE_FORMAT, event.price());
                attvalue.SetN(sbbuffer);
                dict.insert(std::make_pair(JSON_KEY_PRICE, attvalue));

                snprintf(sbbuffer, sizeof(sbbuffer), DOUBLE_FORMAT, st.strikePrice());
                attvalue.SetN(sbbuffer);
                dict.insert(std::make_pair(JSON_KEY_STRIKE_PRICE, attvalue));

                snprintf(sbbuffer, sizeof(sbbuffer), DOUBLE_FORMAT, st.putPrice());
                attvalue.SetN(sbbuffer);
                dict.insert(std::make_pair(JSON_KEY_PUT_PRICE, attvalue));

                snprintf(sbbuffer, sizeof(sbbuffer), DOUBLE_FORMAT, st.callPrice());
                attvalue.SetN(sbbuffer);
                dict.insert(std::make_pair(JSON_KEY_CALL_PRICE, attvalue));

                pir.WithItem(dict);
            }

            auto response = m_dbclient->PutItem(pir);
            if (!response.IsSuccess()) {
                std::cerr << "DB insert failed for epoch " << event.epoch() << ". Error: "
                    << response.GetError().GetMessage() << std::endl;
            } 

            return;
        }

        DBHandler::~DBHandler() { }
    }
}