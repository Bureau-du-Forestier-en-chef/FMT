#include <vector>
#include "FMTareaparser.h"
#include "FMTlandscapeparser.h"
#include "FMTconstants.h"
#include "FMTtheme.h"
#include "FMTversion.h"
#include "FMTutility.h"
#include "FMTdefaultexceptionhandler.h"
#include "FMTdefaultlogger.h"
#include "FMTactualdevelopment.h"
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

int main(int argc, char* argv[])
{
	Logging::FMTdefaultlogger().logstamp();
	if (Version::FMTversion().hasfeature("GDAL"))
	{
		std::string path;
		if (argc > 1) {
			path = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/DatabaseParserTests/";// argv[1];
		}
		else {
			path = "C:\\Users\\Admlocal\\Documents\\issues\\C2_01017018\\Pour_Bruno";
		}
		const boost::filesystem::path FOLDER_PATH(path);
		Core::FMTconstants baseConstants;
		std::vector<Core::FMTtheme>baseThemes;
		Parser::FMTareaparser AreaParser;
		bool testedArea = false;
		bool testedLan = false;
		while (!(testedArea && testedLan))
		{
			for (const auto& ENTRY : boost::make_iterator_range(boost::filesystem::directory_iterator(FOLDER_PATH), {}))
			{
				const std::string EXTENSION = ENTRY.path().extension().string();
				const Core::FMTsection SECTION = AreaParser.fromExtension(EXTENSION);
				const std::string PATH_TO_FILE = ENTRY.path().string();
				try {
					if (!testedLan && SECTION == Core::FMTsection::Landscape)
					{
						Parser::FMTlandscapeparser LandscapeParser;
						baseThemes = LandscapeParser.read(baseConstants, PATH_TO_FILE);
						testedLan = true;
						if (baseThemes.empty())
						{
							AreaParser.getExceptionHandler()->raise(Exception::FMTexc::FMTfunctionfailed, "Landscape parser failed to read " + PATH_TO_FILE,
								"ParsersTests", __LINE__, __FILE__);
						}
					}
					if (SECTION == Core::FMTsection::Area && testedLan)
					{
						const std::vector<Core::FMTactualdevelopment>DEVELOPMENTS = AreaParser.read(baseThemes, baseConstants, PATH_TO_FILE);
						if (DEVELOPMENTS.empty())
						{
							AreaParser.getExceptionHandler()->raise(Exception::FMTexc::FMTfunctionfailed, "Area parser failed to read " + PATH_TO_FILE,
								"ParsersTests", __LINE__, __FILE__);
						}
						testedArea = true;
					}
				}
				catch (...)
				{
					AreaParser.getExceptionHandler()->printexceptions("ParserTests Failed", "", __LINE__, __FILE__);
				}

			}
		}
		
	}
	return 0;
}

