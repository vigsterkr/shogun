/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Soeren Sonnenburg, Thoralf Klein, Bjoern Esser, Sergey Lisitsyn
 */

#include <shogun/lib/IndexBlock.h>
#include <shogun/lib/List.h>

using namespace shogun;

CIndexBlock::CIndexBlock() : CSGObject(),
	m_min_index(0), m_max_index(0),
	m_weight(1.0), m_sub_blocks(NULL)
{
	m_sub_blocks = std::make_shared<CList>(true);

}

CIndexBlock::CIndexBlock(index_t min_index, index_t max_index,
             float64_t weight, const char* name) :
	CSGObject(), m_min_index(min_index), m_max_index(max_index),
	m_weight(weight), m_sub_blocks(NULL)
{
	m_sub_blocks = std::make_shared<CList>(true);

}

CIndexBlock::~CIndexBlock()
{

}

void CIndexBlock::add_sub_block(std::shared_ptr<CIndexBlock> sub_block)
{
	ASSERT(sub_block->get_min_index()>=m_min_index)
	ASSERT(sub_block->get_max_index()<=m_max_index)
	m_sub_blocks->append_element(sub_block);
}

std::shared_ptr<CList> CIndexBlock::get_sub_blocks()
{

	return m_sub_blocks;
}

int32_t CIndexBlock::get_num_sub_blocks()
{
	return m_sub_blocks->get_num_elements();
}
