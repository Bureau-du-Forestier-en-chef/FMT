
#include "FMTYieldRegistration.h"
#include "FMTAgeYieldHandler.h"
#include "FMTTimeYieldHandler.h"
#include "FMTComplexYieldHandler.h"
#include "FMTModelYieldHandler.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>



namespace Core
{
	void registerYieldTypes(boost::archive::binary_iarchive& ar)
	{
		ar.register_type<FMTAgeYieldHandler>();
		ar.register_type<FMTTimeYieldHandler>();
		ar.register_type<FMTComplexYieldHandler>();
		ar.register_type<FMTModelYieldHandler>();
	}

	void registerYieldTypes(boost::archive::binary_oarchive& ar)
	{
		ar.register_type<FMTAgeYieldHandler>();
		ar.register_type<FMTTimeYieldHandler>();
		ar.register_type<FMTComplexYieldHandler>();
		ar.register_type<FMTModelYieldHandler>();
	}
}