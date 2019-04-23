#include <gtest/gtest.h>
#include <shogun/features/CombinedFeatures.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/features/streaming/generators/MeanShiftDataGenerator.h>
#include <shogun/io/fs/FileSystem.h>
#include <shogun/io/serialization/JsonSerializer.h>
#include <shogun/io/serialization/JsonDeserializer.h>
#include <shogun/io/stream/BufferedInputStream.h>
#include <shogun/io/stream/FileInputStream.h>
#include <shogun/io/stream/FileOutputStream.h>
#include <shogun/kernel/CombinedKernel.h>
#include <shogun/kernel/CustomKernel.h>
#include <shogun/kernel/GaussianKernel.h>
#include <shogun/mathematics/Math.h>

using namespace shogun;

TEST(CombinedKernelTest,test_array_operations)
{
	auto combined = std::make_shared<CCombinedKernel>();
	auto gaus_1 = std::make_shared<CGaussianKernel>();
	combined->append_kernel(gaus_1);

	auto gaus_2 = std::make_shared<CGaussianKernel>();
	combined->append_kernel(gaus_2);

	auto gaus_3 = std::make_shared<CGaussianKernel>();
	combined->insert_kernel(gaus_3,1);

	auto gaus_4 = std::make_shared<CGaussianKernel>();
	combined->insert_kernel(gaus_4,0);

	EXPECT_EQ(combined->get_num_kernels(),4);

	combined->delete_kernel(2);

	auto k_1 = combined->get_kernel(0);
	EXPECT_EQ(k_1, gaus_4);
	auto k_2 = combined->get_kernel(1);
	EXPECT_EQ(k_2, gaus_1);
	auto k_3 = combined->get_kernel(2);
	EXPECT_EQ(k_3, gaus_2);


}

TEST(CombinedKernelTest, test_subset_mixed)
{

	int n_runs = 10;

	auto gen = std::make_shared<CMeanShiftDataGenerator>(0, 2);
	auto feats = gen->get_streamed_features(n_runs);

	auto feats_combined = std::make_shared<CCombinedFeatures>();

	auto combined = std::make_shared<CCombinedKernel>();

	auto gaus_1 = std::make_shared<CGaussianKernel>(5);
	auto gaus_2 = std::make_shared<CGaussianKernel>(5);

	auto gaus_ck = std::make_shared<CGaussianKernel>(5);
	gaus_ck->init(feats, feats);

	auto custom_1 = std::make_shared<CCustomKernel>(gaus_ck);
	auto custom_2 = std::make_shared<CCustomKernel>(gaus_ck);
	;

	combined->append_kernel(custom_1);
	combined->append_kernel(gaus_1);
	feats_combined->append_feature_obj(feats);

	combined->append_kernel(custom_2);
	combined->append_kernel(gaus_2);
	feats_combined->append_feature_obj(feats);

	SGVector<index_t> inds(10);
	inds.range_fill();

	for (index_t i = 0; i < n_runs; ++i)
	{
		CMath::permute(inds);

		feats_combined->add_subset(inds);
		combined->init(feats_combined, feats_combined);

		auto ground_truth_kernel = combined->get_kernel(1);
		auto custom_kernel_1 = combined->get_kernel(0);
		auto custom_kernel_2 = combined->get_kernel(2);

		SGMatrix<float64_t> gauss_matrix =
		    ground_truth_kernel->get_kernel_matrix();
		SGMatrix<float64_t> custom_matrix_1 =
		    custom_kernel_1->get_kernel_matrix();
		SGMatrix<float64_t> custom_matrix_2 =
		    custom_kernel_2->get_kernel_matrix();

		for (index_t j = 0; j < n_runs; ++j)
		{
			for (index_t k = 0; k < n_runs; ++k)
			{
				EXPECT_NEAR(gauss_matrix(j, k), custom_matrix_1(j, k), 1e-6);
				EXPECT_NEAR(gauss_matrix(j, k), custom_matrix_1(j, k), 1e-6);
			}
		}

		feats_combined->remove_subset();
	}



}

