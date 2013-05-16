/*
 * Not.hxx
 *
 *  Created on: Mar 1, 2013
 *      Author: dloti
 */

#ifndef NOT_HXX_
#define NOT_HXX_
#include "UnaryOperator.hxx"

class Not :public UnaryOperator {
	 aig_tk::PDDL_Object_Ptr_Vec* allObjects;
public:
	Not(Expression* child, aig_tk::PDDL_Object_Ptr_Vec* allObjects);
	std::vector<int>* GetInterpretation();
	void UpdateInterpretation();
	virtual ~Not();
};

#endif /* NOT_HXX_ */
