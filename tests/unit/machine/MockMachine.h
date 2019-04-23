#ifndef _MOCKCMACHINE_H_
#define _MOCKCMACHINE_H_

#include <gmock/gmock.h>
#include <shogun/machine/Machine.h>

namespace shogun {

	class MockCMachine : public CMachine {
		public:
			MOCK_METHOD1(apply, std::shared_ptr<CLabels>(std::shared_ptr<CFeatures>));
			MOCK_METHOD1(train_locked, bool(SGVector<index_t>));
			MOCK_METHOD1(train_machine, bool(std::shared_ptr<CFeatures>));
			MOCK_METHOD0(store_model_features, void());
			MOCK_CONST_METHOD0(clone, std::shared_ptr<CSGObject>());

			virtual const char* get_name() const { return "MockCMachine"; }
	};

}  // namespace shogun
#endif /* _MOCKCMACHINE_H_ */
