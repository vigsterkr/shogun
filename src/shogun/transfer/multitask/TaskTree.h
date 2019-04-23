/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Yuyu Zhang, Bjoern Esser
 */

#ifndef TASKTREE_H_
#define TASKTREE_H_

#include <shogun/lib/config.h>

#include <shogun/transfer/multitask/Task.h>
#include <shogun/transfer/multitask/TaskRelation.h>

namespace shogun
{

/** @brief class TaskTree used to represent a tree of tasks.
 * Tree is constructed via task with subtasks (and subtasks of subtasks ..)
 * passed to the TaskTree.
 *
 * @see CTask
 */
class CTaskTree : public CTaskRelation
{
public:

	/** default constructor */
	CTaskTree();

	/** constructor
	 * @param root_task root task of the tree
	 */
	CTaskTree(std::shared_ptr<CTask> root_task);

	/** destructor */
	virtual ~CTaskTree();

	/** get tasks indices */
	virtual SGVector<index_t>* get_tasks_indices() const;

	/** get number of leaf tasks in the tree
	 *
	 * @return number of leaf tasks in the tree
	 */
	virtual int32_t get_num_tasks() const;

	/** returns information about task
	 * in SLEP "ind_t" format
	 *
	 * @return SLEP ind_t of the tree
	 */
	SGVector<float64_t> get_SLEP_ind_t();

	/** get root task
	 *
	 * @return root task of the tree
	 */
	std::shared_ptr<CTask> get_root_task() const {  return m_root_task; }

	/** set root task
	 *
	 * @param root_task task to set as root of the tree
	 */
	void set_root_task(std::shared_ptr<CTask> root_task) {   m_root_task = root_task; }

	/** get name
	 *
	 * @return name of the object
	 */
	const char* get_name() const { return "TaskTree"; };

	/** get relation type
	 *
	 * @return TASK_TREE
	 */
	ETaskRelationType get_relation_type() const { return TASK_TREE; }

protected:

	/** root task of the tree */
	std::shared_ptr<CTask> m_root_task;

};

}
#endif

