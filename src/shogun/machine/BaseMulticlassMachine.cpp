/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Chiyuan Zhang
 */

#include <shogun/machine/BaseMulticlassMachine.h>

using namespace shogun;

CBaseMulticlassMachine::CBaseMulticlassMachine()
{
	m_machines = std::make_shared<CDynamicObjectArray>();
	

	SG_ADD((std::shared_ptr<CSGObject>*)&m_machines, "machines", "Machines that jointly make up the multi-class machine.");
}

CBaseMulticlassMachine::~CBaseMulticlassMachine()
{
	
}

int32_t CBaseMulticlassMachine::get_num_machines() const
{
	return m_machines->get_num_elements();
}

EProblemType CBaseMulticlassMachine::get_machine_problem_type() const
{
	return PT_MULTICLASS;
}

bool CBaseMulticlassMachine::is_label_valid(std::shared_ptr<CLabels >lab) const
{
	return lab->get_label_type() == LT_MULTICLASS;
}

