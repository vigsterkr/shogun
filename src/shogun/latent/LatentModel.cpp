/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Viktor Gal, Thoralf Klein, Evgeniy Andreev, Soeren Sonnenburg
 */

#include <shogun/latent/LatentModel.h>
#include <shogun/labels/BinaryLabels.h>

using namespace shogun;

CLatentModel::CLatentModel()
	: m_features(NULL),
	m_labels(NULL),
	m_do_caching(false),
	m_cached_psi(NULL)
{
	register_parameters();
}

CLatentModel::CLatentModel(std::shared_ptr<CLatentFeatures> feats, std::shared_ptr<CLatentLabels> labels, bool do_caching)
	: m_features(feats),
	m_labels(labels),
	m_do_caching(do_caching),
	m_cached_psi(NULL)
{
	register_parameters();


}

CLatentModel::~CLatentModel()
{



}

int32_t CLatentModel::get_num_vectors() const
{
	return m_features->get_num_vectors();
}

void CLatentModel::set_labels(std::shared_ptr<CLatentLabels> labs)
{


	m_labels = labs;
}

std::shared_ptr<CLatentLabels> CLatentModel::get_labels() const
{

	return m_labels;
}

void CLatentModel::set_features(std::shared_ptr<CLatentFeatures> feats)
{


	m_features = feats;
}

void CLatentModel::argmax_h(const SGVector<float64_t>& w)
{
	int32_t num = get_num_vectors();
	auto y = binary_labels(m_labels->get_labels());
	ASSERT(num > 0)
	ASSERT(num == m_labels->get_num_labels())

	// argmax_h only for positive examples
	for (int32_t i = 0; i < num; ++i)
	{
		if (y->get_label(i) == 1)
		{
			// infer h and set it for the argmax_h <w,psi(x,h)>
			auto latent_data = infer_latent_variable(w, i);
			m_labels->set_latent_label(i, latent_data);
		}
	}
}

void CLatentModel::register_parameters()
{
	SG_ADD(&m_features, "features", "Latent features");
	SG_ADD(&m_labels, "labels", "Latent labels");
	SG_ADD(
	    &m_cached_psi, "cached_psi", "Cached PSI features after argmax_h");
	SG_ADD(
	    &m_do_caching, "do_caching",
	    "Indicate whether or not do PSI vector caching after argmax_h");
}


std::shared_ptr<CLatentFeatures> CLatentModel::get_features() const
{

	return m_features;
}

void CLatentModel::cache_psi_features()
{
	if (m_do_caching)
	{
		if (m_cached_psi)

		m_cached_psi = this->get_psi_feature_vectors();

	}
}

std::shared_ptr<CDotFeatures> CLatentModel::get_cached_psi_features() const
{
	if (m_do_caching)
	{

		return m_cached_psi;
	}
	return NULL;
}
