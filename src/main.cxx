#include <planning/FF_PDDL_To_STRIPS.hxx>
#include <planning/STRIPS_Problem.hxx>
#include <planning/Fluent.hxx>
#include <planning/Action.hxx>
#include <planning/PDDL_Type.hxx>
#include <planning/PDDL_Object.hxx>
#include <planning/PDDL_Operator.hxx>
#include <planning/heuristics/Max_Heuristic.hxx>
#include <planning/heuristics/Relaxed_Plan_Extractor.hxx>
#include <planning/Types.hxx>
#include <planning/inference/Propagator.hxx>

#include <search/Node.hxx>
#include <search/Best_First_Search.hxx>

#include <util/time.hxx>
#include <util/ext_math.hxx>

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <typeinfo>
#include <cmath>
#include "Expression.hxx"
#include "ConceptNode.hxx"
#include "RoleNode.hxx"
#include "BinaryOperator.hxx"
#include "Join.hxx"
#include "Not.hxx"
#include "InverseRole.hxx"
#include "TransitiveClosure.hxx"
#include "Equality.hxx"
#include "ValueRestriction.hxx"
#include "Rule.hxx"

using namespace aig_tk;
using namespace std;

map<string, RoleNode*> primitiveRoles;
map<string, RoleNode*> goalRoles;
map<string, ConceptNode*> goalConcepts;
map<string, ConceptNode*> primitiveConcepts;
map<string, ConceptNode*> typeConcepts;
vector<Expression*> rootConcepts;
vector<Expression*> rootRoles;
vector<Rule> ruleSet;
PDDL_Object_Ptr_Vec instanceObjects;
vector<string> actions;

void print_expressions(string title, vector<Expression*>* expressions) {
	vector<Expression*>::iterator it;
	cout << endl << "-" << title << "-" << endl;
	if (expressions->begin() == expressions->end()) {
		cout << "\tNo expressions" << endl;
		return;
	}
	for (it = expressions->begin(); it != expressions->end(); ++it) {
		cout << "\t";
		(*it)->infix(cout);
		cout << " Interp: ";
		for (unsigned i = 0; i < (*it)->GetInterpretation()->size(); i++) {
			cout << instanceObjects[(*(*it)->GetInterpretation())[i]]->signature() << " ";
		}
		cout << endl;
	}
}

void printout() {
	/*Just print out the primitive concepts and roles*/
	map<string, ConceptNode*>::iterator pos;
	cout << "Primitive concepts: " << endl;
	for (pos = primitiveConcepts.begin(); pos != primitiveConcepts.end(); ++pos) {
		cout << "Concept: ";
		pos->second->print(cout);
		cout << endl;
	}
	cout << "Type concepts: " << endl;
	for (pos = typeConcepts.begin(); pos != typeConcepts.end(); ++pos) {
		cout << "Concept: ";
		pos->second->print(cout);
		cout << endl;
	}
	cout << "Goal concepts: " << endl;
	for (pos = goalConcepts.begin(); pos != goalConcepts.end(); ++pos) {
		cout << "Concept: ";
		pos->second->print(cout);
		cout << endl;
	}

	map<string, RoleNode*>::iterator riter;
	cout << "Primitive roles: " << endl;
	for (riter = primitiveRoles.begin(); riter != primitiveRoles.end(); ++riter) {
		cout << "Role: ";
		riter->second->print(cout);
		cout << endl;
	}
	cout << "Goal roles: " << endl;
	for (riter = goalRoles.begin(); riter != goalRoles.end(); ++riter) {
		cout << "Role: ";
		riter->second->print(cout);
		cout << endl;
	}

	print_expressions("Compund concepts", &rootConcepts);
	print_expressions("Compund roles", &rootRoles);
	cout << "Actions" << endl;
	for (unsigned i = 0; i < actions.size(); ++i) {
		cout << actions[i] << endl;
	}
}

