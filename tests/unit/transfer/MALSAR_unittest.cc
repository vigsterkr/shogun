/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Heiko Strathmann
 */
#include <gtest/gtest.h>
#include <shogun/lib/config.h>
#ifdef USE_GPL_SHOGUN
#include <shogun/features/DataGenerator.h>
#include <shogun/transfer/multitask/MultitaskL12LogisticRegression.h>
#include <shogun/transfer/multitask/MultitaskClusteredLogisticRegression.h>
#include <shogun/transfer/multitask/MultitaskTraceLogisticRegression.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/labels/BinaryLabels.h>


#include <utility>

using namespace shogun;

typedef std::pair<CDotFeatures*, CDotFeatures*> SplittedFeatures;
typedef std::pair<SplittedFeatures, CBinaryLabels*> SplittedDataset;

#ifdef HAVE_LAPACK
SplittedDataset generate_data()
{
	index_t num_samples = 50;
	CMath::init_random(5);
	SGMatrix<float64_t> data =
		CDataGenerator::generate_gaussians(num_samples, 2, 2);
	auto features = std::make_shared<CDenseFeatures<float64_t>>(data);

	SGVector<index_t> train_idx(num_samples), test_idx(num_samples);
	SGVector<float64_t> labels(num_samples);
	for (index_t i = 0, j = 0; i < 2*num_samples; ++i)
	{
		if (i % 2 == 0)
			train_idx[j] = i;
		else
			test_idx[j++] = i;

		labels[i/2] = (i < num_samples) ? 1.0 : -1.0;
	}

	auto train_feats = features->copy_subset(train_idx)->as<CDenseFeatures<float64_t>>();
	auto test_feats =  features->copy_subset(test_idx)->as<CDenseFeatures<float64_t>>();

	auto ground_truth = std::make_shared<CBinaryLabels>(labels);


	return SplittedDataset(SplittedFeatures(train_feats, test_feats), ground_truth);
}

TEST(MalsarL12Test, train)
{
	SplittedDataset data = generate_data();

	auto task_group = std::make_shared<CTaskGroup>();
	auto task = std::make_shared<CTask>(0, data.second->get_num_labels());
	task_group->append_task(task);

	auto mtlr = std::make_shared<CMultitaskL12LogisticRegression>(0.1,0.1,data.first.first,data.second,task_group);
	mtlr->train();
	mtlr->set_features(data.first.second);
	mtlr->set_current_task(0);
	auto output = mtlr->apply();


}

TEST(MalsarClusteredTest, train)
{
	SplittedDataset data = generate_data();

	auto task_group = std::make_shared<CTaskGroup>();
	auto task = std::make_shared<CTask>(0, data.second->get_num_labels());
	task_group->append_task(task);

	auto mtlr = std::make_shared<CMultitaskClusteredLogisticRegression>(0.1,0.1,data.first.first,data.second,task_group,1);
	mtlr->train();
	mtlr->set_features(data.first.second);
	mtlr->set_current_task(0);
	auto output = mtlr->apply();


}

TEST(MalsarTraceTest, train)
{
	SplittedDataset data = generate_data();

	auto task_group = std::make_shared<CTaskGroup>();
	auto task = std::make_shared<CTask>(0, data.second->get_num_labels());
	task_group->append_task(task);

	auto mtlr = std::make_shared<CMultitaskTraceLogisticRegression>(0.1,data.first.first,data.second,task_group);
	mtlr->train();
	mtlr->set_features(data.first.second);
	mtlr->set_current_task(0);
	auto output = mtlr->apply();


}
#endif // HAVE_LAPACK
#endif //USE_GPL_SHOGUN
