/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Soeren Sonnenburg, Yuyu Zhang, Sergey Lisitsyn
 */

#ifndef _AUCKERNEL_H___
#define _AUCKERNEL_H___

#include <shogun/lib/config.h>

#include <shogun/features/DenseFeatures.h>
#include <shogun/kernel/DotKernel.h>
#include <shogun/labels/Labels.h>
#include <shogun/lib/common.h>

namespace shogun
{
	class CLabels;
	template <class T>
	class CDenseFeatures;

	/** @brief The AUC kernel can be used to maximize the area under the
	 * receiver operator characteristic curve (AUC) instead of margin in SVM
	 * training.
	 *
	 * It takes as argument a sub-kernel and Labels based on which number of
	 * positive labels times number of negative labels many ``virtual'' examples
	 * are created that ensure that all positive examples get a higher score
	 * than all negative examples in training.
	 */
	class CAUCKernel : public CDotKernel
	{
		void init();

	public:
		/** default constructor  */
		CAUCKernel();

		/** constructor
		 *
		 * @param size cache size
		 * @param subkernel the subkernel
		 * @param labels the labels for AUC maximization
		 */
		CAUCKernel(int32_t size, std::shared_ptr<CKernel> subkernel, std::shared_ptr<CLabels> labels);

		/** destructor */
		virtual ~CAUCKernel();

		/** initialize kernel based on current labeling and subkernel
		 *
		 * @param labels - current labeling
		 * @return whether new labels with AUC maximisation have been learnt
		 */
		bool setup_auc_maximization();

		/** initialize kernel
		 *
		 * @param l features of left-hand side
		 * @param r features of right-hand side
		 * @return if initializing was successful
		 */
		virtual bool init(std::shared_ptr<CFeatures> l, std::shared_ptr<CFeatures> r);

		/** return what type of kernel we are
		 *
		 * @return kernel type AUC
		 */
		virtual EKernelType get_kernel_type()
		{
			return K_AUC;
		}

		/** return the kernel's name
		 *
		 * @return name AUC
		 */
		virtual const char* get_name() const
		{
			return "AUCKernel";
		}

		/** return feature class the kernel can deal with
		 *
		 * @return feature class SIMPLE
		 */
		virtual EFeatureClass get_feature_class()
		{
			return C_DENSE;
		}

		/** return feature type the kernel can deal with
		 *
		 * @return word feature type
		 */
		virtual EFeatureType get_feature_type()
		{
			return F_WORD;
		}

	protected:
		/** compute kernel function for features a and b
		 * idx_{a,b} denote the index of the feature vectors
		 * in the corresponding feature object
		 *
		 * @param idx_a index a
		 * @param idx_b index b
		 * @return computed kernel function at indices a,b
		 */
		virtual float64_t compute(int32_t idx_a, int32_t idx_b);

	protected:
		/** the subkernel */
		std::shared_ptr<CKernel> subkernel;
		/** the labels */
		std::shared_ptr<CLabels> labels;
	};
} // namespace shogun
#endif /* _AUCKERNEL_H__ */
