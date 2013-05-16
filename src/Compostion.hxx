/*
 * Compostion.hxx
 *
 *  Created on: Mar 12, 2013
 *      Author: dloti
 */

#ifndef COMPOSTION_HXX_
#define COMPOSTION_HXX_
#include "BinaryOperator.hxx"
class Compostion : BinaryOperator{
public:
	Compostion();
	void SetLeft(Expression* left);
	void SetRight(Expression* right);
	std::vector<std::pair<int,int> >* GetRoleInterpretation();
	void UpdateInterpretation();
	virtual ~Compostion();
};

#endif /* COMPOSTION_HXX_ */
