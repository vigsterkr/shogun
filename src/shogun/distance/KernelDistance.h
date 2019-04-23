/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Soeren Sonnenburg, Heiko Strathmann, Yuyu Zhang, Sergey Lisitsyn
 */

#include <shogun/lib/config.h>


#ifndef _KERNELDISTANCE_H___
#define _KERNELDISTANCE_H___

#include <shogun/lib/config.h>

#include <shogun/lib/common.h>
#include <shogun/distance/Distance.h>
#include <shogun/kernel/Kernel.h>

namespace shogun
{
	class CDistance;

/** @brief The Kernel distance takes a distance as input.
 *
 * It turns a kernel into something distance like by computing
 *
 * \f[
 *     d({\bf x}, {\bf x'}) = e^{-\frac{k({\bf x}, {\bf x'})}{width}}
 * \f]
 */
class CKernelDistance: public CDistance
{
	public:
		/** default constructor  */
		CKernelDistance();

		/** constructor
		 *
		 * @param width width
		 * @param k kernel
		 */
		CKernelDistance(float64_t width, std::shared_ptr<CKernel> k);

		/** constructor
		 *
		 * @param l features of left-hand side
		 * @param r features of right-hand side
		 * @param width width
		 * @param k kernel
		 */
		CKernelDistance(
			std::shared_ptr<CFeatures >l, std::shared_ptr<CFeatures >r, float64_t width, std::shared_ptr<CKernel> k);

		/** destructor */
		virtual ~CKernelDistance();

		/** initialize kernel
		 *
		 * @param l features of left-hand side
		 * @param r features of right-hand side
		 * @return if initializing was successful
		 */
		virtual bool init(std::shared_ptr<CFeatures> l, std::shared_ptr<CFeatures> r);

		/** return what type of kernel we are
		 *
		 * @return distance type DISTANCE
		 */
		virtual EDistanceType get_distance_type() { return D_UNKNOWN; }
		/** return feature type the distance can deal with
		 *
		 * @return feature type of distance used
		 */
		virtual EFeatureType get_feature_type() { return kernel->get_feature_type(); }

		/** return feature class the distance can deal with
		 *
		 * @return feature class of distance used
		 */
		virtual EFeatureClass get_feature_class() { return kernel->get_feature_class(); }

		/** return the distances's name
		 *
		 * @return name Distance
		 */
		virtual const char* get_name() const { return "KernelDistance"; }

		/** clean up kernel
		 *
		 */
		virtual void cleanup() { if (kernel) kernel->cleanup(); }

	protected:
		/** compute kernel function for features a and b
		 * idx_{a,b} denote the index of the feature vectors
		 * in the corresponding feature object
		 *
		 * @param idx_a index a
		 * @param idx_b index b
		 * @return computed kernel function at indices a,b
		 */
		float64_t compute(int32_t idx_a, int32_t idx_b);

	private:
		void init();

	private:
		/** kernel */
		std::shared_ptr<CKernel> kernel;
		/** width */
		float64_t width;
};
}
#endif /* _KERNELDISTANCE_H__ */
