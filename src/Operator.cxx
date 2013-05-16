/*
 * Operator.cxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#include "Operator.hxx"

Operator::Operator(char op):Expression() {
	this->op = op;
}

void Operator::ClearInterpretation() {
	this->interpretation.clear();
	this->roleInterpretation.clear();
}

std::vector<int>* Operator::GetInterpretation() {
	if(isRole) this->interpretation.clear();
	return &(this->interpretation);
}

std::vector<std::pair<int, int> >* Operator::GetRoleInterpretation() {
	if(!isRole) this->roleInterpretation.clear();
	return &(this->roleInterpretation);
}

Operator::~Operator() {
	// TODO Auto-generated destructor stub
}

