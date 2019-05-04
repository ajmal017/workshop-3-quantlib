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
#include <ql/exercise.hpp>


#include <iostream>

namespace wellsfargo {
    namespace workshop {

        using namespace QuantLib;
        
        namespace {
            struct StrikeOptionType {
                Option::Type    m_type;
                double          m_strike;

                StrikeOptionType( const Option::Type& type, double strike)
                : m_type(type),
                m_strike(strike) {}

                int operator<(const StrikeOptionType& rhs) const 
                {
                    return (m_strike == rhs.m_strike) ? (m_type < rhs.m_type) 
                        : (m_strike < rhs.m_strike);
                }
            };
            std::map< StrikeOptionType, VanillaOption > g_options;

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

                std::shared_ptr<Exercise> americanExercise = std::make_shared<AmericanExercise>(settlementDate, maturity);
                Spread dividendYield = 0.00;
                Rate riskFreeRate = 2.41;// The 3 month treasury Bill Rate;
                Size timeSteps = 1000;

                Handle<YieldTermStructure> flatTermStructure( std::make_shared<FlatForward>(settlementDate, riskFreeRate, dayCounter));
                Handle<YieldTermStructure> flatDividendTS( std::make_shared< FlatForward>(settlementDate, dividendYield, dayCounter));

                for( auto strp : m_strikes )
                {
                    for (auto typ : g_types)
                    {
                        Real underlying = event.price();
                        Handle<Quote> underlyingH(std::make_shared<SimpleQuote>(underlying));
                        Real strike = strp.strikePrice();

                        Volatility volatility = event.volatility();
                        Handle<BlackVolTermStructure> flatVolTS( std::make_shared< BlackConstantVol>(settlementDate, calendar, volatility, dayCounter));
                        {
                            StrikeOptionType optst( typ, strike);
                            auto optfound = g_options.find(optst);
                            if( optfound != g_options.end()) {
                                VanillaOption& americanOption = optfound->second;
                                if (typ == Option::Type::Put)
                                    strp.addPutPrice(americanOption.NPV());
                                else
                                    strp.addCallPrice(americanOption.NPV());
                            }
                            else {
                                std::shared_ptr<StrikedTypePayoff> payoff( std::make_shared<PlainVanillaPayoff>(typ, strike));
                                VanillaOption americanOption(payoff, americanExercise);

                                std::shared_ptr<BlackScholesMertonProcess> bsmProcess( std::make_shared<BlackScholesMertonProcess>(underlyingH, flatDividendTS,
                                                        flatTermStructure, flatVolTS));
                                americanOption.setPricingEngine(std::make_shared< BinomialVanillaEngine<JarrowRudd> >(bsmProcess,timeSteps));
                                if (typ == Option::Type::Put)
                                    strp.addPutPrice(americanOption.NPV());
                                else
                                    strp.addCallPrice(americanOption.NPV());
                                g_options.insert(std::make_pair( optst, americanOption));
                            }
                        }
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