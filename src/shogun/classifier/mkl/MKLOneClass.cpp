#include <shogun/classifier/mkl/MKLOneClass.h>
#include <shogun/classifier/svm/LibSVMOneClass.h>

using namespace shogun;

CMKLOneClass::CMKLOneClass(std::shared_ptr<CSVM> s) : CMKL(s)
{
	if (!s)
		set_svm(std::make_shared<CLibSVMOneClass>());
}

CMKLOneClass::~CMKLOneClass()
{
}

float64_t CMKLOneClass::compute_sum_alpha()
{
	return 0.0;
}

void CMKLOneClass::init_training()
{
	ASSERT(svm)
	ASSERT(svm->get_classifier_type() == CT_LIBSVMONECLASS)
}
