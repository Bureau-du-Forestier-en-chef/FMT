/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/



#include "FMTobject.h"
#include "FMTdevelopment.h"
#include "FMTmask.h"


namespace Graph
{
	class FMTgraphvertextoyield;
}



#ifndef FMTYIELDREQUEST_H_INCLUDED
#define FMTYIELDREQUEST_H_INCLUDED

namespace Core
{
	class FMTyields;
	class FMTyieldhandler;
	// DocString: FMTyieldrequest
	/**
	Using a pointer to a given graph and a FMTdevelopement you can create thise class to request yield values.
	The yield will be calculated using the FMTyields class but also using the FMTgraph so the location of the 
	actual state of the FMTdevelopement.You can also build a yield request without anyreference to a graph for calculatin yield values.
	*/
	class FMTyieldrequest final : public FMTobject
	{
		friend class FMTyields;
		mutable std::vector<const std::unique_ptr<FMTyieldhandler>*> datas;
		mutable FMTmask resume_mask;
		mutable FMTyields const* yldsptr;
		const FMTdevelopment* development;
		const Graph::FMTgraphvertextoyield* graphvertex;
		private:
		void updatedata(const FMTyields& yields) const;
		public:
		FMTyieldrequest(const FMTdevelopment& ldevelopment,
			const Graph::FMTgraphvertextoyield& lgraphvertex);
		FMTyieldrequest(const FMTdevelopment& ldevelopment,
			const FMTyieldrequest& oldrequest);
		FMTyieldrequest(const FMTdevelopment& ldevelopment);
		FMTyieldrequest() = default;
		const FMTdevelopment& getdevelopment() const;
		const std::vector<const std::unique_ptr<FMTyieldhandler>*>&getdatas() const;
		const Core::FMTmask& getresumemask() const;
		const Graph::FMTgraphvertextoyield* getvertexgraphinfo() const;
		const Core::FMTyields& getyields() const;
	};
	


}


#endif // FMTYLD_H_INCLUDED
