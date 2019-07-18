/** This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Viktor Gal
 */
#ifndef __RENKU_H__
#define __RENKU_H__

#include <shogun/evaluation/Evaluation.h>
#include <shogun/machine/Machine.h>
#include <shogun/machine/Pipeline.h>
#include <shogun/transformer/Transformer.h>

namespace shogun
{
	namespace renku
	{
		void to_mls(const std::shared_ptr<shogun::Machine>& m, const std::shared_ptr<shogun::Features>& input_features);
	}
}

#endif

