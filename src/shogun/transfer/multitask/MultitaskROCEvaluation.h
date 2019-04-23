/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Roman Votyakov, Soeren Sonnenburg, Yuyu Zhang, Sergey Lisitsyn
 */

#ifndef MULTITASKROCEVALUATION_H_
#define MULTITASKROCEVALUATION_H_

#include <shogun/lib/config.h>

#include <shogun/transfer/multitask/TaskRelation.h>
#include <shogun/evaluation/ROCEvaluation.h>

namespace shogun
{

/** @brief Class MultitaskROCEvalution used to evaluate ROC
 * (Receiver Operating Characteristic) and an area
 * under ROC curve (auROC) of each task separately.
 *
 */
class CMultitaskROCEvaluation: public CROCEvaluation
{
public:
	/** constructor */
	CMultitaskROCEvaluation() :
		CROCEvaluation(), m_task_relation(NULL), m_tasks_indices(NULL),
		m_num_tasks(0)
	{
	}

	/** constructor */
	CMultitaskROCEvaluation(std::shared_ptr<CTaskRelation> task_relation) :
		CROCEvaluation(), m_task_relation(NULL), m_tasks_indices(NULL),
		m_num_tasks(0)
	{
		set_task_relation(task_relation);
	}

	/** destructor */
	virtual ~CMultitaskROCEvaluation()
	{
		SG_FREE(m_tasks_indices);
	}

	/** set task relation */
	void set_task_relation(std::shared_ptr<CTaskRelation> task_relation)
	{
		
		
		m_task_relation = task_relation;
	}

	/** get task relation */
	std::shared_ptr<CTaskRelation> get_task_relation() const
	{
		
		return m_task_relation;
	}

	/** set absolute indices of labels to be evaluated next
	 * used by multitask evaluations
	 *
	 * @param indices indices
	 */
	virtual void set_indices(SGVector<index_t> indices);

	/** get name */
	virtual const char* get_name() const { return "MultitaskROCEvaluation"; };

	/** evaluate ROC and auROC
	 * @param predicted labels
	 * @param ground_truth labels assumed to be correct
	 * @return auROC
	 */
	virtual float64_t evaluate(std::shared_ptr<CLabels> predicted, std::shared_ptr<CLabels> ground_truth);

	/** get evaluation direction */
	virtual EEvaluationDirection get_evaluation_direction() const
	{
		return ED_MAXIMIZE;
	}

protected:

	/** task relation */
	std::shared_ptr<CTaskRelation> m_task_relation;

	/** indices */
	SGVector<index_t>* m_tasks_indices;

	/** num tasks */
	int32_t m_num_tasks;
};

}

#endif /* MULTITASKROCEVALUATION_H_ */
