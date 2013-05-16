/*
 * BinaryOperator.cxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#include "BinaryOperator.hxx"

BinaryOperator::BinaryOperator(char op):Operator(op) { left = NULL; right = NULL;}



void BinaryOperator::SetLeft(Expression* left) {
	this->left = left;
}

void BinaryOperator::SetRight(Expression* right) {
	this->right = right;
}

Expression* BinaryOperator::GetLeft() {
	return this->left;
}
Expression* BinaryOperator::GetRight() {
	return this->right;
}

void BinaryOperator::UpdateInterpretation(){
	if (this->left != NULL && this->right != NULL) {
			this->left->UpdateInterpretation();
			this->right->UpdateInterpretation();
	}
}

std::vector<Expression*> BinaryOperator::GetChildren() {
	std::vector<Expression*> ret;
	ret.push_back(this->left);
	ret.push_back(this->right);
	return ret;
}

BinaryOperator::~BinaryOperator() {
	// TODO Auto-generated destructor stub
}

