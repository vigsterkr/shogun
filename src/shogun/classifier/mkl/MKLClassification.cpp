#include <shogun/classifier/mkl/MKLClassification.h>
#ifdef USE_SVMLIGHT
#include <shogun/classifier/svm/SVMLight.h>
#endif //USE_SVMLIGHT
#include <shogun/classifier/svm/LibSVM.h>

using namespace shogun;

CMKLClassification::CMKLClassification(std::shared_ptr<CSVM> s) : CMKL(s)
{
	if (!s)
	{
#ifdef USE_SVMLIGHT
		s=std::make_shared<CSVMLight>();
#endif //USE_SVMLIGHT
		if (!s)
			s=std::make_shared<CLibSVM>();
		set_svm(s);
	}
}

CMKLClassification::~CMKLClassification()
{
}
float64_t CMKLClassification::compute_sum_alpha()
{
	float64_t suma=0;
	int32_t nsv=svm->get_num_support_vectors();
	for (int32_t i=0; i<nsv; i++)
		suma+=CMath::abs(svm->get_alpha(i));

	return suma;
}

void CMKLClassification::init_training()
{
	REQUIRE(m_labels, "Labels not set.\n");
	REQUIRE(m_labels->get_num_labels(), "Number of labels is zero.\n");
}

std::shared_ptr<CMKLClassification> CMKLClassification::obtain_from_generic(std::shared_ptr<CMachine> machine)
{
	if (machine == NULL)
		return NULL;

	if (machine->get_classifier_type() != CT_MKLCLASSIFICATION)
		SG_SERROR("Provided machine is not of type CMKLClassification!")

	auto casted = std::dynamic_pointer_cast<CMKLClassification>(machine);
	
	return casted;
}
