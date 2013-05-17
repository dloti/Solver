/*
 * Rule.hxx
 *
 *  Created on: Feb 1, 2013
 *      Author: dloti
 */

#ifndef RULE_HXX_
#define RULE_HXX_
#include "Expression.hxx"
#include <planning/Action.hxx>
class Rule {
protected:
	int coverage, correct;
	int examples;

	Expression* concept;
	std::string action;
	aig_tk::Action* toolkitAction;
public:
	Rule(Expression* concept, std::string action);
	void AddConcept(Expression* concept);
	Expression* GetConcept();
	std::string GetAction();
	inline int GetMaxConcepts() {
		return 1;
	}
	inline int GetCoverage() {
		return coverage;
	}
	inline int GetMised() {
		return examples - correct;
	}
	inline int GetCorrect() {
		return correct;
	}
	inline void SetCorrect(int correct) {
		this->correct = correct;
	}
	inline void SetToolkitAction(aig_tk::Action* action) {
		this->toolkitAction = action;
	}
	inline aig_tk::Action* GetToolkitAction() {
		return this->toolkitAction;
	}
	inline void IncCoverage() {
		coverage++;
	}
	inline void IncCorrect() {
		correct++;
	}
	inline double GetExamples() {
		return examples;
	}
	inline void IncExamples() {
		examples++;
	}
	int GetCurrentCoverage();
	friend std::ostream& operator<<(std::ostream &out, Rule &r);
	friend bool operator==(Rule &r1, Rule &r2);
	friend bool operator!=(Rule &r1, Rule &r2);
	bool operator <(const Rule& r2) const;
	virtual ~Rule();
};

#endif /* RULE_HXX_ */
