#ifdef FMTWITHMOSEK
//#include "FMTexcelcache.h"
#include "FMTdefaultlogger.h"
#include "FMTfreeexceptionhandler.h"
#include "FMTversion.h"
#include <msclr\marshal_cppstd.h>
#include <iostream>



namespace Testing
{

	ref class UnitTestFMToutputCache
		{
		public:
			UnitTestFMToutputCache() : m_cache()
				{
				m_cache = gcnew Wrapper::FMTexcelcache();
				if (m_cache->size() != 0)
					{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Non empty selection mask",
						"UnitTestFMToutputCache::testDefaultConstructor", __LINE__, __FILE__);

					}
				std::cout << "UnitTestFMToutputCache::testDefaultConstructor passed" << std::endl;
				}
			
			void testModelGetValues(std::string p_primary, std::string p_scenario, std::string p_test)
			{
				System::String^ MODEL = gcnew System::String(p_primary.c_str());
				System::String^ SCENARIO = gcnew System::String(p_scenario.c_str());

				m_cache->add(MODEL, SCENARIO);
				if (p_test == "testOutputCache")
				{
					System::String^ THEMENAME = gcnew System::String("THEME14=S00010");
					System::String^ OUTPUT = gcnew System::String("OSUPREGECOCOS");
					std::vector<int> periods = {0, 8, 1, 9, 2, 10};
					std::vector<int> values = {1038, 1038, 1038, 1036, 1038, 1036};
					size_t i = 0;
					for (int period: periods)
					{
						const int OUTPUTVALUE = static_cast<int>(m_cache->getvalue(MODEL, SCENARIO, OUTPUT, THEMENAME, period));
						if (OUTPUTVALUE != values[i])
						{
							std::cout << OUTPUTVALUE << " != " << values[i] << std::endl;
							Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, 
								"Bad values returned at testOutputCache",
								"UnitTestFMToutputCache::testModelGetValues", __LINE__, __FILE__);
						}
						++i;
					}	
				}
				std::cout << "UnitTestFMToutputCache::testModelGetValues passed" << std::endl;
			}

		private:
			Wrapper::FMTexcelcache^ m_cache;

		};

}

#endif


int main(int argc, char *argv[])
{
	#ifdef FMTWITHMOSEK
	std::string PRIMARY;
	std::string SCENARIO;
	std::string TEST;
	if (argc>1)
		{
			PRIMARY = argv[1];
			SCENARIO = argv[2];
			TEST = argv[3];
		}else {
			PRIMARY = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/08251/PC_9943_U08651_2028_MODB01.pri";
			SCENARIO = "120_RegProv_apsp";
			TEST = "testOutputCache";
		}
	Logging::FMTdefaultlogger().logstamp();
	Testing::UnitTestFMToutputCache test;
	test.testModelGetValues(PRIMARY, SCENARIO, TEST);
	#endif 
	return 0;
}

