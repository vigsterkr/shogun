/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Shell Hu, Jiaolong Xu, Bjoern Esser
 */

#include <shogun/structure/MAPInference.h>
#include <shogun/structure/BeliefPropagation.h>
#include <shogun/structure/GraphCut.h>
#include <shogun/structure/GEMPLP.h>
#include <shogun/labels/FactorGraphLabels.h>

using namespace shogun;

CMAPInference::CMAPInference() : CSGObject()
{
	SG_UNSTABLE("CMAPInference::CMAPInference()", "\n");

	init();
}

CMAPInference::CMAPInference(std::shared_ptr<CFactorGraph> fg, EMAPInferType inference_method)
	: CSGObject()
{
	init();
	m_fg = fg;

	REQUIRE(fg != NULL, "%s::CMAPInference(): fg cannot be NULL!\n", get_name());

	switch(inference_method)
	{
		case TREE_MAX_PROD:
			m_infer_impl = std::make_shared<CTreeMaxProduct>(fg);
			break;
		case GRAPH_CUT:
			m_infer_impl = std::make_shared<CGraphCut>(fg);
			break;
		case GEMPLP:
			m_infer_impl = std::make_shared<CGEMPLP>(fg);
			break;
		case LOOPY_MAX_PROD:
			SG_ERROR("%s::CMAPInference(): LoopyMaxProduct has not been implemented!\n",
				get_name());
			break;
		case LP_RELAXATION:
			SG_ERROR("%s::CMAPInference(): LPRelaxation has not been implemented!\n",
				get_name());
			break;
		case TRWS_MAX_PROD:
			SG_ERROR("%s::CMAPInference(): TRW-S has not been implemented!\n",
				get_name());
			break;
		default:
			SG_ERROR("%s::CMAPInference(): unsupported inference method!\n",
				get_name());
			break;
	}



}

CMAPInference::~CMAPInference()
{



}

void CMAPInference::init()
{
	SG_ADD((std::shared_ptr<CSGObject>*)&m_fg, "fg", "factor graph");
	SG_ADD((std::shared_ptr<CSGObject>*)&m_outputs, "outputs", "Structured outputs");
	SG_ADD((std::shared_ptr<CSGObject>*)&m_infer_impl, "infer_impl", "Inference implementation");
	SG_ADD(&m_energy, "energy", "Minimized energy");

	m_outputs = NULL;
	m_infer_impl = NULL;
	m_fg = NULL;
	m_energy = 0;
}

void CMAPInference::inference()
{
	SGVector<int32_t> assignment(m_fg->get_num_vars());
	assignment.zero();
	m_energy = m_infer_impl->inference(assignment);

	// create structured output, with default normalized hamming loss

	SGVector<float64_t> loss_weights(m_fg->get_num_vars());
	SGVector<float64_t>::fill_vector(loss_weights.vector, loss_weights.vlen, 1.0 / loss_weights.vlen);
	m_outputs = std::make_shared<CFactorGraphObservation>(assignment, loss_weights); // already ref() in constructor

}

std::shared_ptr<CFactorGraphObservation> CMAPInference::get_structured_outputs() const
{

	return m_outputs;
}

float64_t CMAPInference::get_energy() const
{
	return m_energy;
}

//-----------------------------------------------------------------

CMAPInferImpl::CMAPInferImpl() : CSGObject()
{
	register_parameters();
}

CMAPInferImpl::CMAPInferImpl(std::shared_ptr<CFactorGraph> fg)
	: CSGObject()
{
	register_parameters();
	m_fg = fg;
}

CMAPInferImpl::~CMAPInferImpl()
{
}

void CMAPInferImpl::register_parameters()
{
	SG_ADD((std::shared_ptr<CSGObject>*)&m_fg, "fg",
		"Factor graph pointer");

	m_fg = NULL;
}

