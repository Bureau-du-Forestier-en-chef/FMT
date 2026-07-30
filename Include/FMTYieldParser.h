/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTyieldparser_Hm_included
#define FMTyieldparser_Hm_included

#include "FMTParser.h"
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
@brief Parser reading and writing yield sections into a FMTYields object.
*/
class FMTEXPORT FMTYieldParser : public FMTParser
    {
    public:
		// DocString: FMTYieldParser()
		/**
		@brief Default constructor for FMTYieldParser.
		*/
        FMTYieldParser();
		// DocString: ~FMTYieldParser()
		/**
		@brief Default destructor for FMTYieldParser.
		*/
		~FMTYieldParser() = default;
		// DocString: FMTYieldParser(const FMTYieldParser&)
		/**
		@brief Copy constructor for FMTYieldParser.
		@param[in] rhs the FMTYieldParser to copy.
		*/
        FMTYieldParser(const FMTYieldParser& rhs)=default;
		// DocString: FMTYieldParser::FMTYieldParser=
		/**
		@brief Copy assignment operator for FMTYieldParser.
		@param[in] rhs the FMTYieldParser to copy.
		@return a reference to this FMTYieldParser.
		*/
        FMTYieldParser& operator = (const FMTYieldParser& rhs)=default;
		// DocString: FMTYieldParser::read
		/**
		@brief Read a yield file into a FMTYields object.
		@param[in] themes the themes.
		@param[in] constants the constants.
		@param[in] location the file location.
		@return the yields.
		*/
        Core::FMTYields read(const std::vector<Core::FMTTheme>& themes,const Core::FMTConstants& constants,const std::string& location);
		// DocString: FMTYieldParser::write
		/**
		@brief Write a FMTYields object to a file.
		@param[in] yields the yields.
		@param[in] location the file location.
		*/
        void write(const Core::FMTYields& yields,const std::string& location) const;
     private:
		// DocString: FMTYieldParser::m_rxyieldtype
		///Regex to capture the yield type.
		const static boost::regex m_rxyieldtype;
		// DocString: FMTYieldParser::m_rxcomplex
		///Regex to capture the complex yields.
		const static boost::regex m_rxcomplex;
		// DocString: FMTYieldParser::m_rxeqs
		///Regex to capture complex yields equations.
		const static boost::regex m_rxeqs;
		// DocString: FMTYieldParser::m_rxdiscount
		///regex to capturediscount factors.
		const static boost::regex m_rxdiscount;
		// DocString: FMTYieldParser::m_rxpredictor
		///regex to capture _pred keyword.
		const static boost::regex m_rxpredictor;
		///regex to capture two keyword for a solo yield declaration.
		const static boost::regex m_rxyieldsolo;
		// DocString: FMTYieldParser::_getYldType
		/**
		@brief Return the yield type from a string.
		@param[in] value the yield type string.
		@return the yield type.
		*/
		Core::FMTyldtype _getYldType(const std::string& value) const;
		// DocString: FMTYieldParser::_getHandler
		/**
		@brief Return the yield handler for a mask and a yield type.
		@param[in] mask the mask.
		@param[in] yldtype the yield type.
		@return the yield handler.
		*/
		std::unique_ptr<Core::FMTYieldHandler> _getHandler(const Core::FMTMask& mask, const Core::FMTyldtype& yldtype) const;
		// DocString: FMTYieldParser::_getYldCtype
		/**
		@brief Return the yield parser operator from a string.
		@param[in] value the operator string.
		@return the yield parser operator.
		*/
		Core::FMTyieldparserop _getYldCtype(const std::string& value) const;
		// DocString: FMTYieldParser::_getYldUse
		/**
		@brief Return the yield names already defined in the yield data based on values.
		@param[in] yielddata the yield data.
		@param[in] actualyield the current yield iterator.
		@param[in] values the values.
		@return the used yield names.
		*/
		std::vector<std::string> _getYldUse(Core::FMTYields& yielddata,
			std::vector<std::pair<Core::FMTMask,std::unique_ptr<Core::FMTYieldHandler>>>::iterator actualyield,
                                   const std::vector<std::string>& values) const;
		// DocString: FMTYieldParser::_setOveridedYlds
		/**
		@brief Modify the yield data based on the current yield handler.
		@param[in,out] yielddata the yield data.
		@param[in] actualyield the current yield iterator.
		@param[in] yld the yield.
		*/
		void _setOveridedYlds(Core::FMTYields& yielddata,
			std::vector<std::pair<Core::FMTMask, std::unique_ptr<Core::FMTYieldHandler>>>::iterator actualyield,
			const std::string& yld) const;
		// DocString: FMTYieldParser::_checkPreExisting
		/**
		@brief Check preexisting yields and throw a warning if one already exists.
		@param[in] preexists the preexisting yields.
		*/
        void _checkPreExisting(const std::vector<std::string>& preexists) const;
		// DocString: FMTYieldParser::_isFunction
		/**
		@brief Return true if the string is a complex yield function.
		@param[in] strfunction the string.
		@return true if the string is a function else false.
		*/
		bool _isFunction(const std::string& strfunction) const;
		// DocString: FMTYieldParser::_getNumwithproportion
		/**
		@brief Return the value of a string based on the constants, temporal proportions and the period.
		@param[in] value the value string.
		@param[in] constants the constants.
		@param[in] proportions the temporal proportions.
		@param[in] location the period.
		@return the value.
		*/
		double _getNumwithproportion(const std::string& value,const Core::FMTConstants& constants,
			const std::vector<double>& proportions, const int& location);
		// DocString: FMTYieldParser::_getEq
		/**
		@brief Generate the FMTData of a complex yield.
		@param[in] basestr the base string.
		@param[in] constants the constants.
		@param[in] ylds the yields.
		@param[in] themes the themes.
		@return the yield data.
		*/
		Core::FMTData _getEq(const std::string& basestr,
                const Core::FMTConstants& constants,
                const Core::FMTYields& ylds,
                const std::vector<Core::FMTTheme>& themes);
		// DocString: FMTYieldParser::_cleanUp
		/**
		@brief Clean up the yields from indexes by replacing the values present in the themes.
		@param[in,out] yields the yields.
		@param[in] themes the themes.
		@param[in] constants the constants.
		*/
		void _cleanUp(Core::FMTYields& yields, const std::vector<Core::FMTTheme>& themes, const Core::FMTConstants& constants) const;
		// DocString: FMTYieldParser::_getIndexValues
		/**
		@brief Return the theme index values for a mask and the index values looked for.
		@param[in] mask the mask.
		@param[in] themes the themes.
		@param[in] indexvalues the index values.
		@param[in] constants the constants.
		@return a map of index name to value.
		*/
		std::map<std::string, double> _getIndexValues(const Core::FMTMask& mask,
			const std::vector<Core::FMTTheme>& themes,
			const std::vector<std::string>&indexvalues, const Core::FMTConstants& constants) const;
		// DocString: FMTYieldParser::_readYieldModel
		/**
		@brief Read a yield model from the YieldPredModels folder for a given model name.
		@param[in] modelname the model name.
		@param[in,out] modelYields the model yields.
		@param[in] mainmask the main mask.
		@return the yield model.
		*/
		std::unique_ptr<Core::FMTYieldModel> _readYieldModel(const std::string& modelname, std::vector<std::string>& modelYields,const Core::FMTMask& mainmask) const;
		// DocString: FMTYieldParser::_cleanAll
		/**
		@brief Update and clean the yields.
		@param[in,out] ylds the yields.
		@param[in] themes the themes.
		@param[in] constants the constants.
		*/
		void _cleanAll(Core::FMTYields& ylds, const std::vector<Core::FMTTheme>& themes, const Core::FMTConstants& constants) const;
    };
}
#endif // FMTyieldparser_Hm_included
