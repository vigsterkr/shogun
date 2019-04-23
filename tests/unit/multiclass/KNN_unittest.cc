#include <gtest/gtest.h>

#include <shogun/labels/MulticlassLabels.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/features/SparseFeatures.h>
#include <shogun/multiclass/KNN.h>
#include <shogun/distance/EuclideanDistance.h>
#include <shogun/labels/BinaryLabels.h>
#include <shogun/features/DataGenerator.h>

using namespace shogun;

void generate_knn_data(SGMatrix<float64_t>& feat, SGVector<float64_t>& lab,
	   	int32_t num, int32_t classes, int32_t feats)
{
	CMath::init_random(1);
	feat = CDataGenerator::generate_gaussians(num,classes,feats);
	for( int32_t i = 0 ; i < classes ; ++i )
		for( int32_t j = 0 ; j < num ; ++j )
			lab[i*num+j] = double(i);

}

class KNNTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		SGVector<float64_t> lab(classes*num);
		SGMatrix<float64_t> feat(feats, classes*num);

		generate_knn_data(feat, lab, num, classes, feats);

		train = SGVector<index_t>(index_t(num*classes*0.75));
		test = SGVector<index_t>(index_t(num*classes*0.25));

		//generate random subset for train and test data
		train.random(0, classes*num-1);
		test.random(0, classes*num-1);

		labels = std::make_shared<CMulticlassLabels>(lab);

		features = std::make_shared<CDenseFeatures<float64_t>>(feat);

		features_test = features->clone()->as<CDenseFeatures<float64_t>>();
		labels_test = labels->clone()->as<CMulticlassLabels>();

		features->add_subset(train);
		labels->add_subset(train);
		features_test->add_subset(test);
		labels_test->add_subset(test);

		distance = std::make_shared<CEuclideanDistance>();

	}

	virtual void TearDown()
	{





	}

	const int32_t k = 4;
	const index_t num = 50;
	const index_t feats = 2;
	const int32_t classes = 3;

	SGVector<index_t> train;
	SGVector<index_t> test;

	std::shared_ptr<CMulticlassLabels> labels;
	std::shared_ptr<CMulticlassLabels> labels_test;
	std::shared_ptr<CDenseFeatures<float64_t>> features;
	std::shared_ptr<CDenseFeatures<float64_t>> features_test;
	std::shared_ptr<CDistance> distance;
};

// FIXME: templated tests but it doesn't work with enums :()
// typedef ::testing::Types<KNN_BRUTE, KNN_KDTREE, KNN_LSH> KNNTypes;
TEST_F(KNNTest, brute_solver)
{
	auto knn = std::make_shared<CKNN>(k, distance, labels, KNN_BRUTE);
	knn->train(features);
	auto output = knn->apply(features_test)->as<CMulticlassLabels>();

	for ( index_t i = 0; i < labels_test->get_num_labels(); ++i )
		EXPECT_EQ(output->get_label(i), labels_test->get_label(i));
}

TEST_F(KNNTest, kdtree_solver)
{
	auto knn = std::make_shared<CKNN>(k, distance, labels, KNN_KDTREE);
	knn->train(features);
	auto output = knn->apply(features_test)->as<CMulticlassLabels>();

	for ( index_t i = 0; i < labels_test->get_num_labels(); ++i )
		EXPECT_EQ(output->get_label(i), labels_test->get_label(i));

}

TEST_F(KNNTest, lsh_solver)
{
	auto knn = std::make_shared<CKNN>(k, distance, labels, KNN_LSH);
	knn->train(features);
	auto output = knn->apply(features_test)->as<CMulticlassLabels>();

	for ( index_t i = 0; i < labels_test->get_num_labels(); ++i )
		EXPECT_EQ(output->get_label(i), labels_test->get_label(i));

}

TEST_F(KNNTest, lsh_solver_sparse)
{
	auto knn = std::make_shared<CKNN>(k, distance, labels, KNN_LSH);
	// TODO: the sparse features should be actually sparse
	auto features_sparse = std::make_shared<CSparseFeatures<float64_t>>(features);
	auto features_test_sparse = std::make_shared<CSparseFeatures<float64_t>>(features_test);
	knn->train(features_sparse);
	auto output = knn->apply(features_test_sparse)->as<CMulticlassLabels>();

	for ( index_t i = 0; i < labels_test->get_num_labels(); ++i )
		EXPECT_EQ(output->get_label(i), labels_test->get_label(i));

}

TEST(KNN, classify_multiple_brute)
{
	int32_t num = 50;
	int32_t feats = 2;
	int32_t classes = 3;

	SGVector< float64_t > lab(classes*num);
	SGMatrix< float64_t > feat(feats, classes*num);

	generate_knn_data(feat, lab, num, classes, feats);
	SGVector<index_t> train (int32_t(num*classes*0.75));
	SGVector<index_t> test (int32_t(num*classes*0.25));
	train.random(0, classes*num-1);
	test.random(0, classes*num-1);

	auto labels = std::make_shared<CMulticlassLabels>(lab);
	auto features = std::make_shared<CDenseFeatures< float64_t >>(feat);
	auto features_test = features->clone()->as<CDotFeatures>();
	auto labels_test = labels->clone()->as<CMulticlassLabels>();

	int32_t k=4;
	auto distance = std::make_shared<CEuclideanDistance>();
	auto knn=std::make_shared<CKNN> (k, distance, labels, KNN_BRUTE);


	features->add_subset(train);
	labels->add_subset(train);
	knn->train(features);

	// classify for multiple k
	features_test->add_subset(test);
	labels_test->add_subset(test);

	auto dist = std::make_shared<CEuclideanDistance>(features, features_test);
	knn->set_distance(dist);
	SGMatrix<int32_t> out_mat =knn->classify_for_multiple_k();
	features_test->remove_subset();

	for ( index_t i = 0; i < labels_test->get_num_labels(); ++i )
		for ( index_t j = 0; j < k; ++j )
			EXPECT_EQ(out_mat(i, j), labels_test->get_label(i));




}


TEST(KNN, classify_multiple_kdtree)
{

	int32_t num = 50;
	int32_t feats = 2;
	int32_t classes = 3;

	SGVector< float64_t > lab(classes*num);
	SGMatrix< float64_t > feat(feats, classes*num);

	generate_knn_data(feat, lab, num, classes, feats);
	SGVector<index_t> train (int32_t(num*classes*0.75));
	SGVector<index_t> test (int32_t(num*classes*0.25));
	train.random(0, classes*num-1);
	test.random(0, classes*num-1);

	auto labels = std::make_shared<CMulticlassLabels>(lab);
	auto features = std::make_shared<CDenseFeatures< float64_t >>(feat);
	auto features_test = features->clone()->as<CDotFeatures>();
	auto labels_test = labels->clone()->as<CMulticlassLabels>();

	int32_t k=4;
	auto distance = std::make_shared<CEuclideanDistance>();
	auto knn=std::make_shared<CKNN>(k, distance, labels, KNN_KDTREE);


	features->add_subset(train);
	labels->add_subset(train);
	knn->train(features);

	// classify for multiple k
	features_test->add_subset(test);
	labels_test->add_subset(test);
	auto dist = std::make_shared<CEuclideanDistance>(features, features_test);
	knn->set_distance(dist);
	SGMatrix<int32_t> out_mat =knn->classify_for_multiple_k();
	features_test->remove_subset();

	for ( index_t i = 0; i < labels_test->get_num_labels(); ++i )
		for ( index_t j = 0; j < k; ++j )
			EXPECT_EQ(out_mat(i, j), labels_test->get_label(i));




}
