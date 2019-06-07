#include "FMTlifespans.h"

namespace Core{
FMTlifespans::FMTlifespans() : FMTlist<int>(){}
FMTlifespans::FMTlifespans(const FMTlifespans& rhs) : FMTlist<int>(rhs){}
FMTlifespans& FMTlifespans::operator = (const FMTlifespans& rhs)
    {
    if (this!=&rhs)
        {
		FMTlist<int>::operator = (rhs);
        }
    return *this;
    }

bool FMTlifespans::operator == (const FMTlifespans& rhs) const
	{
	return (FMTlist<int>::operator ==(rhs));
	}

FMTlifespans::operator string() const
    {
    string line;
	vector<FMTmask>::const_iterator mask_iterator = this->maskbegin();
	vector<int>::const_iterator data_iterator = this->databegin();
    for(size_t id = 0; id < this->size();++id)
        {
        line+=string(*mask_iterator)+" ";
        line+=to_string(*data_iterator);
        line+="\n";
		++mask_iterator;
		++data_iterator;
        }
    return line;
    }
}
