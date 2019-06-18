#include "FMToutput.h"

namespace Core{


FMToutput::FMToutput(const string& lname,const string& ldescription,const int& ltheme_target,
	vector<FMToutputsource>& lsources,vector<FMToperator>& loperators/*,bool islevel*/):
    sources(lsources),
    operators(loperators),
	theme_target(ltheme_target),
	//level(islevel),
    name(lname),
    description(ldescription)

    {

    }
FMToutput::FMToutput(const string& lname) : sources(),operators(), theme_target(-1),/*level(false),*/name(lname),description()
    {

    }
FMToutput::FMToutput() : sources(),operators(), theme_target(-1),/*level(false),*/name(),description()
    {

    }
vector<FMToutputsource> FMToutput::getsources() const
    {
    return sources;
    }
vector<FMToperator> FMToutput::getopes() const
    {
    return operators;
    }
FMToutput::FMToutput(const FMToutput& rhs) :
    sources(rhs.sources),
    operators(rhs.operators),
	theme_target(rhs.theme_target),
	/*level(rhs.level),*/
    name(rhs.name),
    description(rhs.description)
    {

    }
FMToutput& FMToutput::operator = (const FMToutput& rhs)
    {
    if (this!=&rhs)
        {
        name = rhs.name;
        sources = rhs.sources;
		theme_target =rhs.theme_target;
		/*level = rhs.level;*/
        operators = rhs.operators;
        description = rhs.description;
        }
    return *this;
    }
FMToutput& FMToutput::operator +=(const FMToutput& rhs)
	{
	if (!this->name.empty())
		{
		this->name = this->name + "+" + rhs.name;
		this->description = this->description + "+" + rhs.description;
	}else {
		this->name = rhs.name;
		this->description = rhs.description;
		}
    if (!this->sources.empty())
		{
		this->operators.push_back(FMToperator("+"));
		}
	this->sources.insert(this->sources.end(), rhs.sources.begin(), rhs.sources.end());

	this->operators.insert(this->operators.end(),rhs.operators.begin(),rhs.operators.end());
	if (this->theme_target != rhs.theme_target)
		{
		this->theme_target = -1;
		}
	/*if (this->islevel() || rhs.islevel())
		{
		this->level = true;
		}*/
	return *this;
	}
FMToutput& FMToutput::operator -=(const FMToutput& rhs)
	{
	if (!this->name.empty())
		{
		this->name = this->name + "-" + rhs.name;
		this->description = this->description + "-" + rhs.description;
	}else {
		this->name = rhs.name;
		this->description = rhs.description;
		}
    if (!this->sources.empty())
		{
		this->operators.push_back(FMToperator("-"));
		}
	this->sources.insert(this->sources.end(), rhs.sources.begin(), rhs.sources.end());

	if (this->theme_target != rhs.theme_target)
		{
		this->theme_target = -1;
		}
	/*if (this->level || rhs.level)
		{
		this->level = true;
		}*/
	for (const FMToperator& rhsop : rhs.operators)
		{
		operators.push_back(rhsop.reverse());
		}
	return *this;
	}

FMToutput& FMToutput::operator  *= (const double& rhs)
	{
	if (!this->name.empty())
		{
		this->name =  this->name+"*"+to_string(rhs);
		this->description = this->description +"*" + to_string(rhs);
		}
	if (!sources.empty())
		{
		vector<FMToutputsource>new_sources;
		vector<FMToperator>new_operators;
		size_t location = 0;
		for (FMToutputsource& source : sources)
			{
			new_sources.push_back(source);
			if (source.isvariable() || source.isvariablelevel())
				{
                new_operators.push_back(FMToperator("*"));
				new_sources.push_back(FMToutputsource(FMTotar::val, rhs));
				}
			if (location < operators.size())
				{
				new_operators.push_back(operators.at(location));
				}

			++location;
			}
		sources = new_sources;
		operators = new_operators;
		}
	return *this;
	}

FMToutput& FMToutput::operator /=(const double& rhs)
	{
	if (!this->name.empty())
		{
		this->name = this->name + "/" + to_string(rhs);
		this->description = this->description + "/" + to_string(rhs);
		}
	if (!sources.empty())
	{
		vector<FMToutputsource>new_sources;
		vector<FMToperator>new_operators;
		size_t location = 0;
		for (FMToutputsource& source : sources)
		{
			new_sources.push_back(source);
			if (location < operators.size())
			{
				new_operators.push_back(operators.at(location));
			}
			if (source.isvariable() || source.isvariablelevel())
			{
				new_sources.push_back(FMToutputsource(FMTotar::val, rhs));
				new_operators.push_back(FMToperator("/"));
			}
			++location;
		}
		sources = new_sources;
		operators = new_operators;
	}
	return *this;
	}


FMToutput::operator string() const
    {
	string line = "*OUTPUT ";
	if (islevel())
		{
		line = "*LEVEL ";
		}
	line += name;
	if (theme_target!=-1)
		{
		line += " (_TH" + to_string(theme_target + 1) + ")";
		}
	line+=" " + description + "\n";
	if (!sources.empty() && ((islevel() && sources.at(0).getaction().empty()) || (!islevel())))
		{
		line += "*SOURCE ";
		for (size_t id = 0; id < sources.size(); ++id)
			{
			line += string(sources[id]) + " ";
			if (id < operators.size())
			{
				line += string(operators[id]) + " ";
			}
			}
		}
    line+="\n";
    return line;
    }
bool FMToutput::empty() const
	{
	return sources.empty();
	}
bool FMToutput::islevel() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.gettarget() == FMTotar::level)
			{
			return true;
			}
		}
    return false;
	}

