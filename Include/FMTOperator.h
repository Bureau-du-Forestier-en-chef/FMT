/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOPE_Hm_included
#define FMTOPE_Hm_included

#include "FMTutility.h"
#include <string>
#include <vector>
#include <boost/functional/hash.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTException.h"



namespace Core
{

	// DocString: FMTokey
	/**
	@brief Enumerator of the operators usable in an FMTOperator (multiply, divide, add, sub, comma, pow).
	*/
	enum class FMTokey
	{
		multiply = 1,
		divide = 2,
		add = 3,
		sub = 4,
		comma = 5,
		pow = 6,
		notvalid = 7
	};

// DocString: FMTOperator
/**
@brief Mathematical operator usable in yield and output expressions.
*/
class FMTEXPORT FMTOperator
{
    FMTokey key;
	friend class boost::serialization::access;
	// DocString: FMTOperator::serialize
	/**
	@brief Serialize the FMTOperator for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
	*/
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(key);
	}
public:
	// DocString: FMTOperator()
	/**
	@brief Default constructor for FMTOperator.
	*/
	FMTOperator();
	// DocString: ~FMTOperator()
	/**
	@brief Default destructor for FMTOperator.
	*/
	~FMTOperator() = default;
    // DocString: FMTOperator(FMTokey)
    /**
    @brief Construct a FMTOperator from an operator key.
    @param[in] lkey the operator key.
    */
    FMTOperator(FMTokey lkey);
    // DocString: FMTOperator(std::string)
    /**
    @brief Construct a FMTOperator from a string key.
    @param[in] strkey the string representation of the operator.
    */
    FMTOperator(std::string strkey);
	// DocString: FMTOperator::operator==
	/**
	@brief Equality comparison operator of FMTOperator.
	@param[in] rhs the FMTOperator to compare with.
	@return true if both operators are equal else false.
	*/
	bool operator == (const FMTOperator& rhs) const;
	// DocString: FMTOperator::operator!=
	/**
	@brief Inequality comparison operator of FMTOperator.
	@param[in] rhs the FMTOperator to compare with.
	@return true if both operators are different else false.
	*/
	bool operator != (const FMTOperator& rhs) const;
    // DocString: FMTOperator::call
    /**
    @brief Apply the operator to two values.
    @param[in] rhs1 the first operand.
    @param[in] rhs2 the second operand.
    @return the result of the operation.
    */
    double call(const double& rhs1, const double& rhs2) const;
    // DocString: FMTOperator(const FMTOperator&)
    /**
    @brief Copy constructor for FMTOperator.
    @param[in] rhs the FMTOperator to copy.
    */
    FMTOperator(const FMTOperator& rhs);
    // DocString: FMTOperator::operator=
    /**
    @brief Copy assignment operator for FMTOperator.
    @param[in] rhs the FMTOperator to copy.
    @return a reference to this FMTOperator.
    */
    FMTOperator& operator = (const FMTOperator& rhs);
	// DocString: FMTOperator::precedence
	/**
	@brief Return the precedence of the operator.
	@return the precedence.
	*/
	int precedence() const;
	// DocString: FMTOperator::associativity
	/**
	@brief Return the associativity of the operator.
	@return the associativity.
	*/
	std::string associativity() const;
	// DocString: FMTOperator::reverse
	/**
	@brief Return the reversed operator.
	@return the reversed operator.
	*/
	FMTOperator reverse() const;
	// DocString: FMTOperator::valid
	/**
	@brief Return true if the operator is valid.
	@return true if the operator is valid else false.
	*/
	bool valid() const;
	// DocString: FMTOperator::hash
	/**
	@brief Return the hash of the operator.
	@return the hash value.
	*/
	size_t hash() const;
	// DocString: FMTOperator::isFactor
	/**
	@brief Return true if the operator is a multiplication or a division.
	@return true if the operator is a factor else false.
	*/
	bool isFactor() const;
	// DocString: FMTOperator::isDivide
	/**
	@brief Return true if the operator is a division.
	@return true if the operator is a division else false.
	*/
	bool isDivide() const;
	// DocString: FMTOperator::getKey
	/**
	@brief Return the key of the operator.
	@return the operator key.
	*/
	FMTokey getKey() const;
    // DocString: FMTOperator::operator std::string
    /**
    @brief Convert the operator to its string representation.
    @return the string representation of the operator.
    */
    operator std::string() const;
};

}

#endif // FMTOPE_Hm_included
