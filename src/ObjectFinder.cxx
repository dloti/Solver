/*
 * ObjectFinder.cxx
 *
 *  Created on: Oct 15, 2013
 *      Author: dloti
 */

#include "ObjectFinder.hxx"

ObjectFinder::ObjectFinder(aig_tk::PDDL_Object_Ptr_Vec allObjects,
		std::map<std::string, int>* policy,
		std::vector<Expression*>* features) {
	this->allObjects = allObjects;
	this->policy = policy;
	this->features = features;
	MakeJoins();
}

void ObjectFinder::MakeJoins() {
	std::map<std::string, int>::iterator it;
	joinedFeatures.clear();
	UnaryOperator* uo;
	std::cout << "Policy size: " << policy->size() << std::endl;

	for (it = policy->begin(); it != policy->end(); ++it) {
		std::string signature = it->first;
		std::vector<int> pos;
		std::vector<int> neg;
		for (unsigned i = 0; i < signature.size(); ++i) {
			if (signature[i] == '1') {
				std::vector<int>* vec = (*features)[i]->GetInterpretation();
				for (unsigned j = 0; j < vec->size(); ++j) {
					if (std::find(pos.begin(), pos.end(), (*vec)[j])
							== pos.end())
						pos.push_back((*vec)[j]);
					//For debugging
//					if ((*vec)[j] > 8) {
//						(*features)[i]->infix(std::cout);
//						std::cout << std::endl;
//					}
				}
			} else {
				uo = new Not((*features)[i], &allObjects);
				std::vector<int>* vec = uo->GetInterpretation();
				for (unsigned j = 0; j < vec->size(); ++j) {
					if (std::find(neg.begin(), neg.end(), (*vec)[j])
							== neg.end())
						neg.push_back((*vec)[j]);
				}
			}
		}
		std::vector<int> final;
		std::sort(pos.begin(), pos.end());
		std::sort(neg.begin(), neg.end());
		std::set_intersection(pos.begin(), pos.end(), neg.begin(), neg.end(),
				std::back_inserter(final));
		if(final.size() == 0)
			std::cout<<"ERR policy object intersection size 0"<<std::endl;
		joinedFeatures[signature] = final;
	}
}

bool ObjectFinder::AreObjectsIn(std::string signature,std::vector<unsigned> objects){
	std::vector<int> intersection;

	std::vector<int> vec = joinedFeatures[signature];
	std::sort(objects.begin(),objects.end());
	std::set_intersection(vec.begin(), vec.end(), objects.begin(), objects.end(),
					std::back_inserter(intersection));

	if(intersection.size() == objects.size()) return true;

	return false;
}

