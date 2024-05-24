/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTTRA_H_INCLUDED
#define FMTTRA_H_INCLUDED

#include "FMTfork.h"
#include "FMTlist.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/export.hpp>
#include <vector>
#include <map>
#include <string>


namespace Core
{
class FMTyieldrequest;
class FMTdevelopment;
class FMTmask;
class FMTaction;
class FMTyields;
class FMTtheme;
// DocString: FMTtransition
/**
FMTtransition is a list of FMTfork (FMTspec). Each FMTaction has it's FMTtransition.
Using a FMTdevelopment you can simulate the impact of harvesting that development with a FMTtransition.
Each fork is a transition source and its linked targets. FMTaction test for operability and FMTtransition only
"disturb" the FMTdevelopement and generates new FMTdevelopementpath.
*/
class FMTEXPORT FMTtransition : public FMTlist<FMTfork>
    {
    public:
		// DocString: FMTtransition::operator+=
		/**
		 * @brief append OtherTransition to this transition needs to be unskrink or it will throw an exception.
		 * @param OtherTransition the other transition to append to this one.
		 * @returns A reference the the newly appended transition.
		 */
		FMTtransition& operator+=(const FMTtransition& OtherTransition);
		// DocString: FMTtransition()
		/**
		Default constructor for FMTtransition
		*/
        FMTtransition();
		// DocString: ~FMTtransition()
		/**
		Default destructor for FMTtransition
		*/
		~FMTtransition() = default;
		// DocString: FMTtransition(const std::string&)
		/**
		FMTtransition constructor keeping only the name of the FMTtransition.
		*/
		FMTtransition(const std::string& lname);
		// DocString: FMTtransition(const FMTtransition&)
		/**
		FMTtransition copy constructor.
		*/
        FMTtransition(const FMTtransition& rhs);
		// DocString: FMTtransition::operator=
		/**
		FMTtransition assignment operator.
		*/
        FMTtransition& operator = (const FMTtransition& rhs);
		// DocString: FMTtransition::operator std::string
		/**
		FMTtransition conversion to string value like into a transition file.
		*/
        operator std::string() const;
		// DocString: FMTtransition::single
		/**
		A transition can lead to multiple FMTdevelopementpath but for cell basesd models the FMTtransition should lead
		to only one futurdevelopementtype. Calling this function returns the a new FMTtransition that does not allow multiple
		FMTdevelopementpath. It select the path where the area proportion is the higher.
		*/
        FMTtransition single() const;
		// DocString: FMTtransition::getname
		/**
		Getter for the FMTtransition name
		*/
		inline std::string getname() const
			{
			return name;
			}
		// DocString: FMTtransition::isleaking
		/**
		This function checks if each FMTfork has a total proportion equal to 1 if the transition proportion != 1
		then isleaking returns true.
		*/
		bool isleaking() const;
		// DocString: FMTtransition::getfork
		/**
		Returns the FMTfork pointer for which the FMTdevelopment (dev) can be disturbed based on a given FMTyields section (ylds).
		Looking at every possible fork returns only the first usefull fork.
		*/
        const FMTfork* getfork(const Core::FMTdevelopment& developement, const FMTyields& ylds) const;
		// DocString: FMTtransition::getstaticthemes
		/**
		Using a list of (themes) this function returns a subset of the theme list for which their's no attribute change
		in the FMTtransitionmask (what we call here static themes).
		*/
		std::vector<const FMTtheme*>getstaticthemes(const std::vector<const FMTtheme*>& themes) const;
		// DocString: FMTtransition::canproduce
		/**
		The function return all the possibles masks, if pass as a source for the transition, that can produce the given(mask).
		*/
		std::vector<Core::FMTmask> canproduce(const Core::FMTmask& testmask,const std::vector<Core::FMTtheme>& themes) const;
		// DocString: FMTtransition::age_after
		/**
		For GCBM a GCBMtransition is described by the age of the stand after the disturbance of the stand. Using a
		vector of FMTdevelopement (devs) this functions return the average age after disturbances optained calculating the
		return age for each FMTdevelopement in the list.
		*/
		unsigned int age_after(const std::vector<FMTdevelopment>& devs,
			const FMTaction& action,
			const FMTyields& ylds,
			const std::vector<FMTtheme>& themes) const;
		// DocString: FMTtransition::main_target
		/**
		Again for GCBM each GCBMtransition needs to have only one possible mask target. So this function returns 
		the most targeted FMTmask across the developements list (devs) based on a given yields section (ylds).
		*/
        FMTmask main_target(const std::vector<FMTdevelopment>& devs,
                            const FMTyields& ylds) const;
		// DocString: FMTtransition::attribute_targets
		/**
		This function classify the FMTdevelopement list (devs) by FMTmask key based on a FMTyields section (ylds). 
		The returned map key is an FMTmask string separed by "-"
		*/
		std::map<std::string, std::vector<FMTdevelopment>> attribute_targets(const std::vector<FMTdevelopment>& devs,
											const FMTyields& ylds, const std::vector<FMTtheme>& themes) const;
		// DocString: FMTtransition::operator<
		/**
		FMTtransition less than operator.
		*/
        bool operator < (const FMTtransition& rhs) const;
		// DocString: FMTtransition::operator==
		/**
		FMTtransition equality operator check if FMTtransition (rhs) have the same name.
		*/
        bool operator == (const FMTtransition& rhs) const;
		// DocString: FMTtransition::operator!=
		/**
		FMTtransition nonequality operator check if FMTtransition (rhs) have not the same name.
		*/
        bool operator != (const FMTtransition& rhs) const;
		// DocString: FMTtransition::presolve
		/**
		Returns a presolved FMTtransition based on a (basemask), the actual model FMTtheme (originalthemes),
		a (presolvedmask) and the new presolved themes list.
		*/
		FMTtransition presolve(const FMTmaskfilter& filter,
			const std::vector<FMTtheme>& originalthemes,
			std::vector<FMTtheme>& newthemes,bool compressdata=false) const;
	protected:
		// DocString: FMTtransition::name
		/// name of the FMTtranstion same name seen in FMTaction
		std::string name;
	private:
		// DocString: FMTtransition::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
				try {
					ar& boost::serialization::make_nvp("data", boost::serialization::base_object<FMTlist<FMTfork>>(*this));
					ar& BOOST_SERIALIZATION_NVP(name);
				}
				catch (...)
				{
					_exhandler->printexceptions("", "FMTtransition::serialize", __LINE__, __FILE__);
				}
			}
    };

// DocString: FMTtransitioncomparator
/**
This class is made to compare FMTtransition using the std::find_if() function when FMTtransitions are in a std container.
*/
class FMTtransitioncomparator
{
	// DocString: FMTtransitioncomparator::transition_name
	///transition name that we are looking for in the std
	std::string transition_name;
public:
	// DocString: FMTtransitioncomparator(std::string)
	/**
	FMTtransitioncomparator constructor for testing if we can find a transition with the same (name).
	*/
	FMTtransitioncomparator(std::string name);
	// DocString: FMTtransitioncomparator(const FMTtransition&)
	/**
	Matching test operator for FMTtransitioncomparator.
	*/
	bool operator()(const FMTtransition& transition) const;

};

}

BOOST_CLASS_EXPORT_KEY(Core::FMTtransition)
#endif // FMTTRA_H_INCLUDED
