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
			}

		private:
			Wrapper::FMTexcelcache^ m_cache;

		};

}

#endif


int main(int argc, char *argv[])
{
	#ifdef FMTWITHMOSEK
	const std::string PRIMARY = argv[1];
	const std::string SCENARIO = argv[2];
	const std::string TEST = argv[3];
	Logging::FMTdefaultlogger().logstamp();
	Testing::UnitTestFMToutputCache test;
	test.testModelGetValues(PRIMARY, SCENARIO, TEST);
	#endif 
	return 0;
}

