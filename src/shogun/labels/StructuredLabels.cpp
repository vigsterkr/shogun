/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Fernando Iglesias, Thoralf Klein, Evgeniy Andreev, 
 *          Soeren Sonnenburg, Bjoern Esser
 */

#include <shogun/labels/StructuredLabels.h>

using namespace shogun;

CStructuredLabels::CStructuredLabels()
: CLabels()
{
	init();
}

CStructuredLabels::CStructuredLabels(int32_t num_labels)
: CLabels()
{
	init();
	m_labels = std::make_shared<CDynamicObjectArray>(num_labels);
	
}

CStructuredLabels::~CStructuredLabels()
{
	
}

bool CStructuredLabels::is_valid() const
{
	return m_labels != nullptr;
}

void CStructuredLabels::ensure_valid(const char* context)
{
	REQUIRE(is_valid(), "Non-valid StructuredLabels in %s", context);
}

std::shared_ptr<CDynamicObjectArray> CStructuredLabels::get_labels() const
{
	
	return m_labels;
}

std::shared_ptr<CStructuredData> CStructuredLabels::get_label(int32_t idx)
{
	ensure_valid("CStructuredLabels::get_label(int32_t)");
	if ( idx < 0 || idx >= get_num_labels() )
		SG_ERROR("Index must be inside [0, num_labels-1]\n")

	return std::static_pointer_cast<CStructuredData>( m_labels->get_element(idx));
}

void CStructuredLabels::add_label(std::shared_ptr<CStructuredData> label)
{
	ensure_valid_sdt(label);
	m_labels->push_back(label);
}

bool CStructuredLabels::set_label(int32_t idx, std::shared_ptr<CStructuredData> label)
{
	ensure_valid_sdt(label);
	int32_t real_idx = m_subset_stack->subset_idx_conversion(idx);

	if ( real_idx < get_num_labels() )
	{
		return m_labels->set_element(label, real_idx);
	}
	else
	{
		return false;
	}
}

int32_t CStructuredLabels::get_num_labels() const
{
	if ( m_labels == NULL )
		return 0;
	else
		return m_labels->get_num_elements();
}

void CStructuredLabels::init()
{
	SG_ADD((std::shared_ptr<CSGObject>*) &m_labels, "m_labels", "The labels");

	m_labels = NULL;
	m_sdt = SDT_UNKNOWN;
}

void CStructuredLabels::ensure_valid_sdt(std::shared_ptr<CStructuredData> label)
{
	if ( m_sdt == SDT_UNKNOWN )
	{
		m_sdt = label->get_structured_data_type();
	}
	else
	{
		REQUIRE(label->get_structured_data_type() == m_sdt, "All the labels must "
				"belong to the same CStructuredData child class\n");
	}
}
