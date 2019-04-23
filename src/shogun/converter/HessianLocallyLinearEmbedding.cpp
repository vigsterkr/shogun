/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Heiko Strathmann, Evan Shelhamer
 */

#include <shogun/converter/HessianLocallyLinearEmbedding.h>
#include <shogun/kernel/LinearKernel.h>
#include <shogun/io/SGIO.h>
#include <shogun/lib/tapkee/tapkee_shogun.hpp>

using namespace shogun;

CHessianLocallyLinearEmbedding::CHessianLocallyLinearEmbedding() :
		CLocallyLinearEmbedding()
{
}

CHessianLocallyLinearEmbedding::~CHessianLocallyLinearEmbedding()
{
}

const char* CHessianLocallyLinearEmbedding::get_name() const
{
	return "HessianLocallyLinearEmbedding";
};

std::shared_ptr<CFeatures>
CHessianLocallyLinearEmbedding::transform(std::shared_ptr<CFeatures> features, bool inplace)
{
	auto dot_feats = std::static_pointer_cast<CDotFeatures>(features);
	std::shared_ptr<CKernel> kernel = std::make_shared<CLinearKernel>(dot_feats, dot_feats);
	TAPKEE_PARAMETERS_FOR_SHOGUN parameters;
	parameters.n_neighbors = m_k;
	parameters.eigenshift = m_nullspace_shift;
	parameters.method = SHOGUN_HESSIAN_LOCALLY_LINEAR_EMBEDDING;
	parameters.target_dimension = m_target_dim;
	parameters.kernel = kernel.get();
	return tapkee_embed(parameters);
}

