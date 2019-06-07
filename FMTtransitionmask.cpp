#include "FMTtransitionmask.h"

namespace Core{

void FMTtransitionmask::build(const string& lmask, const vector<FMTtheme>& themes )
    {
    int id = 0;
    vector<string>bases;
    boost::split(bases,lmask,boost::is_any_of("\t "), boost::token_compress_on);
    for(const FMTtheme& theme : themes)
        {
        if (theme.isattribute(bases[id]))
            {
            flippedselection.resize(theme.size()+flippedselection.size(),false);
            }else{
            flippedselection.resize(theme.size()+flippedselection.size(),true);
            }
        ++id;
        }
    selection = (flippedselection ^ mask.data);
    }

FMTtransitionmask::FMTtransitionmask():FMTmaskfilter(),FMTspec(),mask(),proportion()
    {

    }
FMTtransitionmask::FMTtransitionmask(const string& lmask,const vector<FMTtheme>& themes,/*const int& llock,*/const double& lproportion):FMTmaskfilter(),FMTspec(),
        mask(lmask,themes),proportion(lproportion)
        {
        this->build(lmask,themes);
        /*int id = 0;
        vector<string>bases;
        boost::split(bases,lmask,boost::is_any_of("\t "));
        for(const FMTtheme& theme : themes)
            {
            if (theme.isattribute(bases[id]))
                {
                flippedselection.resize(theme.size()+flippedselection.size(),false);
                }else{
                flippedselection.resize(theme.size()+flippedselection.size(),true);
                }
            ++id;
            }
        selection = (flippedselection ^ mask.data);*/
        }
    FMTmask FMTtransitionmask::trans(const FMTmask& basemask) const
        {
        return FMTmask(((flippedselection & basemask.data) | selection));
        }
    FMTtransitionmask::FMTtransitionmask(const FMTtransitionmask& rhs) :FMTmaskfilter(rhs),FMTspec(rhs) , mask(rhs.mask),proportion(rhs.proportion)
        {

        }
    FMTtransitionmask::FMTtransitionmask(const FMTtransitionmask& rhs,const FMTmask& lmask,const vector<FMTtheme>& themes):FMTmaskfilter(),
                FMTspec(rhs),/*lock(rhs.lock),*/mask(lmask),proportion(rhs.proportion)
        {
        this->build(string(lmask),themes);
        }
    FMTtransitionmask& FMTtransitionmask::operator = (const FMTtransitionmask& rhs)
        {
        if (this!=&rhs)
            {
            /*lock = rhs.lock;*/
            proportion = rhs.proportion;
            mask = rhs.mask;
            FMTspec::operator = (rhs);
            FMTmaskfilter::operator = (rhs);
            }
        return *this;
        }
    FMTtransitionmask::operator string() const
        {
        string line;
        line += "*TARGET " + string(mask) + " " + to_string(proportion) + " " + FMTspec::operator string();
        /*if (lock>1)
            {
            line += " _LOCK " + to_string(lock);
            }*/
        return line;
        }
    FMTdevelopment FMTtransitionmask::disturb(const FMTdevelopment& dev,const FMTyields& yields,const vector<FMTtheme>& themes, const bool& reset_age) const
        {
        FMTdevelopment newdev(dev);
        newdev.mask = this->trans(dev.mask);
        newdev.lock = 0;
		bool age_change = false;
        if(!lock.empty())
            {
			int lower_lock = lock.getlower();// very weird that a period 1 lock is different when lock > 1
			//if (lower_lock == 1)
				//{
				//++lower_lock;
				//}
            newdev.lock = lower_lock;//welll wtf...
            }
        if(!age.empty())
            {
            newdev.age = age.getlower();
			age_change = true;
            }else if(!ylds.empty())
                {
                newdev.age = yields.getage(dev,*this);
				age_change = true;
				//Logging::FMTlogger(Logging::FMTlogtype::FMT_Info) << "NEW AGE " << newdev.age << "\n";
                }
        if(!themes.empty())
            {
            newdev.mask.update(themes);
            }
		if (reset_age && !age_change)
			{
			newdev.age = 0;
			}
        return newdev;
        }

    void FMTtransitionmask::setproportion(double newproportion)
        {
        proportion  = min(newproportion,100.00);
        }

    double FMTtransitionmask::getproportion() const
        {
            return proportion;
        }

    FMTmask FMTtransitionmask::getmask() const
        {
            return mask;
        }


    map<string,string>FMTtransitionmask::get(const vector<FMTtheme>& themes) const
        {
        map<string,string>mapping;
        int id = 1;
        for(const FMTtheme& theme : themes)
            {
            const string value = mask.get(theme);
            string name = theme.name;
            if (name.empty())
                {
                name = "THEME_"+to_string(id);
                }
            mapping[name] = value;
            ++id;
            }
        return mapping;
        }

	bool FMTtransitionmask::operator == (const FMTtransitionmask& rhs) const
		{
		return (FMTspec::operator== (rhs) &&
			mask == rhs.mask &&
			proportion == rhs.proportion);
		}
}
