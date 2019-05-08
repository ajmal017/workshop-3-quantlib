#include "dbhandler.h"
#include "json.h"
#include "options_configuration_header.h"
#include <hiredis/hiredis.h>

#include <iostream>

namespace wellsfargo {
    namespace workshop {
        namespace {

            class RedisClient {
                public:

                RedisClient():m_client() {}

                ~RedisClient() {
                    //redisFree(m_client.get());
                }
                
                bool isConnected() const { return (m_client != nullptr);}
                void reset( redisContext* client) { m_client.reset(client); }

                redisContext* get() { 
                    if(!isConnected()) { return nullptr;}

                    //redisContext* c = m_client.get();
                    return m_client.get();
                }

                private:
                std::shared_ptr<redisContext> m_client;
            };

            const std::string pass = "well$W0rkSh0p3R3d1s";
            static RedisClient g_client;
        }

        DBHandler::DBHandler() {
            
            if(!g_client.isConnected())
            {
                redisContext *client = 0;
                const char* hostname = "13.234.26.186";
                const int port = 6379;

                client = redisConnect(hostname, port);

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
                redisReply *reply = 0;
                reply = (redisReply *)redisCommand(client, "AUTH %s", pass.c_str());
                if(!reply || reply->type == REDIS_REPLY_ERROR)
                {
                    std::cerr << "error authenticating connection. " << std::endl;
                    throw std::runtime_error("Cannot Authenticate Redis DB");
                }
                freeReplyObject(reply);

                g_client.reset(client);
            }
        }

        void DBHandler::save(const InputMessage& event, const std::vector<StrikeValue> &strikes) 
        { 
            for( auto st : strikes)
            {
                std::string temp = st.payload(event);
                redisReply *reply = 0;
                reply = (redisReply *)redisCommand(g_client.get(), "SADD %ld %s", event.epoch(), temp.c_str());
                if(!reply || reply->type == REDIS_REPLY_ERROR)
                {
                    std::cerr << "error saving to db for <" << temp << "> for epoch <" << event.epoch() 
                        << std::endl;
                }
                freeReplyObject(reply);
            }

            std::cerr <<"Saved " << strikes.size() << " values for epoch " << event.epoch() << std::endl;
            return;
        }

        DBHandler::~DBHandler() { }
    }
}