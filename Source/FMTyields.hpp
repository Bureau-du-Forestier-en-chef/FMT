/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTYLD_H_INCLUDED
#define FMTYLD_H_INCLUDED

#include "FMTlist.hpp"
#include "FMTyieldhandler.hpp"
#include "FMTdevelopment.hpp"
#include "FMTbounds.hpp"
#include <vector>
#include <map>
#include <string>
#include <boost/serialization/serialization.hpp>
#include "FMTutility.hpp"
#include "FMTtheme.hpp"
#include <unordered_map>
#include <memory>
#include <boost/serialization/export.hpp>
#include "FMTyieldrequest.hpp"


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
class FMTEXPORT FMTyields : public FMTlist<std::unique_ptr<FMTyieldhandler>>
    {
	// DocString: FMTyields::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	friend class FMTyieldrequest;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		try{
			ar & boost::serialization::make_nvp("handlers", boost::serialization::base_object<FMTlist<std::unique_ptr<FMTyieldhandler>>>(*this));
			updateyieldpresence();
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTyields::serialize", __LINE__, __FILE__);
			}
	}
	// DocString: FMTyields::yieldpresence
	///If yields section as yield = true else false.
	std::unordered_map<std::string,bool>yieldpresence;
	// DocString: FMTyields::gethandleroftype
	/**
	The function returns a vector of pointer to all yieldhandler of a given (type).
	(Can returns all handler related to time/age/complex ...)
	*/
	std::vector<const FMTyieldhandler*> gethandleroftype(FMTyldtype type) const;
	// DocString: FMTyields::gethandleroftype
	/**
	The function returns a vector of pointer to all yieldhandler of a given (type).
	(Can returns all handler related to time/age/complex ...)
	*/
	std::vector<FMTyieldhandler*> gethandlers(FMTyldtype type);
	// DocString: FMTyields::getmaxbase
	/**
	Each yields can have different size in a yield section. Looking at all yieldhandler (handlers) this function
	returns the maximum age seen in thos yieldhandler.
	*/
	int getmaxbase(const std::vector<const FMTyieldhandler*>& handlers) const;
	// DocString: FMTyields::updateyieldpresence
	/**
	Update the yields presence true or false.
	*/
	void updateyieldpresence();
    public:
		// DocString: FMTyields::gotyieldtype
		/**
		Returns true if the yield section has a given yield type.
		*/
		bool gotyieldtype(FMTyldtype type) const;
		// DocString: FMTyields::setactionsmappingtomodelhandlers
		/**
		Clear the actual actionsmapping of every yield model handlers and replace it with the new.
		*/
		void setactionsmappingtomodelhandlers(const std::vector<int>& actionids);
		// DocString: FMTyields::getallyieldnames
		/**
		return all yield names from the FMTlist.
		*/
		std::vector<std::string> getallyieldnames() const;
		// DocString: FMTyields()
		/**
		Default constructor for FMTyields.
		*/
        FMTyields();
		// DocString: ~FMTyields()
		/**
		Default destructor for FMTyields.
		*/
		~FMTyields()=default;
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
		// DocString: FMTyields::clearcache
		/**
		Clear caching from all FMTdata.
		*/
		void clearcache() final;
		// DocString: FMTyields::isyld
		/**
		This function returns true if the FMTyields section contains a given (value) non null yield.
		*/
        bool isyld(const std::string& value,bool fromsource = false) const;
		// DocString: FMTyields::isnullyld
		/**
		This function returns true if the FMTyields section contains a given (value) null yield.
		*/
		bool isnullyld(const std::string& value) const;
		// DocString: FMTyields::get
		/**
		This function is the main function used to get the yields value (targets) for a given FMTdevelopement (dev),
		looking at age,period,lock,mask etc... it returns a map of yield name (keys) and there vlues(items).
		*/
		//std::vector<double>get(std::vector<FMTyieldrequest>& requests) const;
		// DocString: FMTyields::getsingle
		/**
		This function is the main function used to get the yield value (target) for a given FMTdevelopement (dev),
		looking at age,period,lock,mask etc... it returns  the yield value.
		*/
		//double getsingle(const FMTdevelopment& dev,const std::string& target) const;
		//std::map<std::string,double>getylds(const FMTdevelopment& dev,const FMTspec& spec) const;
		// DocString: FMTyields::getylds
		/**
		This function gets the yields used and its values (map) by a given specification (spec) for a given developement (dev).
		*/
		//std::vector<double>getylds(const FMTdevelopment& dev, const FMTspec& spec) const;
		// DocString: FMTyields::getage
		/**
		This function is used to get the new age of a FMTdevelopement (dev) 
		when disturbed by a given FMTtransition specification (sepc).
		*/
        int getage(const FMTyieldrequest& request,const FMTspec& spec) const;
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
		// DocString: FMTyields::getfromfactor
		/**
		This function multiply yields section with a factor and returns a new yields function.
		If vector of yieldnames is given by the user then only the data within this names list is going
		to be multiplied by the factor.
		*/
		FMTyields getfromfactor(const double& factor,
			std::vector<std::string>yieldnames = std::vector<std::string>()) const;
		// DocString: FMTyields::get
		/**
		This function is the main function used to get the yields value (targets) for a given FMTdevelopement (dev),
		looking at age,period,lock,mask etc... it returns a map of yield name (keys) and there vlues(items).
		*/
		double get(const FMTyieldrequest& request,const std::string& yld) const;
    };
}
BOOST_CLASS_EXPORT_KEY(Core::FMTyields)
#endif // FMTYLD_H_INCLUDED