void initialize_root_concepts() {
	if (rootConcepts.size() > 0 && rootRoles.size() > 0)
		return;
	map<string, ConceptNode*>::iterator conceptIt;
	map<string, RoleNode*>::iterator roleIt;
	for (conceptIt = primitiveConcepts.begin(); conceptIt != primitiveConcepts.end(); ++conceptIt) {
		rootConcepts.push_back(conceptIt->second);
	}
	for (conceptIt = typeConcepts.begin(); conceptIt != typeConcepts.end(); ++conceptIt) {
		rootConcepts.push_back(conceptIt->second);
	}
	for (conceptIt = goalConcepts.begin(); conceptIt != goalConcepts.end(); ++conceptIt) {
		rootConcepts.push_back(conceptIt->second);
	}
	for (roleIt = primitiveRoles.begin(); roleIt != primitiveRoles.end(); ++roleIt) {
		rootRoles.push_back(roleIt->second);
	}
	for (roleIt = goalRoles.begin(); roleIt != goalRoles.end(); ++roleIt) {
		rootRoles.push_back(roleIt->second);
	}
}

void get_type_concepts_interpretation(STRIPS_Problem& prob) {
	//TODO type concepts
//	for (unsigned i = 0; i < prob.num_types(); i++) {
//		string type_signature = prob.types()[i]->signature();
//		if (type_signature.compare("NO-TYPE") == 0
//				|| type_signature.compare("ARTFICIAL-ALL-OBJECTS") == 0)
//			continue;
//		ConceptNode* c = typeConcepts[type_signature];
//		vector<int>* interpretation = c->GetInterpretation();
//		aig_tk::PDDL_Object_Ptr_Vec obj_vec = prob.objects_by_type(i);
//
//		for (unsigned k = 0; k < obj_vec.size(); k++)
//			interpretation->push_back(obj_vec[k]->index());
//	}
}

void get_primitive_concepts_relations(STRIPS_Problem& prob) {
	//Get types as primitive concepts
	for (unsigned i = 0; i < prob.num_types(); i++) {
		string type_signature = prob.types()[i]->signature();
		if (type_signature.compare("NO-TYPE") == 0 || type_signature.compare("ARTFICIAL-ALL-OBJECTS") == 0)
			continue;
		ConceptNode* c = new ConceptNode(type_signature);
		typeConcepts[type_signature] = c;
	}

	//Get fluents as primitive concepts
	aig_tk::Index_Vec types_idxs;
	unsigned fnum = prob.num_fluents();
	for (unsigned k = 0; k < fnum; k++) {
		types_idxs = prob.fluents()[k]->pddl_types_idx();

		if (types_idxs.size() == 1 && prob.types()[types_idxs[0]]->name() == "NO-TYPE") {
			continue;
		}
		if (types_idxs.size() > 1) {
			if (types_idxs.size() == 2) {
				map<string, RoleNode*>::iterator itPR;
				bool inside = false;
				for (itPR = primitiveRoles.begin(); itPR != primitiveRoles.end(); ++itPR) {
					if ((*itPR->second).GetPredicate() == prob.fluents()[k]->predicate()) {
						inside = true;
						break;
					}
				}
				if (inside)
					continue;
				RoleNode* rnode = new RoleNode(prob.fluents()[k]->predicate());
				primitiveRoles[prob.fluents()[k]->predicate()] = rnode;

				rnode = new RoleNode(prob.fluents()[k]->predicate());
				rnode->IsGoal(true);
				goalRoles[prob.fluents()[k]->predicate()] = rnode;
			}
			continue;
		}

		//Get relations as primitive roles
		map<string, ConceptNode*>::iterator it;
		bool inside = false;

		for (it = primitiveConcepts.begin(); it != primitiveConcepts.end(); ++it) {
			if (it->second->GetPredicate() == prob.fluents()[k]->predicate()) {
				inside = true;
				break;
			}
		}

		if (inside || (prob.types()[types_idxs[0]]->name().compare("NO-TYPE")) == 0) {
			continue;
		}

		ConceptNode* c = new ConceptNode(prob.fluents()[k]->predicate());
		primitiveConcepts[prob.fluents()[k]->predicate()] = c;
		c = new ConceptNode(prob.fluents()[k]->predicate());
		c->IsGoal(true);
		goalConcepts[prob.fluents()[k]->predicate()] = c;

	}

	/*Just print out the primitive concepts and roles*/
	map<string, ConceptNode*>::iterator pos;
	cout << "Primitive concepts: " << endl;
	for (pos = primitiveConcepts.begin(); pos != primitiveConcepts.end(); ++pos) {
		cout << "Concept: ";
		pos->second->print(cout);
		cout << endl;
	}
	cout << "Type concepts: " << endl;
	for (pos = typeConcepts.begin(); pos != typeConcepts.end(); ++pos) {
		cout << "Concept: ";
		pos->second->print(cout);
		cout << endl;
	}

	map<string, RoleNode*>::iterator riter;
	cout << "Primitive roles: " << endl;
	for (riter = primitiveRoles.begin(); riter != primitiveRoles.end(); ++riter) {
		cout << "Concept: ";
		riter->second->print(cout);
		cout << endl;
	}
}

