/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTconstraint_H_INCLUDED
#define FMTconstraint_H_INCLUDED

#include "FMToutput.h"
#include <vector>
#include <string>
#include <limits>
#include "FMTbounds.h"
#include "FMToperator.h"
#include "FMTbounds.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Core
{
// DocString: FMTconstrainttype
/**
Constraints and objectives types found in a regular optimization section
also some spatial size/adjacency and greenup constraints for the SAmodel. 
*/
enum  FMTconstrainttype
{
	FMTMAXobjective = 1,
	FMTMINobjective = 2,
	FMTMAXMINobjective = 3,
	FMTMINMAXobjective = 4,
	FMTevenflow = 5,
	FMTnondeclining = 6,
	FMTsequence = 7,
	FMTstandard = 8,///Regular constraint type like output = 1...
	FMTspatialsize = 9,
	FMTspatialadjacency = 10,
	FMTspatialgreenup = 11
};
// DocString: FMTconstraint
/**
FMTconstraint is a representation of a line in the optimize section. an objective is considered a constraint in FMT.
So even for the object a FMTconstraint will be generated using a optimization section.
A FMTconstraint is also a FMToutput with some specification (FMTspec).
The main specification used is the period bounds (1.._LENGTH), RHS and variation (_GOAL).
*/
class FMTconstraint: public FMToutput,public FMTspec
	{
	// DocString: FMTconstraint::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("output", boost::serialization::base_object<FMToutput>(*this));
		ar & boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTspec>(*this));
		ar & BOOST_SERIALIZATION_NVP(type);
	}
	// DocString: FMTconstraint::type
	///This enumerator sets the type of constraint of the FMTconstraint (objective/constraint/evenflow...)
	FMTconstrainttype type;
	// DocString: FMTconstraint::standardstring
	/**
	Function used for string formating for FMTconstraint in to string conversion.
	*/
	void standardstring(std::string& line, std::string& period_bounds, std::string& goal) const;
	// DocString: FMTconstraint::getmaxandmin
	/**
	Returns the maximal and minimal value of a vector of double for the constraint
	*/
	void getmaxandmin(const std::vector<double>& values, double& min,double& max) const;
	// DocString: FMTconstraint::getsum
	/**
	Returns the sum of a vector of double
	*/
	double getsum(const std::vector<double>& values) const;
	// DocString: FMTconstraint::getperiodicvariationcost
	/**
	Returns the variation of harvest for a vector of double
	*/
	double getperiodicvariationcost(const std::vector<double>& values, bool evaluateupper=false) const;

	public:
		// DocString: FMTconstraint()
		/**
		Default constructor for FMTconstraint.
		*/
		FMTconstraint();
		// DocString: FMTconstraint(FMTconstrainttype,const FMToutput&)
		/**
		Partial constructor for FMTconstraint specifying the output and the constraint type.
		*/
		FMTconstraint(FMTconstrainttype ltype, const FMToutput& loutput);
		// DocString: FMTconstraint(const FMTconstraint&)
		/**
		Copy constructor for FMTconstraint.
		*/
		FMTconstraint(const FMTconstraint& rhs);
		// DocString: FMTconstraint::operator=
		/**
		Copy assignment for FMTconstraint.
		*/
		FMTconstraint& operator = (const FMTconstraint& rhs);
		// DocString: FMTconstraint::operator==
		/**
		FMTconstraint equality operator check if FMTconstraint are the same.
		*/
		bool operator == (const FMTconstraint& rhs) const;
		// DocString: FMTconstraint::operator!=
		/**
		FMTconstraint not equality operator check if FMTconstraint are not the same.
		*/
		bool operator != (const FMTconstraint& rhs) const;
		// DocString: FMTconstraint::extravariables
		/**
		Returns true if the FMTconstraint need extravariables in the matrix formulation.
		*/
		bool extravariables() const;
		// DocString: FMTconstraint::isgoal
		/**
		Returns true if the constraint have goal weight
		*/
		bool isgoal() const;
		// DocString: FMTconstraint::getpenalties
		/**
		If the constraint is goal then you can get the name of the penalties of the constraint. like _GOAL(penaltyname,weight)
		*/
		std::vector<std::string>getpenalties(double & sense) const;
		// DocString: FMTconstraint::getvariablelevels
		/**
		If the constraint is a level this function will return the level name of the constraint.
		*/
		std::vector<std::string>getvariablelevels() const;
		// DocString: FMTconstraint::getgoal
		/**
		If the constraint has a goal this function fill up the name of the goal and it's weight.
		*/
		void getgoal(std::string& name,double& value) const;
		// DocString: FMTconstraint::isobjective
		/**
		Returns true if the constraint is an objective.
		*/
		bool isobjective() const;
		// DocString: FMTconstraint::sense
		/**
		If the constraint is an objective the function returns the sense of the objective,
		1 = minimisation, -1 = maximisation.
		*/
		double sense() const;
		// DocString: FMTconstraint::getbounds
		/**
		This function fills up the (lower) and (upper) bound for a given period, if
		the constraint can be applied to this (period). Use with osisolverinterfacer with rowsetbounds().
		*/
		void getbounds(double& lower, double& upper,int period = 0) const;
		// DocString: FMTconstraint::setrhs
		/**
		This functions sets the RHS of this FMTconstraint with a row (lower) and (upper) bound.
		*/
		void setrhs(const double& lower, const double& upper);
		// DocString: FMTconstraint::setlength
		/**
		This functions sets the length of the FMTconstraint
		*/
		void setlength(int firstperiod = 1,int lastperiod = std::numeric_limits<int>::max());
		// DocString: FMTconstraint::getvariations
		/**
		Get the variation for constraint like _EVEN _SEQ,_NDY etc... _EVEN(lowervariation,uppervariation).
		*/
		void getvariations(double& lower, double& upper) const;
		// DocString: FMTconstraint::ismultiple
		/**
		Returns true if the FMTconstraint allow some sort of variability like _EVEN(lowervariation,uppervariation).
		*/
		bool ismultiple() const;
		// DocString: FMTconstraint::acrossperiod
		/**
		Returns true if the FMTconstraint needs so be set across multiple periods like _EVEN,_SEQ,_NDY
		*/
		bool acrossperiod() const;
		// DocString: FMTconstraint::hash
		/**
		Hasghing for FMTconstraint.
		*/
		size_t hash() const;
		// DocString: FMTconstraint::setoutput
		/**
		Setter for the FMToutput of the FMTconstraint.
		*/
		void setoutput(const FMToutput& out);
		// DocString: FMTconstraint::setconstrainttype
		/**
		Setter for the FMTconstrainttype of FMTconstraint.
		*/
		void setconstrainttype(FMTconstrainttype ltype);
		// DocString: FMTconstraint::getconstrainttype
		/**
		Getter for the constrainttype.
		*/
		inline FMTconstrainttype getconstrainttype() const
			{
			return type;
			}
		// DocString: FMTconstraint::operator std::string
		/**
		Get the string reprensentation like in the optimization section  of a FMTconstraint.
		*/
		operator std::string() const;
		// DocString: FMTconstraint::isspatial
		/**
		Returns true if spatial constraint
		*/
		bool isspatial() const;
		// DocString: FMTconstraint::getgroup
		/**
		Returns the group of the constraint (objective = 0,spatial = 1,other = 2 or userdefined)
		*/
		size_t getgroup() const;
		// DocString: FMTconstraint::outputempty()
		/**
		Returns true if the FMTconstraint output is empty.
		*/
		size_t outputempty() const;
		// DocString: FMTconstraint::presolve()
		/**
		Like FMToutput class this function presolve the FMTconstraint and can returns an empty FMTconstraint if 
		based on a (basemask), the (originalthemes), the presolved themes, the presolved actions and presolved yields.
		See FMToutput presolve for more information.
		*/
		FMTconstraint presolve(const FMTmask& basemask,
			const std::vector<FMTtheme>& originalthemes,
			const FMTmask& presolvedmask,
			const std::vector<FMTtheme>& newthemes,
			const std::vector<FMTaction>& actions, const FMTyields& yields) const;
		// DocString: ~FMTconstraint()
		/**
		Default destructor for FMTconstraint
		*/
		~FMTconstraint()=default;
		// DocString: FMTconstraint::evaluate()
		/**
		Given a vector of double this function will return a double value representing the quality of the solution.
		For a real constraint the more the value is close to 0 the better it is. For an objective smaller is better (can be negative).
		*/
		double evaluate(const std::vector<double>& temporalvalues) const;

	};
}

BOOST_CLASS_EXPORT_KEY(Core::FMTconstraint)
#endif
