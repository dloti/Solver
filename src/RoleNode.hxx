/*
 * RoleNode.hxx
 *
 *  Created on: Feb 28, 2013
 *      Author: dloti
 */

#ifndef ROLENODE_HXX_
#define ROLENODE_HXX_
#include "Operand.hxx"
#include <vector>
class RoleNode : public Operand {
protected:
	std::vector<std::pair<int,int> > interpretation;
public:
	RoleNode();
	RoleNode(std::string predicate);
	std::vector<int>* GetInterpretation();
	std::vector<std::pair<int,int> >* GetRoleInterpretation();
	void UpdateInterpretation();
	void ClearInterpretation();
	virtual ~RoleNode();
};

#endif /* ROLENODE_HXX_ */
