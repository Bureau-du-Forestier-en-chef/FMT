/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef FMTactualdevelopment_H_INCLUDED
#define FMTactualdevelopment_H_INCLUDED

#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

namespace Core

{
class FMTactualdevelopment : public FMTdevelopment
	{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("development", boost::serialization::base_object<FMTdevelopment>(*this));
		ar & BOOST_SERIALIZATION_NVP(area);
	}
		double area;
	public:
		FMTactualdevelopment();
		~FMTactualdevelopment()=default;
		FMTactualdevelopment(const FMTactualdevelopment& rhs);
		FMTactualdevelopment(const FMTmask& mask, const int& age, const int& lock, const double& area);
		FMTactualdevelopment(const FMTdevelopment& rhs, double larea);
		FMTactualdevelopment& operator = (const FMTactualdevelopment& rhs);
		bool operator == (const FMTactualdevelopment& rhs) const;
		bool operator != (const FMTactualdevelopment& rhs) const;
		bool operator < (const FMTactualdevelopment& rhs) const;
		operator std::string() const override;
		double getarea() const override;
		void setarea(const double& newarea);
		std::unique_ptr<FMTdevelopment> Clone() const override;
		FMTactualdevelopment presolve(const FMTmask& selectedmask, const std::vector<FMTtheme>&presolvedthemes) const;
	};

class FMTactualdevelopmentcomparator
	{
		const FMTdevelopment* basedev;
	public:
		FMTactualdevelopmentcomparator(const FMTdevelopment* base);
		bool operator()(const FMTactualdevelopment& actualdev) const;

	};

}

BOOST_CLASS_EXPORT_KEY(Core::FMTactualdevelopment);
#endif
