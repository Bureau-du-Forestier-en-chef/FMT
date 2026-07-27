/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTTRA_Hm_included
#define FMTTRA_Hm_included

#include "FMTFork.h"
#include "FMTList.hpp"
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
@brief List of FMTFork describing the transition of an action, disturbing a development to generate new development paths.
@details Each FMTAction has its FMTTransition. Each fork is a transition source with its linked targets.
*/
class FMTEXPORT FMTTransition : public FMTList<FMTFork>
    {
    public:
		// DocString: FMTTransition::operator+=
		/**
		@brief Append another transition to this transition, which needs to be unshrunk otherwise an exception is thrown.
		@param[in] OtherTransition the other transition to append to this one.
		@return a reference to this appended transition.
		*/
		FMTTransition& operator+=(const FMTTransition& OtherTransition);
		// DocString: FMTTransition()
		/**
		@brief Default constructor for FMTTransition.
		*/
        FMTTransition();
		// DocString: ~FMTTransition()
		/**
		@brief Default destructor for FMTTransition.
		*/
		~FMTTransition() = default;
		// DocString: FMTTransition(const std::string&)
		/**
		@brief Construct a transition keeping only its name.
		@param[in] lname the name of the transition.
		*/
		FMTTransition(const std::string& lname);
		// DocString: FMTTransition(const FMTTransition&)
		/**
		@brief Copy constructor for FMTTransition.
		@param[in] rhs the FMTTransition to copy.
		*/
        FMTTransition(const FMTTransition& rhs);
		// DocString: FMTTransition::operator=
		/**
		@brief Copy assignment operator for FMTTransition.
		@param[in] rhs the FMTTransition to copy.
		@return a reference to this FMTTransition.
		*/
        FMTTransition& operator = (const FMTTransition& rhs);
		// DocString: FMTTransition::operator std::string
		/**
		@brief Convert the transition to a string as in a transition file.
		@return the string representation of the transition.
		*/
        operator std::string() const;
		// DocString: FMTTransition::single
		/**
		@brief Return a transition that leads to a single development path, selecting the one with the highest area proportion, for cell based models.
		@return the single path transition.
		*/
        FMTTransition single() const;
		// DocString: FMTTransition::getName
		/**
		@brief Get the name of the transition.
		@return the name of the transition.
		*/
		inline std::string getName() const
			{
			return name;
			}
		// DocString: FMTTransition::setName
		/**
		@brief Set the name of the transition.
		@param[in] p_name the name to set.
		*/
		void setName(const std::string& p_name);
		// DocString: FMTTransition::isLeaking
		/**
		@brief Return true if a fork has a total proportion different from 1.
		@return true if the transition is leaking else false.
		*/
		bool isLeaking() const;
		// DocString: FMTTransition::getFork
		/**
		@brief Return the first useful fork for which the development can be disturbed based on the yields.
		@param[in] development the development.
		@param[in] ylds the yields.
		@return a pointer to the fork, or nullptr.
		*/
        const FMTFork* getFork(const Core::FMTDevelopment& development, const FMTYields& ylds) const;
		// DocString: FMTTransition::getStaticThemes
		/**
		@brief Return the themes that have no attribute change in the transition masks (static themes).
		@param[in] themes the themes to filter.
		@return the static themes.
		*/
		std::vector<const FMTTheme*>getStaticThemes(const std::vector<const FMTTheme*>& themes) const;
		// DocString: FMTTransition::canProduce
		/**
		@brief Return all the source masks that can produce the given mask through the transition.
		@param[in] testmask the mask to produce.
		@param[in] themes the themes.
		@return the source masks that can produce the mask.
		*/
		std::vector<Core::FMTMask> canProduce(const Core::FMTMask& testmask,const std::vector<Core::FMTTheme>& themes) const;
		// DocString: FMTTransition::ageAfter
		/**
		@brief Return the average age after disturbance across the developments, for GCBM.
		@param[in] devs the developments.
		@param[in] action the action.
		@param[in] ylds the yields.
		@param[in] themes the themes.
		@return the average age after disturbance.
		*/
		unsigned int ageAfter(const std::vector<FMTDevelopment>& devs,
			const FMTAction& action,
			const FMTYields& ylds,
			const std::vector<FMTTheme>& themes) const;
		// DocString: FMTTransition::mainTarget
		/**
		@brief Return the most targeted mask across the developments based on the yields, for GCBM.
		@param[in] devs the developments.
		@param[in] ylds the yields.
		@return the main target mask.
		*/
        FMTMask mainTarget(const std::vector<FMTDevelopment>& devs,
                            const FMTYields& ylds) const;
		// DocString: FMTTransition::attributeTargets
		/**
		@brief Classify the developments by mask key based on the yields.
		@param[in] devs the developments.
		@param[in] ylds the yields.
		@param[in] themes the themes.
		@return a map of mask string (separated by -) to developments.
		*/
		std::map<std::string, std::vector<FMTDevelopment>> attributeTargets(const std::vector<FMTDevelopment>& devs,
											const FMTYields& ylds, const std::vector<FMTTheme>& themes) const;
		// DocString: FMTTransition::operator<
		/**
		@brief Less than comparison operator of FMTTransition.
		@param[in] rhs the transition to compare with.
		@return true if this transition is less than rhs else false.
		*/
        bool operator < (const FMTTransition& rhs) const;
		// DocString: FMTTransition::operator==
		/**
		@brief Equality comparison operator of FMTTransition, checking if they have the same name.
		@param[in] rhs the transition to compare with.
		@return true if both transitions have the same name else false.
		*/
        bool operator == (const FMTTransition& rhs) const;
		// DocString: FMTTransition::operator!=
		/**
		@brief Inequality comparison operator of FMTTransition, checking if they have a different name.
		@param[in] rhs the transition to compare with.
		@return true if both transitions have a different name else false.
		*/
        bool operator != (const FMTTransition& rhs) const;
		// DocString: FMTTransition::presolve
		/**
		@brief Return a presolved transition based on a filter, the original themes and the new presolved themes.
		@param[in] filter the mask filter.
		@param[in] originalthemes the original themes.
		@param[in,out] newthemes the presolved themes.
		@param[in] compressdata if true compresses the data.
		@return the presolved transition.
		*/
		FMTTransition presolve(const FMTMaskFilter& filter,
			const std::vector<FMTTheme>& originalthemes,
			std::vector<FMTTheme>& newthemes,bool compressdata=false) const;
		// DocString: FMTTransition::presolveRef
		/**
		@brief Presolve the transition in place based on a filter, the original themes and the new presolved themes.
		@param[in] p_filter the mask filter.
		@param[in] p_originalThemes the original themes.
		@param[in,out] p_newthemes the presolved themes.
		@param[in] p_compressdata if true compresses the data.
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
		@brief Serialize the FMTTransition through its base FMTList<FMTFork> for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
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
@brief Comparator used to compare FMTTransition with std::find_if when transitions are in a std container.
*/
class FMTTransitionComparator
{
public:
	// DocString: FMTTransitionComparator(std::string)
	/**
	@brief Construct the comparator from the transition name to match.
	@param[in] name the name of the transition to match.
	*/
	FMTTransitionComparator(std::string name);
	// DocString: FMTTransitionComparator(const FMTTransition&)
	/**
	@brief Matching test operator for the comparator.
	@param[in] transition the transition to test.
	@return true if the transition matches else false.
	*/
	bool operator()(const FMTTransition& transition) const;

private:
	// DocString: FMTTransitionComparator::m_transitionName
	///transition name that we are looking for in the std
	std::string m_transitionName;
};

}

BOOST_CLASS_EXPORT_KEY(Core::FMTTransition)
#endif // FMTTRA_Hm_included
