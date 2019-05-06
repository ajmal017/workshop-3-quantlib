#include "dbhandler.h"
#include "json.h"
#include "options_configuration_header.h"
#include <hiredis/hiredis.h>

#include <iostream>

namespace wellsfargo {
    namespace workshop {
        namespace {
            const char access_key_id[] = "AKIARCADW6K2H3UMRRN4";
            const char secret_access_key[] = "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";
            const char tablename[] = "PricerVol";

            class RedisClient {
                public:

                RedisClient():m_client() {}

                ~RedisClient() {
                    redisFree(m_client.get());
                }
                
                bool isConnected() const { return (m_client != nullptr);}
                void reset( redisContext* client) { m_client.reset(client); }

                redisContext* get() { 
                    if(!isConnected()) { return nullptr;}

                    redisContext* c = m_client.get();
                    return c;
                }

                private:
                std::shared_ptr<redisContext> m_client;
            };

            static RedisClient g_client;
        }

        DBHandler::DBHandler() {
            /*
            if(!g_client.isConnected())
            {
                redisContext *client = 0;
                const char* hostname = "pricevolcache-001.fy3nza.001.aps1.cache.amazonaws.com:6379";

                struct timeval timeout = {1, 500000}; //1.5 seconds

                client = redisConnectUnixWithTimeout(hostname, timeout);

                if(client == NULL || client->err) {
                    if (client) {
                        std::cerr << "Connection error <" << client->errstr << "> for hostname <"
                            << hostname << ">" << std::endl;
                        throw std::runtime_error("Cannot connect to Redis DB");
                    }
                    else {
                        std::cerr << "Connection error for hostname <" << hostname << ">" << std::endl;
                        throw std::runtime_error("Cannot find Redis DB");
                    }
                }
                g_client.reset(client);
            }*/
        }

        void DBHandler::save(const InputMessage& event, const std::vector<StrikeValue> &strikes) 
        { 
            redisReply *reply = 0;
            for( auto st : strikes)
            {
                std::string temp = st.payload(event);
                //reply = (redisReply *)redisCommand(g_client.get(), "SADD %d %.*s", 
//                                                        event.epoch(), temp.size(), temp.c_str());
                if(!reply || reply->type == REDIS_REPLY_ERROR)
                {
                    std::cerr << "error saving to db for <" << temp << "> for epoch <" << event.epoch() 
                            << ">" << std::endl;
                }
                //freeReplyObject(reply);
            }

            return;
        }

        DBHandler::~DBHandler() { }
    }
}