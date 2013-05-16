/*
 * Rule.cxx
 *
 *  Created on: Feb 1, 2013
 *      Author: dloti
 */

#include "Rule.hxx"

Rule::Rule(aig_tk::Action* action) {
	this->coverage = 0;
	this->correct = 0;
	this->examples = 1;
	this->action = action;
}

aig_tk::Action* Rule::GetAction() {
	return this->action;
}

bool Rule::AddConcept(Expression* concept) {
	//if (concepts.size() >= action->pddl_objs_idx().size())
	if (concepts.size() > 0)
		return false;
	concepts.push_back(concept);
	return true;
}

std::vector<Expression*> Rule::GetConcepts() {
	return this->concepts;
}

bool operator ==(Rule& r1, Rule& r2) {
	bool allConcepts = true;
	if (r1.concepts.size() == r2.concepts.size()) {

		if (r1.concepts[0] != r2.concepts[0]) {
			allConcepts = false;
		}

	} else
		allConcepts = false;
	return (allConcepts );//&& (r1.action->name() == r2.action->name()));
}

bool operator !=(Rule& r1, Rule& r2) {
	return !(r1 == r2);
}

std::ostream& operator <<(std::ostream& out, Rule& r) {
	for (unsigned i = 0; i < r.concepts.size(); i++) {
		r.concepts[i]->infix(out);
	}
	out << ":" << r.action->name();
	return out;
}

bool Rule::operator <(const Rule& r2) const {
	double mised = this->examples - this->correct;
	double r2mised = r2.examples - r2.correct;
	if (mised / this->examples < r2mised / r2.examples)
		return true;
	else if (mised / this->examples > mised / r2.examples)
		return false;
	else
		return (this->correct / this->examples > r2.correct / r2.examples);
}

int Rule::GetCurrentCoverage() {
	return this->concepts[0]->GetInterpretation()->size();
}

Rule::~Rule() {
	// TODO Auto-generated destructor stub
}

