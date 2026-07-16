/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTyieldparser_Hm_included
#define FMTyieldparser_Hm_included

#include "FMTparser.h"
#include "FMTutility.h"
#include <iterator>
#include <boost/regex.h>
#include <string>
#include <vector>
#include <utility>
#include <memory>

namespace Core
{
	class FMTYields;
	class FMTConstants;
	class FMTTheme;
	class FMTData;
	class FMTYieldModel;
	class FMTYieldHandler;
	class FMTMask;
}


namespace Parser
{
// DocString: FMTYieldParser
/**
The yield parser can read and write a yield sections and transform the information into
an FMTYields object.
*/
class FMTEXPORT FMTYieldParser : public FMTParser
    {
     private:
		// DocString: FMTYieldParser::rxyieldtype
		///Regex to capture the yield type.
		const static boost::regex rxyieldtype;
		// DocString: FMTYieldParser::rxcomplex
		///Regex to capture the complex yields.
		const static boost::regex rxcomplex;
		// DocString: FMTYieldParser::rxeqs
		///Regex to capture complex yields equations.
		const static boost::regex rxeqs;
		// DocString: FMTYieldParser::rxdiscount
		///regex to capturediscount factors.
		const static boost::regex rxdiscount;
		// DocString: FMTYieldParser::rxpredictor
		///regex to capture _pred keyword.
		const static boost::regex rxpredictor;
		///regex to capture two keyword for a solo yield declaration.
		const static boost::regex rxyieldsolo;
		// DocString: FMTYieldParser::getYldType
		/**
		Returns the yield type based on the string of the yield type.
		*/
		Core::FMTyldtype getYldType(const std::string& value) const;
		// DocString: FMTYieldParser::getHandler
		/**
		Returns the yield handler based on the type.
		*/
		std::unique_ptr<Core::FMTYieldHandler> getHandler(const Core::FMTMask& mask, const Core::FMTyldtype& yldtype) const;
		// DocString: FMTYieldParser::getYldCtype
		/**
		Returns an operator based on a string value.
		*/
		Core::FMTyieldparserop getYldCtype(const std::string& value) const;
		// DocString: FMTYieldParser::getYldUse
		/**
		This function returns yield names already defined in the yielddata based on (values).
		*/
		std::vector<std::string> getYldUse(Core::FMTYields& yielddata,
			std::vector<std::pair<Core::FMTMask,std::unique_ptr<Core::FMTYieldHandler>>>::iterator actualyield,
                                   const std::vector<std::string>& values) const;
		// DocString: FMTYieldParser::setOveridedYlds
		/**
		modify the yiedldata based on the actualyield handler.
		*/
		void setOveridedYlds(Core::FMTYields& yielddata,
			std::vector<std::pair<Core::FMTMask, std::unique_ptr<Core::FMTYieldHandler>>>::iterator actualyield,
			const std::string& yld) const;
		// DocString: FMTYieldParser::checkPreexisting
		/**
		This function checks precxisting yields and throw warning if it already exist.
		*/
        void checkPreexisting(const std::vector<std::string>& preexists) const;
		// DocString: FMTYieldParser::isFunction
		/**
		Return true if the string is considered a complex yield function.
		*/
		bool isFunction(const std::string& strfunction) const;
		// DocString: FMTYieldParser::getNumwithproportion
		/**
		Using a string value (double or constant), a constants section and a temporal based proportions, and the actual period,
		it will return the value has a double.
		*/
		double getNumwithproportion(const std::string& value,const Core::FMTConstants& constants,
			const std::vector<double>& proportions, const int& location);
		// DocString: FMTYieldParser::getEq
		/**
		The function is only used to generate FMTData of complexe yield.
		*/
		Core::FMTData getEq(const std::string& basestr,
                const Core::FMTConstants& constants,
                const Core::FMTYields& ylds,
                const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTYieldParser::cleanUp
		/**
		Clean up yields from index by replacing values present in the themes.
		*/
		void cleanUp(Core::FMTYields& yields, const std::vector<Core::FMTTheme>& themes, const Core::FMTConstants& constants) const;
		// DocString: FMTYieldParser::getIndexValues
		/**
		This functions returns the value of themes indexes based on a mask, and for the indexvalues we are looking for.
		*/
		std::map<std::string, double>getIndexValues(const Core::FMTMask& mask,
			const std::vector<Core::FMTTheme>& themes,
			const std::vector<std::string>&indexvalues, const Core::FMTConstants& constants) const;
		// DocString: FMTYieldParser::readYieldModel
		/**
		Given a modelname read a yield model in the YieldPredModels folder.
		*/
		std::unique_ptr<Core::FMTYieldModel>readYieldModel(const std::string& modelname, std::vector<std::string>& modelYields,const Core::FMTMask& mainmask) const;
		// DocString: FMTYieldParser::cleanAll
		/**
		Update ans clean the yields
		*/
		void cleanAll(Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes, const Core::FMTConstants& constants) const;
    public:
		// DocString: FMTYieldParser()
		/**
		Default constructor for FMTYieldParser
		*/
        FMTYieldParser();
		// DocString: ~FMTYieldParser()
		/**
		Default destructor for FMTYieldParser.
		*/
		~FMTYieldParser() = default;
		// DocString: FMTYieldParser(const FMTYieldParser&)
		/**
		Copy constructor for FMTYieldParser.
		*/
        FMTYieldParser(const FMTYieldParser& rhs)=default;
		// DocString: FMTYieldParser::FMTYieldParser=
		/**
		Copy assignment for FMTYieldParser.
		*/
        FMTYieldParser& operator = (const FMTYieldParser& rhs)=default;
		// DocString: FMTYieldParser::read
		/**
		The read function will turn a yield file into an FMTYields objectf based on (themes), (constants) and the location of the file.
		*/
        Core::FMTYields read(const std::vector<Core::FMTTheme>& themes,const Core::FMTConstants& constants,const std::string& location);
		// DocString: FMTYieldParser::write
		/**
		The write function will write a FMTYields object to a location.
		*/
        void write(const Core::FMTYields& yields,const std::string& location) const;
    };
}
#endif // FMTyieldparser_Hm_included