void print_ruleset() {
	vector<Rule>::iterator ruleIterator;
	cout << "**************Rules******************" << endl;
	for (ruleIterator = ruleSet.begin(); ruleIterator != ruleSet.end(); ++ruleIterator) {
		cout << "\t" << *ruleIterator << "; Examples: " << ruleIterator->GetExamples() << "; Coverage:"
				<< ruleIterator->GetCoverage() << "; Hits:" << ruleIterator->GetCorrect() << "; Mised:"
				<< ruleIterator->GetMised() << std::endl;
	}
	cout << "*************************************";
}

void print_interpretations(STRIPS_Problem& prob) {
	print_expressions("Concepts", &rootConcepts);
	vector<Expression*>::iterator it;
	cout << "-Roles-" << endl;
	for (it = rootRoles.begin(); it != rootRoles.end(); ++it) {
		cout << "\t";
		(*it)->infix(cout);
		cout << " Interp: ";
		for (unsigned i = 0; i < (*it)->GetRoleInterpretation()->size(); i++) {
			cout << "(" << instanceObjects[(*(*it)->GetRoleInterpretation())[i].first]->signature() << ","
					<< instanceObjects[(*(*it)->GetRoleInterpretation())[i].second]->signature() << ")" << " ";
		}
		cout << endl;
	}
	print_ruleset();
	cout << endl;
}

void print_goal_interpretations(STRIPS_Problem& prob) {
	map<string, ConceptNode*>::iterator pos;
	cout << "-Goal concepts-" << endl;
	for (pos = goalConcepts.begin(); pos != goalConcepts.end(); ++pos) {
		cout << "\t" << pos->first << " Interp: ";
		for (unsigned i = 0; i < pos->second->GetInterpretation()->size(); i++) {
			cout << instanceObjects[(*pos->second->GetInterpretation())[i]]->signature() << " ";
		}
		cout << endl;
	}

//Goal Role interpretations
	cout << "-Goal roles-" << endl;
	map<string, RoleNode*>::iterator itrl;
	vector<pair<int, int> >::iterator itintrp;
	for (itrl = goalRoles.begin(); itrl != goalRoles.end(); ++itrl) {
		cout << "\t" << itrl->first << " Interp: ";
		vector<pair<int, int> >* rinterpretation = (*itrl->second).GetRoleInterpretation();

		for (itintrp = rinterpretation->begin(); itintrp != rinterpretation->end(); ++itintrp) {
			cout << "(" << instanceObjects[(int) (itintrp->first)]->signature() << ",";
			cout << instanceObjects[(int) (itintrp->second)]->signature() << ")" << " ";
		}
		cout << endl;
	}
}

void clear_interpretations() {
	map<string, ConceptNode*>::iterator itPC;
	for (itPC = primitiveConcepts.begin(); itPC != primitiveConcepts.end(); ++itPC) {
		itPC->second->ClearInterpretation();
	}

	map<string, RoleNode*>::iterator itPR;
	for (itPR = primitiveRoles.begin(); itPR != primitiveRoles.end(); ++itPR) {
		itPR->second->ClearInterpretation();
	}
}

