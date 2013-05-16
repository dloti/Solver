/*
 * BinaryOperator.hxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#ifndef BINARYOPERATOR_HXX_
#define BINARYOPERATOR_HXX_
#include "Operator.hxx"
class BinaryOperator: public Operator {
protected:
	Expression *left, *right;
public:
	BinaryOperator(char op);
	virtual void SetLeft(Expression* left);
	virtual void SetRight(Expression* right);
	Expression* GetLeft();
	Expression* GetRight();
	void print(std::ostream& s) const {
		s << op;
	}
	void infix(std::ostream& s) const {
		if (op == '.')
			s << "(-V-";
		s << '(';
		left->infix(s);
		s << ')';
		print(s);
		s << '(';
		right->infix(s);
		s << ')';
		if (op == '.')
			s << ")";
	}
	void UpdateInterpretation();
	std::vector<int>* GetInterpretation() {
		return new std::vector<int>();
	}
	;
	std::vector<Expression*> GetChildren();
	virtual ~BinaryOperator();
};

#endif /* BINARYOPERATOR_HXX_ */
