/*
 * Compostion.cxx
 *
 *  Created on: Mar 12, 2013
 *      Author: dloti
 */

#include "Compostion.hxx"

Compostion::Compostion():BinaryOperator('o') {
	// TODO Auto-generated constructor stub

}

void Compostion::SetLeft(Expression* left) {
	this->left = left;
}

void Compostion::SetRight(Expression* right) {
	this->right = right;
}

std::vector<std::pair<int,int> >* Compostion::GetRoleInterpretation() {
	return &(this->roleInterpretation);
}

void Compostion::UpdateInterpretation() {

}

Compostion::~Compostion() {
	// TODO Auto-generated destructor stub
}