void clear_static_interpretations() {
	map<string, ConceptNode*>::iterator itPC;
	for (itPC = goalConcepts.begin(); itPC != goalConcepts.end(); ++itPC) {
		itPC->second->ClearInterpretation();
	}

	for (itPC = typeConcepts.begin(); itPC != typeConcepts.end(); ++itPC) {
		itPC->second->ClearInterpretation();
	}

	map<string, RoleNode*>::iterator itPR;
	for (itPR = goalRoles.begin(); itPR != goalRoles.end(); ++itPR) {
		itPR->second->ClearInterpretation();
	}
}

void update_compound_interpretations() {
	vector<Expression*>::iterator pos;
	for (pos = rootConcepts.begin(); pos != rootConcepts.end(); ++pos) {
		(*pos)->UpdateInterpretation();
	}
	for (pos = rootRoles.begin(); pos != rootRoles.end(); ++pos) {
		(*pos)->UpdateInterpretation();
	}
}

void get_goal_interpretations(STRIPS_Problem& prob) {
	cout << "-Goal state-" << endl;
	for (unsigned k = 0; k < prob.goal().size(); k++) {
		unsigned p = prob.goal()[k];
		aig_tk::Index_Vec types_idxs = prob.fluents()[p]->pddl_types_idx();
		int arity = types_idxs.size();
		if ((prob.types()[types_idxs[0]]->name().compare("NO-TYPE")) == 0)
			continue;
		cout << prob.fluents()[p]->signature() << endl;

		if (arity > 2)
			continue;
		string current_predicate = prob.fluents()[p]->predicate();
		Index_Vec& objs_idx = prob.fluents()[p]->pddl_objs_idx();

		pair<int, int>* po;
		if (arity == 2 && objs_idx.size() == 2) {
			vector<pair<int, int> >* interpPRVec = goalRoles[current_predicate + 'g']->GetRoleInterpretation();
			po = new pair<int, int>(objs_idx[0], objs_idx[1]);

			if (std::find((*interpPRVec).begin(), (*interpPRVec).end(), *po) == (*interpPRVec).end()) {
				interpPRVec->push_back(*po);
			} else {
				delete po;
			}
			continue;
		}

		//TODO find function
		for (unsigned j = 0; j < objs_idx.size(); j++) {
			bool found = false;
			int primitiveConceptsSize = goalConcepts[current_predicate + 'g']->GetInterpretation()->size();
			for (int k = 0; k < primitiveConceptsSize; k++) {
				if ((*goalConcepts[current_predicate + 'g']->GetInterpretation())[k] == objs_idx[j]) {
					found = true;
					break;
				}
			}
			if (!found) {
				goalConcepts[current_predicate + 'g']->GetInterpretation()->push_back(objs_idx[j]);
				found = false;
			}
		}
	}
}

void update_primitive_interpretations(STRIPS_Problem& prob, Node* n) {
	aig_tk::State* s = n->s();

	int tmp = 0;
	string current_predicate;
	clear_interpretations();
	for (unsigned i = 0; i < s->fluent_vec().size(); i++) {
		tmp = s->fluent_vec()[i];

		aig_tk::Index_Vec types_idxs = prob.fluents()[tmp]->pddl_types_idx();
		int arity = types_idxs.size();

		if ((prob.types()[types_idxs[0]]->name().compare("NO-TYPE")) == 0) {
			continue;
		}

		current_predicate = prob.fluents()[tmp]->predicate();
		cout << current_predicate;
		cout << ":";

		if (arity > 2)
			continue;

		Index_Vec& objs_idx = prob.fluents()[tmp]->pddl_objs_idx();

		pair<int, int>* p;
		if (arity == 2 && objs_idx.size() == 2) {
			cout << "(" << instanceObjects[objs_idx[0]]->signature() << "," << instanceObjects[objs_idx[1]]->signature()
					<< ")|";

			vector<pair<int, int> >* interpPRVec = primitiveRoles[current_predicate]->GetRoleInterpretation();
			p = new pair<int, int>(objs_idx[0], objs_idx[1]);

			if (std::find((*interpPRVec).begin(), (*interpPRVec).end(), *p) == (*interpPRVec).end()) {
				interpPRVec->push_back(*p);
			} else {
				delete p;
			}
			continue;
		}

		for (unsigned j = 0; j < objs_idx.size(); j++) {
			cout << instanceObjects[objs_idx[j]]->signature();
			vector<int>* interpPCVec = primitiveConcepts[current_predicate]->GetInterpretation();

			if (std::find((*interpPCVec).begin(), (*interpPCVec).end(), objs_idx[j]) == (*interpPCVec).end()) {
				interpPCVec->push_back(objs_idx[j]);
			}
		}
		sort(primitiveConcepts[current_predicate]->GetInterpretation()->begin(),
				primitiveConcepts[current_predicate]->GetInterpretation()->end());
		cout << "|";
	}
}

