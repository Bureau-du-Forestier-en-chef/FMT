#ifndef FMTfunctioncall_H_INCLUDED
#define FMTfunctioncall_H_INCLUDED

#include <string>
#include <math.h>

using namespace std;

namespace Core
{
	enum class FMTfkey
		{
		expo = 1,
		ln = 2,
		notvalid = 3
		};

	class FMTfunctioncall
	{
		FMTfkey key;
	public:
		FMTfunctioncall();
		FMTfunctioncall(const string& base);
		double call(const double& rhs) const;
		FMTfunctioncall(const FMTfunctioncall& rhs);
		FMTfunctioncall& operator = (const FMTfunctioncall& rhs);
		bool valid() const;
		~FMTfunctioncall() = default;
	};
}

#endif