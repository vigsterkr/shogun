/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Soeren Sonnenburg, Yuyu Zhang, Evan Shelhamer, 
 *          Roman Votyakov, Leon Kuchenbecker
 */

#ifndef ROCEVALUATION_H_
#define ROCEVALUATION_H_

#include <shogun/lib/config.h>

#include <shogun/evaluation/BinaryClassEvaluation.h>

namespace shogun
{

class CLabels;

/** @brief Class ROCEvalution used to evaluate ROC
 * (Receiver Operating Characteristic) and an area
 * under ROC curve (auROC).
 *
 * Implementation is based on the efficient ROC algorithm as described in
 *
 * Fawcett, Tom (2004) ROC Graphs:
 * Notes and Practical Considerations for Researchers; Machine Learning, 2004
 */
class CROCEvaluation: public CBinaryClassEvaluation
{
public:
	/** constructor */
	CROCEvaluation() :
		CBinaryClassEvaluation(), m_computed(false)
	{
		m_ROC_graph = SGMatrix<float64_t>();
		m_thresholds = SGVector<float64_t>();
	};

	/** destructor */
	virtual ~CROCEvaluation();

	/** get name */
	virtual const char* get_name() const { return "ROCEvaluation"; };

	/** evaluate ROC and auROC
	 * @param predicted labels
	 * @param ground_truth labels assumed to be correct
	 * @return auROC
	 */
	virtual float64_t evaluate(std::shared_ptr<CLabels> predicted, std::shared_ptr<CLabels> ground_truth);

	virtual EEvaluationDirection get_evaluation_direction() const
	{
		return ED_MAXIMIZE;
	}

	/** get auROC
	 * @return area under ROC (auROC)
	 */
	float64_t get_auROC();

	/** get ROC
	 * @return ROC graph matrix
	 */
	SGMatrix<float64_t> get_ROC();

	/** get thresholds corresponding to points on the ROC graph
	 * @return thresholds
	 */
	SGVector<float64_t> get_thresholds();

protected:

	/** evaluate ROC and auROC
	 * @param predicted labels
	 * @param ground_truth labels assumed to be correct
	 * @return auROC
	 */
	float64_t evaluate_roc(std::shared_ptr<CBinaryLabels> predicted, std::shared_ptr<CBinaryLabels> ground_truth);

protected:

	/** 2-d array used to store ROC graph */
	SGMatrix<float64_t> m_ROC_graph;

	/** vector with thresholds corresponding to points on the ROC graph */
	SGVector<float64_t> m_thresholds;

	/** area under ROC graph */
	float64_t m_auROC;

	/** indicator of ROC and auROC being computed already */
	bool m_computed;
};

}

#endif /* ROCEVALUATION_H_ */
