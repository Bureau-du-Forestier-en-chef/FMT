/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef FMTEXCEPTION_H_INCLUDED
#define FMTEXCEPTION_H_INCLUDED

#include <exception>
#include <string>
#include <iostream>
#include <memory>
#include <boost/python.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#if defined (_MSC_VER)
	#define NOMINMAX
	#include <comdef.h>
	#include <windows.h>
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
	#include <boost/filesystem.hpp>
#endif

#if defined (__CYGWIN__)
    #include <windows.h>
    #include <iterator>
    EXTERN_C IMAGE_DOS_HEADER __ImageBase;
    #include <boost/filesystem.hpp>
#endif

#include "FMTutility.h"
#include "FMTlogger.h"

using namespace std;

namespace Exception
{

enum FMTlev
    {
    FMT_None=0,
    FMT_Debug=1,
    FMT_Warning=2,
    FMT_logic=3,
    FMT_range=4
    };

enum FMTexc
    {
    None = 0,
    WSconstants_replacement = 1,
    WScomma_replacement =2,
    WSfutur_types = 3,
    WSinvalid_theme = 5,
    WSinvalid_aggregate = 6,
    WSinvalid_maskrange = 7,
    WSinvalid_yield = 8,
    WSinvalid_action = 9,
    WSinvalid_transition = 10,
    WSinvalid_transition_case = 11,
    WSinvalid_number = 12,
    FMTinvalid_path = 13,
    WStheme_redefinition = 14,
    WSaggregate_redefinition = 15,
    WSempty_theme = 16,
    WSempty_aggregate = 17,
    WSundefined_aggregate_value = 18,
    WSundefined_attribute = 19,
    WSempty_action = 20,
    WSwrong_partial = 21,
    WSpreexisting_yield = 22,
    WSunsupported_yield = 23,
    FMTinvaliddataset = 24,
    FMTinvalidband = 25,
    FMTinvalidrasterblock = 26,
    FMTinvalidlayer = 27,
    FMTmissingfield = 28,
    FMTinvalidoverview = 29,
    FMTmissingrasterattribute = 30,
    WSunsupported_transition = 31,
    WSundefined_action = 32,
    WSempty_transition = 33,
    WSundefined_output = 34,
    WSunsupported_output = 35,
    FMTinvaliddriver = 36,
    FMTinvalidAandT = 37,
    WSleakingtransition = 38,
	WSundefineddeathaction = 39,
	WSundefineddeathtransition = 40,
	WSignore = 41,
	FMTmissingyield = 42,
	WSattribute_redefinition = 43,
	WSundefined_constant = 44,
	FMTmissingdevelopement = 45,
	FMTmissingobjective = 46,
	FMTunsupported_objective = 47,
	FMTinvalid_constraint = 48,
	WSemptybound = 49,
	FMTunboundedperiod = 50,
	FMTnonaddedconstraint = 51
    };

class FMTexception : public std::exception
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("object", boost::serialization::base_object<std::exception>(*this));
		ar & BOOST_SERIALIZATION_NVP(holdup);
		ar & BOOST_SERIALIZATION_NVP(_msg);
		ar & BOOST_SERIALIZATION_NVP(exceptiontype);
		ar & BOOST_SERIALIZATION_NVP(section);
	}
    protected:
		bool holdup;
        string _msg;
		FMTexc exceptiontype;
		FMTwssect section;
    public:
    FMTexception();
    virtual ~FMTexception() = default;
    FMTexception(const FMTexc lexception,const string message);
    FMTexception(const FMTexc lexception,FMTwssect lsection,const string message);
    FMTexception(const FMTexception& rhs);
    FMTexception& operator = (const FMTexception& rhs);
	FMTexc gettype() const;
	FMTwssect getsection() const;
    const char* what() const throw() override;
	bool hold() const;
	void sethold(bool side);
    };

}


#endif // FMTEXCEPTION_H_INCLUDED
