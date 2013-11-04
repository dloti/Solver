/*
 * ObjectFinder.cxx
 *
 *  Created on: Oct 15, 2013
 *      Author: dloti
 */

#include "ObjectFinder.hxx"

ObjectFinder::ObjectFinder(aig_tk::PDDL_Object_Ptr_Vec allObjects,
		std::map<std::string, int>* policy,
		std::vector<std::pair<std::string, int> > decisionList,
		std::vector<Expression*>* features) {
	this->allObjects = allObjects;
	this->policy = policy;
	this->features = features;
	this->decisionList = decisionList;
	MakeDecisionListJoins();
}

//bool ObjectFinder::MakeJoins() {
// incorrect check hasPositive
//	std::map<std::string, int>::iterator it;
//	joinedFeatures.clear();
//	Expression* uo;
//	int emptyjoins = 0;
//	std::cout << "Policy size: " << policy->size() << std::endl;
//	for (it = policy->begin(); it != policy->end(); ++it) {
//		std::string signature = it->first;
//		std::vector<int> pos;
//		std::vector<int> neg;
//		for (unsigned i = 0; i < signature.size(); ++i) {
//			if (signature[i] == '1') {
//				(*features)[i]->UpdateInterpretation();
//				std::vector<int>* vec = (*features)[i]->GetInterpretation();
//				for (unsigned j = 0; j < vec->size(); ++j) {
//					if (std::find(pos.begin(), pos.end(), (*vec)[j])
//							== pos.end())
//						pos.push_back((*vec)[j]);
//				}
//			} else {
//				uo = new Not((*features)[i], &allObjects);
//				uo->UpdateInterpretation();
//				std::vector<int>* vec = uo->GetInterpretation();
//				for (unsigned j = 0; j < vec->size(); ++j) {
//					if (std::find(neg.begin(), neg.end(), (*vec)[j])
//							== neg.end())
//						neg.push_back((*vec)[j]);
//				}
//			}
//
//		}
//		std::vector<int> final;
//		std::sort(pos.begin(), pos.end());
//		std::sort(neg.begin(), neg.end());
//		std::set_intersection(pos.begin(), pos.end(), neg.begin(), neg.end(),
//				std::back_inserter(final));
//		if (final.size() == 0)
//			++emptyjoins;
//		//std::cout<<"ERR policy object intersection size 0"<<std::endl;
//		joinedFeatures[signature] = final;
//	}
//
//	std::cout << "Empty joins: " << emptyjoins << std::endl;
//	if (emptyjoins == policy->size()) {
//		std::cout << "ERR all joins empty" << std::endl;
//		return false;
//	}
//	return true;
//}

//bool ObjectFinder::AreObjectsIn(std::string signature,
//		std::vector<unsigned> objects) {
//	std::vector<int> intersection;
//	std::vector<int> objs;
//	for (int i = 0; i < objects.size(); ++i)
//		objs.push_back(objects[i]);
//	std::vector<int> vec = joinedFeatures[signature];
//	if (vec.size() == 0) {
//		//std::cout << "ERR policy object intersection size 0" << std::endl;
//		return false;
//	}
//	std::sort(objs.begin(), objs.end());
//	std::cout << "Join objects ";
//	for (int i = 0; i < vec.size(); ++i)
//		std::cout << vec[i] << " ";
//	std::cout << std::endl;
//	std::cout << "Action objects ";
//	for (int i = 0; i < objs.size(); ++i)
//		std::cout << objs[i] << " ";
//	std::cout << std::endl;
//
//	std::set_intersection(vec.begin(), vec.end(), objs.begin(), objs.end(),
//			std::back_inserter(intersection));
//	std::cout << "Intersection ";
//	for (int i = 0; i < intersection.size(); ++i)
//		std::cout << intersection[i] << " ";
//	std::cout << std::endl;
//	if (intersection.size() != 0)
//		return true;
//
//	return false;
//}

