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
    FMTcomma_replacement =2,
    FMTfutur_types = 3,
    FMTinvalid_theme = 5,
    FMTinvalid_aggregate = 6,
    FMTinvalid_maskrange = 7,
    FMTinvalid_yield = 8,
    FMTinvalid_action = 9,
    FMTinvalid_transition = 10,
    FMTinvalid_transition_case = 11,
    FMTinvalid_number = 12,
    FMTinvalid_path = 13,
    FMTtheme_redefinition = 14,
    FMTaggregate_redefinition = 15,
    FMTempty_theme = 16,
    FMTempty_aggregate = 17,
    FMTundefined_aggregate_value = 18,
    FMTundefined_attribute = 19,
    FMTempty_action = 20,
    FMTwrong_partial = 21,
    FMTpreexisting_yield = 22,
    FMTunsupported_yield = 23,
    FMTinvaliddataset = 24,
    FMTinvalidband = 25,
    FMTinvalidrasterblock = 26,
    FMTinvalidlayer = 27,
    FMTmissingfield = 28,
    FMTinvalidoverview = 29,
    FMTmissingrasterattribute = 30,
    FMTunsupported_transition = 31,
    FMTundefined_action = 32,
    FMTempty_transition = 33,
    FMTundefined_output = 34,
    FMTunsupported_output = 35,
    FMTinvaliddriver = 36,
    FMTinvalidAandT = 37,
    FMTleakingtransition = 38,
	FMTundefineddeathaction = 39,
	FMTundefineddeathtransition = 40,
	FMTignore = 41,
	FMTmissingyield = 42,
	FMTattribute_redefinition = 43,
	FMTundefined_constant = 44,
	FMTmissingdevelopment = 45,
	FMTmissingobjective = 46,
	FMTunsupported_objective = 47,
	FMTinvalid_constraint = 48,
	FMTemptybound = 49,
	FMTunboundedperiod = 50,
	FMTnonaddedconstraint = 51,
	FMTmissinglicense = 52,
	FMTfunctionfailed = 53,
	FMTcoinerror = 54,
	FMTboostgrapherror = 55,
	FMTunhandlederror = 56,
	FMTnotlinegraph = 57,
	FMTrangeerror = 58,
	FMTGDALerror = 59,
	FMTGDALwarning = 60,
	FMTinfeasibleconstraint = 61,
	FMTthematic_output_diff=62,
	FMToutput_missing_operator=63,
	FMToutput_too_much_operator=64,
	FMTinvalidyield_number = 65,
	FMTgdal_constructor_error= 66,
	FMTinvalid_geometry= 67,
	FMTundefinedoutput_attribute = 68,
	FMTempty_schedules = 69,
	FMTmissing_scenarios= 70,
	FMTschemefailed=71,
	FMTmaxnumberofexception = 72,
	FMTmskerror = 73,
	FMToveridedyield = 74,
	FMTmissing_parameter = 75,
	FMTdeathwithlock = 76,
	FMTreplanningwarning = 77,
	FMTyieldmodelprediction = 78,
	FMTsourcetotarget_transition = 79,
	FMTsame_transitiontargets = 80,
	FMTunclosedforloop = 81,
	FMTthreadcrash = 82,
	FMToutofrangeyield = 83,
	FMTEmptyOA = 84,
	FMTWIN32Error = 85,
	FMTMSKnumerical_problem = 86,
	FMTunreachable_threshold = 87
    };

