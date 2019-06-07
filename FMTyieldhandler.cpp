#include "FMTyieldhandler.h"

namespace Core{

FMTyieldhandler::operator string() const
        {
        string value = "";
        if (yldtype==FMTyldwstype::FMTageyld)
            {
            value += "*Y " + string(mask) + "\n";
            value += "_AGE \t";
            for(map<string,FMTdata>::const_iterator it=elements.begin(); it!=elements.end(); ++it)
                {
                value += it->first + "\t";
                }
            value += "\n";
            int baseid = 0;
            for (const int& base : bases)
                {
                value += to_string(base) + "\t";
                for(map<string,FMTdata>::const_iterator it=elements.begin(); it!=elements.end(); ++it)
                    {
                    const vector<double>* data = &it->second.data;
                    value += to_string(data->at(baseid)) + "\t";
                    }
                value += "\n";
                ++baseid;
                }
            }else if(yldtype==FMTyldwstype::FMTtimeyld)
                {
                value += "*YT " + string(mask) + "\n";
                for(map<string,FMTdata>::const_iterator it=elements.begin(); it!=elements.end(); ++it)
                    {
                    value += it->first + " ";
                    for(const int& base : bases)
                        {
                        value += to_string(base) + " ";
                        }
                    const vector<double>* data = &it->second.data;
                    for(const double & val : *data)
                        {
                        value += to_string(val) + " ";
                        }
                    value += "\n";
                    }
                }else if(yldtype==FMTyldwstype::FMTcomplexyld)
                    {
                    value += "*YC " + string(mask) + "\n";
                    for(map<string,FMTdata>::const_iterator it=elements.begin(); it!=elements.end(); ++it)
                        {
                        value += it->first + " " + string(it->second) + "\n";
                        }

                    }
        return value;
        }


	FMTyieldhandler::FMTyieldhandler():yldtype(),mask(),bases(),elements() {}


    FMTyieldhandler::FMTyieldhandler(FMTyldwstype ltype,const FMTmask& lmask) : yldtype(ltype),mask(lmask),bases(),elements(){}


    FMTyieldhandler::FMTyieldhandler(const FMTyieldhandler& rhs) :
            yldtype(rhs.yldtype),
            mask(rhs.mask),
            bases(rhs.bases),
            elements(rhs.elements)

        {

        }
    FMTyieldhandler& FMTyieldhandler::operator = (const FMTyieldhandler& rhs)
        {
        if (this!=&rhs)
            {
            yldtype = rhs.yldtype;
            mask = rhs.mask;
            bases = rhs.bases;
            elements = rhs.elements;
            }
        return *this;
        }
    bool FMTyieldhandler::push_base(const int& base)
        {
        bases.push_back(base);
        return true;
        }
    bool FMTyieldhandler::push_data(const string& yld,const double& value)
        {
        if (elements.find(yld)==elements.end())
            {
            elements[yld]= FMTdata();
            }
        elements[yld].data.push_back(value);
        return true;
        }

    /*vector<FMTyieldhandler>FMTyieldhandler::decomposeindexes(const vector<FMTtheme>& themes) const
        {
        vector<FMTyieldhandler>handlers;
        vector<FMTmask>allmasks;
        allmasks.push_back(mask);
        for (map<string,FMTdata>::const_iterator data_it = elements.begin(); data_it != elements.end(); data_it++)
            {
            if (data_it->second.getop() == FMTyieldparserop::FMTwsequation)
                {
                for (const FMTtheme& theme : themes)
                    {
                    if (theme.useindex())
                        {
                        if (data_it->second.getop() == FMTyieldparserop::FMTwsequation)
                            {
                            FMTexpression expression = data_it->second.getexpression();
                            vector<string>variables = expression.getvariables();
                            vector<string>newvariables;
                            for (const string& variable : variables)
                                {
                                if (theme.isindex(variable))
                                    {
                                    newvariables.push_back(theme.getindex(,variable));
                                    }else{
                                    newvariables.push_back(variable);
                                    }
                                }
                        vector<FMTmask>localmasking;
                        for (FMTmask mask : allmasks)
                            {
                            vector<FMTmask>decomposed = mask.decompose(theme);
                            localmasking.insert(localmasking.end(),decomposed.begin(),decomposed.end());
                            }
                            }

                        }
                    }
                }
            }
        return handlers;
        }*/

    map<string,FMTdata>FMTyieldhandler::getdataelements() const
        {
        return elements;
        }

