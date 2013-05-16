/*
 * Equality.cxx
 *
 *  Created on: Mar 11, 2013
 *      Author: dloti
 */

#include "Equality.hxx"

Equality::Equality(Expression* left, Expression* right) :
		BinaryOperator('=') {
	this->SetLeft(left);
	this->SetRight(right);

}

std::vector<int>* Equality::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}

void Equality::UpdateInterpretation() {
	this->left->UpdateInterpretation();
	this->right->UpdateInterpretation();
	ClearInterpretation();
	std::vector<std::pair<int, int> >* leftInterpretation = (this->left)->GetRoleInterpretation();
	std::vector<std::pair<int, int> >* rightInterpretation = (this->right)->GetRoleInterpretation();

	if (leftInterpretation == NULL || leftInterpretation->size() == 0 || rightInterpretation == NULL
			|| rightInterpretation->size() == 0)
		return;

	std::vector<std::pair<int, int> >::iterator it = rightInterpretation->begin(), last = rightInterpretation->end(),
			it1 = leftInterpretation->begin(), last1 = leftInterpretation->end();
	if (leftInterpretation->size() < rightInterpretation->size()) {
		it1 = rightInterpretation->begin();
		last1 = rightInterpretation->end();
		it = leftInterpretation->begin();
		last = leftInterpretation->end();
	}
	const std::vector<std::pair<int, int> >::iterator tmp = it1;
	for (; it != last; ++it) {
		for (it1 = tmp; it1 != last1; ++it1) {
			if (it->first == it1->first && it->second == it1->second
					&& std::find(this->interpretation.begin(), this->interpretation.end(), it->first)
							== this->interpretation.end()) {
				this->interpretation.push_back(it->first);
				break;
			}
		}
	}

	std::sort(this->interpretation.begin(),this->interpretation.end());
}

void Equality::SetLeft(Expression* left) {
	this->left = left;
}

void Equality::SetRight(Expression* right) {
	this->right = right;
}

Equality::~Equality() {
	// TODO Auto-generated destructor stub
}

