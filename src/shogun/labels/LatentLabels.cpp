/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Viktor Gal, Soeren Sonnenburg, Evgeniy Andreev, Thoralf Klein, 
 *          Fernando Iglesias, Bjoern Esser
 */

#include <shogun/labels/LatentLabels.h>

using namespace shogun;

CLatentLabels::CLatentLabels()
	: CLabels()
{
	init();
}

CLatentLabels::CLatentLabels(int32_t num_samples)
	: CLabels()
{
	init();
	m_latent_labels = std::make_shared<CDynamicObjectArray>(num_samples);
	
}

CLatentLabels::CLatentLabels(std::shared_ptr<CLabels> labels)
	: CLabels()
{
	init();
	set_labels(labels);

	int32_t num_labels = 0;
	if (m_labels)
		num_labels = m_labels->get_num_labels();

	m_latent_labels = std::make_shared<CDynamicObjectArray>(num_labels);
	
}

CLatentLabels::~CLatentLabels()
{
	
	
}

void CLatentLabels::init()
{
	SG_ADD((std::shared_ptr<CSGObject>*) &m_latent_labels, "m_latent_labels", "The latent labels");
	SG_ADD((std::shared_ptr<CSGObject>*) &m_labels, "m_labels", "The labels");
	m_latent_labels = NULL;
	m_labels = NULL;
}

std::shared_ptr<CDynamicObjectArray> CLatentLabels::get_latent_labels() const
{
	
	return m_latent_labels;
}

std::shared_ptr<CData> CLatentLabels::get_latent_label(int32_t idx)
{
	ASSERT(m_latent_labels != NULL)
	if (idx < 0 || idx >= get_num_labels())
		SG_ERROR("Out of index!\n")

	return std::static_pointer_cast<CData>( m_latent_labels->get_element(idx));
}

void CLatentLabels::add_latent_label(std::shared_ptr<CData> label)
{
	ASSERT(m_latent_labels != NULL)
	m_latent_labels->push_back(label);
}

bool CLatentLabels::set_latent_label(int32_t idx, std::shared_ptr<CData> label)
{
	if (idx < get_num_labels())
	{
		return m_latent_labels->set_element(label, idx);
	}
	else
	{
		return false;
	}
}

bool CLatentLabels::is_valid() const
{
	return m_latent_labels != nullptr;
}

void CLatentLabels::ensure_valid(const char* context)
{
	REQUIRE(is_valid(), "Empty labels provided!\n");
}

int32_t CLatentLabels::get_num_labels() const
{
	if (!m_latent_labels || !m_labels)
		return 0;
	int32_t num_labels = m_latent_labels->get_num_elements();

	ASSERT(num_labels == m_labels->get_num_labels())

	return num_labels;
}

void CLatentLabels::set_labels(std::shared_ptr<CLabels> labels)
{
	
	
	m_labels = labels;
}

std::shared_ptr<CLabels> CLatentLabels::get_labels() const
{
	
	return m_labels;
}

