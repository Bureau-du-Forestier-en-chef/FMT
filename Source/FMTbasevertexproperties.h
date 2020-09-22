/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTbasevertexproperties_H_INCLUDED
#define FMTbasevertexproperties_H_INCLUDED

#include "FMTfuturdevelopment.h"
#include "FMTactualdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <memory>



namespace Graph
{
	class FMTbasevertexproperties
	{
		// DocString: FMTbasevertexproperties::save
		/**
		Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void save(Archive& ar, const unsigned int version) const
			{
			ar & development->getarea();
			ar & (*development); //turn it to a non virtual class
			}
		// DocString: FMTbasevertexproperties::load
		/**
		Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
		*/
		template<class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			double areavalue = 0;
			ar & areavalue;
			Core::FMTdevelopment base;
			ar & base;
			if (areavalue!=0)
				{

				development = std::unique_ptr<Core::FMTdevelopment>(new Core::FMTactualdevelopment(base, areavalue));
			}else {
				development = std::unique_ptr<Core::FMTfuturdevelopment>(new Core::FMTfuturdevelopment(base));
				}
		}
		BOOST_SERIALIZATION_SPLIT_MEMBER()
	protected:
		std::unique_ptr<Core::FMTdevelopment>development;
	public:
		virtual ~FMTbasevertexproperties() = default;
		FMTbasevertexproperties()=default;
		FMTbasevertexproperties(const FMTbasevertexproperties& rhs);
		virtual inline int getconstraintID() const
			{
			return 0;
			}
		inline const Core::FMTdevelopment& get() const
		{
			return (*development);
		}
		bool operator < (const FMTbasevertexproperties& rhs) const;
		FMTbasevertexproperties& operator = (const FMTbasevertexproperties& rhs);
		FMTbasevertexproperties(const Core::FMTfuturdevelopment& ldevelopment);
		FMTbasevertexproperties(const Core::FMTactualdevelopment& ldevelopment);
		FMTbasevertexproperties(const Core::FMTdevelopment& ldevelopment);
		FMTbasevertexproperties(const Core::FMTfuturdevelopment& ldevelopment,
			const int& lconstraintID);
		FMTbasevertexproperties(const Core::FMTactualdevelopment& ldevelopment,
			const int& lconstraintID);
		FMTbasevertexproperties(const Core::FMTdevelopment& ldevelopment,
			const int& lconstraintID);
		void setdevlopementmask(const Core::FMTmask& newmask);
		virtual double getbaseRHS() const;
		virtual size_t hash() const;
		bool operator == (const FMTbasevertexproperties& rhs) const;
		bool operator != (const FMTbasevertexproperties& rhs) const;
	};
}


BOOST_CLASS_EXPORT_KEY(Graph::FMTbasevertexproperties)


namespace boost {

	template <>
	struct hash<Graph::FMTbasevertexproperties>
	{
		std::size_t operator()(const Graph::FMTbasevertexproperties& vertex_properties) const
		{
			return vertex_properties.hash();
		}
	};


}


#endif