int resolve_action_index(string signature) {
	if (signature.compare("STACK") == 0)
		return 1;
	return 0;
}

void reset_globals() {
	/* used to time the different stages of the planner
	 */
	gtempl_time = 0, greach_time = 0, grelev_time = 0, gconn_time = 0;
	gLNF_time = 0, gsearch_time = 0;

	/* number of states that got heuristically evaluated
	 */
	gevaluated_states = 0;

	/* maximal depth of breadth first search
	 */
	gmax_search_depth = 0;
	gbracket_count = 0;
	gproblem_name = NULL;

	/* The current input line number
	 */
	lineno = 1;

	/* The current input filename
	 */
	gact_filename = NULL;

	/* The pddl domain name
	 */
	gdomain_name = NULL;

	/* loaded, uninstantiated operators
	 */
	gloaded_ops = NULL;

	/* stores initials as fact_list
	 */
	gorig_initial_facts = NULL;

	/* not yet preprocessed goal facts
	 */
	gorig_goal_facts = NULL;

	/* axioms as in UCPOP before being changed to ops
	 */
	//gloaded_axioms = NULL;
	/* the types, as defined in the domain file
	 */
	gparse_types = NULL;

	/* the constants, as defined in domain file
	 */
	gparse_constants = NULL;

	/* the predicates and their arg types, as defined in the domain file
	 */
	gparse_predicates = NULL;

	/* the functions and their arg types, as defined in the domain file
	 */
	gparse_functions = NULL;

	/* the objects, declared in the problem file
	 */
	gparse_objects = NULL;

	/* the metric
	 */
	gparse_optimization = NULL;
	gparse_metric = NULL;

	/* connection to instantiation ( except ops, goal, initial )
	 */

	/* all typed objects
	 */
	gorig_constant_list = NULL;

	/* the predicates and their types
	 */
	gpredicates_and_types = NULL;

	/* the functions and their types
	 */
	gfunctions_and_types = NULL;
	gnum_constants = 0;
	//gtype_names[MAX_TYPES];
	//gtype_consts[MAX_TYPES][MAX_TYPE];
	//gis_member[MAX_CONSTANTS][MAX_TYPES];
	//gtype_size[MAX_TYPES];
	gnum_types = 0;
	//gpredicates[MAX_PREDICATES];
	//garity[MAX_PREDICATES];
	//gpredicates_args_type[MAX_PREDICATES][MAX_ARITY];
	gnum_predicates = 0;
	//gfunctions[MAX_FUNCTIONS];
	//gf_arity[MAX_FUNCTIONS];
	//gfunctions_args_type[MAX_FUNCTIONS][MAX_ARITY];
	gnum_functions = 0;

	/* the domain in integer (Fact) representation
	 */
	//goperators[MAX_OPERATORS];
	gnum_operators = 0;
	gfull_initial = NULL;
	gnum_full_initial = 0;
	gfull_fluents_initial = NULL;
	gnum_full_fluents_initial = 0;
	ggoal = NULL;
	gmetric = NULL;

	ginitial = NULL;
	gnum_initial = 0;
	ginitial_predicate = NULL;
	gnum_initial_predicate = NULL;

	/* same thing for functions
	 */
	gf_initial = NULL;
	gnum_f_initial = 0;
	ginitial_function = NULL;
	gnum_initial_function = NULL;

	/* splitted domain: hard n easy ops
	 */
	ghard_operators = NULL;
	gnum_hard_operators = 0;
	geasy_operators = NULL;
	gnum_easy_operators = 0;

	/* so called Templates for easy ops: possible inertia constrained
	 * instantiation constants
	 */
	geasy_templates = NULL;
	gnum_easy_templates = 0;

	/* first step for hard ops: create mixed operators, with conjunctive
	 * precondition and arbitrary effects
	 */
	ghard_mixed_operators = NULL;
	gnum_hard_mixed_operators = 0;

	/* hard ''templates'' : pseudo actions
	 */
	ghard_templates = NULL;
	gnum_hard_templates = 0;

	/* store the final "relevant facts"
	 */
	gnum_relevant_facts = 0;
	gnum_pp_facts = 0;
	/* store the "relevant fluents"
	 */
	gnum_relevant_fluents = 0;
	//delete[] grelevant_fluents_name;
	/* this is NULL for normal, and the LNF for
	 * artificial fluents.
	 */
	//delete[] grelevant_fluents_lnf;
	/* the final actions and problem representation
	 */
	gactions = NULL;
	gnum_actions = 0;
	//ginitial_state;
	glogic_goal = NULL;
	gnum_logic_goal = 0;
	gnumeric_goal_comp = NULL;
	gnumeric_goal_lh = NULL;
	gnumeric_goal_rh = NULL;
	gnum_numeric_goal = 0;

	/* direct numeric goal access
	 */
	//gnumeric_goal_direct_comp = NULL;
	//gnumeric_goal_direct_c = NULL;
	/* to avoid memory leaks; too complicated to identify
	 * the exact state of the action to throw away (during construction),
	 * memory gain not worth the implementation effort.
	 */
	gtrash_actions = NULL;

	/* additional lnf step between finalized inst and
	 * conn graph
	 */
	glnf_goal_comp = NULL;
	glnf_goal_lh = NULL;
	glnf_goal_rh = NULL;
	gnum_lnf_goal = 0;

	//LnfExpNode glnf_metric;
	goptimization_established = FALSE;

	/**********************
	 * CONNECTIVITY GRAPH *
	 **********************/
	/* one ops (actions) array ...
	 */
	gop_conn = NULL;
	gnum_op_conn = 0;

	/* one effects array ...
	 */
	gef_conn = NULL;
	gnum_ef_conn = 0;

	/* one facts array.
	 */
	gft_conn = NULL;
	gnum_ft_conn = 0;

	/* and: one fluents array.
	 */
	gfl_conn = NULL;
	gnum_fl_conn = 0;
	gnum_real_fl_conn = 0;/* number of non-artificial ones */

	/* final goal is also transformed one more step.
	 */
	gflogic_goal = NULL;
	gnum_flogic_goal = 0;
	gfnumeric_goal_comp = NULL;
	gfnumeric_goal_fl = NULL;
	gfnumeric_goal_c = NULL;
	gnum_fnumeric_goal = 0;

	/* direct access (by relevant fluents)
	 */
	gfnumeric_goal_direct_comp = NULL;
	gfnumeric_goal_direct_c = NULL;

	/*******************
	 * SEARCHING NEEDS *
	 *******************/
	/* applicable actions
	 */
	gA = NULL;
	gnum_A = 0;

	/* communication from extract 1.P. to search engine:
	 * 1P action choice
	 */
	gH = NULL;
	gnum_H = 0;
	/* cost of relaxed plan
	 */
	gcost = 0.;
	gnum_plan_ops = 0;

}