bool FMToutput::isconstant() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.gettarget() != FMTotar::val)
			{
			return false;
			}
		}
	return true;
	}

double FMToutput::getconstantvalue() const
	{
	double value = 0;
	vector<double>values;
	if (isconstant())
		{
		for (const FMToutputsource& src : sources)
			{
			values.push_back(src.getvalue());
			}
		value = shuntingyard(values, this->operators);
		}
	return value;
	}


bool FMToutput::containslevel() const
	{
	for (const FMToutputsource& src : sources)
		{
		if (src.islevel())
			{
			return true;
			}
		}
	return false;
	}

bool FMToutput::linear() const
	{
	if (sources.size() > 1 && (find(operators.begin(), operators.end(), FMToperator("*")) != operators.end() ||
		find(operators.begin(), operators.end(), FMToperator("/")) != operators.end()))
		{
		int location = 0;
		int lastnonlinear = -1;
		for (const FMToutputsource& source : sources)
			{
			if (source.isvariable() && lastnonlinear!=-1 && location == (lastnonlinear+1))
				{
				return false;
				}
			if (source.isvariable() && location < int(operators.size()) &&
				(operators[location] == FMToperator("*") || operators[location] == FMToperator("/")))
				{
				lastnonlinear = location;
				}
			++location;
			}

		}
	return true;
	}
double FMToutput::shuntingyard(const vector<double>& sourcevalues,const vector<FMToperator>& simple_operators) const
	{
	map<string, double>mapping;
	int id = 0;
	vector<string>expression_inputs;
	string test = "";
	for (const double& value : sourcevalues)
		{
		expression_inputs.push_back(to_string(value));
		test += to_string(value);
		if (id < simple_operators.size())
			{
			expression_inputs.push_back(simple_operators[id]);
			test += simple_operators[id];
			}
		//mapping[sources[id]] = value;
		++id;
		}
	//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "EXPRESSION: " << test << "\n";
	FMTexpression newexpression(expression_inputs);
	return newexpression.shuntingyard(mapping);
	}

FMToutput FMToutput::boundto(const vector<FMTtheme>& themes, const FMTperbounds& bound, string attribute) const
	{
	FMToutput newoutput(*this);
	if (!attribute.empty())
		{
		newoutput.name = newoutput.name + "("+ attribute +")";
		}
	if (!bound.empty())
		{
		newoutput.name = newoutput.name + "["+ to_string(bound.getlower()) +"]";
		}
	for (FMToutputsource& source : newoutput.sources)
		{
		if (!bound.empty())
			{
			source.setbounds(bound);
			}
		if (!attribute.empty())
			{
			FMTmask oldmask = FMTmask(source.getmask());
			if (oldmask.set(themes.at(theme_target),attribute))
				{
				source.setmask(oldmask);
				}
			}
		}
	return newoutput;
	}

