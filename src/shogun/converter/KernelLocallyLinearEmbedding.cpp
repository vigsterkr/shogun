/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Soeren Sonnenburg, Heiko Strathmann,
 *          Fernando Iglesias, Viktor Gal, Evan Shelhamer
 */

#include <shogun/converter/KernelLocallyLinearEmbedding.h>
#include <shogun/io/SGIO.h>
#include <shogun/lib/tapkee/tapkee_shogun.hpp>

using namespace shogun;

CKernelLocallyLinearEmbedding::CKernelLocallyLinearEmbedding() :
		CLocallyLinearEmbedding()
{
}

CKernelLocallyLinearEmbedding::CKernelLocallyLinearEmbedding(std::shared_ptr<CKernel> kernel) :
		CLocallyLinearEmbedding()
{
	set_kernel(kernel);
}

const char* CKernelLocallyLinearEmbedding::get_name() const
{
	return "KernelLocallyLinearEmbedding";
};

CKernelLocallyLinearEmbedding::~CKernelLocallyLinearEmbedding()
{
}

std::shared_ptr<CFeatures>
CKernelLocallyLinearEmbedding::transform(std::shared_ptr<CFeatures> features, bool inplace)
{
	ASSERT(features)


	// get dimensionality and number of vectors of data
	int32_t N = features->get_num_vectors();
	if (m_k>=N)
		SG_ERROR("Number of neighbors (%d) should be less than number of objects (%d).\n",
		         m_k, N);

	// compute kernel matrix
	ASSERT(m_kernel)
	m_kernel->init(features,features);
	auto embedding = embed_kernel(m_kernel);
	m_kernel->cleanup();

	return embedding;
}

std::shared_ptr<CDenseFeatures<float64_t>> CKernelLocallyLinearEmbedding::embed_kernel(std::shared_ptr<CKernel> kernel)
{
	TAPKEE_PARAMETERS_FOR_SHOGUN parameters;
	parameters.n_neighbors = m_k;
	parameters.eigenshift = m_nullspace_shift;
	parameters.method = SHOGUN_KERNEL_LOCALLY_LINEAR_EMBEDDING;
	parameters.target_dimension = m_target_dim;
	parameters.kernel = kernel.get();
	return tapkee_embed(parameters);
}

