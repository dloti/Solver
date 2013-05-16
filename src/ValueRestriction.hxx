/*
 * ValueRestriction.hxx
 *
 *  Created on: Mar 1, 2013
 *      Author: dloti
 */

#ifndef VALUERESTRICTION_HXX_
#define VALUERESTRICTION_HXX_
#include "BinaryOperator.hxx"
#include "RoleNode.hxx"
#include "Expression.hxx"
#include<algorithm>

class ValueRestriction : public BinaryOperator {
public:
	ValueRestriction(Expression* left, Expression* right);
	void SetLeft(Expression* left);
	void SetRight(Expression* right);
	std::vector<int>* GetInterpretation();
	void UpdateInterpretation();
	virtual ~ValueRestriction();
};

#endif /* VALUERESTRICTION_HXX_ */
