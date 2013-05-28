/*
 * InverseRole.cxx
 *
 *  Created on: Mar 12, 2013
 *      Author: dloti
 */

#include "InverseRole.hxx"

InverseRole::InverseRole(Expression *child) :
		UnaryOperator(child, 'I') {

}

std::vector<std::pair<int, int> >* InverseRole::GetRoleInterpretation() {
	//this->UpdateInterpretation();
	return &(this->roleInterpretation);
}

void InverseRole::UpdateInterpretation() {
	this->child->UpdateInterpretation();
	ClearInterpretation();
	std::vector<std::pair<int, int> >* childInterpretation = (this->child)->GetRoleInterpretation();

	if (childInterpretation == NULL || childInterpretation->size() == 0)
		return;

	std::vector<std::pair<int, int> >::iterator first = childInterpretation->begin(), last = childInterpretation->end();

	for (; first != last; ++first) {
		std::pair<int, int>* p = new std::pair<int, int>(first->second, first->first);
		this->roleInterpretation.push_back(*p);
	}
}

InverseRole::~InverseRole() {
	// TODO Auto-generated destructor stub
}

