/*
 * TransitiveClosure.cxx
 *
 *  Created on: Mar 12, 2013
 *      Author: dloti
 */

#include "TransitiveClosure.hxx"

TransitiveClosure::TransitiveClosure(Expression* child) :
		UnaryOperator(child, '*') {
}

std::vector<std::pair<int, int> >* TransitiveClosure::GetRoleInterpretation() {
	//this->UpdateInterpretation();
	return &(this->roleInterpretation);
}

void TransitiveClosure::UpdateInterpretation() {
	this->child->UpdateInterpretation();
	ClearInterpretation();

	std::vector<std::pair<int, int> >* childInterpretation =
			(this->child)->GetRoleInterpretation();

	if (childInterpretation == NULL || childInterpretation->size() == 0)
		return;

	std::vector<int> elements;
	std::vector<std::pair<int, int> >::iterator first =
			childInterpretation->begin(), last = childInterpretation->end();

	for (; first != last; ++first) {
		if (std::find(elements.begin(), elements.end(), first->first)
				== elements.end()) {
			elements.push_back(first->first);
		}
		if (std::find(elements.begin(), elements.end(), first->second)
				== elements.end()) {
			elements.push_back(first->second);
		}
	}

	bool matrix[elements.size()][elements.size()];
	std::pair<int, int>* p;
	for (unsigned i = 0; i < elements.size(); ++i) {
		for (unsigned j = 0; j < elements.size(); ++j) {
			p = new std::pair<int, int>(elements[i], elements[j]);
			if (std::find(childInterpretation->begin(), last, *p) != last)
				matrix[i][j] = true;
			else
				matrix[i][j] = false;
			delete p;
		}
	}
	for (unsigned k = 0; k < elements.size(); ++k) {
		for (unsigned i = 0; i < elements.size(); ++i) {
			for (unsigned j = 0; j < elements.size(); ++j) {
				if ((matrix[i][k] && matrix[k][j]) || matrix[i][j]) {
					matrix[i][j] = true;
				}
			}
		}
	}

	//std::cout<<std::endl<<"Matrica"<<std::endl;
	for (unsigned i = 0; i < elements.size(); ++i) {
		for (unsigned j = 0; j < elements.size(); ++j) {
			//std::cout<<(matrix[i][j]?"T":"F")<<" ";
			if (matrix[i][j]) {
				p = new std::pair<int,int>(elements[i], elements[j]);

				this->roleInterpretation.push_back(*p);
			}
			//std::cout<<std::endl;
		}
	}

}

TransitiveClosure::~TransitiveClosure() {
	// TODO Auto-generated destructor stub
}

