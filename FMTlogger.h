#ifndef FMTlogger_H_INCLUDED
#define FMTlogger_H_INCLUDED
#include <iostream>
#include <boost/python.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <CoinMessageHandler.hpp>



using namespace std;

namespace Logging
{
	enum FMTlogtype 
		{
		FMT_Debug = 0,
		FMT_Info = 1,
		FMT_Warn = 2,
		FMT_Error = 3
		};


	class FMTlogger : public CoinMessageHandler
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(_msglevel);
			}
		FMTlogtype _msglevel = FMT_Debug;
		string getlevel(FMTlogtype ltype);
		public:
			FMTlogger(FMTlogtype type);
			virtual ~FMTlogger() = default;
			FMTlogger();
			FMTlogger(const FMTlogger& rhs);
			FMTlogger& operator = (const FMTlogger& rhs);
			int print() override;
			void checkSeverity() override;
			CoinMessageHandler * clone() const override;
			template<class T>
			FMTlogger& operator<<(const T &msg)
				{
				#if defined (_MSC_VER) && defined(FMTPY)
					string value = boost::lexical_cast<std::string>(msg);
					PySys_WriteStdout(value.c_str());
				#else
					cout << msg;
				#endif 
					return *this;
				}
		};

}
#endif