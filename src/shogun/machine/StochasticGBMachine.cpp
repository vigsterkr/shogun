/*
 * Copyright (c) The Shogun Machine Learning Toolbox
 * Written (w) 2014 Parijat Mazumdar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Shogun Development Team.
 */

#include <shogun/base/progress.h>
#include <shogun/lib/View.h>
#include <shogun/machine/StochasticGBMachine.h>
#include <shogun/mathematics/Math.h>
#include <shogun/optimization/lbfgs/lbfgs.h>

using namespace shogun;

CStochasticGBMachine::CStochasticGBMachine(std::shared_ptr<CMachine> machine, std::shared_ptr<CLossFunction> loss, int32_t num_iterations,
						float64_t learning_rate, float64_t subset_fraction)
: CMachine()
{
	init();

	if (machine!=NULL)
	{

		m_machine=machine;
	}

	if (loss!=NULL)
	{

		m_loss=loss;
	}

	m_num_iter=num_iterations;
	m_subset_frac=subset_fraction;
	m_learning_rate=learning_rate;
}

CStochasticGBMachine::~CStochasticGBMachine()
{




}

void CStochasticGBMachine::set_machine(std::shared_ptr<CMachine> machine)
{
	REQUIRE(machine,"Supplied machine is NULL\n")

	m_machine=machine;
}

std::shared_ptr<CMachine> CStochasticGBMachine::get_machine() const
{
	if (m_machine==NULL)
		SG_ERROR("machine not set yet!\n")


	return m_machine;
}

void CStochasticGBMachine::set_loss_function(std::shared_ptr<CLossFunction> f)
{
	REQUIRE(f,"Supplied loss function is NULL\n")

	m_loss=f;
}

std::shared_ptr<CLossFunction> CStochasticGBMachine::get_loss_function() const
{
	if (m_loss==NULL)
		SG_ERROR("Loss function not set yet!\n")


	return m_loss;
}

void CStochasticGBMachine::set_num_iterations(int32_t iter)
{
	REQUIRE(iter,"Number of iterations\n")
	m_num_iter=iter;
}

int32_t CStochasticGBMachine::get_num_iterations() const
{
	return m_num_iter;
}

void CStochasticGBMachine::set_subset_fraction(float64_t frac)
{
	REQUIRE((frac>0)&&(frac<=1),"subset fraction should lie between 0 and 1. Supplied value is %f\n",frac)

	m_subset_frac=frac;
}

float64_t CStochasticGBMachine::get_subset_fraction() const
{
	return m_subset_frac;
}

void CStochasticGBMachine::set_learning_rate(float64_t lr)
{
	REQUIRE((lr>0)&&(lr<=1),"learning rate should lie between 0 and 1. Supplied value is %f\n",lr)

	m_learning_rate=lr;
}

float64_t CStochasticGBMachine::get_learning_rate() const
{
	return m_learning_rate;
}

std::shared_ptr<CRegressionLabels> CStochasticGBMachine::apply_regression(std::shared_ptr<CFeatures> data)
{
	REQUIRE(data,"test data supplied is NULL\n")
	auto feats=data->as<CDenseFeatures<float64_t>>();

	SGVector<float64_t> retlabs(feats->get_num_vectors());
	retlabs.fill_vector(retlabs.vector,retlabs.vlen,0);
	for (int32_t i=0;i<m_num_iter;i++)
	{
		float64_t gamma=m_gamma->get_element(i);

		auto machine=m_weak_learners->get_element<CMachine>(i);
		REQUIRE(machine,"%d element of the array of weak learners is NULL. This is not expected\n",i)

		auto dlabels=machine->apply_regression(feats);
		SGVector<float64_t> delta=dlabels->get_labels();

		for (int32_t j=0;j<retlabs.vlen;j++)
			retlabs[j]+=delta[j]*gamma*m_learning_rate;



	}

	return std::make_shared<CRegressionLabels>(retlabs);
}

