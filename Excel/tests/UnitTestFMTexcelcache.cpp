#ifdef FMTWITHMOSEK
//#include "FMTexcelcache.h"
#include "FMTdefaultlogger.h"
#include "FMTfreeexceptionhandler.h"
#include "FMTversion.h"
#include <msclr\marshal_cppstd.h>
#include <iostream>



namespace Testing
{

	ref class UnitTestFMTexcelcache
		{
		public:
			UnitTestFMTexcelcache() : m_cache()
				{
				m_cache = gcnew Wrapper::FMTexcelcache();
				if (m_cache->size() != 0)
					{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Non empty selection mask",
						"UnitTestFMTexcelcache::testDefaultConstructor", __LINE__, __FILE__);

					}
				std::cout << "UnitTestFMTexcelcache::testDefaultConstructor passed" << std::endl;
				}
			void testAddModel()
			{
				System::String^ MODEL = gcnew System::String("../../../../Examples/Models/TWD_land/TWD_land.pri");
				System::String^ SCENARIO = gcnew System::String("LP");
				m_cache->add(MODEL, SCENARIO);
				if (m_cache->size() != 1)
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Non empty selection mask",
						"UnitTestFMTexcelcache::testAddModel", __LINE__, __FILE__);

				}
				std::cout << "UnitTestFMTexcelcache::testAddModel passed" << std::endl;
			}
			void testModelGetValue()
			{
				System::String^ MODEL = gcnew System::String("C:/Users/Admlocal/Documents/issues/280/02_Travail/PC_9943_U08651_2028_MODB01.pri");
				System::String^ SCENARIO = gcnew System::String("120_RegProv_apsp");
				System::String^ THEMENAME = gcnew System::String("THEME14=S00010");
				System::String^ OUTPUT = gcnew System::String("OSUPREGECOCOS");

				m_cache->add(MODEL, SCENARIO);
				std::vector<int> periods = {0, 8, 1, 9, 2, 10};
				for (int period: periods)
				{
					double OUTPUTVALUE = m_cache->getvalue(MODEL, SCENARIO, OUTPUT, THEMENAME, period);
					std::cout << OUTPUTVALUE << std::endl;
				}
			}

		void testRemoveModel()
			{
				System::String^ MODEL = gcnew System::String("../../../../Examples/Models/TWD_land/TWD_land.pri");
				System::String^ SCENARIO = gcnew System::String("LP");
				m_cache->remove(MODEL, SCENARIO);
				if (m_cache->size() != 0)
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Non empty selection mask",
						"UnitTestFMTexcelcache::testRemoveModel", __LINE__, __FILE__);

				}
				std::cout << "UnitTestFMTexcelcache::testRemoveModel passed" << std::endl;
			}
		private:
			Wrapper::FMTexcelcache^ m_cache;

		};

}

#endif


int main()
{
	#ifdef FMTWITHMOSEK
	Logging::FMTdefaultlogger().logstamp();
	Testing::UnitTestFMTexcelcache test;
	test.testAddModel();
	test.testRemoveModel();
	test.testModelGetValue();
	#endif 
	return 0;
}

