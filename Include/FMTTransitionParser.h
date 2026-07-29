/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTtransitionparser_Hm_included
#define FMTtransitionparser_Hm_included

#include "FMTParser.h"
#include <boost/regex.hpp>
#include "string"
#include <vector>


namespace Core {
	class FMTMask;
	class FMTYields;
	class FMTTransition;
	class FMTAction;
	class FMTTheme;
	class FMTConstants;
	class FMTTransitionMask;
	class FMTGCBMTransition;
}

namespace Parser
{
// DocString: FMTTransitionParser
/**
@brief Parser reading and writing transition sections into a vector of FMTTransition.
*/
class FMTEXPORT FMTTransitionParser : public FMTParser
    {
    public:
		// DocString: FMTTransitionParser()
		/**
		@brief Default constructor for FMTTransitionParser.
		*/
        FMTTransitionParser();
		// DocString: ~FMTTransitionParser()
		/**
		@brief Default destructor for FMTTransitionParser.
		*/
		~FMTTransitionParser() = default;
		// DocString: FMTTransitionParser(const FMTTransitionParser&)
		/**
		@brief Copy constructor for FMTTransitionParser.
		@param[in] rhs the FMTTransitionParser to copy.
		*/
        FMTTransitionParser(const FMTTransitionParser& rhs)=default;
		// DocString: FMTTransitionParser::FMTTransitionParser=
		/**
		@brief Copy assignment operator for FMTTransitionParser.
		@param[in] rhs the FMTTransitionParser to copy.
		@return a reference to this FMTTransitionParser.
		*/
        FMTTransitionParser& operator = (const FMTTransitionParser& rhs)=default;
		// DocString: FMTTransitionParser::getSource
		/**
		@brief Return the mask of a transition and fill up its specification.
		@param[in,out] line the line.
		@param[in,out] spec the specification.
		@param[in] themes the themes.
		@param[in] section the section.
		@param[in] constant the constants.
		@param[in] ylds the yields.
		@return the transition source mask.
		*/
		Core::FMTMask getSource(std::string& line, Core::FMTSpec& spec,const std::vector<Core::FMTTheme>& themes,
							Core::FMTsection section,const Core::FMTConstants& constant,
                          const Core::FMTYields& ylds);
		// DocString: FMTTransitionParser::getMaskTran
		/**
		@brief Return the transition masks from a single transition line.
		@param[in] line the line.
		@param[in] themes the themes.
		@param[in] constants the constants.
		@param[in] ylds the yields.
		@param[in] sourcemask the source mask.
		@param[in,out] replaced the number of replaced attributes.
		@return the transition masks.
		*/
        std::vector<Core::FMTTransitionMask> getMaskTran(const std::string& line,const std::vector<Core::FMTTheme>& themes,
                                        const Core::FMTConstants& constants, const Core::FMTYields& ylds,
                                        const Core::FMTMask& sourcemask, int& replaced);
		// DocString: FMTTransitionParser::read
		/**
		@brief Read a vector of transitions.
		@param[in] themes the themes.
		@param[in] actions the actions.
		@param[in] ylds the yields.
		@param[in] constants the constants.
		@param[in] location the file location.
		@return the transitions.
		*/
        std::vector<Core::FMTTransition>read(const std::vector<Core::FMTTheme>& themes,
                           const std::vector<Core::FMTAction>& actions,
                           const Core::FMTYields& ylds,const Core::FMTConstants& constants,
						   const std::string& location);
		// DocString: FMTTransitionParser::write
		/**
		@brief Write a vector of transitions to a file.
		@param[in] transitions the transitions.
		@param[in] location the file location.
		*/
        void write(const std::vector<Core::FMTTransition>& transitions,const std::string& location) const;
		// DocString: FMTTransitionParser::writeGCBM
		/**
		@brief Write a vector of GCBM transitions to a text file.
		@param[in] transitions the GCBM transitions.
		@param[in] location the file location.
		*/
		void writeGCBM(const std::vector<Core::FMTGCBMTransition>& transitions, const std::string& location) const;
		// DocString: FMTTransitionParser::readGCBM
		/**
		@brief Read GCBM transitions from a text file.
		@param[in] location the file location.
		@return the GCBM transitions.
		*/
		std::vector<Core::FMTGCBMTransition>readGCBM(const std::string& location) const;
    private:
		// DocString: FMTTransitionParser::m_rxsection
		///Regex to capture the transition section.
		const static boost::regex m_rxsection;
		// DocString: FMTTransitionParser::m_rxlock
		///Regex to capture the lock level of the transition mask.
		const static boost::regex m_rxlock;
		// DocString: FMTTransitionParser::m_rxage
		///Regex to capture the age level of the transition mask.
		const static boost::regex m_rxage;
		// DocString: FMTTransitionParser::m_rxreplace
		///Regex to capture the age level of the transition mask.
		const static boost::regex m_rxreplace;
		// DocString: FMTTransitionParser::m_rxtyld
		///Regex to capture yield bounds of the transition mask.
		const static boost::regex m_rxtyld;
    };
}
#endif // FMTtransitionparser_Hm_included
