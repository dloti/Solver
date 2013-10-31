/*
 * ObjectFinder.cxx
 *
 *  Created on: Oct 15, 2013
 *      Author: dloti
 */

#include "ObjectFinder.hxx"

ObjectFinder::ObjectFinder(aig_tk::PDDL_Object_Ptr_Vec allObjects, std::map<std::string, int>* policy,
		std::vector<Expression*>* features) {
	this->allObjects = allObjects;
	this->policy = policy;
	this->features = features;
	MakeJoins();
}

bool ObjectFinder::MakeJoins() {
	std::map<std::string, int>::iterator it;
	joinedFeatures.clear();
	Expression* uo;
	int emptyjoins = 0;
	std::cout << "Policy size: " << policy->size() << std::endl;
	for (it = policy->begin(); it != policy->end(); ++it) {
		std::string signature = it->first;
		std::vector<int> pos;
		std::vector<int> neg;
		for (unsigned i = 0; i < signature.size(); ++i) {
			if (signature[i] == '1') {
				std::vector<int>* vec = (*features)[i]->GetInterpretation();
				for (unsigned j = 0; j < vec->size(); ++j) {
					if (std::find(pos.begin(), pos.end(), (*vec)[j]) == pos.end())
						pos.push_back((*vec)[j]);
					//For debugging
//					if ((*vec)[j] > 8) {
//						(*features)[i]->infix(std::cout);
//						std::cout << std::endl;
//					}
				}
			} else {
				uo = new Not((*features)[i], &allObjects);
				uo->UpdateInterpretation();
				std::vector<int>* vec = uo->GetInterpretation();
				for (unsigned j = 0; j < vec->size(); ++j) {
					if (std::find(neg.begin(), neg.end(), (*vec)[j]) == neg.end())
						neg.push_back((*vec)[j]);
				}
			}

		}
		std::vector<int> final;
		std::sort(pos.begin(), pos.end());
		std::sort(neg.begin(), neg.end());
		std::set_intersection(pos.begin(), pos.end(), neg.begin(), neg.end(), std::back_inserter(final));
		if (final.size() == 0)
			++emptyjoins;
		//std::cout<<"ERR policy object intersection size 0"<<std::endl;
		joinedFeatures[signature] = final;
	}

	std::cout << "Empty joins: " << emptyjoins << std::endl;
	if (emptyjoins == policy->size()) {
		std::cout << "ERR all joins empty" << std::endl;
		return false;
	}
	return true;
}

bool ObjectFinder::AreObjectsIn(std::string signature, std::vector<unsigned> objects) {
	std::vector<int> intersection;
	std::vector<int> objs;
	for (int i = 0; i < objects.size(); ++i)
		objs.push_back(objects[i]);
	std::vector<int> vec = joinedFeatures[signature];
	if (vec.size() == 0) {
		//std::cout << "ERR policy object intersection size 0" << std::endl;
		return false;
	}
	std::sort(objs.begin(), objs.end());
	std::cout << "Join objects ";
	for (int i = 0; i < vec.size(); ++i)
		std::cout << vec[i] << " ";
	std::cout << std::endl;
	std::cout << "Action objects ";
	for (int i = 0; i < objs.size(); ++i)
		std::cout << objs[i] << " ";
	std::cout << std::endl;

	std::set_intersection(vec.begin(), vec.end(), objs.begin(), objs.end(), std::back_inserter(intersection));
	std::cout << "Intersection ";
	for (int i = 0; i < intersection.size(); ++i)
		std::cout << intersection[i] << " ";
	std::cout << std::endl;
	if (intersection.size() != 0) {
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		return true;
	}

	return false;
}

