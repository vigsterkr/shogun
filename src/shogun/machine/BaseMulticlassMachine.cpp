/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Chiyuan Zhang
 */

#include <shogun/machine/BaseMulticlassMachine.h>

using namespace shogun;

BaseMulticlassMachine::BaseMulticlassMachine()
{
	m_machines = std::make_shared<DynamicObjectArray>();
	

	SG_ADD((std::shared_ptr<SGObject>*)&m_machines, "machines", "Machines that jointly make up the multi-class machine.");
}

BaseMulticlassMachine::~BaseMulticlassMachine()
{
	
}

int32_t BaseMulticlassMachine::get_num_machines() const
{
	return m_machines->get_num_elements();
}

EProblemType BaseMulticlassMachine::get_machine_problem_type() const
{
	return PT_MULTICLASS;
}

bool BaseMulticlassMachine::is_label_valid(std::shared_ptr<Labels >lab) const
{
	return lab->get_label_type() == LT_MULTICLASS;
}

