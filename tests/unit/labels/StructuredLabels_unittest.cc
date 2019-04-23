/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Thoralf Klein, Fernando Iglesias, Akash Shivram
 */

#include <shogun/labels/StructuredLabels.h>
#include <shogun/structure/MulticlassSOLabels.h>
#include <gtest/gtest.h>

using namespace shogun;


TEST(StructuredLabels, add_label)
{
	int32_t num_labels = 3;
	auto l = std::make_shared<CStructuredLabels>(num_labels);

	l->add_label(std::make_shared<CRealNumber>(3));
	l->add_label(std::make_shared<CRealNumber>(7));
	l->add_label(std::make_shared<CRealNumber>(13));

	std::shared_ptr<CRealNumber> real_number;

	EXPECT_EQ(3, l->get_num_labels());

	real_number = l->get_label(0)->as<CRealNumber>();
	EXPECT_EQ(3, real_number->value);


	real_number = l->get_label(1)->as<CRealNumber>();
	EXPECT_EQ(7, real_number->value);


	real_number = l->get_label(2)->as<CRealNumber>();
	EXPECT_EQ(13, real_number->value);



}

TEST(StructuredLabels, set_label)
{
	int32_t num_labels = 3;
	auto l = std::make_shared<CStructuredLabels>(num_labels);

	l->add_label(std::make_shared<CRealNumber>(3));
	l->add_label(std::make_shared<CRealNumber>(7));
	l->add_label(std::make_shared<CRealNumber>(13));

	l->set_label(1, std::make_shared<CRealNumber>(23));

	std::shared_ptr<CRealNumber> real_number;

	EXPECT_EQ(3, l->get_num_labels());

	real_number = l->get_label(0)->as<CRealNumber>();
	EXPECT_EQ(3, real_number->value);


	real_number = l->get_label(1)->as<CRealNumber>();
	EXPECT_EQ(23, real_number->value);


	real_number = l->get_label(2)->as<CRealNumber>();
	EXPECT_EQ(13, real_number->value);



}
