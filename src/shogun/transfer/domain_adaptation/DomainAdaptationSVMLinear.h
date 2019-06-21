/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Soeren Sonnenburg, Yuyu Zhang, Saurabh Goyal
 */

#ifndef _DomainAdaptation_SVM_LINEAR_H___
#define _DomainAdaptation_SVM_LINEAR_H___

#include <shogun/lib/config.h>

#include <shogun/lib/common.h>
#include <shogun/classifier/svm/LibLinear.h>


namespace shogun
{

#ifdef HAVE_LAPACK

/** @brief class DomainAdaptationSVMLinear */
class CDomainAdaptationSVMLinear : public CLibLinear
{

	public:

		/** default constructor */
		CDomainAdaptationSVMLinear();


		/** constructor
		 *
		 * @param C cost constant C
		 * @param f features
		 * @param lab labels
		 * @param presvm trained SVM to regularize against
		 * @param B trade-off constant B
		 */
		CDomainAdaptationSVMLinear(float64_t C, std::shared_ptr<CDotFeatures> f, std::shared_ptr<CLabels> lab, std::shared_ptr<CLinearMachine> presvm, float64_t B);


		/** destructor */
		virtual ~CDomainAdaptationSVMLinear();


		/** init SVM
		 *
		 * @param presvm trained SVM to regularize against
		 * @param B trade-off constant B
		 * */
		void init(std::shared_ptr<CLinearMachine> presvm, float64_t B);

		/** get classifier type
		 *
		 * @return classifier type DASVMLINEAR
		 */
		virtual EMachineType get_classifier_type() { return CT_DASVMLINEAR; }


		/** classify objects
		 *
		 * @param data (test)data to be classified
		 * @return classified labels
		 */
		virtual std::shared_ptr<CBinaryLabels> apply_binary(std::shared_ptr<CFeatures> data=NULL);


		/** returns SVM that is used as prior information
		 *
		 * @return presvm
		 */
		virtual std::shared_ptr<CLinearMachine> get_presvm();


		/** getter for regularization parameter B
		 *
		 * @return regularization parameter B
		 */
		virtual float64_t get_B();


		/** getter for train_factor
		 *
		 * @return train_factor
		 */
		virtual float64_t get_train_factor();


		/** setter for train_factor
		 *
		 */
		virtual void set_train_factor(float64_t factor);

		/**
		 * get linear term
		 *
		 * @return lin the linear term
		 */
		//virtual std::vector<float64_t> get_linear_term();


		/*
		 * set linear term of the QP
		 *
		 * @param lin the linear term
		 */
		//virtual void set_linear_term(std::vector<float64_t> lin);


		/** @return object name */
		virtual const char* get_name() const { return "DomainAdaptationSVMLinear"; }

	protected:

		/** check sanity of presvm
		 *
		 * @return true if sane, throws SG_ERROR otherwise
		 */
		virtual bool is_presvm_sane();

		/** train SVM classifier
		 *
		 * @param data training data (parameter can be avoided if distance or
		 * kernel-based classifiers are used and distance/kernels are
		 * initialized with train data)
		 *
		 * @return whether training was successful
		 */
		virtual bool train_machine(std::shared_ptr<CFeatures> data=NULL);

	protected:

		/** SVM to regularize against */
		std::shared_ptr<CLinearMachine> presvm;


		/** regularization parameter B */
		float64_t B;


		/** flag to switch off regularization in training */
		float64_t train_factor;


};
#endif //HAVE_LAPACK

} /* namespace shogun  */

#endif //_DomainAdaptation_SVM_LINEAR_H___
