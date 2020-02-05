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

#ifndef FMTMODEL_H_INCLUDED
#define FMTMODEL_H_INCLUDED

#include <vector>
#include <string>

#include "FMTtheme.h"
#include "FMTaction.h"
#include "FMTtransition.h"
#include "FMTyields.h"
#include "FMTlifespans.h"
#include "FMTactualdevelopment.h"
#include "FMTconstants.h"
#include "FMToutput.h"
#include "FMTconstraint.h"
#include <memory>
#include "FMTgraph.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>




namespace Models
{


class FMTmodel : public Core::FMTobject
    {
    friend class Graph::FMTgraph;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(area);
		ar & BOOST_SERIALIZATION_NVP(themes);
		ar & BOOST_SERIALIZATION_NVP(actions);
		ar & BOOST_SERIALIZATION_NVP(action_aggregates);
		ar & BOOST_SERIALIZATION_NVP(transitions);
		ar & BOOST_SERIALIZATION_NVP(yields);
		ar & BOOST_SERIALIZATION_NVP(lifespan);
		ar & BOOST_SERIALIZATION_NVP(outputs);
		ar & BOOST_SERIALIZATION_NVP(constraints);
		ar & BOOST_SERIALIZATION_NVP(name);
	}
    protected:
		std::vector<Core::FMTactualdevelopment>area;
		std::vector<Core::FMTtheme>themes;
		std::vector<Core::FMTaction>actions;
		std::map<std::string, std::vector<std::string>>action_aggregates;
		std::vector<Core::FMTtransition>transitions;
		Core::FMTyields yields;
		Core::FMTlifespans lifespan;
		std::vector<Core::FMToutput> outputs;
		std::vector<Core::FMTconstraint>constraints;
		void setdefaultobjects();
    public:
        std::string name;
		FMTmodel(const std::vector<Core::FMTactualdevelopment>& larea, const std::vector<Core::FMTtheme>& lthemes,
			const std::vector<Core::FMTaction>& lactions, const std::map<std::string, std::vector<std::string>>& laction_aggregates,
             const std::vector<Core::FMTtransition>& ltransitions,const Core::FMTyields& lyields,const Core::FMTlifespans& llifespan,
			const std::string& lname,const std::vector<Core::FMToutput>& loutputs);
		FMTmodel(const std::vector<Core::FMTactualdevelopment>& larea, const std::vector<Core::FMTtheme>& lthemes,
			const std::vector<Core::FMTaction>& lactions, const std::map<std::string, std::vector<std::string>>& laction_aggregates,
			const std::vector<Core::FMTtransition>& ltransitions, const Core::FMTyields& lyields, const Core::FMTlifespans& llifespan,
			const std::string& lname, const std::vector<Core::FMToutput>& loutputs, const std::vector<Core::FMTconstraint>& lconstraints);
    FMTmodel();
    virtual ~FMTmodel()=default;
    virtual std::vector<Core::FMTactualdevelopment>getarea(int period = 0) const;
	void cleanactionsntransitions();
	std::vector<Core::FMTtheme>getthemes() const;
	std::vector<Core::FMTaction>getactions() const;
	std::map<std::string, std::vector<std::string>> getactionaggregates() const;
	std::vector<Core::FMTtransition>gettransitions() const;
	Core::FMTyields getyields() const;
	Core::FMTlifespans getlifespan() const;
	std::vector<Core::FMToutput> getoutputs() const;
	static Core::FMTaction defaultdeathaction(const Core::FMTlifespans& llifespan,
										const std::vector<Core::FMTtheme>& lthemes);
	static Core::FMTtransition defaultdeathtransition(const Core::FMTlifespans& llifespan,
										const std::vector<Core::FMTtheme>& lthemes);
	std::vector<Core::FMTconstraint>getconstraints() const;
	bool addoutput(const std::string& name,const std::string& maskstring, FMTotar outputtarget,
		std::string action = std::string(), std::string yield = std::string(), std::string description = std::string(),int targettheme = -1);
	void setconstraints(const std::vector<Core::FMTconstraint>& lconstraint);
	bool operator == (const FMTmodel& rhs) const;
    bool setarea(const std::vector<Core::FMTactualdevelopment>& ldevs);
    bool setthemes(const std::vector<Core::FMTtheme>& lthemes);
    bool setactions(const std::vector<Core::FMTaction>& lactions);
    bool settransitions(const std::vector<Core::FMTtransition>& ltransitions);
    bool setyields(const Core::FMTyields& lylds);
    bool setlifespan(const Core::FMTlifespans& llifespan);
    bool valid();
	std::vector<Core::FMTtheme> locatestaticthemes() const;
    FMTmodel(const FMTmodel& rhs);
    FMTmodel& operator = (const FMTmodel& rhs);
    };

class FMTmodelcomparator
{
	std::string model_name;
public:
	FMTmodelcomparator(std::string name);
	bool operator()(const FMTmodel& model) const;

};

}

#endif // FMTMODEL_H_INCLUDED