// DocString: FMTException
/**
@brief Base exception class for FMT holding the type, section and message of a given exception.
*/
class FMTEXPORT FMTException : public std::exception
    {
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
    protected:
		// DocString: FMTException::holdup
		///This member is normaly set to false but for the free exception handler
		///we want to let the exception percolate to boost::python and let the user handel the exception when holdup=true.
		bool holdup;
		// DocString: FMTException::_msg
		///Keeps the message string of the exception.
		std::string _msg;
		// DocString: FMTException::exceptiontype
		///Type of the exception thrown.
		FMTexc exceptiontype;
		// DocString: FMTException::section
		///Section in which the exception just happened.
		Core::FMTsection section;
		// DocString: FMTException::method
		///Function where the exception just happened
		std::string method;
		// DocString: FMTException::file
		///Source file where the exception just happened
		std::string file;
		// DocString: FMTException::line
		///Source file line where the exception just happened
		int line;
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

	// DocString: FMTException(const std::exception&)
	/**
	@brief Construct a FMTException from a std::exception.
	@param[in] baseexception the std::exception to construct from.
	*/
	FMTException(const std::exception& baseexception);
	// DocString: FMTException(const FMTexc,const std::string)
	/**
	@brief Construct a FMTException from an exception type and a message.
	@param[in] lexception the exception type.
	@param[in] message the message of the exception.
	*/
    FMTException(const FMTexc lexception,const std::string message);

	// DocString: FMTException(const FMTexc,Core::FMTsection,const std::string)
	/**
	@brief Construct a FMTException from an exception type, a section and a message.
	@param[in] lexception the exception type.
	@param[in] lsection the section in which the exception occurred.
	@param[in] message the message of the exception.
	*/
    FMTException(const FMTexc lexception,Core::FMTsection lsection,const std::string message);
	// DocString: FMTException(const FMTexc,Core::FMTsection,const std::string,const std::string&,const std::string&,const std::string&)
	/**
	@brief Construct a FMTException from an exception type, a section, a message and the location where it occurred.
	@param[in] lexception the exception type.
	@param[in] lsection the section in which the exception occurred.
	@param[in] message the message of the exception.
	@param[in] lmethod the method where the exception occurred.
	@param[in] lfile the file where the exception occurred.
	@param[in] lline the line where the exception occurred.
	*/
	FMTException(const FMTexc lexception,Core::FMTsection lsection,const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline);

	// DocString: FMTException(const FMTexc,Core::FMTsection,const std::string,const std::string&)
	/**
	@brief Construct a FMTException from an exception type, a section, a message and the method where it occurred.
	@param[in] lexception the exception type.
	@param[in] lsection the section in which the exception occurred.
	@param[in] message the message of the exception.
	@param[in] lmethod the method where the exception occurred.
	*/
	FMTException(const FMTexc lexception, Core::FMTsection lsection, const std::string message,
		const std::string& lmethod);

	// DocString: FMTException(const FMTexc, const std::string&,const std::string&,const std::string&,const int&)
	/**
	@brief Construct a FMTException from an exception type, a message and the location where it occurred.
	@param[in] lexception the exception type.
	@param[in] message the message of the exception.
	@param[in] lmethod the method where the exception occurred.
	@param[in] lfile the file where the exception occurred.
	@param[in] lline the line where the exception occurred.
	*/
	FMTException(const FMTexc lexception, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline);

	// DocString: FMTException(const FMTexc, const std::string&,const std::string&,)
	/**
	@brief Construct a FMTException from an exception type, a message and the method where it occurred.
	@param[in] lexception the exception type.
	@param[in] message the message of the exception.
	@param[in] lmethod the method where the exception occurred.
	*/
	FMTException(const FMTexc lexception, const std::string message,const std::string& lmethod);
	// DocString: FMTException(const FMTException&)
	/**
	@brief Default copy constructor for FMTException.
	@param[in] rhs the FMTException to copy.
	*/
    FMTException(const FMTException& rhs);
	// DocString: FMTException::operator=
	/**
	@brief Default copy assignment operator for FMTException.
	@param[in] rhs the FMTException to copy.
	@return a reference to this FMTException.
	*/
    FMTException& operator = (const FMTException& rhs);
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
    const char* what() const throw() override;
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
		return method;
		}
	// DocString: FMTException::getFile
	/**
	@brief Return the source file in which the exception occurred.
	@return the source file.
	*/
	inline std::string getFile() const
		{
		return file;
		}
	// DocString: FMTException::getLine
	/**
	@brief Return the source file line in which the exception occurred.
	@return the source file line.
	*/
	inline int getLine() const
		{
		return line;
		}
	// DocString: FMTException::getSrcInfo
	/**
	@brief Return a formatted string for the location of the exception.
	@return the formatted source location.
	*/
	std::string getSrcInfo() const;
    };

}


#endif // FMTEXCEPTION_Hm_included
