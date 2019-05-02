#include "optionpricer.h"

namespace wellsfargo {
    namespace workshop {

        namespace {
            const std::string queuenames[] = { "q0" /*Banu*/, "q1" /*Bhanu G*/, "q2"/*Bhanu S*/, "q4"/*Manish*/, "q5"/*Prasanna*/, "q6"/*Anurag, Malik, Gaurav*/};
        }
        OptionPricer::OptionPricer(const std::string& queue)
        {
            //This gets the list of strike prices that I need to calculate. ??   
        }

        void OptionPricer::price(const InputMessage& event){
            /*
            */
        }

        void OptionPricer::showPrices(){
            
        }
    }
}