TEST(CombinedKernelTest, test_subset_combined_only)
{

	int n_runs = 10;

	auto gen = std::make_shared<CMeanShiftDataGenerator>(0, 2);
	auto feats = gen->get_streamed_features(n_runs);

	auto combined = std::make_shared<CCombinedKernel>();

	auto gaus_ck = std::make_shared<CGaussianKernel>(5);
	gaus_ck->init(feats, feats);

	auto custom_1 = std::make_shared<CCustomKernel>(gaus_ck);
	auto custom_2 = std::make_shared<CCustomKernel>(gaus_ck);

	combined->append_kernel(custom_1);
	combined->append_kernel(custom_2);

	SGVector<index_t> inds(n_runs);
	inds.range_fill();

	for (index_t i = 0; i < n_runs; ++i)
	{
		CMath::permute(inds);

		feats->add_subset(inds);
		combined->init(feats, feats);
		gaus_ck->init(feats, feats);

		auto custom_kernel_1 = combined->get_kernel(0);
		auto custom_kernel_2 = combined->get_kernel(1);

		SGMatrix<float64_t> gauss_matrix = gaus_ck->get_kernel_matrix();
		SGMatrix<float64_t> custom_matrix_1 =
		    custom_kernel_1->get_kernel_matrix();
		SGMatrix<float64_t> custom_matrix_2 =
		    custom_kernel_2->get_kernel_matrix();

		for (index_t j = 0; j < n_runs; ++j)
		{
			for (index_t k = 0; k < n_runs; ++k)
			{
				EXPECT_NEAR(gauss_matrix(j, k), custom_matrix_1(j, k), 1e-6);
				EXPECT_NEAR(gauss_matrix(j, k), custom_matrix_1(j, k), 1e-6);
			}
		}

		feats->remove_subset();
	}



}

TEST(CombinedKernelTest,weights)
{
	auto combined = std::make_shared<CCombinedKernel>();
	combined->append_kernel(std::make_shared<CGaussianKernel>());
	combined->append_kernel(std::make_shared<CGaussianKernel>());
	combined->append_kernel(std::make_shared<CGaussianKernel>());

	SGVector<float64_t> weights(3);
	weights[0]=17.0;
	weights[1]=23.0;
	weights[2]=42.0;

	combined->set_subkernel_weights(weights);

	SGVector<float64_t> w=combined->get_subkernel_weights();

	EXPECT_EQ(weights[0], w[0]);
	EXPECT_EQ(weights[1], w[1]);
	EXPECT_EQ(weights[2], w[2]);

}

//FIXME
TEST(CombinedKernelTest, DISABLED_serialization)
{
	auto combined = std::make_shared<CCombinedKernel>();
	combined->append_kernel(std::make_shared<CGaussianKernel>(10, 4));
	combined->append_kernel(std::make_shared<CGaussianKernel>(10, 3));
	combined->append_kernel(std::make_shared<CGaussianKernel>(10, 9));

	SGVector<float64_t> weights(3);
	weights[0]=17.0;
	weights[1]=23.0;
	weights[2]=42.0;

	combined->set_subkernel_weights(weights);

	auto fs = io::FileSystemRegistry::instance();
	std::string filename("combined_kernel.weights");
	ASSERT_TRUE(fs->file_exists(filename));
	std::unique_ptr<io::WritableFile> file;
	ASSERT_FALSE(fs->new_writable_file(filename, &file));
	auto fos = std::make_shared<io::CFileOutputStream>(file.get());
	auto serializer = std::make_unique<io::CJsonSerializer>();
	serializer->attach(fos);
	serializer->write(wrap<CSGObject>(combined));

	std::unique_ptr<io::RandomAccessFile> raf;
	ASSERT_FALSE(fs->new_random_access_file(filename, &raf));
	auto fis = std::make_shared<io::CFileInputStream>(raf.get());
	auto bis = std::make_shared<io::CBufferedInputStream>(fis.get());
	auto deserializer = std::make_unique<io::CJsonDeserializer>();
	deserializer->attach(bis);
	auto deser_obj = deserializer->read_object();
	auto combined_read = deser_obj->as<CCombinedKernel>();
	ASSERT_FALSE(fs->delete_file(filename));

	auto k0 = combined_read->get_kernel(0)->as<CGaussianKernel>();
	auto k1 = combined_read->get_kernel(1)->as<CGaussianKernel>();
	auto k2 = combined_read->get_kernel(2)->as<CGaussianKernel>();

	EXPECT_NEAR(k0->get_width(), 4, 1e-9);
	EXPECT_NEAR(k1->get_width(), 3, 1e-9);
	EXPECT_NEAR(k2->get_width(), 9, 1e-9);

	SGVector<float64_t> w=combined_read->get_subkernel_weights();
	EXPECT_EQ(weights[0], w[0]);
	EXPECT_EQ(weights[1], w[1]);
	EXPECT_EQ(weights[2], w[2]);
}

