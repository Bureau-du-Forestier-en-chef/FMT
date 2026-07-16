/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTTRA_Hm_included
#define FMTTRA_Hm_included

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
class FMTYieldRequest;
class FMTDevelopment;
class FMTMask;
class FMTAction;
class FMTYields;
class FMTTheme;
// DocString: FMTTransition
/**
FMTTransition is a list of FMTFork (FMTSpec). Each FMTAction has it's FMTTransition.
Using a FMTDevelopment you can simulate the impact of harvesting that development with a FMTTransition.
Each fork is a transition source and its linked targets. FMTAction test for operability and FMTTransition only
"disturb" the FMTDevelopment and generates new FMTDevelopmentPath.
*/
class FMTEXPORT FMTTransition : public FMTList<FMTFork>
    {
    public:
		// DocString: FMTTransition::operator+=
		/**
		 * @brief append OtherTransition to this transition needs to be unskrink or it will throw an exception.
		 * @param OtherTransition the other transition to append to this one.
		 * @returns A reference the the newly appended transition.
		 */
		FMTTransition& operator+=(const FMTTransition& OtherTransition);
		// DocString: FMTTransition()
		/**
		Default constructor for FMTTransition
		*/
        FMTTransition();
		// DocString: ~FMTTransition()
		/**
		Default destructor for FMTTransition
		*/
		~FMTTransition() = default;
		// DocString: FMTTransition(const std::string&)
		/**
		FMTTransition constructor keeping only the name of the FMTTransition.
		*/
		FMTTransition(const std::string& lname);
		// DocString: FMTTransition(const FMTTransition&)
		/**
		FMTTransition copy constructor.
		*/
        FMTTransition(const FMTTransition& rhs);
		// DocString: FMTTransition::operator=
		/**
		FMTTransition assignment operator.
		*/
        FMTTransition& operator = (const FMTTransition& rhs);
		// DocString: FMTTransition::operator std::string
		/**
		FMTTransition conversion to string value like into a transition file.
		*/
        operator std::string() const;
		// DocString: FMTTransition::single
		/**
		A transition can lead to multiple FMTDevelopmentPath but for cell basesd models the FMTTransition should lead
		to only one futurdevelopmenttype. Calling this function returns the a new FMTTransition that does not allow multiple
		FMTDevelopmentPath. It select the path where the area proportion is the higher.
		*/
        FMTTransition single() const;
		// DocString: FMTTransition::getName
		/**
		Getter for the FMTTransition name
		*/
		inline std::string getName() const
			{
			return name;
			}
		// DocString: FMTTransition::setName
		/**
		@brief Setter for FMTTransition name
		@param[in] 
		*/
		void setName(const std::string& p_name);
		// DocString: FMTTransition::isLeaking
		/**
		This function checks if each FMTFork has a total proportion equal to 1 if the transition proportion != 1
		then isLeaking returns true.
		*/
		bool isLeaking() const;
		// DocString: FMTTransition::getFork
		/**
		Returns the FMTFork pointer for which the FMTDevelopment (dev) can be disturbed based on a given FMTYields section (ylds).
		Looking at every possible fork returns only the first usefull fork.
		*/
        const FMTFork* getFork(const Core::FMTDevelopment& development, const FMTYields& ylds) const;
		// DocString: FMTTransition::getStaticThemes
		/**
		Using a list of (themes) this function returns a subset of the theme list for which their's no attribute change
		in the FMTTransitionMask (what we call here static themes).
		*/
		std::vector<const FMTTheme*>getStaticThemes(const std::vector<const FMTTheme*>& themes) const;
		// DocString: FMTTransition::canProduce
		/**
		The function return all the possibles masks, if pass as a source for the transition, that can produce the given(mask).
		*/
		std::vector<Core::FMTMask> canProduce(const Core::FMTMask& testmask,const std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTTransition::ageAfter
		/**
		For GCBM a GCBMtransition is described by the age of the stand after the disturbance of the stand. Using a
		vector of FMTDevelopment (devs) this functions return the average age after disturbances optained calculating the
		return age for each FMTDevelopment in the list.
		*/
		unsigned int ageAfter(const std::vector<FMTDevelopment>& devs,
			const FMTAction& action,
			const FMTYields& ylds,
			const std::vector<FMTTheme>& themes) const;
		// DocString: FMTTransition::mainTarget
		/**
		Again for GCBM each GCBMtransition needs to have only one possible mask target. So this function returns 
		the most targeted FMTMask across the developments list (devs) based on a given yields section (ylds).
		*/
        FMTMask mainTarget(const std::vector<FMTDevelopment>& devs,
                            const FMTYields& ylds) const;
		// DocString: FMTTransition::attributeTargets
		/**
		This function classify the FMTDevelopment list (devs) by FMTMask key based on a FMTYields section (ylds). 
		The returned map key is an FMTMask string separed by "-"
		*/
		std::map<std::string, std::vector<FMTDevelopment>> attributeTargets(const std::vector<FMTDevelopment>& devs,
											const FMTYields& ylds, const std::vector<FMTTheme>& themes) const;
		// DocString: FMTTransition::operator<
		/**
		FMTTransition less than operator.
		*/
        bool operator < (const FMTTransition& rhs) const;
		// DocString: FMTTransition::operator==
		/**
		FMTTransition equality operator check if FMTTransition (rhs) have the same name.
		*/
        bool operator == (const FMTTransition& rhs) const;
		// DocString: FMTTransition::operator!=
		/**
		FMTTransition nonequality operator check if FMTTransition (rhs) have not the same name.
		*/
        bool operator != (const FMTTransition& rhs) const;
		// DocString: FMTTransition::presolve
		/**
		Returns a presolved FMTTransition based on a (baseMask), the actual model FMTTheme (originalthemes),
		a (presolvedmask) and the new presolved themes list.
		*/
		FMTTransition presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			std::vector<FMTTheme>& newthemes,bool compressdata=false) const;
		// DocString: FMTTransition::presolveRef
		/**
		@brief Returns a presolved FMTTransition based on a (baseMask), the actual model FMTTheme (originalthemes),
		a (presolvedmask) and the new presolved themes list.
		@param[in] p_filter
		@param[in] p_originalThemes
		@param[in] p_newthemes
		@param[in] p_compressdata
		*/
		void presolveRef(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_originalThemes,
			std::vector<FMTTheme>& p_newthemes, bool p_compressdata = false);
	protected:
		// DocString: FMTTransition::name
		/// name of the FMTtranstion same name seen in FMTAction
		std::string name;
	private:
		// DocString: FMTTransition::serialize
		/**
		serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
				try {
					ar& boost::serialization::make_nvp("data", boost::serialization::base_object<FMTList<FMTFork>>(*this));
					ar& BOOST_SERIALIZATION_NVP(name);
				}
				catch (...)
				{
					_exhandler->printExceptions("", "FMTTransition::serialize", __LINE__, __FILE__);
				}
			}
    };

// DocString: FMTTransitionComparator
/**
This class is made to compare FMTTransition using the std::find_if() function when FMTtransitions are in a std container.
*/
class FMTTransitionComparator
{
	// DocString: FMTTransitionComparator::transition_name
	///transition name that we are looking for in the std
	std::string transition_name;
public:
	// DocString: FMTTransitionComparator(std::string)
	/**
	FMTTransitionComparator constructor for testing if we can find a transition with the same (name).
	*/
	FMTTransitionComparator(std::string name);
	// DocString: FMTTransitionComparator(const FMTTransition&)
	/**
	Matching test operator for FMTTransitionComparator.
	*/
	bool operator()(const FMTTransition& transition) const;

};

}

BOOST_CLASS_EXPORT_KEY(Core::FMTTransition)
#endif // FMTTRA_Hm_included
