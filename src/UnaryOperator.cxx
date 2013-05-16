/*
 * UnaryOperator.cxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#include "UnaryOperator.hxx"

UnaryOperator::UnaryOperator(Expression *child,char op):Operator(op) {
	this->child = child;
}

void UnaryOperator::UpdateInterpretation(){
	if (this->child != NULL) {
			this->child->UpdateInterpretation();
	}
}

UnaryOperator::~UnaryOperator() {
	// TODO Auto-generated destructor stub
}

std::vector<Expression*> UnaryOperator::GetChildren() {
	std::vector<Expression*> ret;
	ret.push_back(this->child);
	return ret;
}
