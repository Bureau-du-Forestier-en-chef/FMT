
#include "FMTDefaultLogger.h"
#include "FMTFreeExceptionHandler.h"
#include "FMTVersion.h"
#include "FMTParser.h"
#include <iostream>
#include "FMTConstants.h"



namespace Testing
{

	class UnitTestFMTParser
		{
		public:
			UnitTestFMTParser() : m_Parser()
				{
				
				}
			void testStringToConstants()
				{
					Core::FMTConstants constants;
					constants.set("TEST1", std::vector<double>(1,1.0));
					constants.set("TEST2", std::vector<double>(1, 2.0));
					const std::string TEST_STRING("test of #TEST1 is not so valid #TEST2");
					const std::string VALID_STRING("test of 1 is not so valid 2");
					const std::string TEST_RESULT = m_Parser._processConstants(TEST_STRING, constants);
					if (TEST_RESULT != VALID_STRING)
						{
						Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, 
								"Non valid constant conversion",
							"UnitTestFMTexcelcache::testStringToConstants", __LINE__, __FILE__);
						}
					std::cout << "UnitTestFMTParser::testStringToConstants passed" << std::endl;
				}
			void testQuotedConstantsAreNotConverted()
				{
					Core::FMTConstants constants;
					constants.set("TEST1", std::vector<double>(1, 1.0));
					constants.set("TEST2", std::vector<double>(1, 2.0));
					// A constant wrapped in single or double quotes must be left untouched,
					// while an unquoted one is still converted.
					const std::string TEST_STRING("keep '#TEST1' and \"#TEST2\" but convert #TEST1");
					const std::string VALID_STRING("keep '#TEST1' and \"#TEST2\" but convert 1");
					const std::string TEST_RESULT = m_Parser._processConstants(TEST_STRING, constants);
					if (TEST_RESULT != VALID_STRING)
						{
						Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,
								"Quoted constant should not be converted",
							"UnitTestFMTParser::testQuotedConstantsAreNotConverted", __LINE__, __FILE__);
						}
					std::cout << "UnitTestFMTParser::testQuotedConstantsAreNotConverted passed" << std::endl;
				}
			void testConstantsWithPunctuationDelimiters()
				{
					Core::FMTConstants constants;
					constants.set("TEST1", std::vector<double>(1, 1.0));
					constants.set("TEST2", std::vector<double>(1, 2.0));
					const std::string TEST_STRING(",#TEST1, #TEST2) (#TEST1)");
					const std::string VALID_STRING(",1, 2) (1)");
					const std::string TEST_RESULT = m_Parser._processConstants(TEST_STRING, constants);
					if (TEST_RESULT != VALID_STRING)
						{
						Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,
								"Constant delimited by punctuation should be converted",
							"UnitTestFMTParser::testConstantsWithPunctuationDelimiters", __LINE__, __FILE__);
						}
					std::cout << "UnitTestFMTParser::testConstantsWithPunctuationDelimiters passed" << std::endl;
				}
		private:
			Parser::FMTParser m_Parser;

		};

}



int main()
{
	Logging::FMTDefaultLogger().logStamp();
	Testing::UnitTestFMTParser test;
	test.testStringToConstants();
	test.testQuotedConstantsAreNotConverted();
	test.testConstantsWithPunctuationDelimiters();
	return 0;
}

