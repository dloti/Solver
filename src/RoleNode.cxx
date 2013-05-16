/*
 * RoleNode.cxx
 *
 *  Created on: Feb 28, 2013
 *      Author: dloti
 */

#include "RoleNode.hxx"

RoleNode::RoleNode() {
}

RoleNode::RoleNode(std::string predicate) {
	this->predicate = predicate;
}

RoleNode::~RoleNode() {
	// TODO Auto-generated destructor stub
}

std::vector<int>* RoleNode::GetInterpretation(){
	return NULL;
}

std::vector<std::pair<int,int> >* RoleNode::GetRoleInterpretation() {
	return &(this->interpretation);
}

void RoleNode::UpdateInterpretation(){
	//TODO
}

void RoleNode::ClearInterpretation(){
	this->interpretation.clear();
}
