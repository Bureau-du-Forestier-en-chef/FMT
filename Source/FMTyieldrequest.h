/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/



#include "FMTobject.h"
#include "FMTmask.h"
#include "FMTdevelopment.h"
#include "FMTgraphvertextoyield.h"

namespace Core 
{
	class FMTyields;
	class FMTyieldhandler;
	//class FMTdevelopement;

}



#ifndef FMTYIELDREQUEST_H_INCLUDED
#define FMTYIELDREQUEST_H_INCLUDED

namespace Core
{
	
	// DocString: FMTyieldrequest
	/**
	Using a pointer to a given graph and a FMTdevelopement you can create thise class to request yield values.
	The yield will be calculated using the FMTyields class but also using the FMTgraph so the location of the 
	actual state of the FMTdevelopement.You can also build a yield request without anyreference to a graph for calculatin yield values.
	*/
	class FMTyieldrequest : public FMTobject
	{
		mutable std::vector<const std::unique_ptr<FMTyieldhandler>*> datas;
		mutable FMTmask resume_mask;
		const FMTdevelopment* development;
		const Graph::FMTgraphvertextoyield* graphvertex;
		public:
		FMTyieldrequest(const FMTdevelopment& ldevelopment,
			const Graph::FMTgraphvertextoyield& lgraphvertex);
		FMTyieldrequest(const FMTdevelopment& ldevelopment);
		FMTyieldrequest() = default;
		void updatedata(const FMTyields& yields) const;
		inline const FMTdevelopment& getdevelopment() const
			{
			return *development;
			}
		inline const std::vector<const std::unique_ptr<FMTyieldhandler>*>getdatas() const
			{
			return datas;
			}
		inline const Core::FMTmask& getresumemask() const
			{
			return resume_mask;
			}
		inline const Graph::FMTgraphvertextoyield* getvertexgraphinfo() const
			{
			return graphvertex;
			}
	};
	


}


#endif // FMTYLD_H_INCLUDED
