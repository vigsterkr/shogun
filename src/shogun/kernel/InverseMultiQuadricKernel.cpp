/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Soeren Sonnenburg, Evan Shelhamer
 */

#include <shogun/kernel/InverseMultiQuadricKernel.h>
#include <shogun/mathematics/Math.h>

using namespace shogun;

CInverseMultiQuadricKernel::CInverseMultiQuadricKernel(): CKernel(0), distance(NULL), coef(0.0001)
{
	init();
}

CInverseMultiQuadricKernel::CInverseMultiQuadricKernel(int32_t cache, float64_t coefficient, std::shared_ptr<CDistance> dist)
: CKernel(cache), distance(dist), coef(coefficient)
{
	
	init();
}

CInverseMultiQuadricKernel::CInverseMultiQuadricKernel(std::shared_ptr<CFeatures >l, std::shared_ptr<CFeatures >r, float64_t coefficient, std::shared_ptr<CDistance> dist)
: CKernel(10), distance(dist), coef(coefficient)
{
	
	init();
	init(l, r);
}

CInverseMultiQuadricKernel::~CInverseMultiQuadricKernel()
{
	cleanup();
	
}

bool CInverseMultiQuadricKernel::init(std::shared_ptr<CFeatures> l, std::shared_ptr<CFeatures> r)
{
	CKernel::init(l,r);
	distance->init(l,r);
	return init_normalizer();
}

void CInverseMultiQuadricKernel::load_serializable_post() noexcept(false)
{
	CKernel::load_serializable_post();
}

void CInverseMultiQuadricKernel::init()
{
	SG_ADD(&coef, "coef", "Kernel Coefficient.", ParameterProperties::HYPER);
	SG_ADD(&distance, "distance", "Distance to be used.",
	    ParameterProperties::HYPER);
}

float64_t CInverseMultiQuadricKernel::compute(int32_t idx_a, int32_t idx_b)
{
	float64_t dist = distance->distance(idx_a, idx_b);
	return 1/sqrt(dist*dist + coef*coef);
}