vector<FMToutputnode> FMToutput::getnodes() const
	{
	//set a expression and get the nodes! check if the node is positive or negative accross the equation!!!
	vector<FMToutputnode>nodes;
	size_t src_id = 0;
	size_t op_id = 0;
	FMToutputsource main_source;
	FMToutputsource main_factor = FMToutputsource(FMTotar::val,1);
	double constant = 1;
	//if (!islevel())
		//{
         //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<" source size "<<sources.size()<<"\n";
		for (const FMToutputsource& source : sources)
			{
             //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<" element: "<< source.getmask().getstr()<<" "<<source.isconstant()<<" "<<source.getvalue()<<" "<<"\n";
            if ((source.isvariable() || source.islevel()))
                {
                //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"non empty main? "<<!main_source.getmask().empty()<<"\n";
                if (!main_source.getmask().empty()||main_source.isvariablelevel())
                    {
                    //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"added1 "<<string(main_source)<<" factor "<<string(main_factor)<<" constant "<<constant<<"\n";
					FMToutputnode newnode(main_source, main_factor, constant);
					if (!newnode.isnull())
						{
						//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "GOT NULL!!! " << string(main_source) << " factor ";
						nodes.push_back(newnode);
						}
                    }
				main_factor = FMToutputsource(FMTotar::val, 1);
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"got source! "<<string(source)<<" "<<source.getaction()<<"\n";
                main_source = source;
                constant = 1;
                }
            if (src_id!=0 && (op_id < operators.size()) && !operators.at(op_id).isfactor())
                {
                constant *= operators.at(op_id).call(0,1);
                //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<" OPSSS1 "<< string(operators.at(op_id))<<"\n";
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " constant of1: " << constant << "\n";
                }
            if (source.istimeyield())
				{
				main_factor = source;
			}else if (source.isconstant() && (op_id < operators.size()) && operators.at(op_id).isfactor())
				{
				double value = source.getvalue();
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << " OPSSS2 " << string(operators.at(op_id)) << "\n";
                constant = operators.at(op_id).call(constant, value);
               // Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<" constant of2: "<< constant<<"\n";
				}

			if (src_id > 0)
				{
				++op_id;
				}
            //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<" done: "<< src_id<<"\n";
			++src_id;
			}

		//if (!main_source.empty())
			//{
        //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<" done2: "<<string(main_source)<<"\n";
       // Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) <<"added2 "<<string(main_source)<<" factor "<<string(main_factor)<<" constant "<<constant<<"\n";
       if (main_source.isvariablelevel() || main_source.isvariable())
            {
		   FMToutputnode newnode(main_source, main_factor, constant);
		   if (!newnode.isnull())
				{
			    nodes.push_back(newnode);
				}
           }

			//}
		//}
    //Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<" output of: "<< name<<"\n";
    // Logging::FMTlogger(Logging::FMTlogtype::FMT_Info)<<" done node of: "<< nodes.size()<<"\n";
	return nodes;
	}

bool FMToutput::issingleperiod() const
	{
	for (const FMToutputsource& source : this->sources)
		{
		if (source.emptyperiod() || (source.getperiodlowerbound() != source.getperiodupperbound()))
			{
			return false;
			}
		}
	return true;
	}

int FMToutput::gettargetperiod() const
	{
	int target = -1;
	if (issingleperiod())
		{
		return sources.at(0).getperiodlowerbound();
		}
	return target;
	}

size_t FMToutput::hash() const
	{
	size_t seed = 0;
	boost::hash_combine(seed, name);
	for (const FMToperator& op : operators)
		{
		boost::hash_combine(seed,op.hash());
		}
	for (const FMToutputsource& src : sources)
		{
		boost::hash_combine(seed, src.hash());
		}
	return seed;
	}


bool FMToutput::operator == (const FMToutput& rhs) const
    {
    return (name == rhs.name ||
		(!sources.empty() && !rhs.empty() &&
		description == rhs.description &&
		/*level == rhs.level &&*/
		theme_target == rhs.theme_target &&
		sources == rhs.sources &&
		operators == rhs.operators));
    }

bool FMToutput::operator != (const FMToutput& rhs) const
    {
    return !(*this== rhs);
    }

vector<string> FMToutput::getdecomposition(const vector<FMTtheme>& themes) const
	{
	vector<string>validdecomp;
	if (theme_target!=-1)
		{
		int srcid = 0;
		for (const FMToutputsource& source : sources)
			{
			if (source.isvariable())
				{
				FMTmask srcmask = source.getmask();
				vector<string>unique_selection;
				for (const FMTmask& decmask : srcmask.decompose(themes[theme_target]))
					{
					unique_selection.push_back(decmask.get(themes[theme_target]));
					}
				if (srcid==0)
					{
					validdecomp = unique_selection;
				}else {
					vector<string>newvalid;
					set_intersection(validdecomp.begin(), validdecomp.end(),
						unique_selection.begin(), unique_selection.end(), back_inserter(newvalid));
					validdecomp = newvalid;
				}
				++srcid;
				}

			}
		}
	return validdecomp;
	}

FMTtheme FMToutput::targettheme(const vector<FMTtheme>& themes) const
	{
	if (theme_target>=0)
		{
		return themes[theme_target];
		}
	return FMTtheme();
	}


FMToutputcomparator::FMToutputcomparator(string name) : output_name(name)
	{

	}

bool FMToutputcomparator::operator()(const FMToutput& output) const
	{
	return output_name == output.name;
	}

}
