/*
 * ValueRestriction.cxx
 *
 *  Created on: Mar 1, 2013
 *      Author: dloti
 */

#include "ValueRestriction.hxx"
#include<vector>

ValueRestriction::ValueRestriction(Expression* left, Expression* right) :
		BinaryOperator('.') {
	this->SetLeft(left);
	this->SetRight(right);
}

void ValueRestriction::SetLeft(Expression* left) {
	this->left = left;
}
void ValueRestriction::SetRight(Expression* right) {
	this->right = right;
}
std::vector<int>* ValueRestriction::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}

void ValueRestriction::UpdateInterpretation() {
	this->left->UpdateInterpretation();
	this->right->UpdateInterpretation();
	std::vector<std::pair<int, int> >* ri = (this->left)->GetRoleInterpretation();
	//(dynamic_cast<RoleNode*>(this->left))->GetRoleInterpretation();
	std::vector<int> riFirst;
	std::vector<std::pair<int, int> >::iterator pairIterator;
	std::vector<int>::iterator it, end;
	for (pairIterator = ri->begin(); pairIterator != ri->end(); ++pairIterator) {
		riFirst.push_back(pairIterator->second);
	}
	it = riFirst.begin();
	end = riFirst.end();
	//NOTE: added sort
	std::sort(it, end);
	std::vector<int>::iterator first2 = this->right->GetInterpretation()->begin();
	std::vector<int>::iterator last2 = this->right->GetInterpretation()->end();
	std::sort(first2, last2);
	this->ClearInterpretation();
	std::vector<int> secondPart;
	std::set_intersection(it, end, first2, last2, std::back_inserter(secondPart));

	for (int j = 0; j < secondPart.size(); ++j) {
		for (pairIterator = ri->begin(); pairIterator != ri->end(); ++pairIterator) {
			if (pairIterator->second == secondPart[j]) {
				if (std::find(this->interpretation.begin(), this->interpretation.end(), pairIterator->first)
						== this->interpretation.end()) {
					this->interpretation.push_back(pairIterator->first);
					break;
				}
			}
		}
	}
}

ValueRestriction::~ValueRestriction() {
	// TODO Auto-generated destructor stub
}