void set_instance_objects(STRIPS_Problem& prob) {
	instanceObjects.clear();
	instanceObjects = prob.objects();
	PDDL_Object_Ptr_Vec::iterator itr;
	for (itr = instanceObjects.begin(); itr != instanceObjects.end(); ++itr) {
		if ((*itr)->signature().compare("NO-OBJECT") == 0) {
			instanceObjects.erase(itr);
			break;
		}
	}
}

void solve(STRIPS_Problem& prob) {
	aig_tk::Node* n = aig_tk::Node::root(prob);
	int max = 150;
	set_instance_objects(prob);
	clear_static_interpretations();
	get_type_concepts_interpretation(prob);
	get_goal_interpretations(prob);
	print_interpretations(prob);
	update_primitive_interpretations(prob, n);
	update_compound_interpretations();
	while (!(n->s()->entails(prob.goal())) && max > 0) {
		vector<Rule>::iterator it = ruleSet.begin();
		bool applied = false;
		while (!applied && it != ruleSet.end()) {
			Rule r = *(it);
			Expression* concept = r.GetConcept();
			if (r.GetCurrentCoverage() > 0) {
				cout << "Covers: " << r << endl;
				vector<int>* interp = concept->GetInterpretation();
				for (unsigned i = 0; i < prob.num_actions(); i++) {
					aig_tk::Action* a = prob.actions()[i];
					int index = resolve_action_index(a->name());
					Index_Vec objs_idx = a->pddl_objs_idx();
					if (a->name().compare(r.GetAction()) == 0){ //&& (*interp)[0] == objs_idx[index]) {
						if (a->can_be_applied_on(*(n->s())) && n->successor(a) != n->parent()) {
							n = n->successor(a);
							cout << endl << "\t" << r << ":" << a->signature() << endl;
							update_primitive_interpretations(prob, n);
							update_compound_interpretations();
							applied = true;
							break;
						}
					}
				}
			}
			++it;
		}
		--max;
	}
	if (max != 0)
		cout << endl << "Goal reached" << endl;
	else
		cout << endl << "Goal NOT reached" << endl;
}

