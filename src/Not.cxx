/*
 * Not.cxx
 *
 *  Created on: Mar 1, 2013
 *      Author: dloti
 */

#include "Not.hxx"

Not::Not(Expression* child, aig_tk::PDDL_Object_Ptr_Vec* allObjects) :
		UnaryOperator(child, '!') {
	this->allObjects = allObjects;
}

std::vector<int>* Not::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}

void Not::UpdateInterpretation() {
	this->ClearInterpretation();
	this->child->UpdateInterpretation();
	std::vector<int>* childInterpretation = this->child->GetInterpretation();
	if (childInterpretation == NULL && childInterpretation->size() > 0)
		return;

	for (unsigned i = 0; i < allObjects->size(); i++) {
		if (std::find(childInterpretation->begin(), childInterpretation->end(),i) != childInterpretation->end())
			continue;
		interpretation.push_back(i);
	}
	std::sort(this->interpretation.begin(),this->interpretation.end());
}

Not::~Not() {
}

