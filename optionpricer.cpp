#include "optionpricer.h"
#include "options_configuration_header.h"

#include <ql/qldefines.hpp>
#include <ql/settings.hpp>
#include <ql/instruments/vanillaoption.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/utilities/dataformatters.hpp>
//#include <ql/quotes/simplequote.hpp>
//#include <ql/termstructures/yield/flatforward.hpp>
//#include <ql/termstructures/volatility/equityfx/blackconstantvol.hpp>
//#include <ql/processes/blackscholesprocess.hpp>
#include <ql/pricingengines/vanilla/binomialengine.hpp>
#include <ql/pricingengines/vanilla/analyticeuropeanengine.hpp>
#include <ql/exercise.hpp>


#include <iostream>

namespace wellsfargo {
    namespace workshop {

        using namespace QuantLib;
        
        namespace {
            const Option::Type g_types[] = { Option::Type::Put, Option::Type::Call};
        }

        void OptionPricer::price(const std::string& strikebucket, const InputMessage& event)
        {
            try
            {
                Tickers config;
                auto strikes = config.listStrikesForBucket(strikebucket, event.symbol());
                m_strikes.reserve(strikes.size());
                std::cerr << "Start pricing for ";
                for(auto strike : strikes ) {
                    m_strikes.push_back(StrikeValue(strike));
                    std::cerr << strike << ", ";
                }
                std::cerr << "for SB " << strikebucket << std::endl;

                 // set up dates
                Calendar calendar = TARGET();
                Date todaysDate(8, May, 2019);
                Date settlementDate(11, May, 2019);
                Settings::instance().evaluationDate() = todaysDate;
                Date maturity(17, July, 2019);
                DayCounter dayCounter = Actual365Fixed();

                std::shared_ptr<Exercise> exercise = std::make_shared<EuropeanExercise>(maturity);
                Spread dividendYield = 0.00;
                Rate riskFreeRate = 0.017;// www.livevol.com/option-calculator

                Handle<YieldTermStructure> flatTermStructure( std::make_shared<FlatForward>(settlementDate, riskFreeRate, dayCounter));
                Handle<YieldTermStructure> flatDividendTS( std::make_shared< FlatForward>(settlementDate, dividendYield, dayCounter));

                for( StrikeValue& strp : m_strikes )
                {
                    for (auto typ : g_types)    //for both put and call
                    {
                        Real underlying = event.price();
                        Handle<Quote> underlyingH(std::make_shared<SimpleQuote>(underlying));
                        Real strike = strp.strikePrice();

                        Volatility volatility = event.volatility();
                        Handle<BlackVolTermStructure> flatVolTS( std::make_shared< BlackConstantVol>(settlementDate, calendar, volatility, dayCounter));
                        std::shared_ptr<StrikedTypePayoff> payoff( std::make_shared<PlainVanillaPayoff>(typ, strike));
                        VanillaOption europeanOption(payoff, exercise);

                        std::shared_ptr<BlackScholesMertonProcess> bsmProcess( std::make_shared<BlackScholesMertonProcess>(underlyingH, flatDividendTS,
                                                flatTermStructure, flatVolTS));
                        //europeanOption.setPricingEngine(std::make_shared<  BinomialVanillaEngine<JarrowRudd> >(bsmProcess, timeSteps));
                        europeanOption.setPricingEngine(std::make_shared<  AnalyticEuropeanEngine >(bsmProcess));
                        if (typ == Option::Type::Put)
                            strp.addPutPrice(europeanOption.NPV());
                        else
                            strp.addCallPrice(europeanOption.NPV());
                    }
                    std::cerr << "Priced Option " << strp << std::endl;

                }
            }
            catch(const std::exception& e)
            {
                std::cerr << "Exception caught " << e.what() << std::endl;
            }
            
        }

        void OptionPricer::showPrices(){
            
        }
    }
}