bool CStochasticGBMachine::train_machine(std::shared_ptr<CFeatures> data)
{
	REQUIRE(data,"training data not supplied!\n")
	REQUIRE(m_machine,"machine not set!\n")
	REQUIRE(m_loss,"loss function not specified\n")
	REQUIRE(m_labels, "labels not specified\n")

	auto feats=data->as<CDenseFeatures<float64_t>>();

	// initialize weak learners array and gamma array
	initialize_learners();

	// cache predicted labels for intermediate models
	auto interf=std::make_shared<CRegressionLabels>(feats->get_num_vectors());

	for (int32_t i=0;i<interf->get_num_labels();i++)
		interf->set_label(i,0);

	for (auto i : SG_PROGRESS(range(m_num_iter)))
	{
		const auto result = get_subset(feats, interf);
		const auto& feats_iter = std::get<0>(result);
		const auto& interf_iter = std::get<1>(result);
		const auto& labels_iter = std::get<2>(result);

		// compute pseudo-residuals
		auto pres =
		    compute_pseudo_residuals(interf_iter, labels_iter);

		// fit learner
		auto wlearner = fit_model(feats_iter, pres);
		m_weak_learners->push_back(wlearner);

		// compute multiplier
		auto hm = wlearner->apply_regression(feats_iter);

		float64_t gamma = compute_multiplier(interf_iter, hm, labels_iter);
		m_gamma->push_back(gamma);

		// update intermediate function value
		auto dlabels=wlearner->apply_regression(feats);
		SGVector<float64_t> delta=dlabels->get_labels();
		for (int32_t j=0;j<interf->get_num_labels();j++)
			interf->set_label(j,interf->get_label(j)+delta[j]*gamma*m_learning_rate);




	}


	return true;
}

float64_t CStochasticGBMachine::compute_multiplier(
    std::shared_ptr<CRegressionLabels> f, std::shared_ptr<CRegressionLabels> hm, std::shared_ptr<CLabels> labs)
{
	REQUIRE(f->get_num_labels()==hm->get_num_labels(),"The number of labels in both input parameters should be equal\n")

	auto instance=std::make_shared<CDynamicObjectArray>();
	instance->push_back(labs);
	instance->push_back(f);
	instance->push_back(hm);
	instance->push_back(m_loss);

	float64_t ret=get_gamma(instance.get());


	return ret;
}

std::shared_ptr<CMachine> CStochasticGBMachine::fit_model(std::shared_ptr<CDenseFeatures<float64_t>> feats, std::shared_ptr<CRegressionLabels> labels)
{
	// clone base machine
	auto c=m_machine->clone()->as<CMachine>();
	// train cloned machine
	c->set_labels(labels);
	c->train(feats);

	return c;
}

std::shared_ptr<CRegressionLabels> CStochasticGBMachine::compute_pseudo_residuals(
    std::shared_ptr<CRegressionLabels> inter_f, std::shared_ptr<CLabels> labs)
{
	auto labels = labs->as<CDenseLabels>()->get_labels();
	SGVector<float64_t> f=inter_f->get_labels();

	SGVector<float64_t> residuals(f.vlen);
	for (int32_t i=0;i<residuals.vlen;i++)
		residuals[i]=-m_loss->first_derivative(f[i],labels[i]);

	return std::make_shared<CRegressionLabels>(residuals);
}

std::tuple<std::shared_ptr<CDenseFeatures<float64_t>>, std::shared_ptr<CRegressionLabels>,
           std::shared_ptr<CLabels>>
CStochasticGBMachine::get_subset(
    std::shared_ptr<CDenseFeatures<float64_t>> f, std::shared_ptr<CRegressionLabels> interf)
{
	if (m_subset_frac == 1.0)
		return std::make_tuple(f, interf, m_labels);

	int32_t subset_size=m_subset_frac*(f->get_num_vectors());
	SGVector<index_t> idx(f->get_num_vectors());
	idx.range_fill();
	CMath::permute(idx);

	SGVector<index_t> subset(subset_size);
	sg_memcpy(subset.vector,idx.vector,subset.vlen*sizeof(index_t));

	return std::make_tuple(
	    view(f, subset), view(interf, subset),
	    view(m_labels, subset));
}