bool ObjectFinder::MakeDecisionListJoins() {
	joinedFeatures.clear();
	Expression* uo;
	int emptyjoins = 0;
	std::cout<<std::endl;
	for (unsigned i = 0; i < decisionList.size(); ++i) {
		std::string signature = decisionList[i].first;
		std::vector<int> pos;
		std::vector<int> neg;
		bool hasPositive = false;
		bool hasNegative = false;
		for (unsigned j = 0; j < signature.size(); ++j) {
			if (signature[j] == '1') {
				hasPositive = true;
				(*features)[j]->UpdateInterpretation();
				std::vector<int>* vec = (*features)[j]->GetInterpretation();
				for (unsigned k = 0; k < vec->size(); ++k) {
					if (std::find(pos.begin(), pos.end(), (*vec)[k])
							== pos.end())
						pos.push_back((*vec)[k]);
				}
			} else if (signature[j] == '0') {
				hasNegative = true;
				uo = new Not((*features)[j], &allObjects);
				uo->UpdateInterpretation();
				std::vector<int>* vec = uo->GetInterpretation();
				for (unsigned k = 0; k < vec->size(); ++k) {
					if (std::find(neg.begin(), neg.end(), (*vec)[k])
							== neg.end())
						neg.push_back((*vec)[k]);
				}
			}
		}
		std::vector<int> final;
		std::sort(pos.begin(), pos.end());
		std::sort(neg.begin(), neg.end());
		std::set_intersection(pos.begin(), pos.end(), neg.begin(), neg.end(),
				std::back_inserter(final));

		//std::cout<<"ERR policy object intersection size 0"<<std::endl;
		std::cout << signature << "-";
		if (!hasPositive) {
			if (neg.size() == 0)
				++emptyjoins;
			joinedFeatures[signature] = neg;
			for (unsigned k = 0; k < neg.size(); ++k)
				std::cout << allObjects[neg[k]]->name() << " ";
		} else if (!hasNegative) {
			if (pos.size() == 0)
				++emptyjoins;
			joinedFeatures[signature] = pos;
			for (unsigned k = 0; k < pos.size(); ++k)
				std::cout << allObjects[pos[k]]->name() << " ";
		} else {
			if (final.size() == 0)
				++emptyjoins;
			joinedFeatures[signature] = final;
			for (unsigned k = 0; k < final.size(); ++k)
				std::cout << allObjects[final[k]]->name() << " ";
		}

		std::cout << std::endl;
	}
	std::cout << "Empty joins: " << emptyjoins << " Decision List size:"
			<< decisionList.size() << std::endl;
	if (emptyjoins == decisionList.size()) {
		std::cout << "ERR all joins empty" << std::endl;
		return false;
	}
	return true;
}

bool ObjectFinder::isSignatureMatch(std::string signature,
		std::string signature1) {
	for (int i = 0; i < signature.length(); ++i) {
		if (signature[i] == signature1[i])
			continue;
		if (signature[i] == '*' || signature1[i] == '*')
			continue;
		if (signature[i] != signature1[i]) {
			return false;
		}
	}
	return true;
}

bool ObjectFinder::IsObjectIn(std::string signature, int object) {
	std::map<std::string, std::vector<int> >::iterator it;
	std::vector<int> vec;
	std::string resolved_signature;
	for (it = joinedFeatures.begin(); it != joinedFeatures.end(); ++it)
		if (isSignatureMatch(it->first, signature)) {
			vec = joinedFeatures[it->first];
			resolved_signature = it->first;
		}
	std::cout<<std::endl;
	std::cout << signature << ":" << resolved_signature << "-";
	for (unsigned k = 0; k < vec.size(); ++k)
		std::cout << allObjects[vec[k]]->name() << " ";
	std::cout << std::endl;
	if (vec.size() == 0) {
		std::cout << "ERR intersection size 0" << std::endl;
		return false;
	}

	if (std::find(vec.begin(), vec.end(), object) != vec.end())
		return true;
	return false;
}

