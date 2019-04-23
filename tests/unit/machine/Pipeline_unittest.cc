#include "features/MockFeatures.h"
#include "labels/MockLabels.h"
#include "machine/MockMachine.h"
#include "transformer/MockTransformer.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <shogun/lib/exception/InvalidStateException.h>
#include <shogun/machine/Pipeline.h>
#include <stdexcept>

using namespace shogun;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;
using ::testing::InSequence;

class PipelineTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		ON_CALL(*transformer1, train_require_labels())
		    .WillByDefault(Return(false));
		ON_CALL(*transformer2, train_require_labels())
		    .WillByDefault(Return(true));
	}

	std::shared_ptr<NiceMock<MockCTransformer>> transformer1 =
	    std::make_shared<NiceMock<MockCTransformer>>();
	std::shared_ptr<NiceMock<MockCTransformer>> transformer2 =
	    std::make_shared<NiceMock<MockCTransformer>>();
	std::shared_ptr<NiceMock<MockCMachine>> machine = std::make_shared<NiceMock<MockCMachine>>();
};

TEST_F(PipelineTest, no_machine)
{
	EXPECT_THROW(
	    std::make_shared<CPipelineBuilder>()->over(transformer1)->build(),
	    InvalidStateException);
}

TEST_F(PipelineTest, fit_predict)
{
	auto features = std::make_shared<NiceMock<MockCFeatures>>();
	auto labels = std::make_shared<NiceMock<MockCLabels>>();
	auto pipeline = std::make_shared<CPipelineBuilder>()
	                    ->over(transformer1)
	                    ->over(transformer2)
	                    ->then(machine);

	// no labels given
	EXPECT_THROW(pipeline->train(features), ShogunException);

	InSequence s;

	EXPECT_CALL(*transformer1, fit(_));
	EXPECT_CALL(*transformer1, fit(_, _)).Times(0);
	EXPECT_CALL(*transformer1, transform(_, _));
	EXPECT_CALL(*transformer2, fit(_)).Times(0);
	EXPECT_CALL(*transformer2, fit(_, _));
	EXPECT_CALL(*transformer2, transform(_, _));
	EXPECT_CALL(*machine, train_machine(_));

	pipeline->set_labels(labels);
	pipeline->train(features);

	Mock::VerifyAndClearExpectations(transformer1.get());
	Mock::VerifyAndClearExpectations(transformer2.get());
	Mock::VerifyAndClearExpectations(machine.get());

	EXPECT_CALL(*transformer1, transform(_, _));
	EXPECT_CALL(*transformer2, transform(_, _));
	EXPECT_CALL(*machine, apply(_));

	pipeline->apply(features);
}

TEST_F(PipelineTest, get)
{

	std::string transformer_name = "my_transformer";

	auto pipeline = std::make_shared<CPipelineBuilder>()
	                    ->over(transformer1)
	                    ->over(transformer_name, transformer2)
	                    ->then(machine);

	EXPECT_THROW(
	    pipeline->get_transformer("not_exists"), std::invalid_argument);
	EXPECT_EQ(
	    pipeline->get_transformer("MockCTransformer"), transformer1);
	EXPECT_EQ(pipeline->get_transformer(transformer_name), transformer2);
	EXPECT_EQ(pipeline->get_machine(), machine);
}
