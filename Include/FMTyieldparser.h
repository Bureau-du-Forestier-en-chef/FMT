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
	class FMTyields;
	class FMTconstants;
	class FMTtheme;
	class FMTdata;
	class FMTyieldmodel;
	class FMTyieldhandler;
	class FMTmask;
}


namespace Parser
{
// DocString: FMTyieldparser
/**
The yield parser can read and write a yield sections and transform the information into
an FMTyields object.
*/
class FMTEXPORT FMTyieldparser : public FMTparser
    {
     private:
		// DocString: FMTyieldparser::rxyieldtype
		///Regex to capture the yield type.
		const static boost::regex rxyieldtype;
		// DocString: FMTyieldparser::rxcomplex
		///Regex to capture the complex yields.
		const static boost::regex rxcomplex;
		// DocString: FMTyieldparser::rxeqs
		///Regex to capture complex yields equations.
		const static boost::regex rxeqs;
		// DocString: FMTyieldparser::rxdiscount
		///regex to capturediscount factors.
		const static boost::regex rxdiscount;
		// DocString: FMTyieldparser::rxpredictor
		///regex to capture _pred keyword.
		const static boost::regex rxpredictor;
		///regex to capture two keyword for a solo yield declaration.
		const static boost::regex rxyieldsolo;
		// DocString: FMTyieldparser::getYldType
		/**
		Returns the yield type based on the string of the yield type.
		*/
		Core::FMTyldtype getYldType(const std::string& value) const;
		// DocString: FMTyieldparser::getHandler
		/**
		Returns the yield handler based on the type.
		*/
		std::unique_ptr<Core::FMTyieldhandler> getHandler(const Core::FMTmask& mask, const Core::FMTyldtype& yldtype) const;
		// DocString: FMTyieldparser::getYldCtype
		/**
		Returns an operator based on a string value.
		*/
		Core::FMTyieldparserop getYldCtype(const std::string& value) const;
		// DocString: FMTyieldparser::getYldUse
		/**
		This function returns yield names already defined in the yielddata based on (values).
		*/
		std::vector<std::string> getYldUse(Core::FMTyields& yielddata,
			std::vector<std::pair<Core::FMTmask,std::unique_ptr<Core::FMTyieldhandler>>>::iterator actualyield,
                                   const std::vector<std::string>& values) const;
		// DocString: FMTyieldparser::setOveridedYlds
		/**
		modify the yiedldata based on the actualyield handler.
		*/
		void setOveridedYlds(Core::FMTyields& yielddata,
			std::vector<std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>>>::iterator actualyield,
			const std::string& yld) const;
		// DocString: FMTyieldparser::checkPreexisting
		/**
		This function checks precxisting yields and throw warning if it already exist.
		*/
        void checkPreexisting(const std::vector<std::string>& preexists) const;
		// DocString: FMTyieldparser::isFunction
		/**
		Return true if the string is considered a complex yield function.
		*/
		bool isFunction(const std::string& strfunction) const;
		// DocString: FMTyieldparser::getNumwithproportion
		/**
		Using a string value (double or constant), a constants section and a temporal based proportions, and the actual period,
		it will return the value has a double.
		*/
		double getNumwithproportion(const std::string& value,const Core::FMTconstants& constants,
			const std::vector<double>& proportions, const int& location);
		// DocString: FMTyieldparser::getEq
		/**
		The function is only used to generate FMTdata of complexe yield.
		*/
		Core::FMTdata getEq(const std::string& basestr,
                const Core::FMTconstants& constants,
                const Core::FMTyields& ylds,
                const std::vector<Core::FMTtheme>& themes);
		// DocString: FMTyieldparser::cleanUp
		/**
		Clean up yields from index by replacing values present in the themes.
		*/
		void cleanUp(Core::FMTyields& yields, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& constants) const;
		// DocString: FMTyieldparser::getIndexValues
		/**
		This functions returns the value of themes indexes based on a mask, and for the indexvalues we are looking for.
		*/
		std::map<std::string, double>getIndexValues(const Core::FMTmask& mask,
			const std::vector<Core::FMTtheme>& themes,
			const std::vector<std::string>&indexvalues, const Core::FMTconstants& constants) const;
		// DocString: FMTyieldparser::readYieldModel
		/**
		Given a modelname read a yield model in the YieldPredModels folder.
		*/
		std::unique_ptr<Core::FMTyieldmodel>readYieldModel(const std::string& modelname, std::vector<std::string>& modelYields,const Core::FMTmask& mainmask) const;
		// DocString: FMTyieldparser::cleanAll
		/**
		Update ans clean the yields
		*/
		void cleanAll(Core::FMTyields& ylds, const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& constants) const;
    public:
		// DocString: FMTyieldparser()
		/**
		Default constructor for FMTyieldparser
		*/
        FMTyieldparser();
		// DocString: ~FMTyieldparser()
		/**
		Default destructor for FMTyieldparser.
		*/
		~FMTyieldparser() = default;
		// DocString: FMTyieldparser(const FMTyieldparser&)
		/**
		Copy constructor for FMTyieldparser.
		*/
        FMTyieldparser(const FMTyieldparser& rhs)=default;
		// DocString: FMTyieldparser::FMTyieldparser=
		/**
		Copy assignment for FMTyieldparser.
		*/
        FMTyieldparser& operator = (const FMTyieldparser& rhs)=default;
		// DocString: FMTyieldparser::read
		/**
		The read function will turn a yield file into an FMTyields objectf based on (themes), (constants) and the location of the file.
		*/
        Core::FMTyields read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants,const std::string& location);
		// DocString: FMTyieldparser::write
		/**
		The write function will write a FMTyields object to a location.
		*/
        void write(const Core::FMTyields& yields,const std::string& location) const;
    };
}
#endif // FMTyieldparser_Hm_included
