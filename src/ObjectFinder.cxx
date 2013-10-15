/*
 * ObjectFinder.cxx
 *
 *  Created on: Oct 15, 2013
 *      Author: dloti
 */

#include "ObjectFinder.hxx"

ObjectFinder::ObjectFinder(std::vector<std::string> allObjects, std::map<std::string, int>* policy,
		std::vector<Expression*>* features) {
	this->allObjects = allObjects;
	this->policy = policy;
	this->features = features;
	MakeJoins();
}

void ObjectFinder::MakeJoins() {
	std::map<std::string, int>::iterator it;
	joinedFeatures.clear();
	std::cout << "Policy size: " << policy->size() << std::endl;

	for (it = policy->begin(); it != policy->end(); ++it) {
		std::string signature = it->first;
		std::vector<int> tmp;
		for (unsigned i = 0; i < signature.size(); ++i) {
			if (signature[i] == '1') {
				std::vector<int>* vec = (*features)[i]->GetInterpretation();
				for (unsigned j = 0; j < vec->size(); ++j) {
					if (std::find(tmp.begin(), tmp.end(), (*vec)[j]) == tmp.end())
						tmp.push_back((*vec)[j]);
					if ((*vec)[j] > 8) {
						(*features)[i]->infix(std::cout);
						std::cout << std::endl;
					}
				}
			}
		}
		std::sort(tmp.begin(), tmp.end());
		joinedFeatures[signature] = tmp;
	}
}

