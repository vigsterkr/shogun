/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Jacob Walker, Heiko Strathmann, Sergey Lisitsyn, Yuyu Zhang,
 *          Soeren Sonnenburg, Giovanni De Toni
 */

#ifndef CMACHINEEVALUATION_H_
#define CMACHINEEVALUATION_H_

#include <shogun/evaluation/Evaluation.h>
#include <shogun/evaluation/EvaluationResult.h>
#include <shogun/evaluation/MachineEvaluation.h>
#include <shogun/lib/StoppableSGObject.h>
#include <shogun/lib/config.h>

namespace shogun
{
	class CMachine;
	class CFeatures;
	class CLabels;
	class CSplittingStrategy;
	class CEvaluation;

	/** @brief Machine Evaluation is an abstract class
	 * that evaluates a machine according to some criterion.
	 *
	 */
	class CMachineEvaluation : public CStoppableSGObject
	{

	public:
		CMachineEvaluation();

		/** constructor
		 * @param machine learning machine to use
		 * @param features features to use for cross-validation
		 * @param labels labels that correspond to the features
		 * @param splitting_strategy splitting strategy to use
		 * @param evaluation_criterion evaluation criterion to use
		 * @param autolock whether machine should be auto-locked before
		 * evaluation
		 */
		CMachineEvaluation(
		    std::shared_ptr<CMachine> machine, std::shared_ptr<CFeatures> features, std::shared_ptr<CLabels> labels,
		    std::shared_ptr<CSplittingStrategy> splitting_strategy,
		    std::shared_ptr<CEvaluation> evaluation_criterion, bool autolock = true);

		/** constructor, for use with custom kernels (no features)
		 * @param machine learning machine to use
		 * @param labels labels that correspond to the features
		 * @param splitting_strategy splitting strategy to use
		 * @param evaluation_criterion evaluation criterion to use
		 * @param autolock autolock
		 */
		CMachineEvaluation(
		    std::shared_ptr<CMachine> machine, std::shared_ptr<CLabels> labels,
		    std::shared_ptr<CSplittingStrategy> splitting_strategy,
		    std::shared_ptr<CEvaluation> evaluation_criterion, bool autolock = true);

		virtual ~CMachineEvaluation();

		/** @return in which direction is the best evaluation value? */
		EEvaluationDirection get_evaluation_direction();

		/** method for evaluation. Performs cross-validation.
		 * Is repeated m_num_runs. If this number is larger than one, a
		 * confidence
		 * interval is calculated if m_conf_int_alpha is (0<p<1).
		 * By default m_num_runs=1 and m_conf_int_alpha=0
		 *
		 * @return result of evaluation (already SG_REF'ed)
		 */
		virtual std::shared_ptr<CEvaluationResult> evaluate();

		/** @return underlying learning machine */
		std::shared_ptr<CMachine> get_machine() const;

		/** setter for the autolock property. If true, machine will tried to be
		 * locked before evaluation */
		void set_autolock(bool autolock)
		{
			m_autolock = autolock;
		}

	protected:
		/** Initialize Object */
		virtual void init();

		/**
		 * Implementation of the evaluation procedure. Called
		 * by evaluate() method. This method has to SG_REF its result
		 * before returning it.
		 * @return the evaluation result
		 */
		virtual std::shared_ptr<CEvaluationResult> evaluate_impl() = 0;

		/** connect the machine instance to the signal handler */
	protected:
		/** Machine to be Evaluated */
		std::shared_ptr<CMachine> m_machine;

		/** Features to be used*/
		std::shared_ptr<CFeatures> m_features;

		/** Labels for the features */
		std::shared_ptr<CLabels> m_labels;

		/** Splitting Strategy to be used */
		std::shared_ptr<CSplittingStrategy> m_splitting_strategy;

		/** Criterion for evaluation */
		std::shared_ptr<CEvaluation> m_evaluation_criterion;

		/** whether machine will automatically be locked before evaluation */
		bool m_autolock;

		/** whether machine should be unlocked after evaluation */
		bool m_do_unlock;
	};

} /* namespace shogun */

#endif /* CMACHINEEVALUATION_H_ */
