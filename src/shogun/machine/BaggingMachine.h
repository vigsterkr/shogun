/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Viktor Gal, Fernando Iglesias, Heiko Strathmann, Yuyu Zhang,
 *          Olivier NGuyen, Bjoern Esser, Thoralf Klein, Soeren Sonnenburg,
 *          Soumyajit De
 */

#ifndef BAGGINGMACHINE_H
#define BAGGINGMACHINE_H

#include <shogun/lib/config.h>

#include <shogun/machine/Machine.h>

namespace shogun
{
	class CCombinationRule;
	class CEvaluation;

	/**
	 * @brief: Bagging algorithm
	 * i.e. bootstrap aggregating
	 */
	class CBaggingMachine : public CMachine
	{
		public:
			/** default ctor */
			CBaggingMachine();

			/**
			 * constructor
			 *
			 * @param features training features
			 * @param labels training labels
			 */
			CBaggingMachine(std::shared_ptr<CFeatures> features, std::shared_ptr<CLabels> labels);

			virtual ~CBaggingMachine() = default;

			virtual std::shared_ptr<CBinaryLabels> apply_binary(std::shared_ptr<CFeatures> data=NULL);
			virtual std::shared_ptr<CMulticlassLabels> apply_multiclass(std::shared_ptr<CFeatures> data=NULL);
			virtual std::shared_ptr<CRegressionLabels> apply_regression(std::shared_ptr<CFeatures> data=NULL);

			/**
			 * Set number of bags/machine to create
			 *
			 * @param num_bags number of bags
			 */
			void set_num_bags(int32_t num_bags);

			/**
			 * Get number of bags/machines
			 *
			 * @return number of bags
			 */
			int32_t get_num_bags() const;

			/**
			 * Set number of feature vectors to use
			 * for each bag/machine
			 *
			 * @param bag_size number of vectors to use for a bag
			 */
			virtual void set_bag_size(int32_t bag_size);

			/**
			 * Get number of feature vectors that are use
			 * for training each bag/machine
			 *
			 * @return number of vectors used for training for each bag.
			 */
			virtual int32_t get_bag_size() const;

			/**
			 * Get machine for bagging
			 *
			 * @return machine that is being used in bagging
			 */
			std::shared_ptr<CMachine> get_machine() const;

			/**
			 * Set machine to use in bagging
			 *
			 * @param machine the machine to use for bagging
			 */
			virtual void set_machine(std::shared_ptr<CMachine> machine);

			/**
			 * Set the combination rule to use for aggregating the classification
			 * results
			 *
			 * @param rule combination rule
			 */
			void set_combination_rule(std::shared_ptr<CCombinationRule> rule);

			/**
			 * Get the combination rule that is used for aggregating the results
			 *
			 * @return CCombinationRule
			 */
			std::shared_ptr<CCombinationRule> get_combination_rule() const;

			/** get classifier type
			 *
			 * @return classifier type CT_BAGGING
			 */
			virtual EMachineType get_classifier_type() { return CT_BAGGING; }

			/** get out-of-bag error
			 * CombinationRule is used for combining the predictions.
			 *
			 * @param eval Evaluation method to use for calculating the error
			 * @return out-of-bag error.
			 */
			float64_t get_oob_error(std::shared_ptr<CEvaluation> eval) const;

			/** name **/
			virtual const char* get_name() const { return "BaggingMachine"; }

		protected:
			virtual bool train_machine(std::shared_ptr<CFeatures> data=NULL);

			/**
			 * sets parameters of CMachine - useful in Random Forest
			 *
			 * @param m machine
			 * @param idx indices of training vectors chosen in current bag
			 */
			virtual void set_machine_parameters(std::shared_ptr<CMachine> m, SGVector<index_t> idx);

			/** helper function for the apply_{regression,..} functions that
			 * computes the output
			 *
			 * @param data the data to compute the output for
			 * @return predictions
			 */
			SGVector<float64_t> apply_get_outputs(std::shared_ptr<CFeatures> data);

		    /** helper function for the apply_{binary,..} functions that
		     * computes the output probabilities without combination rules
		     *
		     * @param data the data to compute the output for
		     * @return predictions
		     */
		    SGMatrix<float64_t>
		    apply_outputs_without_combination(std::shared_ptr<CFeatures> data);

		    /** Register paramaters */
		    void register_parameters();

		    /** Initialize the members with default values */
		    void init();

		    /**
		     * get the vector of indices for feature vectors that are out of bag
		     *
		     * @param in_bag vector of indices that are in bag.
		     * NOTE: in_bag is a randomly generated with replacement
		     * @return the vector of indices
		     */
		    std::shared_ptr<CDynamicArray<index_t>>
		    get_oob_indices(const SGVector<index_t>& in_bag);

		protected:
			/** bags array */
			std::vector<std::shared_ptr<CMachine>> m_bags;

			/** features to train on */
			std::shared_ptr<CFeatures> m_features;

			/** machine to use for bagging */
			std::shared_ptr<CMachine> m_machine;

			/** number of bags to create */
			int32_t m_num_bags;

			/** number of vectors to use from the training features */
			int32_t m_bag_size;

			/** combination rule to use */
			std::shared_ptr<CCombinationRule> m_combination_rule;

			/** indices of all feature vectors that are out of bag */
			SGVector<bool> m_all_oob_idx;

			/** array of oob indices */
			std::shared_ptr<CDynamicObjectArray> m_oob_indices;
	};
}

#endif /* BAGGINGMACHINE_H */
