/*
 * Operand.cxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#include "Operand.hxx"

Operand::Operand():Expression() {
	this->goal = false;
}

Operand::~Operand() {
	// TODO Auto-generated destructor stub
}

bool Operand::IsGoal() {
	return goal;
}

void Operand::IsGoal(bool goal) {
	this->goal = goal;
}
