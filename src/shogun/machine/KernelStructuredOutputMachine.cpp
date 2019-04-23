/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Fernando Iglesias, Thoralf Klein, Shell Hu
 */

#include <shogun/machine/KernelStructuredOutputMachine.h>
#include <shogun/kernel/Kernel.h>

using namespace shogun;

CKernelStructuredOutputMachine::CKernelStructuredOutputMachine()
: CStructuredOutputMachine(), m_kernel(NULL)
{
	register_parameters();
}

CKernelStructuredOutputMachine::CKernelStructuredOutputMachine(
		std::shared_ptr<CStructuredModel>  model,
		std::shared_ptr<CStructuredLabels> labs,
		std::shared_ptr<CKernel>           kernel)
: CStructuredOutputMachine(model, labs), m_kernel(NULL)
{
	set_kernel(kernel);
	register_parameters();
}

CKernelStructuredOutputMachine::~CKernelStructuredOutputMachine()
{
	
}

void CKernelStructuredOutputMachine::set_kernel(std::shared_ptr<CKernel> k)
{
	
	
	m_kernel = k;
}

std::shared_ptr<CKernel> CKernelStructuredOutputMachine::get_kernel() const
{
	
	return m_kernel;
}

void CKernelStructuredOutputMachine::register_parameters()
{
	SG_ADD((std::shared_ptr<CSGObject>*)&m_kernel, "m_kernel", "The kernel", ParameterProperties::HYPER);
}
