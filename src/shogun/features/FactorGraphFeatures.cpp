/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Shell Hu
 */

#include <shogun/features/FactorGraphFeatures.h>

using namespace shogun;

CFactorGraphFeatures::CFactorGraphFeatures(): CFactorGraphFeatures(0)
{
}

CFactorGraphFeatures::CFactorGraphFeatures(int32_t num_samples)
{
	init();
	m_samples = std::make_shared<CDynamicObjectArray>(num_samples);

}

CFactorGraphFeatures::~CFactorGraphFeatures()
{

}

std::shared_ptr<CFeatures> CFactorGraphFeatures::duplicate() const
{
	return std::make_shared<CFactorGraphFeatures>(*this);
}

EFeatureType CFactorGraphFeatures::get_feature_type() const
{
	return F_ANY;
}

EFeatureClass CFactorGraphFeatures::get_feature_class() const
{
	return C_FACTOR_GRAPH;
}


int32_t CFactorGraphFeatures::get_num_vectors() const
{
	if (m_samples == NULL)
		return 0;
	else
		return m_samples->get_array_size();
}

bool CFactorGraphFeatures::add_sample(std::shared_ptr<CFactorGraph> example)
{
	if (m_samples != NULL)
	{
		m_samples->push_back(example);
		return true;
	}
	else
		return false;
}

std::shared_ptr<CFactorGraph> CFactorGraphFeatures::get_sample(index_t idx)
{
	REQUIRE(m_samples != NULL, "%s::get_sample(): m_samples is NULL!\n", get_name());
	REQUIRE(idx >= 0 && idx < get_num_vectors(), "%s::get_sample(): out of index!\n", get_name());

	return m_samples->get_element<CFactorGraph>(idx);
}

void CFactorGraphFeatures::init()
{
	SG_ADD((std::shared_ptr<CSGObject>*) &m_samples, "samples", "Array of examples");
}

std::shared_ptr<CFactorGraphFeatures> CFactorGraphFeatures::obtain_from_generic(std::shared_ptr<CFeatures> base_feats)
{
	REQUIRE(base_feats != NULL, "CFactorGraphFeatures::obtain_from_generic(): base_feats is NULL!\n");

	if (base_feats->get_feature_class() == C_FACTOR_GRAPH)
		return std::dynamic_pointer_cast<CFactorGraphFeatures>(base_feats);
	else
		SG_SERROR("base_labels must be of dynamic type CFactorGraph!\n")

	return NULL;
}

