/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Sergey Lisitsyn, Soeren Sonnenburg, Heiko Strathmann
 */

#include <shogun/lib/config.h>

#ifdef USE_SVMLIGHT

#include <shogun/transfer/domain_adaptation/DomainAdaptationSVM.h>
#include <shogun/io/SGIO.h>
#include <shogun/labels/Labels.h>
#include <shogun/labels/BinaryLabels.h>
#include <shogun/labels/RegressionLabels.h>
#include <iostream>
#include <vector>

using namespace shogun;

CDomainAdaptationSVM::CDomainAdaptationSVM() : SVMLight()
{
	init();
}

CDomainAdaptationSVM::CDomainAdaptationSVM(float64_t C, std::shared_ptr<Kernel> k, std::shared_ptr<Labels> lab, std::shared_ptr<SVM> pre_svm, float64_t B_param) : SVMLight(C, k, lab)
{
	init();
	init(pre_svm, B_param);
}

CDomainAdaptationSVM::~CDomainAdaptationSVM()
{

	SG_DEBUG("deleting DomainAdaptationSVM\n")
}


void CDomainAdaptationSVM::init(std::shared_ptr<SVM> pre_svm, float64_t B_param)
{
	REQUIRE(pre_svm != NULL, "Pre SVM should not be null");
	// increase reference counts


	this->presvm=pre_svm;
	this->B=B_param;
	this->train_factor=1.0;

	// set bias of parent svm to zero
	this->presvm->set_bias(0.0);

	// invoke sanity check
	is_presvm_sane();
}

bool CDomainAdaptationSVM::is_presvm_sane()
{
	if (!presvm) {
		SG_ERROR("presvm is null")
	}

	if (presvm->get_num_support_vectors() == 0) {
		SG_ERROR("presvm has no support vectors, please train first")
	}

	if (presvm->get_bias() != 0) {
		SG_ERROR("presvm bias not set to zero")
	}

	if (presvm->get_kernel()->get_kernel_type() != this->get_kernel()->get_kernel_type()) {
		SG_ERROR("kernel types do not agree")
	}

	if (presvm->get_kernel()->get_feature_type() != this->get_kernel()->get_feature_type()) {
		SG_ERROR("feature types do not agree")
	}

	return true;
}


bool CDomainAdaptationSVM::train_machine(std::shared_ptr<Features> data)
{

	if (data)
	{
		if (m_labels->get_num_labels() != data->get_num_vectors())
			SG_ERROR("Number of training vectors does not match number of labels\n")
		kernel->init(data, data);
	}

	if (m_labels->get_label_type() != LT_BINARY)
		SG_ERROR("DomainAdaptationSVM requires binary labels\n")

	int32_t num_training_points = get_labels()->get_num_labels();
	auto labels = binary_labels(get_labels());

	float64_t* lin_term = SG_MALLOC(float64_t, num_training_points);

	// grab current training features
	auto train_data = get_kernel()->get_lhs();

	// bias of parent SVM was set to zero in constructor, already contains B
	auto parent_svm_out = presvm->apply_binary(train_data);

	// pre-compute linear term
	for (int32_t i=0; i<num_training_points; i++)
	{
		lin_term[i] = train_factor * B * labels->get_label(i) * parent_svm_out->get_label(i) - 1.0;
	}

	//set linear term for QP
	this->set_linear_term(SGVector<float64_t>(lin_term, num_training_points));

	//train SVM
	bool success = SVMLight::train_machine();


	ASSERT(presvm)

	return success;

}


std::shared_ptr<SVM> CDomainAdaptationSVM::get_presvm()
{

	return presvm;
}


float64_t CDomainAdaptationSVM::get_B()
{
	return B;
}


float64_t CDomainAdaptationSVM::get_train_factor()
{
	return train_factor;
}


void CDomainAdaptationSVM::set_train_factor(float64_t factor)
{
	train_factor = factor;
}


std::shared_ptr<BinaryLabels> CDomainAdaptationSVM::apply_binary(std::shared_ptr<Features> data)
{
	ASSERT(data)
	ASSERT(presvm->get_bias()==0.0)

	int32_t num_examples = data->get_num_vectors();

	auto out_current = SVMLight::apply_binary(data);

	// recursive call if used on DomainAdaptationSVM object
	auto out_presvm = presvm->apply_binary(data);

	// combine outputs
	SGVector<float64_t> out_combined(num_examples);
	for (int32_t i=0; i<num_examples; i++)
	{
		out_combined[i] = out_current->get_value(i) + B*out_presvm->get_value(i);
	}



	return std::make_shared<BinaryLabels>(out_combined);

}

void CDomainAdaptationSVM::init()
{
	presvm = NULL;
	B = 0;
	train_factor = 1.0;

	SG_ADD((std::shared_ptr<SGObject>*) &presvm, "presvm", "SVM to regularize against.");
	SG_ADD(&B, "B", "regularization parameter B.", ParameterProperties::HYPER);
	SG_ADD(&train_factor, "train_factor",
			"flag to switch off regularization in training.", ParameterProperties::HYPER);
}

#endif //USE_SVMLIGHT
