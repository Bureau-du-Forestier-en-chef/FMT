/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
	


#ifndef FMTEXCEPTION_Hm_included
#define FMTEXCEPTION_Hm_included

#include <exception>
#include <string>
#include <iostream>
#include <memory>
#include "FMTutility.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/string.hpp>



/// Namespace for exceptions and exceptions handling (warnings and errors) thrown by FMT and all exceptions handlers available.
namespace Exception
{
// DocString: FMTlev
/**
@brief Enumerator describing the kind of exception thrown by FMT.
*/
enum FMTlev
    {
    FMT_None=0,
    FMT_Debug=1,
    FMT_Warning=2,
    FMT_logic=3,
    FMT_range=4
    };

// DocString: FMTexc
/**
@brief Enumerator describing the exceptions thrown by FMT.
*/
enum FMTexc
{
	None = 0,
	FMTconstants_replacement = 1,
	FMTcomma_replacement = 2,
	FMTfutur_types = 3,
	FMTinvalid_theme = 4,
	FMTinvalid_aggregate = 5,
	FMTinvalid_maskrange = 6,
	FMTinvalid_yield = 7,
	FMTinvalid_action = 8,
	FMTinvalid_transition = 9,
	FMTinvalid_transition_case = 10,
	FMTinvalid_number = 11,
	FMTinvalid_path = 12,
	FMTtheme_redefinition = 13,
	FMTaggregate_redefinition = 14,
	FMTempty_theme = 15,
	FMTempty_aggregate = 16,
	FMTundefined_aggregate_value = 17,
	FMTundefined_attribute = 18,
	FMTempty_action = 19,
	FMTwrong_partial = 20,
	FMTpreexisting_yield = 21,
	FMTunsupported_yield = 22,
	FMTinvaliddataset = 23,
	FMTinvalidband = 24,
	FMTinvalidrasterblock = 25,
	FMTinvalidlayer = 26,
	FMTmissingfield = 27,
	FMTinvalidoverview = 28,
	FMTmissingrasterattribute = 29,
	FMTunsupported_transition = 30,
	FMTundefined_action = 31,
	FMTempty_transition = 32,
	FMTundefined_output = 33,
	FMTunsupported_output = 34,
	FMTinvaliddriver = 35,
	FMTinvalidAandT = 36,
	FMTleakingtransition = 37,
	FMTundefineddeathaction = 38,
	FMTundefineddeathtransition = 39,
	FMTignore = 40,
	FMTmissingyield = 41,
	FMTattribute_redefinition = 42,
	FMTundefined_constant = 43,
	FMTmissingdevelopment = 44,
	FMTmissingobjective = 45,
	FMTunsupported_objective = 46,
	FMTinvalid_constraint = 47,
	FMTemptybound = 48,
	FMTunboundedperiod = 49,
	FMTnonaddedconstraint = 50,
	FMTmissinglicense = 51,
	FMTfunctionfailed = 52,
	FMTcoinerror = 53,
	FMTboostgrapherror = 54,
	FMTunhandlederror = 55,
	FMTnotlinegraph = 56,
	FMTrangeerror = 57,
	FMTGDALerror = 58,
	FMTGDALwarning = 59,
	FMTinfeasibleconstraint = 60,
	FMTthematic_output_diff = 61,
	FMToutput_missing_operator = 62,
	FMToutput_too_much_operator = 63,
	FMTinvalidyield_number = 64,
	FMTgdal_constructor_error = 65,
	FMTinvalid_geometry = 66,
	FMTundefinedoutput_attribute = 67,
	FMTempty_schedules = 68,
	FMTmissing_scenarios = 69,
	FMTschemefailed = 70,
	FMTmskerror = 71,
	FMToveridedyield = 72,
	FMTmissing_parameter = 73,
	FMTdeathwithlock = 74,
	FMTreplanningwarning = 75,
	FMTyieldmodelprediction = 76,
	FMTsourcetotarget_transition = 77,
	FMTsame_transitiontargets = 78,
	FMTunclosedforloop = 79,
	FMTthreadcrash = 80,
	FMToutofrangeyield = 81,
	FMTEmpty_OA = 82,
	FMTWIN32_Error = 83,
	FMTMSKnumerical_problem = 84,
	FMTunreachable_threshold = 85,
	FMTmaxnumberofexception = 86
};

// DocString: FMTException
/**
@brief Base exception class for FMT holding the type, section and message of a given exception.
*/
class FMTEXPORT FMTException : public std::exception
    {
    public:
		// DocString: FMTException()
		/**
		@brief Default constructor for FMTException.
		*/
		FMTException();
		// DocString: ~FMTException()
		/**
		@brief Default virtual destructor for FMTException.
		*/
		virtual ~FMTException() = default;
		// DocString: FMTException(FMTexc,FMTlev,Core::FMTsection,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const std::string&)
		/**
		@brief Construct a FMTException from an exception type, a section, a message and the location where it occurred.
		@param[in] p_exception the exception type.
		@param[in] p_level level of the exception type.
		@param[in] p_section the section in which the exception occurred.
		@param[in] p_message the message of the exception.
		@param[in] p_method the method where the exception occurred.
		@param[in] p_file the file where the exception occurred.
		@param[in] p_line the line where the exception occurred.
		@param[in] p_FrenchDescription french description.
		@param[in] p_EnglishDescription english description.
		*/
		FMTException(FMTexc p_exception, FMTlev p_level,Core::FMTsection p_section,const std::string& p_message,
			const std::string& p_method, const std::string& p_file, const int& p_line,
			const std::string& p_FrenchDescription, const std::string& p_EnglishDescription);
		// DocString: FMTException(const FMTException&)
		/**
		@brief Default copy constructor for FMTException.
		@param[in] rhs the FMTException to copy.
		*/
		FMTException(const FMTException& rhs)=default;
		// DocString: FMTException::operator=
		/**
		@brief Default copy assignment operator for FMTException.
		@param[in] rhs the FMTException to copy.
		@return a reference to this FMTException.
		*/
		FMTException& operator = (const FMTException& rhs)=default;
		// DocString: FMTException::getType
		/**
		@brief Return the type of exception held by this FMTException.
		@return the exception type.
		*/
		FMTexc getType() const;
		// DocString: FMTException::getSection
		/**
		@brief Return the section in which the exception occurred.
		@return the section.
		*/
		Core::FMTsection getSection() const;
		// DocString: FMTException::what
		/**
		@brief Override the what function of the base exception class returning the message string.
		@return the message string.
		*/
		const char* what() const noexcept override;
		// DocString: FMTException::hold
		/**
		@brief Return the value of the holdup member.
		@return the holdup value.
		*/
		bool hold() const;
		// DocString: FMTException::setHold
		/**
		@brief Set the value of the holdup member.
		@param[in] side the value to set.
		*/
		void setHold(bool side);
		// DocString: FMTException::getMethod
		/**
		@brief Return the method in which the exception occurred.
		@return the method.
		*/
		inline std::string getMethod() const
			{
			return m_method;
			}
		// DocString: FMTException::getFile
		/**
		@brief Return the source file in which the exception occurred.
		@return the source file.
		*/
		inline std::string getFile() const
			{
			return m_file;
			}
		// DocString: FMTException::getLine
		/**
		@brief Return the source file line in which the exception occurred.
		@return the source file line.
		*/
		inline int getLine() const
			{
			return m_line;
			}
		// DocString: FMTException::getDescription(bool)
		/**
		@brief Return the description of the exception thrown
		@param[in] p_french if true will be in french
		@return the description
		*/
		const std::string& getDescription(bool p_french=false) const;
		// DocString: FMTException::getSrcInfo
		/**
		@brief Return a formatted string for the location of the exception.
		@return the formatted source location.
		*/
		std::string getSrcInfo() const;
		// DocString: FMTException::getMessage
		/**
		@brief Return the message of the exception
		@return the message string
		*/
		const std::string& getMessage() const;
		// DocString: FMTException::getLevel
		/**
		@brief Return the level of the exception
		@return the level
		*/
		FMTlev getLevel() const;
		// DocString: FMTException::isFatal
		/**
		@brief return true if it is fatal
		@return true or false
		*/
		bool isFatal() const;
		// DocString: FMTException::setIgnore
		/**
		@brief set a ignore message to the exception and turn the level to warning
		*/
		void setIgnore();
		// DocString: FMTException::setPrintLevel
		/**
		@brief set the level of print and change the message
		@param[in] p_level the level of print
		*/
		void setPrintLevel(int p_level);
    private:
		// DocString: FMTException::_getPrintLevel
		/**
		@brief Get the print level
		@return the print level
		*/
		int _getPrintLevel() const;
		// DocString: FMTException::_getUserMessage
		/**
		@brief Returne the message of the user
		@return a beautifull message
		*/
		const std::string& _getUserMessage() const;
		// DocString: FMTException::_buildMessage
		/**
		@brief Build the resulting message
		*/
		void _buildMessage();
		// DocString: FMTException::serialize
		/**
		@brief Serialize function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version);
		// DocString: FMTException::m_holdup
			///This member is normaly set to false but for the free exception handler
			///we want to let the exception percolate to boost::python and let the user handel the exception when holdup=true.
		bool m_holdup;
		// DocString: FMTException::_msg
		///Keeps the message string of the exception.
		std::string m_msg;
		// DocString: FMTException::m_exceptiontype
		///Type of the exception thrown.
		FMTexc m_exceptiontype;
		// DocString: FMTException::m_section
		///Section in which the exception just happened.
		Core::FMTsection m_section;
		// DocString: FMTException::m_method
		///Function where the exception just happened
		std::string m_method;
		// DocString: FMTException::m_file
		///Source file where the exception just happened
		std::string m_file;
		// DocString: FMTException::m_line
		///Source file line where the exception just happened
		int m_line;
		// DocString: FMTException::m_FrenchDescription 
		///The exception description in french
		std::string m_FrenchDescription;
		// DocString: FMTException::m_EnglishDescription 
		///The exception description in english
		std::string m_EnglishDescription;
		// DocString: FMTException::m_level
		///The exception level
		FMTlev m_level;
		// DocString: FMTException::m_PrintLevel
		///the level of verbosity
		int m_PrintLevel;
		// DocString: FMTException::m_UserMessage 
		///The base user message
		std::string m_UserMessage;
    };

}


#endif // FMTEXCEPTION_Hm_included
