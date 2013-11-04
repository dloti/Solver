/*
 * ObjectFinder.hxx
 *
 *  Created on: Oct 15, 2013
 *      Author: dloti
 */

#ifndef OBJECTFINDER_HXX_
#define OBJECTFINDER_HXX_
#include<iostream>
#include<vector>
#include<map>
#include <planning/PDDL_Type.hxx>
#include <planning/PDDL_Object.hxx>
#include "Expression.hxx"
#include "Operator.hxx"
#include "UnaryOperator.hxx"
#include "Not.hxx"
class ObjectFinder {
	aig_tk::PDDL_Object_Ptr_Vec allObjects;
	std::map<std::string, int >* policy;
	std::vector<Expression* >* features;
	std::map<std::string, std::vector<int> > joinedFeatures;
	std::vector<std::pair<std::string, int> > decisionList;

public:
	ObjectFinder(aig_tk::PDDL_Object_Ptr_Vec allObjects, std::map<std::string, int>* policy,
			std::vector<std::pair<std::string, int> > decisionList,
			std::vector<Expression*>* features);
//	bool MakeJoins();
//	bool AreObjectsIn(std::string signature, std::vector<unsigned> objects);
	bool isSignatureMatch(std::string signature, std::string signature1);
	bool MakeDecisionListJoins();
	bool IsObjectIn(std::string signature, int object);

};

#endif /* OBJECTFINDER_HXX_ */