    vector<string> FMTyieldhandler::indexes(const vector<string>& names) const
        {
        vector<string>indexs;
        if (yldtype == FMTyldwstype::FMTcomplexyld)
            {
            for (map<string,FMTdata>::const_iterator data_it = elements.begin(); data_it != elements.end(); data_it++)
                {
                if (data_it->second.getop() == FMTyieldparserop::FMTwsequation)
                    {
                    vector<string>variables = data_it->second.getsource();
                    for(const string& variable : variables)
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
        return indexs;
        }

    bool FMTyieldhandler::push_data(const string& yld,const FMTdata& data)
        {
        elements[yld]= FMTdata(data);
        return true;
        }
    FMTyldwstype FMTyieldhandler::gettype() const
        {
        return yldtype;
        }
    vector<string> FMTyieldhandler::compare(const vector<string>& keys) const
        {
        vector<string>same;
        for(const string& key : keys)
            {
            if (elements.find(key) != elements.end())
                {
                same.push_back(key);
                }
            }
        return same;
        }

     map<string,const FMTyieldhandler*> FMTyieldhandler::getdata(const vector<const FMTyieldhandler*>& datas,
                                    const vector<string>& names) const
        {
        map<string,const FMTyieldhandler*>alldata;
            for(const FMTyieldhandler* yield : datas)
                {
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "data size " << datas.size() << "\n";
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "source of COMPLEX!!!! " << string(*yield)<< "\n";
                for (const string& name : names)
                    {
                    if (yield->elements.find(name)!=yield->elements.end() && alldata.find(name)== alldata.end())
                        {
                        alldata[name] = yield;
                        }
                    if(alldata.size() == names.size())
                        {
                        return alldata;
                        }
                    }
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


     map<string,double> FMTyieldhandler::getsources( const map<string, const FMTyieldhandler*>& srcdata, const vector<const FMTyieldhandler*>& datas,
		 const int& age,const int& period, const FMTmask& resume_mask,bool& age_only) const
        {
       /* map<string,double>alldata;
        const FMTdata* lyld = &elements.at(name);
        const vector<string> sources = lyld->getsource();
        map<string,const FMTyieldhandler*> srcdata = getdata(datas,sources);
        for(map<string,const FMTyieldhandler*>::const_iterator datait = srcdata.begin(); datait != srcdata.end(); datait++)
            {
            //alldata[datait->first] = this->get(datas,datait->first,age,period);
			const FMTyieldhandler* yldata = datait->second;
			alldata[datait->first] = yldata->get(datas, datait->first, age, period);
            }
        return alldata;*/
		 map<string, double>alldata;
		 for (map<string, const FMTyieldhandler*>::const_iterator datait = srcdata.begin(); datait != srcdata.end(); datait++)
			{
			 //alldata[datait->first] = this->get(datas,datait->first,age,period);
			 const FMTyieldhandler* yldata = datait->second;
			 if (yldata->gettype()!= FMTyldwstype::FMTageyld)
				{
				age_only = false;
				}
			 alldata[datait->first] = yldata->get(datas, datait->first, age, period, resume_mask);
			}
		 return alldata;
        }

    double FMTyieldhandler::get(const vector<const FMTyieldhandler*>& datas,
                           const string yld,const int& age,const int& period,
							const FMTmask& resume_mask) const //recursive function... ?caching here? why not...
        {
        double value = 0;
        int target = 0;
         if (yldtype == FMTyldwstype::FMTageyld)
            {
			 target = age;
			 if (elements.find(yld) != elements.end())
				{
				 const FMTdata* lvalues = &elements.at(yld);
				 value = getlinearvalue(lvalues->data, target);
				}
            }else if(yldtype==FMTyldwstype::FMTtimeyld)
                {
                target = period;
				if (elements.find(yld) != elements.end())
					{
					const FMTdata* lvalues = &elements.at(yld);
					value = lvalues->data.back();
					if (target < lvalues->data.size())
						{
						value = lvalues->data.at(target);
						}
					}
                }else{
                //complex yields...
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "ENTERING COMPLEX!!!! "<< yld<< "\n";
				/*for (map<string, FMTdata>::const_iterator testit =elements.begin(); testit != elements.end(); testit++)
					{
					Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << testit->first << "\n";
					}*/
				const FMTdata* cdata = &elements.at(yld);
				if (cdata->cachevalue(resume_mask,age,period)) //check if in cache!?!?!?
					{
					//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "USING CACHE!!!!!! " << "\n";
					return cdata->get(resume_mask, age, period);
					}
				bool age_only = true;
				//int test = int(cdata->getop());
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "IN COMPLEX!!!! " << yld << test<<"\n";
               //const map<string,double>sources = this->getsources(datas,yld,age,period);
				const vector<string> sources = cdata->getsource();
				map<string, const FMTyieldhandler*> srcsdata = this->getdata(datas, sources);
                switch(cdata->getop())
                    {
                    case FMTyieldparserop::FMTwsrange:
                        {
                        size_t srcid = 0;
                        value = 1;
						const map<string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
                        for(map<string,double>::const_iterator srcit = source_values.begin();srcit!= source_values.end();srcit++)
                            {
                            const double lower = cdata->data.at(srcid);
                            const double upper = cdata->data.at(srcid+1);
                            if (srcit->second < lower || srcit->second > upper)
                                {
                                value = 0;
                                }
                            srcid+=2;
                            }
                        break;
                        }
                    case FMTyieldparserop::FMTwsmultiply:
                        {
                        value = 1;
						const map<string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
                        for(map<string,double>::const_iterator srcit = source_values.begin();srcit!= source_values.end();srcit++)
                            {
                            value *= srcit->second;
                            }
                        for(const double& vecvalue : cdata->data)
                            {
                            value *= vecvalue;
                            }
                        break;
                        }
                    case FMTyieldparserop::FMTwssum:
                        {
						const map<string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
                        for(map<string,double>::const_iterator srcit = source_values.begin();srcit!= source_values.end();srcit++)
                            {
                            value += srcit->second;
                            }
                        for(const double& vecvalue : cdata->data)
                            {
                            value += vecvalue;
                            }
                        break;
                        }
                    case FMTyieldparserop::FMTwssubstract:
                        {
						//ordering means something here!!!!
						const map<string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
						vector<double>values = cdata->tovalues(source_values);
						value = values.front();
						values.erase(values.begin());
						for (const double& yldvalue : values)
							{
							value -= yldvalue;
							}

                        break;
                        }
                    case FMTyieldparserop::FMTwsdivide:
                        {
						//ordering means something here!!!!
						const map<string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
						vector<double>values = cdata->tovalues(source_values);
						value = values.front();
						values.erase(values.begin());
						for (const double& yldvalue : values)
							{
							value /= yldvalue;
							}
                        break;
                        }
                    case FMTyieldparserop::FMTwsytp:
                        {
						//const vector<string> sources = cdata->getsource();
                        //map<string,const FMTyieldhandler*> srcsdata = this->getdata(datas,sources);
                        const FMTyieldhandler* ddata = srcsdata.begin()->second;
                        if (ddata->gettype() == FMTyldwstype::FMTageyld)
                            {
                            value = ddata->getpeak(srcsdata.begin()->first,age);
							//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "peak " << value << "\n";
                           // value = peak-age;
                            }
                        break;
                        }
                    case FMTyieldparserop::FMTwsmai:
                        {
						//const vector<string> sources = cdata->getsource();
                       // map<string,const FMTyieldhandler*> srcsdata = this->getdata(datas, sources);
                        double year = 1;
                        if (cdata->data.begin()!=cdata->data.end())
                            {
                            year = *cdata->data.begin();
                            }
                        const FMTyieldhandler* ddata = srcsdata.begin()->second;
                        if (ddata->gettype() == FMTyldwstype::FMTageyld)
                            {
							const FMTyieldhandler* handler = srcsdata.at(sources[0]);
							const FMTdata* lvalues = &handler->elements.at(sources[0]);
							value = (ddata->getlinearvalue(lvalues->data, age) / (year*age));
                            }
                        break;
                        }
                    case FMTyieldparserop::FMTwscai:
                        {
						//const vector<string> sources_yld = cdata->getsource();
                        //map<string,const FMTyieldhandler*> srcsdata = this->getdata(datas, sources_yld);
                        double year = 1;
                        if (cdata->data.begin()!=cdata->data.end())
                            {
                            year = *cdata->data.begin();
                            }
                        const FMTyieldhandler* ddata = srcsdata.begin()->second;
                        if (ddata->gettype() == FMTyldwstype::FMTageyld)
                            {
							const FMTyieldhandler* handler = srcsdata.at(sources[0]);
							const FMTdata* lvalues = &handler->elements.at(sources[0]);
							const double upval = ddata->getlinearvalue(lvalues->data, age);
							int newage = age - 1;
                            const double dwval = ddata->getlinearvalue(lvalues->data, newage);
                            value = ((upval - dwval) / (year));
                            }
                        break;
                        }
					case FMTyieldparserop::FMTwsequation:
						{
						FMTexpression expression = cdata->toexpression();
						const map<string, double>source_values = this->getsources(srcsdata, datas, age, period,resume_mask, age_only);
						/*for (const FMTyieldhandler* tyld : datas)
							{
							Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "data size " << string(tyld->mask) << "\n";
							}*/
						value = expression.shuntingyard(source_values);
						break;
						}
                    default:
                    break;
                    }
				cdata->set(value, resume_mask, age, period, age_only); //cache_back the complex yield
				//to help cache we should check at source so if all age base forget the period...
                return value;
                }



        return value;
        }

	double FMTyieldhandler::getlinearvalue(const vector<double>& dls, const int& agetarget) const
		{
		double value = 0;
		if (agetarget >= bases.back())//after and at back!
			{
			value = dls.back();
		}else if(agetarget < bases.front()) //before front!
			{
			value = (agetarget * (dls[0] / double(bases[0])));
			}else{ //within distribution
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
				double factor = ((dls[highindex] - dls[lowindex]) / (double(bases[highindex]) - double(bases[lowindex])));
				double lastvalue = dls[lowindex];
				value = lastvalue + ((agetarget - bases[lowindex]) * factor);
				}else{
				value = dls[highindex];
				}
			}
		return value;
		}

	int FMTyieldhandler::getlastbase() const
		{
		return bases.back();
		}

	double FMTyieldhandler::getlastvalue(const string yld) const
		{
		map<string, FMTdata>::const_iterator it = elements.find(yld);
		return it->second.data.back();
		}

    double FMTyieldhandler::getpeak(const string& yld, const int& targetage) const
        {
        double value = 0;
        if (this->gettype() == FMTyldwstype::FMTageyld)
            {
            map<string,FMTdata>::const_iterator it = elements.find(yld);
            double maxvalue = numeric_limits<double>::lowest();
            int location = 0;
			int peak = -1;
            vector<double>::const_iterator dblit = it->second.data.begin();
			//int  lowindex = -1;
			//int highindex = -1;
            while(dblit != it->second.data.end())
                {
					/*if (bases[location] <= targetage)
						{
						lowindex = location;
						}
					if (bases[location] >= targetage && highindex < 0)
						{
						highindex = location;
						}*/
                if (*dblit > maxvalue)
                    {
                    maxvalue = *dblit;
					peak = location;
					}
                ++location;
				++dblit;
                }
			if (peak > 0)
				{
				vector<double>peakvalues(bases.size(),0.0);
				int id = 0;
				for (const int& base : bases)
					{
					peakvalues[id]=(bases[peak]-base);
					++id;
					}
				value = getlinearvalue(peakvalues, targetage);
				/*if (lowindex!= highindex)
					{
					double bottom = (bases[peak] - bases[lowindex]);
					double top = (bases[peak] - bases[highindex]);
					double factor = ((top - bottom) / (double(bases[highindex]) - double(bases[lowindex])));
					double lastvalue = bottom;
					value = lastvalue + ((targetage - bases[lowindex]) * factor);
				}else {
					value = (bases[peak] - bases[highindex]);
					}*/
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "peak at " << peak << "\n";
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "value of " << value <<" for age "<< targetage << "\n";

				}
			}
        return value;
        }


    int FMTyieldhandler::getage(const string yld, const double& value) const
        {
        int age = 0;
        map<string,FMTdata>::const_iterator it = elements.find(yld);
        if (it!=elements.end())
            {
            const FMTdata* ldata = &it->second;
            vector<double>::const_iterator dit = ldata->data.begin();
            int high_index = 0;
            while (*(dit+ high_index)<=value)
                {
                ++high_index;
                }
			if (*(dit + high_index) == value)
				{
				age = bases[high_index];
			}else {
				int low_index = max((high_index - 1), 0);
				double top_age = (double(bases[high_index]) - double(bases[low_index]));
				double bottom_value = (*(dit + high_index) - *(dit + low_index));
				double known_value = (value - *(dit + low_index));
				double age_gap = (top_age * known_value) / bottom_value;
				age = max(1,bases[low_index] + int(round(age_gap)));
				}

            }
        return age;
        }
}
