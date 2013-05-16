/*
 * ConceptNode.cxx
 *
 *  Created on: Feb 14, 2013
 *      Author: dloti
 */

#include "ConceptNode.hxx"
#include <algorithm>
ConceptNode::ConceptNode() {
	this->goal = false;
	this->predicate = "";
}


ConceptNode::ConceptNode(std::string predicate) {
	this->goal = false;
	this->predicate = predicate;
}

ConceptNode::~ConceptNode() {
}

void ConceptNode::UpdateInterpretation() {
}

void ConceptNode::ClearInterpretation() {
	this->interpretation.clear();
}

std::vector<std::pair<int,int> >* ConceptNode::GetRoleInterpretation() {
	return NULL;
}

std::vector<int>* ConceptNode::GetInterpretation() {
	return &(this->interpretation);
}

//std::ostream& operator<< (std::ostream &out, ConceptNode &cNode)
//{
//	out<<cNode.left;
//	if(cNode.left==NULL && cNode.right==NULL)
//	out<< "(" << cNode.left->predicate <<"^"<<cNode.right->predicate;
//	out<<cNode.right;
//    return out;
//}