TEST(CombinedKernelTest,combination)
{
	auto combined_list = CCombinedKernel::combine_kernels(std::make_shared<CList>());
	EXPECT_EQ(combined_list->get_num_elements(),0);


	auto kernel_list = std::make_shared<CList>(true);
	combined_list = CCombinedKernel::combine_kernels(kernel_list);
	EXPECT_EQ(combined_list->get_num_elements(),0);


	auto sub_list_1 = std::make_shared<CList>(true);
	auto ck1 = std::make_shared<CGaussianKernel>(10,3);
	sub_list_1->append_element(ck1);
	auto ck2 = std::make_shared<CGaussianKernel>(10,5);
	sub_list_1->append_element(ck2);
	auto ck3 = std::make_shared<CGaussianKernel>(10,7);
	sub_list_1->append_element(ck3);
	kernel_list->insert_element(sub_list_1);

	float64_t combs1[3]= {3, 5, 7};

	combined_list = CCombinedKernel::combine_kernels(kernel_list);
	index_t i = 0;
	for (auto kernel=combined_list->get_first_element(); kernel;
			kernel=combined_list->get_next_element())
	{
		auto c_kernel = kernel->as<CCombinedKernel>();
		auto subkernel = c_kernel->get_first_kernel();
		auto c_subkernel = subkernel->as<CGaussianKernel>();

		EXPECT_EQ(c_kernel->get_num_subkernels(), 1);
		EXPECT_EQ(c_subkernel->get_width(), combs1[i++]);
	}


	auto sub_list_2 = std::make_shared<CList>(true);
	auto ck4 = std::make_shared<CGaussianKernel>(20,21);
	sub_list_2->append_element(ck4);
	auto ck5 = std::make_shared<CGaussianKernel>(20,31);
	sub_list_2->append_element(ck5);
	kernel_list->append_element(sub_list_2);

	float64_t combs2[2][6] = {{   3,   5,    7,  3,   5,    7},
											{ 21, 21, 21, 31, 31, 31}};

	combined_list = CCombinedKernel::combine_kernels(kernel_list);

	index_t j = 0;
	for (auto kernel=combined_list->get_first_element(); kernel;
			kernel=combined_list->get_next_element())
	{
		auto c_kernel = kernel->as<CCombinedKernel>();
		EXPECT_EQ(c_kernel->get_num_subkernels(), 2);
		i = 0;
		for (index_t k_idx=0; k_idx<c_kernel->get_num_kernels(); k_idx++)
		{
			auto c_subkernel =
					c_kernel->get_kernel(k_idx)->as<CGaussianKernel>();
			EXPECT_NEAR(c_subkernel->get_width(), combs2[i++][j], 1e-9);
		}
		++j;
	}



	auto sub_list_3 = std::make_shared<CList>(true);
	auto ck6 = std::make_shared<CGaussianKernel>(25, 109);
	sub_list_3->append_element(ck6);
	auto ck7 = std::make_shared<CGaussianKernel>(25, 203);
	sub_list_3->append_element(ck7);
	auto ck8 = std::make_shared<CGaussianKernel>(25, 308);
	sub_list_3->append_element(ck8);
	auto ck9 = std::make_shared<CGaussianKernel>(25, 404);
	sub_list_3->append_element(ck9);
	kernel_list->append_element(sub_list_3);

	float64_t combs[3][24] = {
		{	3,		5,		7,		3,		5,		7,		3,		5,		7,		3,		5,		7,		3,		5,		7,		3,		5,		7,		3,		5,		7,		3,		5,		7},
		{	21,	21,	21,	31,	31,	31,	21,	21,	21,	31,	31,	31,	21,	21,	21,	31,	31,	31,	21,	21,	21,	31,	31,	31},
		{	109,	109,	109,	109,	109,	109,	203,	203,	203,	203,	203,	203,	308,	308,	308,	308,	308,	308,	404,	404,	404,	404,	404,	404}
		};

	combined_list = CCombinedKernel::combine_kernels(kernel_list);

	j = 0;
	for (auto kernel=combined_list->get_first_element(); kernel;
			kernel=combined_list->get_next_element())
	{
		auto c_kernel = kernel->as<CCombinedKernel>();
		i = 0;
		EXPECT_EQ(c_kernel->get_num_subkernels(), 3);
		for (index_t k_idx=0; k_idx<c_kernel->get_num_kernels(); k_idx++)
		{
			auto c_subkernel =
					c_kernel->get_kernel(k_idx)->as<CGaussianKernel>();
			EXPECT_NEAR(c_subkernel->get_width(), combs[i++][j], 1e-9);
		}
		++j;
	}



}
