#ifndef FMTdebugexceptionhandler_H_INCLUDED
#define FMTdebugexceptionhandler_H_INCLUDED
#include "FMTexceptionhandler.h"
namespace Exception
{
	class FMTdebugexceptionhandler : public FMTexceptionhandler
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & boost::serialization::make_nvp("parent_handler", boost::serialization::base_object<FMTexceptionhandler>(*this));
		}
	protected:
		string getsrcinfo(const int& line,
			const string& file) const;
	public:
		FMTdebugexceptionhandler();
		~FMTdebugexceptionhandler() = default;
		FMTlev raise(FMTexc lexception, FMTwssect lsection, string text,
			const int& line, const string& file) override;
	};

}

#endif
