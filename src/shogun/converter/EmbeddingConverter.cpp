/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Evan Shelhamer, Thoralf Klein, Soeren Sonnenburg,
 *          Chiyuan Zhang
 */

#include <shogun/converter/EmbeddingConverter.h>
#include <shogun/kernel/LinearKernel.h>
#include <shogun/distance/EuclideanDistance.h>

using namespace shogun;

namespace shogun
{
CEmbeddingConverter::CEmbeddingConverter()
: CConverter()
{
	m_target_dim = 1;
	m_distance = std::make_shared<CEuclideanDistance>();
	
	m_kernel = std::make_shared<CLinearKernel>();
	

	init();
}

CEmbeddingConverter::~CEmbeddingConverter()
{
	
	
}

void CEmbeddingConverter::set_target_dim(int32_t dim)
{
	ASSERT(dim>0)
	m_target_dim = dim;
}

int32_t CEmbeddingConverter::get_target_dim() const
{
	return m_target_dim;
}

void CEmbeddingConverter::set_distance(std::shared_ptr<CDistance> distance)
{
	
	
	m_distance = distance;
}

std::shared_ptr<CDistance> CEmbeddingConverter::get_distance() const
{
	
	return m_distance;
}

void CEmbeddingConverter::set_kernel(std::shared_ptr<CKernel> kernel)
{
	
	
	m_kernel = kernel;
}

std::shared_ptr<CKernel> CEmbeddingConverter::get_kernel() const
{
	
	return m_kernel;
}

void CEmbeddingConverter::init()
{
	SG_ADD(&m_target_dim, "target_dim",
      "target dimensionality of preprocessor", ParameterProperties::HYPER);
	SG_ADD(
		&m_distance, "distance", "distance to be used for embedding",
		ParameterProperties::HYPER);
	SG_ADD(
		&m_kernel, "kernel", "kernel to be used for embedding", ParameterProperties::HYPER);
}
}
