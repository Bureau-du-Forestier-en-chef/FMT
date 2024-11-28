/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
	


#ifndef FMTEXCEPTION_H_INCLUDED
#define FMTEXCEPTION_H_INCLUDED

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
This enumerator describes the kind of the Exception trown by FMT.
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
This enumerator describes the exceptions thrown by FMT.
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
	FMTmissingdevelopement = 45,
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
	FMTWIN32Error = 85
    };

// DocString: FMTexception
/**
FMTexception is the exception base class for FMT. All the informations regarding a given exception
is kept by this class (type,section and message).
*/
class FMTEXPORT FMTexception : public std::exception
    {
	// DocString: FMTexception::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);
    protected:
		// DocString: FMTexception::holdup
		///This member is normaly set to false but for the free exception handler
		///we want to let the exception percolate to boost::python and let the user handel the exception when holdup=true.
		bool holdup;
		// DocString: FMTexception::_msg
		///Keeps the message string of the exception.
		std::string _msg;
		// DocString: FMTexception::exceptiontype
		///Type of the exception thrown.
		FMTexc exceptiontype;
		// DocString: FMTexception::section
		///Section in which the exception just happened.
		Core::FMTsection section;
		// DocString: FMTexception::method
		///Function where the exception just happened
		std::string method;
		// DocString: FMTexception::file
		///Source file where the exception just happened
		std::string file;
		// DocString: FMTexception::line
		///Source file line where the exception just happened
		int line;
    public:
	// DocString: FMTexception()
	/**
	FMTexception default constructor.
	*/
    FMTexception();
	// DocString: ~FMTexception()
	/**
	FMTexception default virtual destructor.
	*/
    virtual ~FMTexception() = default;

	// DocString: FMTexception()
	/**
	FMTexception with std exception
	*/
	FMTexception(const std::exception& baseexception);
	// DocString: FMTexception(const FMTexc,const std::string)
	/**
	FMTexception constructor taking a exception type and a message.
	*/
    FMTexception(const FMTexc lexception,const std::string message);

	// DocString: FMTexception(const FMTexc,Core::FMTsection,const std::string)
	/**
	FMTexception constructor taking a exception type a section and a message.
	*/
    FMTexception(const FMTexc lexception,Core::FMTsection lsection,const std::string message);
	// DocString: FMTexception(const FMTexc,Core::FMTsection,const std::string,const std::string&,const std::string&,const std::string&)
	/**
	FMTexception constructor taking a exception type a section and a message, the method, file and line
	where the exception juste happened.
	*/
	FMTexception(const FMTexc lexception,Core::FMTsection lsection,const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline);

	// DocString: FMTexception(const FMTexc,Core::FMTsection,const std::string,const std::string&)
	/**
	FMTexception constructor taking a exception type a section and a message, the method
	where the exception juste happened.
	*/
	FMTexception(const FMTexc lexception, Core::FMTsection lsection, const std::string message,
		const std::string& lmethod);

	// DocString: FMTexception(const FMTexc, const std::string&,const std::string&,const std::string&,const int&)
	/**
	FMTexception constructor taking a exception type and a message.
	*/
	FMTexception(const FMTexc lexception, const std::string message,
		const std::string& lmethod, const std::string& lfile, const int& lline);

	// DocString: FMTexception(const FMTexc, const std::string&,const std::string&,)
	/**
	FMTexception constructor taking a exception type and a message and method.
	*/
	FMTexception(const FMTexc lexception, const std::string message,const std::string& lmethod);
	// DocString: FMTexception(const FMTexception&)
	/**
	FMTexception default copy constructor.
	*/
    FMTexception(const FMTexception& rhs);
	// DocString: FMTexception::operator=
	/**
	FMTexception default copy assignment operator.
	*/
    FMTexception& operator = (const FMTexception& rhs);
	// DocString: FMTexception::gettype
	/**
	The function returns the type of exception kept by this FMTexception.
	*/
	FMTexc gettype() const;
	// DocString: FMTexception::getsection
	/**
	The function returns the section in which the exception occured.
	*/
	Core::FMTsection getsection() const;
	// DocString: FMTexception::what
	/**
	This function override the what function of the exception base class returning the message string.
	*/
    const char* what() const throw() override;
	// DocString: FMTexception::hold
	/**
	Returns the value of the holdup member.
	*/
	bool hold() const;
	// DocString: FMTexception::sethold
	/**
	The function sets the value of holdup member.
	*/
	void sethold(bool side);
	// DocString: FMTexception::getmethod
	/**
	Returns the method in which the exception occured.
	*/
	inline std::string getmethod() const
		{
		return method;
		}
	// DocString: FMTexception::getfile
	/**
	Returns the source file in which the exception occured.
	*/
	inline std::string getfile() const
		{
		return file;
		}
	// DocString: FMTexception::getline
	/**
	Returns the source file line in which the exception occured.
	*/
	inline int getline() const
		{
		return line;
		}
	// DocString: FMTexception::getsrcinfo
	/**
	Returns a formated string for the location of the exception.
	*/
	std::string getsrcinfo() const;
    };

}


#endif // FMTEXCEPTION_H_INCLUDED
