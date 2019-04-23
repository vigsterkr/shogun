/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Roman Votyakov, Soeren Sonnenburg, Jacob Walker, Heiko Strathmann, 
 *          Sergey Lisitsyn
 */

#include <shogun/modelselection/ModelSelection.h>
#include <shogun/modelselection/ModelSelectionParameters.h>
#include <shogun/evaluation/CrossValidation.h>
#include <shogun/base/Parameter.h>

using namespace shogun;

CModelSelection::CModelSelection()
{
	init();
}

CModelSelection::CModelSelection(std::shared_ptr<CMachineEvaluation> machine_eval,
	std::shared_ptr<CModelSelectionParameters> model_parameters)
{
	init();

	m_model_parameters=model_parameters;
	

	m_machine_eval=machine_eval;
	
}

void CModelSelection::init()
{
	m_model_parameters=NULL;
	m_machine_eval=NULL;

	SG_ADD((std::shared_ptr<CSGObject>*)&m_model_parameters, "model_parameters",
			"Parameter tree for model selection");

	SG_ADD((std::shared_ptr<CSGObject>*)&m_machine_eval, "machine_evaluation",
			"Machine evaluation strategy");
}

CModelSelection::~CModelSelection()
{
	
	
}
