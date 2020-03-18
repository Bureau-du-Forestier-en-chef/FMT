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

#ifndef FMTTRA_H_INCLUDED
#define FMTTRA_H_INCLUDED

#include "FMTfork.h"
#include "FMTmask.h"
#include "FMTlist.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTdevelopment.h"
#include "FMTtheme.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <vector>
#include <map>
#include <string>


namespace Core
{
class FMTfork;
/**
FMTtransition is a list of FMTfork (FMTspec). Each FMTaction has it's FMTtransition.
Using a FMTdevelopment you can simulate the impact of harvesting that development with a FMTtransition.
Each fork is a transition source and its linked targets. FMTaction test for operability and FMTtransition only
"disturb" the FMTdevelopement and generates new FMTdevelopementpath.
*/
class FMTtransition : public FMTlist<FMTfork>
    {
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("data", boost::serialization::base_object<FMTlist<FMTfork>>(*this));
		ar & BOOST_SERIALIZATION_NVP(name);
	}
	protected:
		/// name of the FMTtranstion same name seen in FMTaction
		std::string name;
    public:
		/**
		Default constructor for FMTtransition
		*/
        FMTtransition();
		/**
		FMTtransition constructor keeping only the name of the FMTtransition.
		*/
		FMTtransition(const std::string& lname);
		/**
		FMTtransition copy constructor.
		*/
        FMTtransition(const FMTtransition& rhs);
        FMTtransition& operator = (const FMTtransition& rhs);
        operator std::string() const;
		/**
		A transition can lead to multiple FMTdevelopementpath but for cell basesd models the FMTtransition should lead
		to only one futurdevelopementtype. Calling this function returns the a new FMTtransition that does not allow multiple
		FMTdevelopementpath. It select the path where the area proportion is the higher.
		*/
        FMTtransition single() const;
		/**
		Getter for the FMTtransition name
		*/
		inline std::string getname() const
			{
			return name;
			}
		/**
		This function checks if each FMTfork has a total proportion equal to 1 if the transition proportion != 1
		then isleaking returns true.
		*/
		bool isleaking() const;
		/**
		Returns the FMTfork pointer for which the FMTdevelopment (dev) can be disturbed based on a given FMTyields section (ylds).
		Looking at every possible fork returns only the first usefull fork.
		*/
        const FMTfork* getfork(const FMTdevelopment& dev,
                               const FMTyields& ylds) const;
		/**
		Using a list of (themes) this function returns a subset of the theme list for which their's no attribute change
		in the FMTtransitionmask (what we call here static themes).
		*/
		std::vector<FMTtheme>getstaticthemes(const std::vector<FMTtheme>& themes) const;
		/**
		For GCBM a GCBMtransition is described by the age of the stand after the disturbance of the stand. Using a
		vector of FMTdevelopement (devs) this functions return the average age after disturbances optained calculating the
		return age for each FMTdevelopement in the list.
		*/
		unsigned int age_after(const std::vector<FMTdevelopment>& devs,
			const FMTaction& action,
			const FMTyields& ylds,
			const std::vector<FMTtheme>& themes) const;
		/**
		Again for GCBM each GCBMtransition needs to have only one possible mask target. So this function returns 
		the most targeted FMTmask across the developements list (devs) based on a given yields section (ylds).
		*/
        FMTmask main_target(const std::vector<FMTdevelopment>& devs,
                            const FMTyields& ylds) const;
		/**
		This function classify the FMTdevelopement list (devs) by FMTmask key based on a FMTyields section (ylds). 
		The returned map key is an FMTmask string separed by "-"
		*/
		std::map<std::string, std::vector<FMTdevelopment>> attribute_targets(const std::vector<FMTdevelopment>& devs,
											const FMTyields& ylds, const std::vector<FMTtheme>& themes) const;
		/**
		FMTtransition less than operator.
		*/
        bool operator < (const FMTtransition& rhs) const;
		/**
		FMTtransition equality operator check if FMTtransition (rhs) have the same name.
		*/
        bool operator == (const FMTtransition& rhs) const;
		/**
		FMTtransition nonequality operator check if FMTtransition (rhs) have not the same name.
		*/
        bool operator != (const FMTtransition& rhs) const;
		/**
		Returns a presolved FMTtransition based on a (basemask), the actual model FMTtheme (originalthemes),
		a (presolvedmask) and the new presolved themes list.
		*/
		FMTtransition presolve(const FMTmask& basemask,
			const std::vector<FMTtheme>& originalthemes,
			const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes) const;
    };

/**
This class is made to compare FMTtransition using the std::find_if() function when FMTtransitions are in a std container.
*/
class FMTtransitioncomparator
{
	///transition name that we are looking for in the std
	std::string transition_name;
public:
	/**
	FMTtransitioncomparator constructor for testing if we can find a transition with the same (name).
	*/
	FMTtransitioncomparator(std::string name);
	/**
	Matching test operator for FMTtransitioncomparator.
	*/
	bool operator()(const FMTtransition& transition) const;

};

}


#endif // FMTTRA_H_INCLUDED
