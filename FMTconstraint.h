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

/**
FMTconstraint is a representation of a line in the optimize section. an objective is considered a constraint in FMT.
So even for the object a FMTconstraint will be generated using a optimization section.
A FMTconstraint is also a FMToutput with some specification (FMTspec).
The main specification used is the period bounds (1.._LENGTH), RHS and variation (_GOAL).
*/
class FMTconstraint: public FMToutput,public FMTspec
	{
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
	///This enumerator sets the type of constraint of the FMTconstraint (objective/constraint/evenflow...)
	FMTconstrainttype type;
	/**
	Function used for string formating for FMTconstraint in to string conversion.
	*/
	void standardstring(std::string& line, std::string& period_bounds, std::string& goal) const;
	public:
		/**
		Default constructor for FMTconstraint.
		*/
		FMTconstraint();
		/**
		Partial constructor for FMTconstraint specifying the output and the constraint type.
		*/
		FMTconstraint(FMTconstrainttype ltype, const FMToutput& loutput);
		/**
		Copy constructor for FMTconstraint.
		*/
		FMTconstraint(const FMTconstraint& rhs);
		/**
		Copy assignment for FMTconstraint.
		*/
		FMTconstraint& operator = (const FMTconstraint& rhs);
		/**
		FMTconstraint equality operator check if FMTconstraint are the same.
		*/
		bool operator == (const FMTconstraint& rhs) const;
		/**
		FMTconstraint not equality operator check if FMTconstraint are not the same.
		*/
		bool operator != (const FMTconstraint& rhs) const;
		/**
		Returns true if the FMTconstraint need extravariables in the matrix formulation.
		*/
		bool extravariables() const;
		/**
		Returns true if the constraint have goal weight
		*/
		bool isgoal() const;
		/**
		If the constraint is goal then you can get the name of the penalties of the constraint. like _GOAL(penaltyname,weight)
		*/
		std::vector<std::string>getpenalties(double & sense) const;
		/**
		If the constraint is a level this function will return the level name of the constraint.
		*/
		std::vector<std::string>getvariablelevels() const;
		/**
		If the constraint has a goal this function fill up the name of the goal and it's weight.
		*/
		void getgoal(std::string& name,double& value) const;
		/**
		Returns true if the constraint is an objective.
		*/
		bool isobjective() const;
		/**
		If the constraint is an objective the function returns the sense of the objective,
		1 = minimisation, -1 = maximisation.
		*/
		double sense() const;
		/**
		This function fills up the (lower) and (upper) bound for a given period, if
		the constraint can be applied to this (period). Use with osisolverinterfacer with rowsetbounds().
		*/
		void getbounds(double& lower, double& upper,int period = 0) const;
		/**
		This functions sets the RHS of this FMTconstraint with a row (lower) and (upper) bound.
		*/
		void setrhs(const double& lower, const double& upper);
		/**
		This functions sets the length of the FMTconstraint
		*/
		void setlength(int firstperiod = 1,int lastperiod = std::numeric_limits<int>::max());
		/**
		Get the variation for constraint like _EVEN _SEQ,_NDY etc... _EVEN(lowervariation,uppervariation).
		*/
		void getvariations(double& lower, double& upper) const;
		/**
		Returns true if the FMTconstraint allow some sort of variability like _EVEN(lowervariation,uppervariation).
		*/
		bool ismultiple() const;
		/**
		Returns true if the FMTconstraint needs so be set across multiple periods like _EVEN,_SEQ,_NDY
		*/
		bool acrossperiod() const;
		/**
		Hasghing for FMTconstraint.
		*/
		size_t hash() const;
		/**
		Setter for the FMToutput of the FMTconstraint.
		*/
		void setoutput(const FMToutput& out);
		/**
		Setter for the FMTconstrainttype of FMTconstraint.
		*/
		void setconstrainttype(FMTconstrainttype ltype);
		/**
		Getter for the constrainttype.
		*/
		inline FMTconstrainttype getconstrainttype() const
			{
			return type;
			}
		/**
		Get the string reprensentation like in the optimization section  of a FMTconstraint.
		*/
		operator std::string() const;
		/**
		Returns true if the FMTconstraint output is empty.
		*/
		size_t outputempty() const;
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
		~FMTconstraint()=default;
	};
}

BOOST_CLASS_EXPORT_KEY(Core::FMTconstraint);
#endif
