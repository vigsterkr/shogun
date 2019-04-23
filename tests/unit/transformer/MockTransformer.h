#include <gmock/gmock.h>
#include <shogun/transformer/Transformer.h>

namespace shogun
{

	class MockCTransformer : public CTransformer
	{
	public:
		MOCK_METHOD1(fit, void(std::shared_ptr<CFeatures>));

		MOCK_METHOD2(fit, void(std::shared_ptr<CFeatures>, std::shared_ptr<CLabels>));

		MOCK_METHOD2(transform, std::shared_ptr<CFeatures>(std::shared_ptr<CFeatures>, bool));

		MOCK_CONST_METHOD0(train_require_labels, bool());

		virtual const char* get_name() const
		{
			return "MockCTransformer";
		}
	};

} // namespace shogun
