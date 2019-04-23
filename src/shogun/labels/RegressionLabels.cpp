#include <shogun/labels/DenseLabels.h>
#include <shogun/labels/RegressionLabels.h>
#include <shogun/mathematics/linalg/LinalgNamespace.h>

using namespace shogun;

CRegressionLabels::CRegressionLabels() : CDenseLabels()
{
}

CRegressionLabels::CRegressionLabels(int32_t num_labels) : CDenseLabels(num_labels)
{
}

CRegressionLabels::CRegressionLabels(const SGVector<float64_t> src) : CDenseLabels()
{
	set_labels(src);
}

CRegressionLabels::CRegressionLabels(std::shared_ptr<CFile> loader) : CDenseLabels(loader)
{
}

ELabelType CRegressionLabels::get_label_type() const
{
	return LT_REGRESSION;
}

std::shared_ptr<CLabels> CRegressionLabels::shallow_subset_copy()
{
	SGVector<float64_t> shallow_copy_vector(m_labels);
	auto shallow_copy_labels=std::make_shared<CRegressionLabels>(m_labels.size());

	shallow_copy_labels->set_labels(shallow_copy_vector);
	if (m_subset_stack->has_subsets())
		shallow_copy_labels->add_subset(m_subset_stack->get_last_subset()->get_subset_idx());

	return shallow_copy_labels;
}

std::shared_ptr<CLabels> CRegressionLabels::duplicate() const
{
	return std::make_shared<CRegressionLabels>(*this);
}

namespace shogun
{
	std::shared_ptr<CRegressionLabels> regression_labels(std::shared_ptr<CLabels> orig)
	{
		REQUIRE(orig, "No labels provided.\n");
		try
		{
			switch (orig->get_label_type())
			{
			case LT_REGRESSION:
				return std::static_pointer_cast<CRegressionLabels>(orig);
			default:
				SG_SNOTIMPLEMENTED
			}
		}
		catch (const ShogunException& e)
		{
			SG_SERROR(
			    "Cannot convert %s to regression labels: \n", e.what(),
			    orig->get_name());
		}

		return nullptr;
	}
} // namespace shogun
