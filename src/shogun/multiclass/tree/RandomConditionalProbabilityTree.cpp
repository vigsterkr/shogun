/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Chiyuan Zhang
 */

#include <shogun/mathematics/Math.h>
#include <shogun/multiclass/tree/RandomConditionalProbabilityTree.h>

using namespace shogun;

bool RandomConditionalProbabilityTree::which_subtree(std::shared_ptr<bnode_t> node, SGVector<float32_t> ex)
{
	if (Math::random(0.0, 1.0) > 0.5)
		return true;
	return false;
}
