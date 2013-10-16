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
#include "Expression.hxx"
#include "Operator.hxx"
#include "UnaryOperator.hxx"
#include "Not.hxx"
class ObjectFinder {
	aig_tk::PDDL_Object_Ptr_Vec allObjects;
	std::map<std::string, int >* policy;
	std::vector<Expression* >* features;
	std::map<std::string, std::vector<int> > joinedFeatures;

public:
	ObjectFinder(aig_tk::PDDL_Object_Ptr_Vec allObjects, std::map<std::string, int>* policy,
			std::vector<Expression*>* features);
	void MakeJoins();
	bool AreObjectsIn(std::string signature, std::vector<unsigned> objects);

};

#endif /* OBJECTFINDER_HXX_ */
