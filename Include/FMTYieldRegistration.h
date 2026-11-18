#ifndef FMTYIELDREGISTRATION_H
#define FMTYIELDREGISTRATION_H

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace Core
{
	void registerYieldTypes(boost::archive::binary_iarchive& ar);
	void registerYieldTypes(boost::archive::binary_oarchive& ar);
}

#endif