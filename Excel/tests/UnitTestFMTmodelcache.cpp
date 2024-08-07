#include <vector>
#include <string>

#ifdef FMTWITHMOSEK
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTfreeexceptionhandler.h"
#include "FMTmodelcache.h"
#include "FMtmask.h"

namespace Testing
{
	class UnitTestFMTmodelcache
		{
		public:
			UnitTestFMTmodelcache() : m_cache()
				{
				Parser::FMTmodelparser modelParser;
				const std::string PRIMARY_LOCATION = "../../../../Examples/Models/TWD_land/TWD_land.pri";
				const std::vector<std::string>SCENARIOS(1, "LP");
				const std::vector<Models::FMTmodel> MODELS = modelParser.readproject(PRIMARY_LOCATION, SCENARIOS);
				m_cache = Wrapper::FMTmodelcache(MODELS.at(0), "");
				}
			void testThemeSelectionToMask()
			{
				const std::string EMPTY_FILTER = "";
				const Core::FMTmask EMPTY_MASK = m_cache.themeSelectionToMask(EMPTY_FILTER);
				if (EMPTY_MASK.size()!=0)
					{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Non empty selection mask",
						"UnitTestFMTmodelcache::testThemeSelectionToMask", __LINE__, __FILE__);
					}
				const std::string FIRST_FILTER = "THEME1=UNITE1";
				const Core::FMTmask FIRST_MASK = m_cache.themeSelectionToMask(FIRST_FILTER);
				if ((FIRST_MASK.size()-FIRST_MASK.count())!=2)
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Selection mask",
						"UnitTestFMTmodelcache::testThemeSelectionToMask", __LINE__, __FILE__);
				}
				if (std::string(FIRST_MASK) != "UNITE1 ? ?")
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Selection mask",
						"UnitTestFMTmodelcache::testThemeSelectionToMask", __LINE__, __FILE__);
				}
				const std::string COMPOSED_FILTER = "THEME1=UNITE1;THEME2=PEUPLEMENT1";
				const Core::FMTmask  COMPOSED_MASK = m_cache.themeSelectionToMask(COMPOSED_FILTER);
				if ((COMPOSED_MASK.size() - COMPOSED_MASK.count()) != 5)
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Selection mask",
						"UnitTestFMTmodelcache::testThemeSelectionToMask", __LINE__, __FILE__);
				}
				if (std::string(COMPOSED_MASK) != "UNITE1 PEUPLEMENT1 ?")
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Selection mask",
						"UnitTestFMTmodelcache::testThemeSelectionToMask", __LINE__, __FILE__);
				}
				const std::string MULTIPLE_FILTER = "THEME1={UNITE1,UNITE2}";
				const Core::FMTmask MULTIPLE_MASK = m_cache.themeSelectionToMask(MULTIPLE_FILTER);
				if ((MULTIPLE_MASK.size() - MULTIPLE_MASK.count()) != 1)
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Selection mask",
						"UnitTestFMTmodelcache::testThemeSelectionToMask", __LINE__, __FILE__);
				}
				
				if (std::string(MULTIPLE_MASK) != "UNITE1,UNITE2 ? ?")
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Selection mask",
						"UnitTestFMTmodelcache::testThemeSelectionToMask", __LINE__, __FILE__);
				}
				const std::string MULTIPLE_FILTER2 = "THEME1={UNITE1,UNITE2};THEME2={PEUPLEMENT1,PEUPLEMENT2}";
				const Core::FMTmask MULTIPLE2_MASK = m_cache.themeSelectionToMask(MULTIPLE_FILTER2);
				if ((MULTIPLE2_MASK.size() - MULTIPLE2_MASK.count()) != 3)
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Selection mask",
						"UnitTestFMTmodelcache::testThemeSelectionToMask", __LINE__, __FILE__);
				}
				if (std::string(MULTIPLE2_MASK) != "UNITE1,UNITE2 PEUPLEMENT1,PEUPLEMENT2 ?")
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Selection mask",
						"UnitTestFMTmodelcache::testThemeSelectionToMask", __LINE__, __FILE__);
				}
			}
		private:
			Wrapper::FMTmodelcache m_cache;

		};

}

#endif



int main()
{
	#ifdef FMTWITHMOSEK
		Logging::FMTdefaultlogger().logstamp();
		Testing::UnitTestFMTmodelcache test;
		test.testThemeSelectionToMask();
	#endif 
	return 0;
}

