/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTmodelstats.h"

namespace Models
{
	FMTmodelstats& FMTmodelstats::operator += (const FMTmodelstats& rhs)
		{
		themes += rhs.themes;
		themesdata += rhs.themesdata;
		actions += rhs.actions;
		actionsdata += rhs.actionsdata;
		transitions += rhs.transitions;
		transitionsdata += rhs.transitionsdata;
		yieldsdata += rhs.yieldsdata;
		lifespansdata += rhs.lifespansdata;
		outputs += rhs.outputs;
		outputsdata += rhs.outputsdata;
		constraints += rhs.constraints;
		constraintsdata += rhs.constraintsdata;
		return *this;
		}
	FMTmodelstats& FMTmodelstats::operator -= (const FMTmodelstats& rhs)
		{
		themes -= rhs.themes;
		themesdata -= rhs.themesdata;
		actions -= rhs.actions;
		actionsdata -= rhs.actionsdata;
		transitions -= rhs.transitions;
		transitionsdata -= rhs.transitionsdata;
		yieldsdata -= rhs.yieldsdata;
		lifespansdata -= rhs.lifespansdata;
		outputs -= rhs.outputs;
		outputsdata -= rhs.outputsdata;
		constraints -= rhs.constraints;
		constraintsdata -= rhs.constraintsdata;
		return *this;
		}
	FMTmodelstats FMTmodelstats::operator + (const FMTmodelstats& rhs)
		{
		FMTmodelstats newstats(*this);
		newstats.themes += rhs.themes;
		newstats.themesdata += rhs.themesdata;
		newstats.actions += rhs.actions;
		newstats.actionsdata += rhs.actionsdata;
		newstats.transitions += rhs.transitions;
		newstats.transitionsdata += rhs.transitionsdata;
		newstats.yieldsdata += rhs.yieldsdata;
		newstats.lifespansdata += rhs.lifespansdata;
		newstats.outputs += rhs.outputs;
		newstats.outputsdata += rhs.outputsdata;
		newstats.constraints += rhs.constraints;
		newstats.constraintsdata += rhs.constraintsdata;
		return newstats;
		}
	FMTmodelstats FMTmodelstats::operator - (const FMTmodelstats& rhs)
		{
		FMTmodelstats newstats(*this);
		newstats.themes -= rhs.themes;
		newstats.themesdata -= rhs.themesdata;
		newstats.actions -= rhs.actions;
		newstats.actionsdata -= rhs.actionsdata;
		newstats.transitions -= rhs.transitions;
		newstats.transitionsdata -= rhs.transitionsdata;
		newstats.yieldsdata -= rhs.yieldsdata;
		newstats.lifespansdata -= rhs.lifespansdata;
		newstats.outputs -= rhs.outputs;
		newstats.outputsdata -= rhs.outputsdata;
		newstats.constraints -= rhs.constraints;
		newstats.constraintsdata -= rhs.constraintsdata;
		return newstats;
		}
	bool FMTmodelstats::operator == (const FMTmodelstats& rhs) const
		{
		return (themes == rhs.themes&&
		themesdata == rhs.themesdata&&
		actions == rhs.actions&&
		actionsdata == rhs.actionsdata&&
		transitions == rhs.transitions&&
		transitionsdata == rhs.transitionsdata&&
		yieldsdata == rhs.yieldsdata&&
		lifespansdata == rhs.lifespansdata&&
		outputs == rhs.outputs&&
		outputsdata == rhs.outputsdata&&
		constraints == rhs.constraints&&
		constraintsdata == rhs.constraintsdata);
		}
	bool FMTmodelstats::operator != (const FMTmodelstats& rhs) const
		{
		return (!(*this==rhs));
		}
	FMTmodelstats::FMTmodelstats(const size_t& lthemes, const size_t& lthemesdata, const size_t& lactions, const size_t& lactionsdata,
		const size_t& ltransitions, const size_t& ltransitionsdata, const size_t& lyieldsdata, const size_t& llifespansdata,
		const size_t& loutputs, const size_t& loutputsdata, const size_t& lconstraints, const size_t& lconstraintsdata) :
		themes(static_cast<int>(lthemes)),
		themesdata(static_cast<int>(lthemesdata)),
		actions(static_cast<int>(lactions)),
		actionsdata(static_cast<int>(lactionsdata)),
		transitions(static_cast<int>(ltransitions)),
		transitionsdata(static_cast<int>(ltransitionsdata)),
		yieldsdata(static_cast<int>(lyieldsdata)),
		lifespansdata(static_cast<int>(llifespansdata)),
		outputs(static_cast<int>(loutputs)),
		outputsdata(static_cast<int>(loutputsdata)),
		constraints(static_cast<int>(lconstraints)),
		constraintsdata(static_cast<int>(lconstraintsdata))
		{

		}




	FMTmodelstats::operator std::string() const
		{
		std::string values = "";
		values += "Themes: " + std::to_string(themes);
		values += " Themes data: " + std::to_string(themesdata);
		values += " Actions: " + std::to_string(actions);
		values += " Actions data: " + std::to_string(actionsdata);
		values += " Transitions: " + std::to_string(transitions);
		values += " Transitions data: " + std::to_string(transitionsdata);
		values += " Yields data: " + std::to_string(yieldsdata);
		values += " Lifespans data: " + std::to_string(lifespansdata);
		values += " Outputs: " + std::to_string(outputs);
		values += " Outputs data: " + std::to_string(outputsdata);
		values += " Constraints: " + std::to_string(constraints);
		values += " Constraints data: " + std::to_string(constraintsdata);
		return values;
		}

}