Expression* find_expression(string line) {
	Expression* ex = NULL;
	stringstream ss;
	string cline;
	for (unsigned i = 0; i < rootConcepts.size(); ++i) {
		rootConcepts[i]->prefix(ss);
		getline(ss, cline);

		if (cline.compare(line) == 0) {
			//cout << cline << "-----" << line << endl;
			ex = rootConcepts[i];
			return ex;
		}
		ss.clear();
		cline.clear();
	}
	for (unsigned i = 0; i < rootRoles.size(); ++i) {
		rootRoles[i]->prefix(ss);
		getline(ss, cline);
		if (cline.compare(line) == 0) {
			//cout << cline << "-----" << line << endl;
			ex = rootRoles[i];
			return ex;
		}
		ss.clear();
		cline.clear();
	}
	return ex;
}

string get_inner(string str) {
	int parcnt = 0, chcnt = 0;
	bool validf(false), valids(false);
	for (unsigned i = 1; i < str.length(); ++i) {
		if (str[i] == '(') {
			parcnt++;
			validf = true;
			continue;
		}
		if (str[i] == ')') {
			parcnt--;
			if (parcnt == 0) {
				chcnt = i - 2;
				valids = true;
				break;
			}
		}
	}
	if (!validf || !valids) {
		cout << "ERR: " << str << " not valid" << endl;
		return "";
	}
	string ret(str, 2, chcnt);
	return ret;
}

vector<string> splitline(string line) {
	vector<string> ret;
	string str = get_inner(line), field;
	stringstream iss(str);
	int parcnt = 0, chcnt = 0;
	for (unsigned i = 1; i < str.length(); ++i) {
		if (str[i] == '(') {
			parcnt++;
			continue;
		}
		if (str[i] == ')')
			parcnt--;

		if (str[i] == ',' && parcnt == 0) {
			chcnt = i;
			break;
		}
	}
	string part(str, 0, chcnt);
	ret.push_back(part);
	part = str.substr(chcnt + 1, str.length() - 1);
	ret.push_back(part);
	return ret;
}

