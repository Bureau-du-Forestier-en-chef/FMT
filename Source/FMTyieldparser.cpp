/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyields.hpp"
#include "FMTconstants.hpp"
#include "FMTtheme.hpp"
#include "FMTdata.hpp"
#include "FMTyieldmodelpools.hpp"
#include "FMTyieldmodelnep.hpp"
#include "FMTyieldparser.hpp"
#include "FMTageyieldhandler.hpp"
#include "FMTtimeyieldhandler.hpp"
#include "FMTcomplexyieldhandler.hpp"
#include "FMTmodelyieldhandler.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <memory>
#include "FMTfunctioncall.hpp"
#include <set>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>


namespace Parser{


FMTyieldparser::FMTyieldparser():FMTparser(),
    rxyieldtype("^(\\*Y)([^\\s^\\t]*)([\\s\\t]*)(.+)(_OVERRIDE)|^(\\*Y)([^\\s^\\t]*)([\\s\\t]*)(.+)", std::regex_constants::ECMAScript| std::regex_constants::icase),
    rxcomplex("^([^\\s^\\t]*)([\\s\\t]*)((_RANGE)|(_MULTIPLY)|(_SUM)|(_SUBTRACT)|(_YTP)|(_MAI)|(_CAI)|(_DIVIDE)|(_EQUATION)|(_ENDPOINT)|(_DELTA))([\\s\\t]*)(\\()(.+)(\\))", std::regex_constants::ECMAScript| std::regex_constants::icase),
	rxeqs("([\\(\\)\\-\\+\\*\\/]*)([^\\(\\)\\-\\+\\*\\/]*)"),
	rxdiscount("^(_DISCOUNTFACTOR)(\\()([\\s\\t]*[\\d]*)([^,]*)(,)([^,]*)(,)([\\s\\t]*(NONE|HALF|FULL)[\\s\\t]*)(\\))"),
	rxpredictor("^(.+)(_PRED)(\\()(.+)(\\))")
        {
		setsection(Core::FMTsection::Yield);
        }

FMTyieldparser::FMTyieldparser(const FMTyieldparser& rhs) : FMTparser(rhs),rxyieldtype(rhs.rxyieldtype),rxcomplex(rhs.rxcomplex), rxeqs(rhs.rxeqs), rxdiscount(rhs.rxdiscount)
    {
	setsection(Core::FMTsection::Yield);
    }
FMTyieldparser& FMTyieldparser::operator = (const FMTyieldparser& rhs)
    {
    if(this!=&rhs)
        {
        FMTparser::operator = (rhs);
        rxyieldtype = rhs.rxyieldtype;
        rxcomplex = rhs.rxcomplex;
		rxeqs = rhs.rxeqs;
		rxdiscount = rhs.rxdiscount;
		rxpredictor = rhs.rxpredictor;
		setsection(Core::FMTsection::Yield);
        }
    return *this;
    }
Core::FMTyldtype FMTyieldparser::getyldtype(const std::string& value) const
    {
	try {
		if (value == "Y")
		{
			return Core::FMTyldtype::FMTageyld;
		}
		else if (value == "YT")
		{
			return Core::FMTyldtype::FMTtimeyld;
		}
		else if (value == "YC")
		{
			return Core::FMTyldtype::FMTcomplexyld;
		}
		else if (value == "YM")
		{
			return Core::FMTyldtype::FMTmodelyld;
		}
		else {
			_exhandler->raise(Exception::FMTexc::FMTinvalid_yield," at line " + std::to_string(_line),"FMTyieldparser::getyldtype", __LINE__, __FILE__, _section);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch("for " + value,"FMTyieldparser::getyldtype", __LINE__, __FILE__ ,_section);
		}
    return Core::FMTyldtype::FMTageyld;
    }

std::unique_ptr<Core::FMTyieldhandler> FMTyieldparser::gethandler(const Core::FMTmask& mask,const Core::FMTyldtype& yldtype) const
{
	try {
		switch (yldtype)
		{
		case Core::FMTyldtype::FMTageyld:
		{
			return std::unique_ptr<Core::FMTyieldhandler>(new Core::FMTageyieldhandler(mask));
			break;
		}
		case Core::FMTyldtype::FMTtimeyld:
		{
			return std::unique_ptr<Core::FMTyieldhandler>(new Core::FMTtimeyieldhandler(mask));
			break;
		}
		case Core::FMTyldtype::FMTcomplexyld:
		{
			return std::unique_ptr<Core::FMTyieldhandler>(new Core::FMTcomplexyieldhandler(mask));
			break;
		}
		case Core::FMTyldtype::FMTmodelyld:
		{
			return std::unique_ptr<Core::FMTyieldhandler>(new Core::FMTmodelyieldhandler(mask));
			break;
		}
		
		default:
			break;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyieldparser::getyldtype", __LINE__, __FILE__, _section);
	}
	return std::unique_ptr<Core::FMTyieldhandler>();
}

Core::FMTyieldparserop FMTyieldparser::getyldctype(const std::string& value) const
    {
	try {
		if (value == "_RANGE")
		{
			return Core::FMTyieldparserop::FMTrange;
		}
		else if (value == "_MULTIPLY")
		{
			return Core::FMTyieldparserop::FMTmultiply;
		}
		else if (value == "_SUM")
		{
			return Core::FMTyieldparserop::FMTsum;
		}
		else if (value == "_SUBTRACT")
		{
			return Core::FMTyieldparserop::FMTsubstract;
		}
		else if (value == "_YTP")
		{
			return Core::FMTyieldparserop::FMTytp;
		}
		else if (value == "_MAI")
		{
			return Core::FMTyieldparserop::FMTmai;
		}
		else if (value == "_CAI")
		{
			return Core::FMTyieldparserop::FMTcai;
		}
		else if (value == "_DIVIDE")
		{
			return Core::FMTyieldparserop::FMTdivide;
		}
		else if (value == "_EQUATION")
		{
			return Core::FMTyieldparserop::FMTequation;
		}
		else if (value == "_ENDPOINT")
		{
			return Core::FMTyieldparserop::FMTendpoint;
		}
		else if (value == "_DELTA")
		{
			return Core::FMTyieldparserop::FMTdelta;
		}else {
			_exhandler->raise(Exception::FMTexc::FMTinvalid_yield," at line " + std::to_string(_line),"FMTyieldparser::getyldctype", __LINE__, __FILE__, _section);
		}
	}catch (...)
		{
		_exhandler->raisefromcatch(
			"for " + value,"FMTyieldparser::getyldctype", __LINE__, __FILE__, _section);
		}
	return Core::FMTyieldparserop::FMTnone;
    }
void FMTyieldparser::setoveridedylds(Core::FMTyields& yielddata,
	std::vector<std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>>>::iterator actualyield,
	const std::string& yld) const
{
	try {
		std::vector<std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>>>::iterator it = yielddata.begin();
		const Core::FMTmask& actual_msk = actualyield->first;//(yielddata.begin() + std::distance(yielddata.begin(), actualyield))->first;
		++it;//because this overided yield is the first on the list..
		size_t location = 1;
		while (it != yielddata.end())
		{
			if (it->first.issubsetof(actual_msk) ||
				actual_msk.issubsetof(it->first) ||
				it->first == actual_msk)
			{
				if (it->second->gettype()==Core::FMTyldtype::FMTcomplexyld)
				{
					Core::FMTcomplexyieldhandler* yldptr = dynamic_cast<Core::FMTcomplexyieldhandler*>(it->second.get());
					const Core::FMTcomplexyieldhandler* actptr = dynamic_cast<const Core::FMTcomplexyieldhandler*>(actualyield->second.get());
					if (!yldptr->comparesources(yld, *actptr))
					{
						_exhandler->raise(Exception::FMTexc::FMToveridedyield, "For overrided yield " + yld
							+ " at line " + std::to_string(_line) + " ",
							"FMTyieldparser::getylduse", __LINE__, __FILE__, _section);
					}
					if (actualyield->second->getoverrideindex()==0)
					{
						_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Cannot use non overrided at line " + std::to_string(_line) + " ",
							"FMTyieldparser::getylduse", __LINE__, __FILE__, _section);
					}
					yldptr->settabou(actualyield->second->getoverrideindex());
				}
				}
			++it;
			++location;
		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyieldparser::setoveridedylds", __LINE__, __FILE__, _section);
	}
}


std::vector<std::string> FMTyieldparser::getylduse(Core::FMTyields& yielddata,
	std::vector<std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>>>::iterator actualyield,
                                   const std::vector<std::string>& values) const
    {
	std::vector<std::string>dump;
	
	try {
		std::vector<std::pair<Core::FMTmask,std::unique_ptr<Core::FMTyieldhandler>>>::const_iterator it = yielddata.begin();
		const Core::FMTmask& actual_msk = actualyield->first;//(yielddata.begin() + std::distance(yielddata.begin(), actualyield))->first;
		while (it != actualyield)
		{
			if (it->first.issubsetof(actual_msk) ||
				actual_msk.issubsetof(it->first) ||
				it->first == actual_msk)
			{
				const std::vector<std::string> pyld = it->second->compare(values);
				for (const std::string& name : values)
				{
					if (std::find(pyld.begin(), pyld.end(), name) != pyld.end()
						&& std::find(dump.begin(), dump.end(), name) == dump.end())
					{
						dump.push_back(name);
					}
				}
				
			}
			++it;
		}
	}catch (...)
	{
		_exhandler->raisefromcatch("","FMTyieldparser::getylduse", __LINE__, __FILE__,_section);
	}
    return dump;
    }
void FMTyieldparser::checkpreexisting(const std::vector<std::string>& preexists) const
    {
    if (!preexists.empty())
        {
        for (const std::string& yl : preexists)
            {
            _exhandler->raise(Exception::FMTexc::FMTpreexisting_yield,
				yl+" at line "+ std::to_string(_line),"FMTyieldparser::checkpreexisting", __LINE__, __FILE__, _section);
            }
        }
    }

double FMTyieldparser::getnumwithproportion(const std::string& value,const Core::FMTconstants& constants,
	const std::vector<double>& proportions, const int& location)
	{
	try {
		double factor = 1;
		if (!proportions.empty())
		{
			factor = proportions.at(std::min(static_cast<int>(proportions.size()) - 1, location));
		}
		return getnum<double>(value, constants)*factor;
	}
	catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyieldparser::getnumwithproportion", __LINE__, __FILE__, _section);
	}
	return 0;
	}

bool FMTyieldparser::isfunction(const std::string& strfunction) const
	{
	try {
		const std::vector<std::string>cplxf = { "EXP","LN" };
		if (std::find(cplxf.begin(), cplxf.end(), strfunction) != cplxf.end())
		{
			return true;
		}
	}catch (...)
	{
		_exhandler->raisefromcatch("", "FMTyieldparser::isfunction", __LINE__, __FILE__, _section);
	}
	return false;
	}

Core::FMTdata FMTyieldparser::geteq(const std::string& basestr,
	const Core::FMTconstants& constants, const Core::FMTyields& ylds,
	const std::vector<Core::FMTtheme>& themes)
	{
		std::vector<std::string> valuesnoperators;
		std::vector<double>numbers;
		try {
			std::smatch kmatch;
			const boost::char_separator<char>separators("", "+-*/()^");
			const boost::tokenizer<boost::char_separator<char>>tokens(basestr, separators);
			for (std::string token : tokens)
			{
				boost::trim(token);
				if (isvalid(token))
				{
					double number = 0;
					std::string source_value(token);
					if (isnum(token) || constants.isconstant(token))
					{
						number = getnum<double>(token, constants);
					}
					else if (!Core::FMToperator(token).valid() && !Core::FMTfunctioncall(token).valid() && token != "(" && token != ")")
					{
						if (!ylds.isyld(token, true))
						{
							bool should_throw = true;
							for (const Core::FMTtheme& theme : themes)
							{
								if (theme.isindex(token))
								{
									should_throw = false;
									break;
								}
							}
							if (should_throw)
							{
								/*_exhandler->raise(Exception::FMTexc::FMTinvalid_yield,
									token + " at line " + std::to_string(_line),"FMTyieldparser::geteq",
									__LINE__, __FILE__, _section);*/
								
								_exhandler->raise(Exception::FMTexc::FMTignore,
									"No theme index provided for " + token +" replaced by a value of 0" , "FMTyieldparser::read", __LINE__, __FILE__, _section);
								source_value = "0";
							}
						}
						
					}
					valuesnoperators.push_back(source_value);
					numbers.push_back(number);
				}


			}
		}catch (...)
			{
			_exhandler->raisefromcatch("for "+ basestr,"FMTyieldparser::geteq", __LINE__, __FILE__,_section);
			}
		return Core::FMTdata(numbers, Core::FMTyieldparserop::FMTequation,valuesnoperators);
	}


std::unique_ptr<Core::FMTyieldmodel>FMTyieldparser::readyieldmodel(const std::string& modelname) const
{
	try {
		const boost::filesystem::path modeldirectory = boost::filesystem::path(getruntimelocation()) / boost::filesystem::path("YieldPredModels") / boost::filesystem::path(modelname);
		if (!boost::filesystem::is_directory(modeldirectory))
		{
			_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
							  modeldirectory.string() + " is not a valid directory ",
							  "FMTyieldparser::readyieldmodel", __LINE__, __FILE__, _section);
		}
		const boost::filesystem::path modeljson = modeldirectory / boost::filesystem::path(modelname + ".json");
		if (!boost::filesystem::is_regular_file(modeljson))
		{

			_exhandler->raise(Exception::FMTexc::FMTinvalid_path,
							  modeljson.string(),
							  "FMTyieldparser::readyieldmodel", __LINE__, __FILE__, _section);
		}
		std::ifstream jsonstream(modeljson.string());
		if (!FMTparser::tryopening(jsonstream, modeljson.string()))
		{
			_exhandler->raise(Exception::FMTunhandlederror,
							  modeljson.string(),
							  "FMTyieldparser::readyieldmodel", __LINE__, __FILE__, _section);
		}

		boost::property_tree::ptree root;
		boost::property_tree::read_json(jsonstream, root);
		jsonstream.close();

		//Get the FMTyieldmodel
		boost::property_tree::ptree::const_assoc_iterator modelTypeIt = root.find("modelType"); //temporary hardcoded string
		if (modelTypeIt == root.not_found())
		{
			_exhandler->raise(Exception::FMTunhandlederror,
				modeljson.string(),
				"FMTyieldparser::readyieldmodel", __LINE__, __FILE__, _section);
		}

		std::string modelType = modelTypeIt->second.data();
		if(modelType == "POOLS")
			return std::unique_ptr<Core::FMTyieldmodel>(new Core::FMTyieldmodelpools(root));
		if (modelType == "NEP")
			return std::unique_ptr<Core::FMTyieldmodel>(new Core::FMTyieldmodelnep(root));
		else
		{
			_exhandler->raise(Exception::FMTunhandlederror,
							  modeljson.string(),
							  "FMTyieldparser::readyieldmodel", __LINE__, __FILE__, _section);
		}
	} catch (...)
	{
		_exhandler->raisefromcatch("While reading model "+modelname,"FMTyieldparser::readyieldmodel", __LINE__, __FILE__, _section);
	}
}

Core::FMTyields FMTyieldparser::read(const std::vector<Core::FMTtheme>& themes,const Core::FMTconstants& constants,const std::string& location)
    {
	Core::FMTyields yields;
	std::string lineerror;
	try {
		std::ifstream yieldstream(location);
		std::vector<std::string>yldsnames;
		std::vector<std::string>dump;
		std::vector<std::pair<Core::FMTmask,std::unique_ptr<Core::FMTyieldhandler>>>::iterator actualyield;
		std::vector<size_t>indexed_yields;
		std::vector<std::pair<Core::FMTmask, std::unique_ptr<Core::FMTyieldhandler>>>::iterator datait = yields.end();
		Core::FMTmask tmask;
		bool sided = false;
		bool multipledef = false;
		bool overyld = false;
		std::vector<double>proportion;
		size_t overrideid = 1;
		if (FMTparser::tryopening(yieldstream, location))
		{
			while (yieldstream.is_open())
			{
				std::string line = FMTparser::getcleanlinewfor(yieldstream, themes, constants);
				if (!line.empty())
				{
					lineerror = line;
					std::smatch kmatch;
					if (std::regex_search(line, kmatch, rxyieldtype))
					{
						
						overyld = false;
						const std::string yieldtype = "Y" + std::string(kmatch[2]) + std::string(kmatch[7]);
						std::string mask = std::string(kmatch[4]) + std::string(kmatch[9]);
						boost::trim(mask);
						sided = true;
						if (!std::string(kmatch[5]).empty())
						{
							overyld = true;
						}

						if (!Core::FMTtheme::validate(themes, mask, " at line " + std::to_string(_line))) continue;
						Core::FMTyldtype yldtype = getyldtype(yieldtype);
						tmask = Core::FMTmask(mask, themes);
						std::unique_ptr<Core::FMTyieldhandler>newyield = gethandler(tmask, yldtype);
						if (!overyld)
						{
							yields.push_back(tmask, newyield);
							datait = --yields.end();
						}
						else {
							newyield->setoverrideindex(overrideid);
							++overrideid;
							yields.push_front(tmask, newyield);
							datait = yields.begin();
						}
						actualyield = datait;

						proportion.clear();
					}
					else {
						std::vector<std::string>values;
						boost::split(values, line, boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
						if ((actualyield->second->gettype() == Core::FMTyldtype::FMTageyld ||
							actualyield->second->gettype() == Core::FMTyldtype::FMTtimeyld) && values[0] == "*P")
						{
							values.erase(values.begin());
							proportion.clear();
							//size_t proportion_id = 0;
							for (const std::string& value : values)
							{
								proportion.push_back(getnum<double>(value) / 100.0);
							}
							continue;
						}
						if (actualyield->second->gettype() == Core::FMTyldtype::FMTageyld)
						{
							if (values[0] == "_AGE")
							{
								sided = false;
								multipledef = false;
								yldsnames.clear();
								dump.clear();
								values.erase(values.begin());
								yldsnames = values;
								dump = getylduse(yields, actualyield, values);
								checkpreexisting(dump);
							}
							else if (!sided)
							{
								if (!isnum(values.at(0), constants))
								{
									continue;
								}
								const int newbase = getnum<int>(values[0], constants);
								const std::vector<int>& bases = actualyield->second->getbases();
								if (std::find(bases.begin(), bases.end(), newbase) == bases.end())
								{
									actualyield->second->push_base(newbase);
								}
								values.erase(values.begin());
								int id = 0;
								std::set<std::string> passednames;
								for (const std::string& yldname : yldsnames)
								{
									const std::string& value = values.at(id);
									if(passednames.find(yldname)==passednames.end())
									{
										if (!isnum(value, constants))
										{
											continue;
										}
										actualyield->second->push_data(yldname, getnumwithproportion(value, constants, proportion, id));
										passednames.insert(yldname);
									}else if(!multipledef)
									{
										_exhandler->raise(Exception::FMTexc::FMTignore,
											yldname + " at line " + std::to_string(_line)+ " multiple definition", "FMTyieldparser::read", __LINE__, __FILE__, _section);
										multipledef = true;

									}
									++id;
								}
							}
							else {
								std::vector<std::string>tyld = { values[0] };
								values.erase(values.begin());
								dump = getylduse(yields, actualyield, tyld);
								checkpreexisting(dump);
								if (actualyield->second->empty())
								{
									actualyield->second->push_base(getnum<int>(values[0], constants));
								}
								const int location = static_cast<int>(actualyield->second->size());
								if (!isnum(values.at(1),constants))
									{
									continue;
									}
								actualyield->second->push_data(tyld[0], getnumwithproportion(values[1], constants, proportion, location));
							}

						}
						else if (actualyield->second->gettype() == Core::FMTyldtype::FMTtimeyld)
						{

							if (values[0] == "_CP")
							{
								sided = false;
								yldsnames.clear();
								dump.clear();
								values.erase(values.begin());
								yldsnames = values;
								dump = getylduse(yields, actualyield, values);
								checkpreexisting(dump);
							}
							else if (!sided)
							{
								if (!isnum(values.at(0), constants))
								{
									continue;
								}
								if (actualyield->second->empty())
								{
									actualyield->second->push_base(getnum<int>(values[0], constants));
									values.erase(values.begin());
									int id = 0;
									for (const std::string& yldname : yldsnames)
									{
										const std::string& value = values.at(id);
										actualyield->second->push_data(yldname, getnumwithproportion(value, constants, proportion, id));
										++id;
									}
								}
								else {
									const int newbase = getnum<int>(values[0], constants);
									values.erase(values.begin());
									for (int base = actualyield->second->getlastbase(); base <= newbase; ++base)
									{
										actualyield->second->push_base(base);
										int id = 0;
										for (const std::string& yldname : yldsnames)
										{
											const std::string& value = values.at(id);
											double thevalue = actualyield->second->getlastvalue(yldname);
											if (base == newbase)
											{
												thevalue = getnum<double>(value, constants);
											}
											actualyield->second->push_data(yldname, thevalue);
											++id;
										}
									}
								}


							}
							else {
								if (isnum(values.at(0),constants,false))
								{
									int id = 0;
									for (const std::string& value : values)
									{
										actualyield->second->push_data(yldsnames.back(), getnumwithproportion(value, constants, proportion, id));
										++id;
									}
								}
								else {


									const std::string yldname = values[0];
									yldsnames.push_back(values[0]);
									dump.clear();
									dump = getylduse(yields, actualyield, values);
									checkpreexisting(dump);
									values.erase(values.begin());
									//Need to check if rest of values is a _discountfactor!!!
									const std::string joinedvalues = boost::algorithm::join(values, "");
									std::smatch discountmatch;
									if (std::regex_search(joinedvalues, discountmatch, rxdiscount))
									{
										std::vector<double>yielddata;
										std::vector<std::string>sources;
										std::vector<bool>stacking;
										const std::string percentage = boost::trim_copy(std::string(discountmatch[3]));
										double valueper = getnum<double>(percentage, constants);
										const std::string dopercentage = std::string(discountmatch[4]);
										if (dopercentage.find("%") != std::string::npos)
										{
											valueper /= 100;
										}
										yielddata.push_back(valueper);
										stacking.push_back(false);
										std::string yperperiod = boost::trim_copy(std::string(discountmatch[6]));
										yielddata.push_back(getnum<double>(yperperiod, constants));
										stacking.push_back(false);
										sources.push_back(boost::trim_copy(std::string(discountmatch[9])));
										stacking.push_back(true);
										actualyield->second->push_data(yldname, Core::FMTdata(yielddata, Core::FMTyieldparserop::FMTdiscountfactor, sources, stacking));
									}
									else {
										if (!isnum(values.at(0), constants))
											{
											continue;
											}
										actualyield->second->push_base(getnum<int>(values[0], constants));
										values.erase(values.begin());
										int id = 0;
										for (const std::string& value : values)
										{
											actualyield->second->push_data(yldname, getnumwithproportion(value, constants, proportion, id));
											++id;
										}
									}
								}
							}


						}


						else if (actualyield->second->gettype() == Core::FMTyldtype::FMTcomplexyld)
						{
							std::smatch kmatch;
							const size_t should_be_equation = line.find_first_of("+-*/");
							bool simple_match = std::regex_search(line, kmatch, rxcomplex);
							if (simple_match || should_be_equation > 0)
							{
								std::string yldname;
								std::string cyld;
								std::string data;
								if (!simple_match && should_be_equation > 0)
								{
									std::vector<std::string>wrong_equation;
									boost::split(wrong_equation, line, boost::is_any_of(FMT_STR_SEPARATOR), boost::token_compress_on);
									yldname = wrong_equation[0];
									cyld = "_EQUATION";
									for (size_t cid = 1; cid < wrong_equation.size(); ++cid)
									{
										data += wrong_equation.at(cid);
									}
									boost::trim(data);
								}
								else {
									yldname = kmatch[1];
									for (int id = 4; id < 15; ++id) //12 - > 13
									{
										cyld += std::string(kmatch[id]);
									}
									data = kmatch[17];
								}
								dump.clear();
								const std::vector<std::string>theylds = { yldname };
								
								const Core::FMTyieldparserop complextype = getyldctype(cyld);
								std::vector<std::string>values;
								boost::trim_if(data, boost::is_any_of(FMT_STR_SEPARATOR));
								const std::string yldsplitc = FMT_STR_SEPARATOR + std::string(",");
								boost::split(values, data, boost::is_any_of(yldsplitc), boost::token_compress_on);
								std::vector<double>cvalues;
								std::vector<std::string>csource;
								if (complextype == Core::FMTyieldparserop::FMTequation)
								{
									actualyield->second->push_data(yldname, geteq(data, constants, yields, themes));
								}
								else {
									std::vector<bool>stacking;
									for (size_t id = 0; id < values.size(); ++id)
									{
										if (isnum(values[id]) || constants.isconstant(values[id]))
										{
											const double value = getnum<double>(values[id], constants);
											cvalues.push_back(value);
											stacking.push_back(false);
										}
										else if (values[id].find("#") != std::string::npos)
										{
											_exhandler->raise(Exception::FMTexc::FMTundefined_constant,
												values[id] + " at line " + std::to_string(_line), "FMTyieldparser::read", __LINE__, __FILE__, _section);
										}
										else {
											if (!yields.isyld(values[id], true))
											{
												_exhandler->raise(Exception::FMTexc::FMTignore,
													values[id] + " at line " + std::to_string(_line), "FMTyieldparser::read", __LINE__, __FILE__, _section);
											}
											stacking.push_back(true);
											csource.push_back(values[id]);
										}
									}
									actualyield->second->push_data(yldname, Core::FMTdata(cvalues, complextype, csource, stacking));
								}
								dump = getylduse(yields, actualyield, theylds);
								checkpreexisting(dump);
								if (overyld)//deal with the last inserted yield to take into account override!
								{
									setoveridedylds(yields, actualyield,yldname);
								}
								
								

							}
							else {
								_exhandler->raise(Exception::FMTexc::FMTunsupported_yield,
									line + " at line " + std::to_string(_line), "FMTyieldparser::read", __LINE__, __FILE__, _section);
							}
						}else if (actualyield->second->gettype() == Core::FMTyldtype::FMTmodelyld)
							{
							std::smatch predmatch;
							if (std::regex_search(line, predmatch,rxpredictor))
								{
								const std::string yieldsarray = boost::trim_copy(std::string(predmatch[1]));
								const std::string modelname = boost::trim_copy(std::string(predmatch[4]));
								std::vector<std::string>newyields;
								const std::string yieldseparators = FMT_STR_SEPARATOR + std::string(",");
								boost::split(newyields, yieldsarray, boost::is_any_of(yieldseparators), boost::token_compress_on);
								dump = getylduse(yields, actualyield, newyields);
								checkpreexisting(dump);
								
								Core::FMTmodelyieldhandler* handlerptr = dynamic_cast<Core::FMTmodelyieldhandler*>(actualyield->second.get());
								const std::map<std::string, size_t>handlermodels = handlerptr->getmodelsnamebyindex();
								size_t modelid = 0;
								if (handlermodels.find(modelname)!= handlermodels.end())
								{
									modelid = handlermodels.at(modelname);
								}else {
									modelid = handlerptr->size();
									const std::unique_ptr<Core::FMTyieldmodel>yldmodel = readyieldmodel(modelname);
									const std::vector<std::string> allyields = yields.getallyieldnames();
									if (!yldmodel->Validate(allyields))
										{
										//_exhandler->raise(Exception::FMTexc::FMTinvalid_yield,
										//	"Missing source yields for " +modelname+" yield model " + std::to_string(_line),
										//	"FMTyieldparser::read", __LINE__, __FILE__, _section);
										}
									if (newyields.size()!=yldmodel->GetYieldsOutputs().size())
										{
										//_exhandler->raise(Exception::FMTexc::FMTinvalidyield_number,
										//	"Different size in model "+yldmodel->GetModelName()+" outputs for yields "+ yieldsarray+" " + std::to_string(_line),
										//	"FMTyieldparser::read", __LINE__, __FILE__, _section);
										}
									handlerptr->push_backmodel(yldmodel);
								}
								size_t yldid = 0;
								for (const std::string& yldname : newyields)
									{
									handlerptr->setyield(modelid, yldid, yldname);
									++yldid;
									}
								
								#ifndef FMTWITHONNXR
									//_exhandler->raise(Exception::FMTexc::FMTinvalid_yield,
									//	"Found a model yield but FMT is not compiled with Torch at line " + std::to_string(_line),
									//	"FMTyieldparser::read", __LINE__, __FILE__, _section);
								#endif
							}else {
								_exhandler->raise(Exception::FMTexc::FMTunsupported_yield,
									line + " at line " + std::to_string(_line), "FMTyieldparser::read", __LINE__, __FILE__, _section);
								}
							
							}
					}
				}
			}
		}
		yields.update();
		cleanup(yields, themes,constants);
	}catch(...)
		{
		_exhandler->raisefromcatch(
			"In " + _location + " at line " + std::to_string(_line),"FMTyieldparser::read", __LINE__, __FILE__,_section);
		}
    return yields;
    }

	std::map<std::string, double>FMTyieldparser::getindexvalues(const Core::FMTmask& mask,
		const std::vector<Core::FMTtheme>& themes,const std::vector<std::string>&indexvalues, const Core::FMTconstants& constants) const
	{
		std::map<std::string, double>handler_values;
		std::string indexerror;
		size_t themeid;
		try {
			
			for (const Core::FMTtheme& theme : themes)
			{
				const std::string attribute = mask.get(theme);
				themeid = theme.getid()+1;
				for (const std::string& index : indexvalues)
				{
					indexerror = index;
					if (isnum(index) || constants.isconstant(index))
						{
						handler_values[index] = getnum<double>(index,constants);
						continue;
						}
					if (theme.isindex(attribute,index))
					{
						handler_values[index] = theme.getindex(attribute, index);
					}else {
						_exhandler->raise(Exception::FMTexc::FMTignore,
							"No theme index provided for "+attribute+" at theme "+std::to_string(themeid)+" replaced "+index+" by a value of 0", "FMTyieldparser::read", __LINE__, __FILE__, _section);
						handler_values[index] = 0;
						}
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch(
				"for "+std::string(mask)+" at index "+ indexerror+" for theme "+std::to_string(themeid), "FMTyieldparser::getindexvalues", __LINE__, __FILE__, _section);
		}
	return handler_values;
	}

void FMTyieldparser::cleanup(Core::FMTyields& yields,const std::vector<Core::FMTtheme>& themes, const Core::FMTconstants& constants) const
	{
	try {
		//iterate on all yieldhandler if equation with index then take the handler
			//delete the handler at it's yields location
			//decompose and insert all new handlers
			//std::vector<Core::FMTyieldhandler>::iterator handler_it = yields.databegin();
		std::vector<std::pair<Core::FMTmask,std::unique_ptr<Core::FMTyieldhandler>>>::iterator handler_it = yields.begin();
		const std::vector<std::string>yldnames = yields.getallyieldnames();
		while (handler_it != yields.end())
		{
			const std::vector<std::string>indexvalues = handler_it->second->indexes(yldnames);
			if (!indexvalues.empty())
			{
				size_t location = std::distance(yields.begin(), handler_it);
				const Core::FMTmask oldmask = (yields.begin() + location)->first;
				const std::unique_ptr<Core::FMTyieldhandler> oldhandler = handler_it->second->clone();
				yields.erase(location);
				std::vector<Core::FMTmask>todecompose;
				todecompose.push_back(oldmask);
				std::vector<Core::FMTtheme>themes_windex;
				for (const Core::FMTtheme& theme : themes)
				{
					
					if (theme.useindex())
					{
						for (const std::string& index : indexvalues)
						{
							if (theme.isindex(index))
							{
								std::vector<Core::FMTmask>newdecomposer;
								while (!todecompose.empty())
								{
									const std::vector<Core::FMTmask>allmasks = (todecompose.front()).decompose(theme);
									todecompose.erase(todecompose.begin());
									newdecomposer.insert(newdecomposer.end(), allmasks.begin(), allmasks.end());
								}
								todecompose = newdecomposer;
								themes_windex.push_back(theme);
								break;
							}
						}
					}
				}
				for (const Core::FMTmask& newmask : todecompose)
				{
					std::unique_ptr<Core::FMTyieldhandler>newhandler = gethandler(newmask, Core::FMTyldtype::FMTcomplexyld);
					std::map<std::string, double>handler_values = getindexvalues(newmask, themes_windex, indexvalues,constants);
					const Core::FMTcomplexyieldhandler* oldhandlerptr = dynamic_cast<const Core::FMTcomplexyieldhandler*>(oldhandler.get());
					Core::FMTcomplexyieldhandler* newhandlerptr = dynamic_cast<Core::FMTcomplexyieldhandler*>(newhandler.get());
					newhandlerptr->settabou(*oldhandlerptr);
					const std::map<std::string, Core::FMTdata>& alladata = oldhandlerptr->getdataelements();
					for (std::map<std::string, Core::FMTdata>::const_iterator datait = alladata.begin(); datait != alladata.end(); datait++)
					{
						const Core::FMTexpression expression = datait->second.toexpression();
						const std::vector<std::string>allvalues = expression.getinfix();
						std::vector<double>numbers(allvalues.size(), 0);
						std::vector<std::string>valuesnoperators(allvalues.size());
						size_t data_loc = 0;
						for (const std::string& source : allvalues)
						{
							if (handler_values.find(source) != handler_values.end())
							{
								numbers[data_loc] = handler_values.at(source);
							}
							else if (isnum(source))
							{
								numbers[data_loc] = std::stod(source);
							}
							else {
								valuesnoperators[data_loc] = source;
							}
							++data_loc;
						}
						newhandler->push_data(datait->first, Core::FMTdata(numbers, Core::FMTyieldparserop::FMTequation, valuesnoperators));
					}
					yields.insert(location, newmask, newhandler);
					++location;
				}
				handler_it = (yields.begin() + location);
			}
			else {
				++handler_it;
			}
		}
	yields.update();
	//yields.passinobject(*this);
	}catch (...)
		{
		_exhandler->raisefromcatch(
			"", "FMTyieldparser::cleanup", __LINE__, __FILE__, _section);
		}
	}

void FMTyieldparser::write(const Core::FMTyields& yields,const std::string& location) const
    {
	try {
		std::ofstream yieldstream;
		yieldstream.open(location);
		if (tryopening(yieldstream, location))
		{
			const std::vector<std::string>stackedyields = yields.getstacked();
			for (const std::string& val : stackedyields)
			{
				yieldstream << std::string(val);
			}
			yieldstream.close();
		}
		}catch (...)
			{
			_exhandler->raisefromcatch("at " + location,"FMTyieldparser::write", __LINE__, __FILE__,_section);
			}
    }
}
