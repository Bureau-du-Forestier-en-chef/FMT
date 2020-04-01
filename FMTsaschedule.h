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

#ifndef FMTSASCHEDULE_H
#define FMTSASCHEDULE_H

#include <memory>
#include <string>
#include <vector>


namespace Spatial
{

class FMTsaschedule
//Abstract class for cooling schedule
    {
    protected:
		std::string schedule_type;
    public:
        FMTsaschedule();//Constructor
        virtual~FMTsaschedule()=default;//Destructor
        virtual double reduce_temp(double& temp);//
        virtual std::string get_schedule_type()const;
        virtual std::unique_ptr<FMTsaschedule> Clone()const;
    };

class FMTexponentialschedule : public FMTsaschedule
    {
    protected:
        double alpha;
		std::string schedule_type;
    public:
        FMTexponentialschedule(double lalpha = 0.9);
        ~FMTexponentialschedule()=default;
        double reduce_temp(double& temp);
		std::string get_schedule_type()const;
		std::unique_ptr<FMTsaschedule> Clone()const;

    };
}
#endif // FMTSASCHEDULE_H
