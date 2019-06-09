/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Viktor Gal, Thoralf Klein, Giovanni De Toni, Soumyajit De,
 *          Bjoern Esser
 */

#ifndef _WIN32
#include <unistd.h>
#endif
#include <gtest/gtest.h>

#include <shogun/features/streaming/StreamingDenseFeatures.h>
#include <shogun/io/CSVFile.h>
#include <shogun/io/streaming/StreamingAsciiFile.h>
#include "../utils/Utils.h"

using namespace shogun;

TEST(StreamingDenseFeaturesTest, example_reading_from_file)
{
	index_t n=20;
	index_t dim=2;
	char fname[] = "StreamingDenseFeatures_reading.XXXXXX";
	generate_temp_filename(fname);

	SGMatrix<float64_t> data(dim,n);
	for (index_t i=0; i<dim*n; ++i)
		data.matrix[i] = sg_rand->std_normal_distrib();

	auto orig_feats=std::make_shared<DenseFeatures<float64_t>>(data);
	auto saved_features = std::make_shared<CSVFile>(fname, 'w');
	orig_feats->save(saved_features);
	saved_features->close();


	auto input = std::make_shared<StreamingAsciiFile>(fname);
	input->set_delimiter(',');
	auto feats
		= std::make_shared<StreamingDenseFeatures<float64_t>>(input, false, 5);

	index_t i = 0;
	feats->start_parser();
	while (feats->get_next_example())
	{
		SGVector<float64_t> example = feats->get_vector();
		SGVector<float64_t> expected = orig_feats->get_feature_vector(i);

		ASSERT_EQ(dim, example.vlen);

		for (index_t j = 0; j < dim; j++)
			EXPECT_NEAR(expected.vector[j], example.vector[j], 1E-5);

		feats->release_example();
		i++;
	}
	feats->end_parser();




	std::remove(fname);
}

TEST(StreamingDenseFeaturesTest, example_reading_from_features)
{
	index_t n=20;
	index_t dim=2;

	SGMatrix<float64_t> data(dim,n);
	for (index_t i=0; i<dim*n; ++i)
		data.matrix[i] = sg_rand->std_normal_distrib();

	auto orig_feats=std::make_shared<DenseFeatures<float64_t>>(data);
	auto feats = std::make_shared<StreamingDenseFeatures<float64_t>>(orig_feats);

	index_t i = 0;
	feats->start_parser();
	while (feats->get_next_example())
	{
		SGVector<float64_t> example = feats->get_vector();
		SGVector<float64_t> expected = orig_feats->get_feature_vector(i);

		ASSERT_EQ(dim, example.vlen);

		for (index_t j = 0; j < dim; j++)
			EXPECT_DOUBLE_EQ(expected.vector[j], example.vector[j]);

		feats->release_example();
		i++;
	}
	feats->end_parser();


}

TEST(StreamingDenseFeaturesTest, reset_stream)
{
	index_t n=20;
	index_t dim=2;

	SGMatrix<float64_t> data(dim,n);
	for (index_t i=0; i<dim*n; ++i)
		data.matrix[i]=sg_rand->std_normal_distrib();

	auto orig_feats=std::make_shared<DenseFeatures<float64_t>>(data);
	auto feats=std::make_shared<StreamingDenseFeatures<float64_t>>(orig_feats);

	feats->start_parser();

	auto streamed=feats->get_streamed_features(n)->as<DenseFeatures<float64_t>>();
	ASSERT_TRUE(streamed!=nullptr);
	ASSERT_TRUE(orig_feats->equals(streamed));


	feats->reset_stream();

	streamed=feats->get_streamed_features(n)->as<DenseFeatures<float64_t>>();
	ASSERT_TRUE(streamed!=nullptr);
	ASSERT_TRUE(orig_feats->equals(streamed));


	feats->end_parser();
}