void CStochasticGBMachine::initialize_learners()
{

	m_weak_learners=std::make_shared<CDynamicObjectArray>();



	m_gamma=std::make_shared<CDynamicArray<float64_t>>();

}

float64_t CStochasticGBMachine::get_gamma(void* instance)
{
	lbfgs_parameter_t lbfgs_param;
	lbfgs_parameter_init(&lbfgs_param);
	lbfgs_param.linesearch=2;

	float64_t gamma=0;
	lbfgs(1,&gamma,NULL,CStochasticGBMachine::lbfgs_evaluate,NULL,instance,&lbfgs_param);

	return gamma;
}

float64_t CStochasticGBMachine::lbfgs_evaluate(void *obj, const float64_t *parameters, float64_t *gradient, const int dim,
												const float64_t step)
{
	REQUIRE(obj,"object cannot be NULL\n")
	auto objects=(CDynamicObjectArray*)obj;
	REQUIRE((objects->get_num_elements()==2) || (objects->get_num_elements()==4),"Number of elements in obj array"
	" (%d) does not match expectations(2 or 4)\n",objects->get_num_elements())

	if (objects->get_num_elements()==2)
	{
		// extract labels
		auto lab=objects->get_element<CDenseLabels>(0);
		REQUIRE(lab,"0 index element of objects is NULL\n")
		SGVector<float64_t> labels=lab->get_labels();

		// extract loss function
		auto lossf =objects->get_element<CLossFunction>(1);
		REQUIRE(lossf,"1 index element of objects is NULL\n")

		*gradient=0;
		float64_t ret=0;
		for (int32_t i=0;i<labels.vlen;i++)
		{
			*gradient+=lossf->first_derivative((*parameters),labels[i]);
			ret+=lossf->loss((*parameters),labels[i]);
		}



		return ret;
	}

	// extract labels
	auto lab=objects->get_element<CDenseLabels>(0);
	REQUIRE(lab,"0 index element of objects is NULL\n")
	SGVector<float64_t> labels=lab->get_labels();

	// extract f
	auto func=objects->get_element<CDenseLabels>(1);
	REQUIRE(func,"1 index element of objects is NULL\n")
	SGVector<float64_t> f=func->get_labels();

	// extract hm
	auto delta=objects->get_element<CDenseLabels>(2);
	REQUIRE(delta,"2 index element of objects is NULL\n")
	SGVector<float64_t> hm=delta->get_labels();

	// extract loss function
	auto lossf=objects->get_element<CLossFunction>(3);
	REQUIRE(lossf,"3 index element of objects is NULL\n")

	*gradient=0;
	float64_t ret=0;
	for (int32_t i=0;i<labels.vlen;i++)
	{
		*gradient+=lossf->first_derivative((*parameters)*hm[i]+f[i],labels[i]);
		ret+=lossf->loss((*parameters)*hm[i]+f[i],labels[i]);
	}





	return ret;
}

void CStochasticGBMachine::init()
{
	m_machine=NULL;
	m_loss=NULL;
	m_num_iter=0;
	m_subset_frac=0;
	m_learning_rate=0;

	m_weak_learners=std::make_shared<CDynamicObjectArray>();


	m_gamma=std::make_shared<CDynamicArray<float64_t>>();


	SG_ADD((std::shared_ptr<CSGObject>*)&m_machine,"m_machine","machine");
	SG_ADD((std::shared_ptr<CSGObject>*)&m_loss,"m_loss","loss function");
	SG_ADD(&m_num_iter,"m_num_iter","number of iterations");
	SG_ADD(&m_subset_frac,"m_subset_frac","subset fraction");
	SG_ADD(&m_learning_rate,"m_learning_rate","learning rate");
	SG_ADD((std::shared_ptr<CSGObject>*)&m_weak_learners,"m_weak_learners","array of weak learners");
	SG_ADD((std::shared_ptr<CSGObject>*)&m_gamma,"m_gamma","array of learner weights");
}
