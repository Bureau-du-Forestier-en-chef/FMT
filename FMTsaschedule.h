#ifndef FMTSASCHEDULE_H
#define FMTSASCHEDULE_H

#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace Spatial
{

class FMTsaschedule
//Abstract class for cooling schedule
    {
    protected:
        string schedule_type;
    public:
        FMTsaschedule();//Constructor
        virtual~FMTsaschedule()=default;//Destructor
        virtual double reduce_temp(double& temp);//
        virtual string get_schedule_type()const;
        virtual unique_ptr<FMTsaschedule> Clone()const;
    };

class FMTexponentialschedule : public FMTsaschedule
    {
    protected:
        double alpha;
        string schedule_type;
    public:
        FMTexponentialschedule(double lalpha = 0.9);
        ~FMTexponentialschedule()=default;
        double reduce_temp(double& temp);
        string get_schedule_type()const;
        unique_ptr<FMTsaschedule> Clone()const;

    };
}
#endif // FMTSASCHEDULE_H
