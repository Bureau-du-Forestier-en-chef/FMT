#include "FMToutputsource.h"

namespace Core{

FMToutputsource::FMToutputsource() : FMTspec(), mask(),
		target(FMTotar::val),
		action(),
		yield(),
		values()
	{

	}


FMToutputsource::FMToutputsource(const FMTotar ltarget,double lvalue, string lyield,string laction) : FMTspec(),mask(),
    target(ltarget),
    action(laction),
    yield(lyield),
    values(1,lvalue)
    {

    }


FMToutputsource::FMToutputsource(const FMTotar ltarget,vector<double>lvalues): FMTspec(),mask(),
    target(ltarget),
    action(),
    yield(),
    values(lvalues)
    {



    }

FMToutputsource::FMToutputsource(const FMTspec& spec,const FMTmask& lmask,
               const FMTotar ltarget,string lyield,
               string laction) :
                FMTspec(spec),mask(lmask),
                target(ltarget),action(laction),yield(lyield),values()
    {


    }

FMToutputsource::FMToutputsource(const FMToutputsource& rhs) : FMTspec(rhs),mask(rhs.mask),target(rhs.target),
    action(rhs.action),yield(rhs.yield),values(rhs.values)
    {

    }

FMToutputsource& FMToutputsource::operator = (const FMToutputsource& rhs)
    {
    if(this!=&rhs)
        {
        target = rhs.target;
        values = rhs.values;
        action = rhs.action;
        yield = rhs.yield;
        FMTspec::operator=(rhs);
        mask = rhs.mask;
        }
    return *this;
    }

FMToutputsource::operator string() const
    {
	string line = "";
	if (!mask.empty())
		{
		line += string(mask)+" ";
		}
	if (!FMTspec::empty())
		{
		line += FMTspec::operator string() + " ";
		}
    switch (target)
        {
        case FMTotar::val:
            for (const double lvalue : values)
                {
                line += std::to_string(lvalue) + " ";
                }
            if (!line.empty())
                {
                line.pop_back();
                }
        break;
		case FMTotar::timeyld:
			line += yield;
		break;
        case FMTotar::actual:
            if (!action.empty())
                {
                line += action +" ";
                }
            if (!yield.empty())
                {
                line += yield+" ";
                }else{
                line+="_AREA";
                }
        break;
        case FMTotar::inventory:
            if (!action.empty())
                {
                line+="_INVENT("+action+") ";
				}
				else if (this->lock.getlower() > 0)
				{
				line += "_INVLOCK ";
				}else{
                line+= "_INVENT ";
                }
            if (!yield.empty())
                {
                line += yield;
                }else{
                line+="_AREA";
                }
        break;
        case FMTotar::level:
            if (!action.empty())
                {
                 line += action;
                }else{
                for (const double lvalue : values)
                    {
                    line += std::to_string(lvalue) + " ";
                    }
                 if (!line.empty())
                    {
                    line.pop_back();
                    }
                }
        break;
        default:
            line+="";
        break;
        };
    return line;
    }

bool FMToutputsource::operator == (const FMToutputsource& rhs) const
	{
	return (mask==rhs.mask && target==rhs.target &&
		action == rhs.action && yield == rhs.yield && values == rhs.values);
	}

bool FMToutputsource::operator != (const FMToutputsource& rhs) const
	{
	return (!(*this==rhs));
	}

const FMTmask& FMToutputsource::getmask() const
	{
	return mask;
	}

bool FMToutputsource::issubsetof(const FMToutputsource& rhs,
	const map<string, vector<string>>& actaggregates) const
	{
	if ((this->isvariable() && rhs.isvariable() && 
		target == rhs.target && FMTspec::issubsetof(rhs) && 
		!((!action.empty() && rhs.action.empty()) || (!rhs.action.empty() && action.empty()))) && 
		(mask.data.is_subset_of(rhs.mask.data) && 
		((action.empty() && rhs.action.empty()) || 
		(!action.empty() && !rhs.action.empty() && 
		(action == rhs.action || (actaggregates.find(rhs.action) != actaggregates.end() && 
		std::find(actaggregates.at(rhs.action).begin(), actaggregates.at(rhs.action).end(), action) != actaggregates.at(rhs.action).end()))))))
			{
			return true;
			}
	return false;
	}

bool FMToutputsource::isvariable() const
	{
	return bool(mask);
	}

bool FMToutputsource::islevel() const
	{
	return (target == FMTotar::level);
	}

bool FMToutputsource::isvariablelevel() const
	{
	return (!action.empty() && islevel());
	}

bool FMToutputsource::isconstant() const
	{
	return (target == FMTotar::val);
	}

string FMToutputsource::getlevel() const
	{
	return yield;
	}

bool FMToutputsource::isnull(const FMTyields& ylds) const
	{
	if (!yield.empty())
		{
		return ylds.isnullyld(yield);
		}
	if (!isvariable())
		{
		return (getvalue() == 0);
		}
	return false;
	}

bool FMToutputsource::istimeyield() const
	{
	return (target == FMTotar::timeyld);
	}
double FMToutputsource::getvalue(int period) const
	{
	double returnvalue = 0;
	if (target == FMTotar::val||target == FMTotar::level)
		{
        if (period >= int(values.size()))
            {
            period = static_cast<int>(values.size() - 1);
            }
		returnvalue= values.at(period);
		}
	return returnvalue;
	}

void FMToutputsource::setmask(const FMTmask& newmask)
	{
	mask = newmask;
	}


string FMToutputsource::getaction() const
	{
	return action;
	}

string FMToutputsource::getyield() const
	{
	return yield;
	}
FMTotar FMToutputsource::gettarget() const
	{
	return target;
	}

bool FMToutputsource::use(const FMTdevelopment& development, const FMTyields& ylds) const
	{
	if (mask)
	{
		if (development.mask.data.is_subset_of(mask.data) && development.is(*this, ylds))
		{
			return true;
		}
	}
	return false;
	}

vector<int>FMToutputsource::targets(const vector<FMTaction>& actions,
			const map<string,vector<string>>& aggregates) const
	{
	vector<int>action_IDS;
	if (target != FMTotar::level)
        {
        vector<string>ordered_action(actions.size());
        int id = 0;
        for (const FMTaction& theaction : actions)
            {
            ordered_action[id] = theaction.name;
            ++id;
            }
        if (!action.empty())
        {
            vector<string>::iterator ait = find(ordered_action.begin(), ordered_action.end(), action);
            if (aggregates.find(action) != aggregates.end())
            {
                for (const string& actvalue : aggregates.at(action))
                {
                    vector<string>::iterator it = find(ordered_action.begin(), ordered_action.end(), actvalue);
                    if (it != ordered_action.end())
                    {
                        action_IDS.push_back(static_cast<int>(std::distance(ordered_action.begin(), it)));
                    }

                }
            }else if (ait!= ordered_action.end()){
                action_IDS.push_back(static_cast<int>(std::distance(ordered_action.begin(),ait)));
            }
        }
        }
	return action_IDS;
	}

bool FMToutputsource::useinedges() const
	{
	return (target == FMTotar::inventory);
	}

bool FMToutputsource::useoutedges() const
	{
	return (target == FMTotar::actual);
	}


/*vector<boost::dynamic_bitset<>> FMToutputsource::getclassifiers(const vector<FMTaction>& actions,
	const map<string, vector<string>>& aggregates,
	const int& maxage, const int& maxperiod, const int& periodid) const
	{

	vector<boost::dynamic_bitset<>> periods = linearcombination(periodid, periodid+1, maxperiod+1);
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<" period size"<< periods.size()<< "\n";
	boost::dynamic_bitset<> actionselection = actionbit(actions, aggregates);
	vector<boost::dynamic_bitset<>> actionsbits = actionscombination(actionselection);
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<< "action size " << actionsbits.size() << "\n";
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "MAX AGE "<<localmaxage<<" MINAGE "<< minage << "\n";
	//vector<boost::dynamic_bitset<>> ages = linearcombination(minage, localmaxage+1, maxage+1);
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << ages.size() << "\n";
	vector<bool> start_periods;
	if (useinedges())
		{
		start_periods.push_back(true);
		}else {
		start_periods.push_back(true);
		start_periods.push_back(false);
		}
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "starts size " << start_periods.size() << "\n";
	vector<bool>locked;
	if (!this->emptylock())
		{
		locked.push_back(true);
		}else {
		locked.push_back(true);
		locked.push_back(false);
		}
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "lcoked size " << locked.size() << "\n";
	vector<boost::dynamic_bitset<>>bits;
	//boost::dynamic_bitset<>newkey(periods[0].size() + actionsbits[0].size() + ages[0].size() + 2);
	boost::dynamic_bitset<>newkey(periods[0].size() + actionsbits[0].size() + 2);
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "FULL SIZE "<<newkey.size() << "\n";
	for (const boost::dynamic_bitset<>& period : periods)
		{
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "period size " << period.size() << "\n";
		int bid = 0;
		for (;bid < period.size();++bid)
			{
			newkey[bid] = period[bid];
			}
		for (const boost::dynamic_bitset<>& action : actionsbits)
			{
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "action size " << action.size() << "\n";
			bid = period.size();
			for (int actid =0; actid < action.size(); ++actid)
				{
				newkey[bid] = action[actid];
				++bid;
				}
			//for (const boost::dynamic_bitset<>& age : ages)
				//{
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "age size " << age.size() << "\n";

				for (bool start : start_periods)
					{
					bid = period.size() + action.size();
					newkey[bid] = start;
					//bid = 10
					for (bool lock : locked)
						{
						bid = period.size() + action.size() + 1;
						newkey[bid] = lock;



						bits.push_back(newkey);
						}

					}
				//}

			}

		}
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "decmpositions:  " << bits.size() << "\n";
	return bits;
	}*/

/*
boost::dynamic_bitset<> FMToutputsource::actionbit(const vector<FMTaction>& actions,
	const map<string, vector<string>>& aggregates) const
	{
	vector<int>selected = targets(actions, aggregates);
	boost::dynamic_bitset<>newbits(actions.size(), false);
	for (const int& act : selected)
		{
		newbits[act] = true;
		}
	return newbits;
	}*/
/*
vector<boost::dynamic_bitset<>> FMToutputsource::linearcombination(const int& minclass, const int& maxclass, const int& lenght) const
	{
	vector<boost::dynamic_bitset<>>results((maxclass-minclass),boost::dynamic_bitset<>(lenght, false));
	int bitstreamid = 0;
	for (int id = minclass ; id < maxclass; ++id)
		{
		results[bitstreamid][id] = true;
		++bitstreamid;
		}
	return results;
	}*/
/*
vector<boost::dynamic_bitset<>> FMToutputsource::actionscombination(const boost::dynamic_bitset<>& actioned) const
	{
	//https://ide.geeksforgeeks.org/niipMp
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<"ACTIONED " <<actioned.size()<< "\n";
	size_t nbits = actioned.size() - actioned.count();
	vector<boost::dynamic_bitset<>>results;
	boost::dynamic_bitset<>allfalse(actioned.size(), false);
	int bidstack = 0;
	for (int bid = 0; bid < actioned.size(); ++bid)
		{
		if (actioned[bid])
			{
			allfalse[bid] = true;
			}

		}
	results.push_back(allfalse);
	for (int len = 1; len <= nbits; len++)
		{
		int zero_num = nbits - len;
		vector<bool>newb(zero_num,false);
		for (int j = 0; j < len; ++j)
			{
			newb.push_back(true);
			}
		do {
		boost::dynamic_bitset<>newbit(actioned.size(), false);
		int bidstack = 0;
		for (int bid = 0; bid < actioned.size(); ++bid)
			{
				if (actioned[bid])
				{
					newbit[bid] = true;
				}
				else {
					newbit[bid] = newb[bidstack];
					++bidstack;
				}
			//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << newbit[bid]<<" ";
			}
		//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "\n";
		results.push_back(newbit);
		}while(next_permutation(newb.begin(), newb.end()));
		}
	return results;
	}*/

double FMToutputsource::getcoef(const FMTdevelopment& development,
	const FMTyields& yields, const FMTaction& modelaction,
	const vector<FMTdevelopmentpath>& paths) const
	{
	double coef = 1;
	if (isvariable())
		{
		if (!yield.empty())
			{
			if (target == FMTotar::inventory)
			{
				coef = development.getinventorycoef(yields, yield);
			}
			else {
				coef = development.getharvestcoef(paths, modelaction, yields, yield);
			}
			}
		}else if (istimeyield())
			{
			coef = development.getinventorycoef(yields, yield);
			}else {
			coef = values.at(0);
			}
	return coef;
	}

size_t FMToutputsource::hash(int period) const
	{
	size_t seed = 0;
	boost::hash_combine(seed,mask);
	boost::hash_combine(seed,target);
	boost::hash_combine(seed,action);
	//boost::hash_combine(seed,yield);
	for (const double& lvalue : values)
        {
        boost::hash_combine(seed,lvalue);
        }
	//boost::hash_combine(seed,value);
	boost::hash_combine(seed,FMTspec::hash());
	if (period >= 0)
		{
		boost::hash_combine(seed, period);
		}
	return seed;
	}

FMToutputsourcecomparator::FMToutputsourcecomparator(bool lvariable) : variable(lvariable) {}
bool FMToutputsourcecomparator::operator()(const FMToutputsource& source) const
	{
	if (variable)
		{
		return source.isvariable();
	}else {
		return source.isconstant();
	}
	return false;
	}



}
