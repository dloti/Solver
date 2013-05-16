/*
 * Join.cxx
 *
 *  Created on: Feb 27, 2013
 *      Author: dloti
 */
#include<algorithm>
#include<iostream>
#include "Join.hxx"

Join::Join(Expression* left, Expression* right) :
		BinaryOperator('^') {
	this->SetLeft(left);
	this->SetRight(right);
}

std::vector<int>* Join::GetInterpretation() {
	//this->UpdateInterpretation();
	return &(this->interpretation);
}

void Join::UpdateInterpretation() {
	this->left->UpdateInterpretation();
	this->right->UpdateInterpretation();
	std::vector<int>::iterator first1 =
			this->left->GetInterpretation()->begin();
	std::vector<int>::iterator last1 = this->left->GetInterpretation()->end();
	std::vector<int>::iterator first2 =
			this->right->GetInterpretation()->begin();
	std::vector<int>::iterator last2 = this->right->GetInterpretation()->end();

	this->ClearInterpretation();

	std::set_intersection(first1, last1, first2, last2,
			std::back_inserter(this->interpretation));
	//std::sort(this->interpretation.begin(),this->interpretation.end());
}

Join::~Join() {
	// TODO Auto-generated destructor stub
}

