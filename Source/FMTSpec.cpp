#include "FMTSpec.h"

#include <algorithm>
#include <functional>
#include <iterator>

namespace Core
{
FMTSpec::FMTSpec()
    : per(), age(), lock(), yieldnames(), yieldbounds()
{
}

FMTSpec::FMTSpec(const FMTSpec& rhs)
    : per(rhs.per),
      age(rhs.age),
      lock(rhs.lock),
      yieldnames(rhs.yieldnames),
      yieldbounds(rhs.yieldbounds)
{
}

FMTSpec& FMTSpec::operator=(const FMTSpec& rhs)
{
    if (this != &rhs)
    {
        per = rhs.per;
        age = rhs.age;
        yieldnames = rhs.yieldnames;
        yieldbounds = rhs.yieldbounds;
        lock = rhs.lock;
    }
    return *this;
}

bool FMTSpec::add(const FMTSpec& rhs)
{
    if (!rhs.per.empty())
    {
        per = rhs.per;
    }

    if (!rhs.age.empty())
    {
        age = rhs.age;
    }

    for (size_t ylid = 0; ylid < rhs.yieldnames.size(); ++ylid)
    {
        const auto nameit = std::find(
            yieldnames.begin(),
            yieldnames.end(),
            rhs.yieldnames.at(ylid));

        if (nameit != yieldnames.end())
        {
            yieldbounds[std::distance(yieldnames.begin(), nameit)] =
                rhs.yieldbounds.at(ylid);
        }
        else
        {
            yieldbounds.push_back(rhs.yieldbounds.at(ylid));
            yieldnames.push_back(rhs.yieldnames.at(ylid));
        }
    }

    return true;
}

bool FMTSpec::setBounds(const FMTPerBounds& bound)
{
    return per.add(bound);
}

bool FMTSpec::addBounds(const FMTAgeBounds& bound)
{
    return age.add(bound);
}

bool FMTSpec::addBounds(const FMTYldBounds& bound)
{
    const auto nameit = std::find(yieldnames.begin(), yieldnames.end(), bound.yield);

    if (nameit != yieldnames.end())
    {
        yieldbounds[std::distance(yieldnames.begin(), nameit)].add(bound);
    }
    else
    {
        yieldbounds.push_back(bound);
        yieldnames.push_back(bound.yield);
    }

    return true;
}

bool FMTSpec::addBounds(const FMTLockBounds& bound)
{
    return lock.add(bound);
}

FMTSpec::operator std::string() const
{
    std::string line;
    bool andstuff = false;

    if (!per.empty())
    {
        line += std::string(per) + " ";
        andstuff = true;
    }

    if (!age.empty())
    {
        if (andstuff)
        {
            line += "AND ";
        }
        line += std::string(age) + " ";
        andstuff = true;
    }

    if (!yieldnames.empty())
    {
        if (andstuff)
        {
            line += "AND ";
        }

        size_t nyld = 1;
        for (size_t ylid = 0; ylid < yieldnames.size(); ++ylid)
        {
            if (nyld != yieldbounds.size())
            {
                line += std::string(yieldbounds.at(ylid)) + " AND ";
            }
            else
            {
                line += std::string(yieldbounds.at(ylid)) + " ";
            }
            ++nyld;
        }
    }

    if (!lock.empty())
    {
        line += std::string(lock);
    }

    return line;
}

bool FMTSpec::operator==(const FMTSpec& rhs) const
{
    return per == rhs.per &&
           age == rhs.age &&
           lock == rhs.lock &&
           yieldnames == rhs.yieldnames &&
           yieldbounds == rhs.yieldbounds;
}

bool FMTSpec::operator<(const FMTSpec& rhs) const
{
    if (per < rhs.per) return true;
    if (rhs.per < per) return false;
    if (age < rhs.age) return true;
    if (rhs.age < age) return false;
    if (lock < rhs.lock) return true;
    if (rhs.lock < lock) return false;
    if (yieldnames < rhs.yieldnames) return true;
    if (rhs.yieldnames < yieldnames) return false;
    if (yieldbounds < rhs.yieldbounds) return true;
    if (rhs.yieldbounds < yieldbounds) return false;
    return false;
}

bool FMTSpec::empty() const
{
    return per.empty() && age.empty() && yieldnames.empty() && lock.empty();
}

size_t FMTSpec::hash() const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, std::hash<int>()(per.getLower()));
    boost::hash_combine(seed, std::hash<int>()(per.getUpper()));
    boost::hash_combine(seed, std::hash<int>()(age.getLower()));
    boost::hash_combine(seed, std::hash<int>()(age.getUpper()));
    boost::hash_combine(seed, std::hash<int>()(lock.getLower()));
    boost::hash_combine(seed, std::hash<int>()(lock.getUpper()));

    for (size_t ylid = 0; ylid < yieldnames.size(); ++ylid)
    {
        boost::hash_combine(seed, std::hash<std::string>()(yieldnames.at(ylid)));
        boost::hash_combine(seed, std::hash<double>()(yieldbounds.at(ylid).getLower()));
        boost::hash_combine(seed, std::hash<double>()(yieldbounds.at(ylid).getUpper()));
    }

    return seed;
}

bool FMTSpec::emptyLock() const
{
    return lock.empty();
}

bool FMTSpec::emptyAge() const
{
    return age.empty();
}

bool FMTSpec::emptyYlds() const
{
    return yieldnames.empty();
}

bool FMTSpec::emptyPeriod() const
{
    return per.empty();
}

int FMTSpec::getPeriodUpperBound() const
{
    return per.upper;
}

int FMTSpec::getPeriodLowerBound() const
{
    return per.lower;
}

int FMTSpec::getLockUpperBound() const
{
    return lock.upper;
}

int FMTSpec::getLockLowerBound() const
{
    return lock.lower;
}

int FMTSpec::getAgeUpperBound() const
{
    return age.upper;
}

int FMTSpec::getAgeLowerBound() const
{
    return age.lower;
}

bool FMTSpec::isSubsetOf(const FMTSpec& rhs) const
{
    bool persubset = true;
    if (!per.empty() && !rhs.per.empty())
    {
        persubset = per.getLower() >= rhs.per.getLower() &&
                    per.getUpper() <= rhs.per.getUpper();
    }
    else if (!rhs.per.empty() && per.empty())
    {
        persubset = false;
    }

    bool agesubset = true;
    if (!age.empty() && !rhs.age.empty())
    {
        agesubset = age.getLower() >= rhs.age.getLower() &&
                    age.getUpper() <= rhs.age.getUpper();
    }
    else if (!rhs.age.empty() && age.empty())
    {
        agesubset = false;
    }

    bool yldssubset = true;
    if (!yieldnames.empty() && !rhs.yieldnames.empty())
    {
        for (size_t ylid = 0; ylid < yieldnames.size(); ++ylid)
        {
            const auto it = std::find(
                rhs.yieldnames.begin(),
                rhs.yieldnames.end(),
                yieldnames.at(ylid));

            if (it == rhs.yieldnames.end())
            {
                yldssubset = false;
                break;
            }

            const size_t location = std::distance(rhs.yieldnames.begin(), it);
            yldssubset =
                rhs.yieldbounds.at(location).getLower() >= yieldbounds.at(ylid).getLower() &&
                yieldbounds.at(ylid).getUpper() <= rhs.yieldbounds.at(location).getUpper();

            if (!yldssubset)
            {
                break;
            }
        }
    }
    else if (!rhs.yieldnames.empty() && yieldnames.empty())
    {
        yldssubset = false;
    }

    return persubset && agesubset && yldssubset;
}
}
