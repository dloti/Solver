/*
 * UnaryOperator.hxx
 *
 *  Created on: Feb 26, 2013
 *      Author: dloti
 */

#ifndef UNARYOPERATOR_HXX_
#define UNARYOPERATOR_HXX_
#include "Operator.hxx"
class UnaryOperator: public Operator {
protected:
	Expression* child;
public:
	UnaryOperator(Expression *child, char op);
	virtual ~UnaryOperator();
	void print(std::ostream& s) const {
		s << op;
	}
	void infix(std::ostream& s) const {
		;
		print(s);
		s << "(";
		child->infix(s);
		s << ")";
	}
	void prefix(std::ostream& s) const {
		print(s);
		s << "(";
		child->prefix(s);
		s << ")";
	}
	void UpdateInterpretation();
	std::vector<Expression*> GetChildren();
};

#endif /* UNARYOPERATOR_HXX_ */
