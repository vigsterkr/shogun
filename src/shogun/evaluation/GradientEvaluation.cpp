/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Jacob Walker, Roman Votyakov, Heiko Strathmann, Giovanni De Toni,
 *          Sergey Lisitsyn
 */

#include <shogun/evaluation/GradientEvaluation.h>
#include <shogun/evaluation/GradientResult.h>

using namespace shogun;

CGradientEvaluation::CGradientEvaluation() : CMachineEvaluation()
{
	init();
}

CGradientEvaluation::CGradientEvaluation(std::shared_ptr<CMachine> machine, std::shared_ptr<CFeatures> features,
		std::shared_ptr<CLabels> labels, std::shared_ptr<CEvaluation> evaluation_crit, bool autolock) :
		CMachineEvaluation(machine, features, labels, NULL, evaluation_crit, autolock)
{
	init();
}

void CGradientEvaluation::init()
{
	m_diff=NULL;
	m_parameter_dictionary=NULL;

	SG_ADD(
	    &m_diff, "differentiable_function", "Differentiable "
	                                        "function",
	    ParameterProperties::HYPER);
}

CGradientEvaluation::~CGradientEvaluation()
{


}

void CGradientEvaluation::update_parameter_dictionary()
{


	m_parameter_dictionary=std::make_shared<CMap<TParameter*, CSGObject*>>();
	m_diff->build_gradient_parameter_dictionary(m_parameter_dictionary);

}

std::shared_ptr<CEvaluationResult> CGradientEvaluation::evaluate_impl()
{
	if (parameter_hash_changed())
		update_parameter_dictionary();

	// create gradient result object
	auto result=std::make_shared<CGradientResult>();


	// set function value
	result->set_value(m_diff->get_value());

	auto gradient=m_diff->get_gradient(m_parameter_dictionary);

	// set gradient and parameter dictionary
	result->set_gradient(gradient);
	result->set_paramter_dictionary(m_parameter_dictionary);



	update_parameter_hash();

	return result;
}
