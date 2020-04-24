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

#ifndef FMTYLD_H_INCLUDED
#define FMTYLD_H_INCLUDED

#include "FMTlist.h"
#include "FMTyieldhandler.h"
#include "FMTdevelopment.h"
#include "FMTbounds.h"
#include <vector>
#include <map>
#include <string>
#include <boost/serialization/serialization.hpp>
#include "FMTutility.h"
#include "FMTtheme.h"
#include <boost/serialization/export.hpp>


namespace Core
{

class FMTdevelopment;
// DocString: FMTyields
/**
FMTyields is one FMTlist containing multiple yieldhandlers has seen in the yield section.
FMTyields hold all the information related to the forest productivity this class is sometime super large.
FMTyields is a class used to check if a given FMTdevelopement can be operable to an action, calculate outputs,
constraints and disturb a forest stand in a FMTtransition.
*/
class FMTyields : public FMTlist<FMTyieldhandler>
    {
	// DocString: FMTyields::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("handlers", boost::serialization::base_object<FMTlist<FMTyieldhandler>>(*this));
		ar & BOOST_SERIALIZATION_NVP(names);
		ar & BOOST_SERIALIZATION_NVP(null_names);
	}
	// DocString: FMTyields::names
	///this data member is used has a caching facility to have all the yield name that the FMTlist contains.
	std::vector<std::string>names;
	// DocString: FMTyields::null_names
	///this data member is used has a caching facility containing the yield names that are equal to 0.
	std::vector<std::string>null_names;
	// DocString: FMTyields::gethandleroftype
	/**
	The function returns a vector of pointer to all yieldhandler of a given (type).
	(Can returns all handler related to time/age/complex ...)
	*/
	std::vector<const FMTyieldhandler*> gethandleroftype(FMTyldtype type) const;
	// DocString: FMTyields::getmaxbase
	/**
	Each yields can have different size in a yield section. Looking at all yieldhandler (handlers) this function
	returns the maximum age seen in thos yieldhandler.
	*/
	int getmaxbase(const std::vector<const FMTyieldhandler*>& handlers) const;
    public:
		// DocString: FMTyields()
		/**
		Default constructor for FMTyields.
		*/
        FMTyields();
		// DocString: FMTyields(const FMTyields&)
		/**
		Default copy constructor for FMTyields.
		*/
        FMTyields(const FMTyields& rhs);
		// DocString: FMTyields::operator=
		/**
		Default copy assignment for FMTyields.
		*/
        FMTyields& operator = (const FMTyields& rhs);
		// DocString: FMTyields::isyld
		/**
		This function returns true if the FMTyields section contains a given (value) non null yield.
		*/
        bool isyld(const std::string& value) const;
		// DocString: FMTyields::isnullyld
		/**
		This function returns true if the FMTyields section contains a given (value) null yield.
		*/
		bool isnullyld(const std::string& value) const;
		// DocString: FMTyields::getyldsnames
		/**
		Getter for the non null yield names of the FMTyields.
		*/
		std::vector<std::string>getyldsnames() const;
		// DocString: FMTyields::getnullyldsnames
		/**
		Getter for the null yield names of the FMTyields.
		*/
		std::vector<std::string>getnullyldsnames() const;
		// DocString: FMTyields::get
		/**
		This function is the main function used to get the yields value (targets) for a given FMTdevelopement (dev),
		looking at age,period,lock,mask etc... it returns a map of yield name (keys) and there vlues(items).
		*/
		std::map<std::string, double>get(const FMTdevelopment& dev,
			const std::vector<std::string>& targets) const;
		// DocString: FMTyields::getylds
		/**
		This function gets the yields used and its values (map) by a given specification (spec) for a given developement (dev).
		*/
		std::map<std::string,double>getylds(const FMTdevelopment& dev,const FMTspec& spec) const;
		// DocString: FMTyields::getage
		/**
		This function is used to get the new age of a FMTdevelopement (dev) 
		when disturbed by a given FMTtransition specification (sepc).
		*/
        int getage(const FMTdevelopment& dev,const FMTspec& spec) const;
		// DocString: FMTyields::getallyields
		/**
		This function returns a map with mask has key (with only one FMTtheme) for only given FMTyieldhandler type (type).
		The map contains all the yield values for each yield name (map key). This function is used for generating a text file 
		containing all the yields values for GCBM (might be only usefull for Forestier en chef) .
		*/
		std::map<std::string, std::map<std::string, std::vector<double>>>getallyields(const FMTtheme& target,FMTyldtype type) const;
		// DocString: FMTyields::operator==
		/**
		FMTyields equality operator check if FMTyields are the same.
		*/
		bool operator == (const FMTyields& rhs) const;
		// DocString: FMTyields::update
		/**
		FMTyields being an FMTlist an update function needs to be implemented to update 
		the yieldnames and nullyieldsname caching.
		*/
        void update() override;
		// DocString: FMTyields::presolve
		/**
		Presolving might be realy usefull for FMTyields because this class tend to get realy large and contains
		sometime useless stuff. So using the same presolved information it returns a presolved FMTyields section.
		*/
		FMTyields presolve(const FMTmask& basemask,
			const std::vector<FMTtheme>& originalthemes,
			const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes) const;
		// DocString: FMTyields::getstacked
		/**
		This function returns the FMTyields has a string in a vector.
		*/
		std::vector<std::string>getstacked() const;
    };
}
BOOST_CLASS_EXPORT_KEY(Core::FMTyields);
#endif // FMTYLD_H_INCLUDED
