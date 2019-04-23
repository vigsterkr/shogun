/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Heiko Strathmann, Soeren Sonnenburg,
 *          Evan Shelhamer
 */

#include <shogun/converter/LocalTangentSpaceAlignment.h>
#include <shogun/io/SGIO.h>
#include <shogun/kernel/LinearKernel.h>
#include <shogun/lib/tapkee/tapkee_shogun.hpp>

using namespace shogun;

CLocalTangentSpaceAlignment::CLocalTangentSpaceAlignment() :
		CLocallyLinearEmbedding()
{
}

CLocalTangentSpaceAlignment::~CLocalTangentSpaceAlignment()
{
}

const char* CLocalTangentSpaceAlignment::get_name() const
{
	return "LocalTangentSpaceAlignment";
};

std::shared_ptr<CFeatures>
CLocalTangentSpaceAlignment::transform(std::shared_ptr<CFeatures> features, bool inplace)
{
	auto dot_feats = std::static_pointer_cast<CDotFeatures>(features);
	auto kernel = std::make_shared<CLinearKernel>(dot_feats, dot_feats);
	TAPKEE_PARAMETERS_FOR_SHOGUN parameters;
	parameters.n_neighbors = m_k;
	parameters.eigenshift = m_nullspace_shift;
	parameters.method = SHOGUN_LOCAL_TANGENT_SPACE_ALIGNMENT;
	parameters.target_dimension = m_target_dim;
	parameters.kernel = kernel.get();
	return tapkee_embed(parameters);
}

