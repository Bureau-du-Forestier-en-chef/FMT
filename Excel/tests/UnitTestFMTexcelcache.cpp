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
	#endif 
	return 0;
}

