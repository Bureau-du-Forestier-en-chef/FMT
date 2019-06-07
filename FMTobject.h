#ifndef FMTobject_H_INCLUDED
#define FMTobject_H_INCLUDED

#include "FMTexceptionhandler.h"
#include "FMTquietexceptionhandler.h"
#include "FMTdefaultexceptionhandler.h"
#include "FMTdebugexceptionhandler.h"
#include "FMTfreeexceptionhandler.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/filesystem.hpp>

#if defined _MSC_VER || __CYGWIN__

#else
    #include <boost/dll/runtime_symbol_info.hpp>
#endif


#include <memory>

namespace Exception
{
class FMTobject
	{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(_exhandler);
	}
	protected:
		std::shared_ptr<FMTexceptionhandler> _exhandler;
		void checksignals();
		string getruntimelocation();
	public:
		FMTobject();
		virtual ~FMTobject();
		FMTobject(const std::shared_ptr<FMTexceptionhandler> exhandler);
		FMTobject(const FMTobject& rhs);
		FMTobject& operator = (const FMTobject& rhs);
		//virtual ~FMTobject()=default;
		void passinexceptionhandler(const std::shared_ptr<FMTexceptionhandler>& exhandler);
		void setdefaultexceptionhandler();
		void setquietexceptionhandler();
		void setdebugexceptionhandler();
		void setfreeexceptionhandler();
	};
}

#endif

