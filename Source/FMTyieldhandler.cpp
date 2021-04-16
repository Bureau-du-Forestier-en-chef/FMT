/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldhandler.h"

namespace Core{

FMTyieldhandler::operator std::string() const
        {
		std::string value = "";
		try{
        if (yldtype==FMTyldtype::FMTageyld)
            {
            value += "*Y " + std::string(mask) + "\n";
            value += "_AGE \t";
            for(std::map<std::string,FMTdata>::const_iterator it=elements.begin(); it!=elements.end(); ++it)
                {
                value += it->first + "\t";
                }
            value += "\n";
            int baseid = 0;
            for (const int& base : bases)
                {
                value += std::to_string(base) + "\t";
                for(std::map<std::string,FMTdata>::const_iterator it=elements.begin(); it!=elements.end(); ++it)
                    {
                    const std::vector<double>* data = &it->second.data;
                    value += std::to_string(data->at(baseid)) + "\t";
                    }
                value += "\n";
                ++baseid;
                }
            }else if(yldtype==FMTyldtype::FMTtimeyld)
                {
                value += "*YT " + std::string(mask) + "\n";
                for(std::map<std::string,FMTdata>::const_iterator it=elements.begin(); it!=elements.end(); ++it)
                    {
                    value += it->first + " ";
                    for(const int& base : bases)
                        {
                        value += std::to_string(base) + " ";
                        }
                    const std::vector<double>* data = &it->second.data;
                    for(const double & val : *data)
                        {
                        value += std::to_string(val) + " ";
                        }
                    value += "\n";
                    }
                }else if(yldtype==FMTyldtype::FMTcomplexyld)
                    {
                    value += "*YC " + std::string(mask) + "\n";
                    for(std::map<std::string,FMTdata>::const_iterator it=elements.begin(); it!=elements.end(); ++it)
                        {
                        value += it->first + " " + std::string(it->second) + "\n";
                        }

                    }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::std::string()", __LINE__, __FILE__,Core::FMTsection::Yield);
		}
        return value;
        }



    FMTyieldhandler::FMTyieldhandler(FMTyldtype ltype,const FMTmask& lmask) : FMTobject(), yldtype(ltype),mask(lmask),bases(),elements(), lookat(){}


    FMTyieldhandler::FMTyieldhandler(const FMTyieldhandler& rhs) :
			FMTobject(rhs),
            yldtype(rhs.yldtype),
            mask(rhs.mask),
            bases(rhs.bases),
            elements(rhs.elements),
			lookat(rhs.lookat)

        {

        }
    FMTyieldhandler& FMTyieldhandler::operator = (const FMTyieldhandler& rhs)
        {
        if (this!=&rhs)
            {
			FMTobject::operator=(rhs);
            yldtype = rhs.yldtype;
            mask = rhs.mask;
            bases = rhs.bases;
            elements = rhs.elements;
			lookat = rhs.lookat;
            }
        return *this;
        }
    bool FMTyieldhandler::push_base(const int& base)
        {
        bases.push_back(base);
        return true;
        }
    bool FMTyieldhandler::push_data(const std::string& yld,const double& value)
        {
        if (elements.find(yld)==elements.end())
            {
            elements[yld]= FMTdata();
            }
        elements[yld].data.push_back(value);
        return true;
        }

	std::map<std::string,FMTdata>FMTyieldhandler::getdataelements() const
        {
        return elements;
        }

