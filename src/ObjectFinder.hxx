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
class ObjectFinder {
	std::vector<std::string> allObjects;
	std::map<std::string, int >* policy;
	std::vector<Expression* >* features;
	std::map<std::string, std::vector<int> > joinedFeatures;

public:
	ObjectFinder(std::vector<std::string> allObjects, std::map<std::string, int>* policy,
			std::vector<Expression*>* features);
	void MakeJoins();

};

#endif /* OBJECTFINDER_HXX_ */
