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
			void testModelGetValue()
			{
				System::String^ MODEL = gcnew System::String("../../../../Examples/Models/TWD_land/TWD_land.pri");
				System::String^ SCENARIO = gcnew System::String("LP");
				System::String^ THEMENAME = gcnew System::String("THEME1=UNITE1");
				System::String^ OUTPUT = gcnew System::String("OVOLREC");

				m_cache->add(MODEL, SCENARIO);
				std::vector<int> periods = {1, 8, 1, 9, 2, 10};
				for (int period: periods)
				{
					double OUTPUTVALUE = m_cache->getvalue(MODEL, SCENARIO, OUTPUT, THEMENAME, period);
					//std::cout << OUTPUTVALUE << std::endl;
				}
				m_cache->remove(MODEL, SCENARIO);
				std::cout << "UnitTestFMTexcelcache::testModelGetValue passed" << std::endl;
			}

		
		void testGetGraphStats()
			{
			System::String^ MODEL = gcnew System::String("../../../../Examples/Models/TWD_land/TWD_land.pri");
			System::String^ SCENARIO = gcnew System::String("LP");
			System::String^ THEME_NAME1 = gcnew System::String("THEME1=UNITE1");
			System::String^ THEME_NAME2 = gcnew System::String("THEME1=UNITE2");
			m_cache->add(MODEL, SCENARIO);
			System::Collections::Generic::List<int>^ GRAPH_STATS1 = m_cache->getGraphStatsSubset(MODEL, SCENARIO,THEME_NAME1);
			System::Collections::Generic::List<int>^ GRAPH_STATS2 = m_cache->getGraphStatsSubset(MODEL, SCENARIO,THEME_NAME2);
			if (GRAPH_STATS1->Count==0|| GRAPH_STATS2->Count == 0)
				{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Non empty selection mask",
					"UnitTestFMTexcelcache::testAddModel", __LINE__, __FILE__);

				}
			/*std::cout << "UNIT1" << "\n";
			for each (int^ STATS in GRAPH_STATS1)
				{
				std::cout << *STATS << "\n";
				}
			std::cout << "UNIT2" << "\n";
			for each (int^ STATS in GRAPH_STATS2)
				{
				std::cout << *STATS << "\n";
				}*/
			m_cache->remove(MODEL, SCENARIO);
			std::cout << "UnitTestFMTexcelcache::testGetGraphStats passed" << std::endl;
			}
		void testAddAndBuild()
		{
			System::String^ MODEL = gcnew System::String("../../../../Examples/Models/TWD_land/TWD_land.pri");
			System::String^ SCENARIO = gcnew System::String("LP");
			int LENGTH = 5;
			bool BUILDED = m_cache->addAndBuild(MODEL, SCENARIO, LENGTH);
			if (!BUILDED)
			{
				Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "non builded",
					"UnitTestFMTexcelcache::testAddAndBuild", __LINE__, __FILE__);

			}
			System::Collections::Generic::List<int>^ GRAPH_STATS = m_cache->getgraphstats(MODEL, SCENARIO);
			for each (int^ STATS in GRAPH_STATS)
			{
				if (*STATS == 0)
				{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Empty graph",
						"UnitTestFMTexcelcache::testAddAndBuild", __LINE__, __FILE__);
				}
			}
			m_cache->remove(MODEL, SCENARIO);
			std::cout << "UnitTestFMTexcelcache::testAddAndBuild passed" << std::endl;
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
	test.testGetGraphStats();
	test.testAddAndBuild();
	#endif 
	return 0;
}