	std::vector<std::string> FMTyieldhandler::indexes(const std::vector<std::string>& names) const
        {
		std::vector<std::string>indexs;
		try{
        if (yldtype == FMTyldtype::FMTcomplexyld)
            {
            for (std::map<std::string,FMTdata>::const_iterator data_it = elements.begin(); data_it != elements.end(); data_it++)
                {
                if (data_it->second.getop() == FMTyieldparserop::FMTequation)
                    {
					const std::vector<std::string>variables = data_it->second.getsource();
                    for(const std::string& variable : variables)
                        {
                        if (!variable.empty() && std::find(names.begin(),names.end(),variable)==names.end() &&
                            !FMTfunctioncall(variable).valid() &&
                            !FMToperator(variable).valid() &&
                            (variable != ")" && variable != "("))
                            {
                            indexs.push_back(variable);
                            }
                        }
                    }
                }
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::indexes", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return indexs;
        }

    bool FMTyieldhandler::push_data(const std::string& yld,const FMTdata& data)
        {
        elements[yld]= FMTdata(data);
        return true;
        }
    FMTyldtype FMTyieldhandler::gettype() const
        {
        return yldtype;
        }

	FMTmask FMTyieldhandler::getmask() const
		{
		return mask;
		}
	std::vector<std::string> FMTyieldhandler::compare(const std::vector<std::string>& keys) const
        {
		std::vector<std::string>same;
		try{
        for(const std::string& key : keys)
            {
            if (elements.find(key) != elements.end())
                {
                same.push_back(key);
                }
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::compare", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return same;
        }

	std::map<std::string,const FMTyieldhandler*> FMTyieldhandler::getdata(const std::vector<const FMTyieldhandler*>& datas,
                                    const std::vector<std::string>& names,const std::string& original) const
        {
		std::map<std::string,const FMTyieldhandler*>alldata;
		try {
			for (const FMTyieldhandler* yield : datas)
			{
				for (const std::string& name : names)
				{
					if (yield->elements.find(name) != yield->elements.end() && alldata.find(name) == alldata.end() &&
						!(this == yield && original == name) && (yield->lookat.find(name) == yield->lookat.end()))
					{
						alldata[name] = yield;
					}
					if (alldata.size() == names.size())
					{
						return alldata;
					}
				}
			}
			if (alldata.size() != names.size())
				{
				for (const std::string& name : names)
					{
					if (!name.empty()&&alldata.find(name)==alldata.end())
						{
						alldata[name] = nullptr;
						/*_exhandler->raise(Exception::FMTexc::FMTignore,
							"Missing requested yield "+name, "FMTyieldhandler::getdata", __LINE__, __FILE__);*/
						}

					}

				}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getdata", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return alldata;
        }

	 bool FMTyieldhandler::operator == (const FMTyieldhandler& rhs) const
	 {
		 return (yldtype == rhs.yldtype &&
			 mask == rhs.mask &&
			 bases == rhs.bases &&
			 elements == rhs.elements);
	 }

	 std::vector<double>FMTyieldhandler::getsourcesarray(const std::map<std::string, const FMTyieldhandler*>& srcdata, const std::vector<const FMTyieldhandler*>& datas,
		 const int& age, const int& period, const FMTmask& resume_mask, bool& age_only) const
	 {
		 std::vector<double>alldata(srcdata.size(),0.0);
		 try {
			 size_t location = 0;
			 for (std::map<std::string, const FMTyieldhandler*>::const_iterator datait = srcdata.begin(); datait != srcdata.end(); datait++)
			 {
				 if (datait->second != nullptr)
				 {
					 const FMTyieldhandler* yldata = datait->second;
					 if (yldata->gettype() != FMTyldtype::FMTageyld)
					 {
						 age_only = false;
					 }
					 alldata[location]= yldata->get(datas, datait->first, age, period, resume_mask);
				 }
				 ++location;
			 }
		 }
		 catch (...)
		 {
			 _exhandler->raisefromcatch("", "FMTyieldhandler::getsourcesarray", __LINE__, __FILE__, Core::FMTsection::Yield);
		 }
		 return alldata;
	 }


	 std::map<std::string,double> FMTyieldhandler::getsources( const std::map<std::string, const FMTyieldhandler*>& srcdata, const std::vector<const FMTyieldhandler*>& datas,
		 const int& age,const int& period, const FMTmask& resume_mask,bool& age_only) const
        {
		 std::map<std::string, double>alldata;
		 try{
		 for (std::map<std::string, const FMTyieldhandler*>::const_iterator datait = srcdata.begin(); datait != srcdata.end(); datait++)
			{
			 alldata[datait->first] = 0;
			 if (datait->second!=nullptr)
				{
				const FMTyieldhandler* yldata = datait->second;
				if (yldata->gettype() != FMTyldtype::FMTageyld)
					{
					 age_only = false;
					}
				alldata[datait->first] = yldata->get(datas, datait->first, age, period, resume_mask);
				}
			 
			}
		 }
		 catch (...)
		 {
			 _exhandler->raisefromcatch("", "FMTyieldhandler::getsources", __LINE__, __FILE__, Core::FMTsection::Yield);
		 }
		 return alldata;
        }

    double FMTyieldhandler::get(const std::vector<const FMTyieldhandler*>& datas,
                           const std::string yld,const int& age,const int& period,
							const FMTmask& resume_mask) const
        {
        double value = 0;
		try{
		int target = 0;
        if (yldtype == FMTyldtype::FMTageyld)
            {
			 try{
			 target = age;
			 std::map<std::string, Core::FMTdata>::const_iterator data0it = elements.find(yld);
			 if (data0it != elements.end())
				{
				 value = getlinearvalue(data0it->second.data, target);
				}
			 }catch (...)
				{
				 _exhandler->raisefromcatch("Getting age yield", "FMTyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
				 }
            }else if(yldtype==FMTyldtype::FMTtimeyld)
                {
				try{
                target = period;
				std::map<std::string, Core::FMTdata>::const_iterator data0it = elements.find(yld);
				if (data0it != elements.end())
					{
					const FMTdata* lvalues = &data0it->second;
					if (lvalues->getop() == FMTyieldparserop::FMTdiscountfactor)
						{
						const double perioddbl = static_cast<double>(period);
						const double pertio = lvalues->data.at(1);
						const std::string discounttype = lvalues->getsource().at(0);
						const double rateofreturn = lvalues->data.at(1);
						double exponant = perioddbl; //full
						if (discounttype == "NONE")
							{
							exponant = perioddbl -1;
							}else if (discounttype == "HALF")
								{
								exponant = perioddbl *0.5;
								}
						value = (1 / std::pow((1 + rateofreturn), pertio * exponant));
					}else {
						value = lvalues->data.back();
						if (target < static_cast<int>(lvalues->data.size()))
							{
							value = lvalues->data.at(target);
							}
						}
					}
				}catch (...)
				{
					_exhandler->raisefromcatch("Getting time yield", "FMTyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
                }else{
				const FMTdata* cdata = &elements.at(yld);
				if (cdata->cachevalue(resume_mask,age,period))
					{
					return cdata->get(resume_mask, age, period);
					}
				bool age_only = true;
				const std::vector<std::string> sources = cdata->getsource();
				const std::map<std::string, const FMTyieldhandler*> srcsdata = this->getdata(datas, sources, yld);
				try{
					if (lookat.find(yld) == lookat.end())
					{
						lookat.insert(yld);
					}
					else {
						_exhandler->raise(Exception::FMTexc::FMTinvalid_yield, "Recursivity detected for complexe yield " + yld,
							"FMTyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
					}
                switch(cdata->getop())
                    {
                    case FMTyieldparserop::FMTrange:
                        {
                        size_t srcid = 0;
                        value = 1;
						const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
						for (const std::string& yldrange : sources)
							{
							const double lower = cdata->data.at(srcid);
							const double upper = cdata->data.at(srcid + 1);
							if ((source_values.find(yldrange)==source_values.end()) || (source_values.at(yldrange) < lower || source_values.at(yldrange) > upper))
								{
								value = 0;
								break;
								}
							srcid += 2;
							}
                        break;
                        }
                    case FMTyieldparserop::FMTmultiply:
                        {
                        value = 1;
						//const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
                        for(const double& sourcevalue : getsourcesarray(srcsdata, datas, age, period, resume_mask, age_only))
                            {
                            value *= sourcevalue;
                            }
                        for(const double& vecvalue : cdata->data)
                            {
                            value *= vecvalue;
                            }
                        break;
                        }
                    case FMTyieldparserop::FMTsum:
                        {
						//const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
                        for(const double& sourcevalue : getsourcesarray(srcsdata, datas, age, period, resume_mask, age_only))
                            {
                            value += sourcevalue;
                            }
                        for(const double& vecvalue : cdata->data)
                            {
                            value += vecvalue;
                            }
                        break;
                        }
                    case FMTyieldparserop::FMTsubstract:
                        {
						//ordering means something here!!!!
						const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
						std::vector<double>values = cdata->tovalues(source_values);
						value = values.front();
						values.erase(values.begin());
						for (const double& yldvalue : values)
							{
							value -= yldvalue;
							}

                        break;
                        }
                    case FMTyieldparserop::FMTdivide:
                        {
						const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
						std::vector<double>values = cdata->tovalues(source_values);
						value = values.front();
						values.erase(values.begin());
						for (const double& yldvalue : values)
							{
							if (yldvalue != 0)
								{
								value /= yldvalue;
							}
							else {
								value = 0;
							}
							}
                        break;
                        }
                    case FMTyieldparserop::FMTytp:
                        {
                        const FMTyieldhandler* ddata = srcsdata.begin()->second;
                        if (ddata->gettype() == FMTyldtype::FMTageyld)
                            {
                            value = ddata->getpeak(srcsdata.begin()->first,age);
                            }
                        break;
                        }
                    case FMTyieldparserop::FMTmai:
                        {
                        double year = 1;
                        if (cdata->data.begin()!=cdata->data.end())
                            {
                            year = *cdata->data.begin();
                            }
                        const FMTyieldhandler* ddata = srcsdata.begin()->second;
                        if (ddata->gettype() == FMTyldtype::FMTageyld)
                            {
							const FMTyieldhandler* handler = srcsdata.at(sources[0]);
							const FMTdata* lvalues = &handler->elements.at(sources[0]);
							value = (ddata->getlinearvalue(lvalues->data, age) / (year*age));
                            }
                        break;
                        }
                    case FMTyieldparserop::FMTcai:
                        {
                        double year = 1;
                        if (cdata->data.begin()!=cdata->data.end())
                            {
                            year = *cdata->data.begin();
                            }
                        const FMTyieldhandler* ddata = srcsdata.begin()->second;
                        if (ddata->gettype() == FMTyldtype::FMTageyld)
                            {
							const FMTyieldhandler* handler = srcsdata.at(sources[0]);
							const FMTdata* lvalues = &handler->elements.at(sources[0]);
							const double upval = ddata->getlinearvalue(lvalues->data, age);
							const int newage = age - 1;
                            const double dwval = ddata->getlinearvalue(lvalues->data, newage);
                            value = ((upval - dwval) / (year));
                            }
                        break;
                        }
					case FMTyieldparserop::FMTequation:
						{
						const FMTexpression expression = cdata->toexpression();
						try{
							const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
							value = expression.shuntingyard(source_values);
							}catch (...)
								{
								std::string baseeq;
								for (const std::string& opstr : expression.getinfix())
									{
									baseeq += opstr + " ";
									}
								_exhandler->raisefromcatch("For complex equation " + baseeq, "FMTyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
								}
						break;
						}
					case FMTyieldparserop::FMTendpoint:
						{
						value = 0;
						const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period, resume_mask, age_only);
						const double lowerbound = cdata->data.at(0);
						const double upperbound = cdata->data.at(1);
						const std::vector<std::string> ylds = cdata->getsource();
						int peak = -1;
						int lowerpeak = -1;
						const FMTyieldhandler* ddata;
						if (source_values.at(ylds.at(0)) < lowerbound)
							{
							ddata = srcsdata.at(ylds.at(0));
							peak = ddata->getendpoint(ylds.at(0), lowerpeak, lowerbound, source_values.at(ylds.at(0)));
							value = (-getchangesfrom(age, peak));
							}
						if (source_values.at(ylds.at(1)) > upperbound)
							{
							ddata = srcsdata.at(ylds.at(0));
							lowerpeak = ddata->getendpoint(ylds.at(0), lowerpeak, lowerbound, std::numeric_limits<double>::lowest());
							ddata = srcsdata.at(ylds.at(1));
							peak = ddata->getendpoint(ylds.at(1), lowerpeak, upperbound, source_values.at(ylds.at(1)));
							value = (-getchangesfrom(age, peak));
							}
						break;
						}
					case FMTyieldparserop::FMTdelta:
						{
						const std::map<std::string, double>source_values = this->getsources(srcsdata, datas, age, period, resume_mask, age_only);
						int periodtolookat = std::max(0,period + static_cast<int>(cdata->data.back()));
						const std::map<std::string, double>periodic_source_values = this->getsources(srcsdata, datas, age, periodtolookat, resume_mask, age_only);
						value = std::abs(source_values.begin()->second - periodic_source_values.begin()->second);
						break;
						}
                    default:
                    break;
                    }
				lookat.erase(yld);
				}catch (...)
					{
						_exhandler->raisefromcatch("Getting complex yield of type "+std::to_string(static_cast<int>(cdata->getop())), "FMTyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
					}
				value = std::round(value * 100000000) / 100000000;
				cdata->set(value, resume_mask, age, period, age_only);
                }
		}catch (...)
			{
			_exhandler->raisefromcatch("at yield "+yld, "FMTyieldhandler::get", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		
        return value;
        }

	double FMTyieldhandler::getlinearvalue(const std::vector<double>& dls, const int& agetarget) const
		{
		double value = 0;
		try{
		if (agetarget >= bases.back())
			{
			value = dls.back();
		}else if(agetarget < bases.front())
			{
			value = (agetarget * (dls[0] / static_cast<double>(bases[0])));
			}else{
			int id = 0;
			int highindex = -1;
			int lowindex = -1;
			for (const int& base : bases)
				{
				if (base <= agetarget)
					{
					lowindex = id;
					}
				if (base >= agetarget)
					{
					highindex = id;
					break;
					}
				++id;
				}
			if (lowindex != highindex)
				{
				const double factor = ((dls[highindex] - dls[lowindex]) / (static_cast<double>(bases[highindex]) - static_cast<double>(bases[lowindex])));
				const double lastvalue = dls[lowindex];
				value = lastvalue + ((agetarget - bases[lowindex]) * factor);
				}else{
				value = dls[highindex];
				}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getlinearvalue", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
		}

	int FMTyieldhandler::getlastbase() const
		{
		return bases.back();
		}

	const std::vector<int>& FMTyieldhandler::getbases() const
		{
		return bases;
		}

	double FMTyieldhandler::getlastvalue(const std::string yld) const
		{
		std::map<std::string, FMTdata>::const_iterator it = elements.find(yld);
		return it->second.data.back();
		}

	double FMTyieldhandler::getchangesfrom(const int& targetage, const int& peakstep) const
		{
		double value = 0;
		try{
		if (peakstep > 0)
			{
			const size_t agesize = static_cast<size_t>(targetage);
			std::vector<double>peakvalues(std::max(agesize, bases.size()) + 1, 0.0);
			const int peakage = bases[peakstep];
			int id = 0;
			for (double& pvalue : peakvalues)
			{
				pvalue = (peakage - id);
				++id;
			}
			value = peakvalues.at(targetage);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getchangesfrom", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
		}

	double FMTyieldhandler::getpeakfrom(const std::string& yld, double maxvalue) const
		{
		std::map<std::string, FMTdata>::const_iterator it = elements.find(yld);
		int location = 0;
		int peak = -1;
		try{
		std::vector<double>::const_iterator dblit = it->second.data.begin();
		while (dblit != it->second.data.end())
			{
				if (*dblit > maxvalue)
				{
					maxvalue = *dblit;
					peak = location;
				}
				++location;
				++dblit;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getpeakfrom", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return peak;
		}

	int FMTyieldhandler::getendpoint(const std::string& yld, const int& lowerstep, const double& bound, const double& value) const
		{
		size_t locid = 0;
		try{
		if (this->gettype() == FMTyldtype::FMTageyld)
			{
				std::map<std::string, FMTdata>::const_iterator it = elements.find(yld);
				std::vector<double>::const_iterator location;
				if (value<bound)
					{
					location = std::lower_bound(it->second.data.begin(), it->second.data.end(), bound);
					}else if (value>bound)
						{
						std::vector<double>::const_iterator startinglocation = it->second.data.begin() + lowerstep;
						location = std::upper_bound(startinglocation, it->second.data.end(), bound);
						}
				locid = std::distance(it->second.data.begin(), location);
				locid = std::min(locid, (it->second.data.size() - 1));
				locid = std::max(size_t(0), locid);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getendpoint", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return static_cast<int>(locid);
		}


    double FMTyieldhandler::getpeak(const std::string& yld, const int& targetage) const
        {
        double value = 0;
		try{
        if (this->gettype() == FMTyldtype::FMTageyld)
            {
			const int peak = static_cast<int>(getpeakfrom(yld));
			value = getchangesfrom(targetage, peak);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getpeak", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return value;
        }


    int FMTyieldhandler::getage(const std::string yld, const double& value, const int& starting_age) const
        {
        int age = 0;
		try{
		std::map<std::string,FMTdata>::const_iterator it = elements.find(yld);
        if (it!=elements.end())
            {
            const FMTdata* ldata = &it->second;
			//std::vector<double>::const_iterator dit = ldata->data.begin();
			double minimal_gap = std::numeric_limits<double>::infinity();
			size_t minimal_gap_index = 0;
			size_t index = 0;
			for (const double& vdata : ldata->data)
				{
				if (abs(vdata-value) <= minimal_gap && index > 0 && bases.at(index) <= starting_age)
					{
					minimal_gap = abs(vdata - value);
					minimal_gap_index = index;
					}
				++index;
				}
			age = std::max(bases.at(minimal_gap_index),1);
            }
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getage", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
        return age;
        }

	FMTyieldhandler FMTyieldhandler::presolve(const FMTmask& presolvedmask, const std::vector<FMTtheme>& newthemes) const
		{
		FMTyieldhandler newhandler(*this);
		try {
			newhandler.mask = newhandler.mask.presolve(presolvedmask, newthemes);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return newhandler;
		}

	FMTyieldhandler FMTyieldhandler::getfromfactor(const double& factor,
		std::vector<std::string>yieldnames) const
		{
		FMTyieldhandler newhandler(*this);
		try{
		if (newhandler.gettype() == FMTyldtype::FMTageyld)
			{
			std::vector<std::map<std::string, FMTdata>::iterator>iterators;
			if (yieldnames.empty())
				{
				for (std::map<std::string, FMTdata>::iterator it = newhandler.elements.begin();
					it!= newhandler.elements.end();it++)
					{
					iterators.push_back(it);
					}
			}else {
				for (const std::string& yldname : yieldnames)
					{
					std::map<std::string, FMTdata>::iterator it = newhandler.elements.find(yldname);
					if (it != newhandler.elements.end())
						{
						iterators.push_back(it);
						}
					}
				}
				for (std::map<std::string, FMTdata>::iterator datait : iterators)
					{
					datait->second = ((datait->second)*factor);
					}
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldhandler::getfromfactor", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return newhandler;
		}
}