Expression* contruct_concept(string line) {
	Expression* ex = NULL;
	ex = find_expression(line);
	if (ex != NULL)
		return ex;

	if (line[0] == '!') {
		ex = new Not(contruct_concept(get_inner(line)), &instanceObjects);
		rootConcepts.push_back(ex);
	} else if (line[0] == '*') {
		ex = new TransitiveClosure(contruct_concept(get_inner(line)));
		//ex->SetRole(true);
		rootRoles.push_back(ex);
	} else if (line[0] == 'I') {
		ex = new InverseRole(contruct_concept(get_inner(line)));
		//ex->SetRole(true);
		rootRoles.push_back(ex);
	} else if (line[0] == '^') {
		vector<string> strv = splitline(line);
		ex = new Join(contruct_concept(strv[0]), contruct_concept(strv[1]));
		rootConcepts.push_back(ex);
	} else if (line[0] == '.') {
		vector<string> strv = splitline(line);
		ex = new ValueRestriction(contruct_concept(strv[0]), contruct_concept(strv[1]));
		rootConcepts.push_back(ex);
	} else if (line[0] == '=') {
		vector<string> strv = splitline(line);
		ex = new Equality(contruct_concept(strv[0]), contruct_concept(strv[1]));
		rootConcepts.push_back(ex);
	}
	return ex;
}

void get_input() {
	string line;
	string field;
	ifstream fin("policy.txt");
	if (fin.is_open()) {
		int i = 0;
		Expression* ex;
		while (!fin.eof() && getline(fin, line)) {
			istringstream iss(line);
			if (i < 2) {
				while (getline(iss, field, ' ')) {
					if (i == 0) {
						ConceptNode* c = new ConceptNode(field);
						primitiveConcepts[field] = c;
						c = new ConceptNode(field);
						c->IsGoal(true);
						goalConcepts[field + 'g'] = c;
					} else if (i == 1) {
						RoleNode* r = new RoleNode(field);
						primitiveRoles[field] = r;
						r = new RoleNode(field);
						r->IsGoal(true);
						goalRoles[field + 'g'] = r;
					}
				}
			} else {
				if (i == 2) {
					initialize_root_concepts();
				}
				if (i % 2 == 0) {
					ex = contruct_concept(line);
//					if (ex->IsRole())
//						rootRoles.push_back(ex);
//					else
//						rootConcepts.push_back(ex);
				} else {
					Rule r(ex, line);
					ruleSet.push_back(r);
					//actions.push_back(line);
				}
			}
			i++;
		}
	}
}

//void bind_actions(STRIPS_Problem& prob){
//	aig_tk::Action_Ptr_Vec act = prob.actions();
//	for(unsigned i=0;i<ruleSet.size();++i){
//		for(unsigned j=0;j<act.size();++j){
//			if(act[j]->name().compare(ruleSet[i].GetAction())){
//				ruleSet[i].SetToolkitAction(act[j]);
//				break;
//			}
//		}
//	}
//}

int main(int argc, char** argv) {
	if (argc < 3) {
		std::cerr << "No prob description provided, bailing out!" << std::endl;
		std::exit(1);
	}

	std::string folder(argv[1]);
	int instance_num(atoi(argv[2]));
	std::string txt_output_filename("prob.txt.strips");
	string instance("instance");

	string instance_path;
	folder = "./tests/" + folder + "/";
	string domain = folder + "domain.pddl";
	vector<vector<aig_tk::Action*>*> plans;
	aig_tk::STRIPS_Problem* strips_prob;
	aig_tk::FF_PDDL_To_STRIPS adl_compiler;
	get_input();
	//printout();
	for (int i = 0; i < instance_num; i++) {

		instance_path = folder + instance + static_cast<ostringstream*>(&(ostringstream() << (i + 1)))->str() + ".pddl";
		cout << endl << "Using ruleset to solve instance #" << i + 1 << endl;
		reset_globals();
		strips_prob = new STRIPS_Problem();
		adl_compiler = FF_PDDL_To_STRIPS();
		adl_compiler.get_problem_description(domain, instance_path, *strips_prob, true);
//		if(i==0) bind_actions(*strips_prob);
		solve(*strips_prob);
	}

	return 0;
}
