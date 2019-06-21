/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Fernando Iglesias, Shell Hu, Thoralf Klein, Yuyu Zhang, 
 *          Bjoern Esser, Sergey Lisitsyn, Soeren Sonnenburg
 */

#ifndef _LINEAR_STRUCTURED_OUTPUT_MACHINE__H__
#define _LINEAR_STRUCTURED_OUTPUT_MACHINE__H__

#include <shogun/lib/config.h>

#include <shogun/machine/StructuredOutputMachine.h>
#include <shogun/lib/SGVector.h>

namespace shogun
{

class CFeatures;

/** TODO doc */
class CLinearStructuredOutputMachine : public CStructuredOutputMachine
{
	public:
		/** default constructor  */
		CLinearStructuredOutputMachine();

		/** standard constructor
		 *
		 * @param model structured model with application specific functions
		 * @param labs structured labels
		 */
		CLinearStructuredOutputMachine(std::shared_ptr<CStructuredModel> model, std::shared_ptr<CStructuredLabels> labs);

		/** destructor */
		virtual ~CLinearStructuredOutputMachine();

		/** set w (useful for modular interfaces)
		 *
		 * @param w weight vector to set
		 */
		void set_w(SGVector< float64_t > w);

		/** get w
		 *
		 * @return w
		 */
		SGVector< float64_t > get_w() const;

		/**
		 * apply structured machine to data for Structured Output (SO)
		 * problem
		 *
		 * @param data (test)data to be classified
		 *
		 * @return classified 'labels'
		 */
		virtual std::shared_ptr<CStructuredLabels> apply_structured(std::shared_ptr<CFeatures> data = NULL);

		/** Stores feature data of underlying model. Does nothing because
		 * Linear machines store the normal vector of the separating hyperplane
		 * and therefore the model anyway
		 */
		virtual void store_model_features();

		/** @return object name */
		virtual const char* get_name() const
		{
			return "LinearStructuredOutputMachine";
		}

	private:
		/** register class members */
		void register_parameters();

	protected:
		/** weight vector */
		SGVector< float64_t > m_w;

}; /* class CLinearStructuredOutputMachine */

} /* namespace shogun */

#endif /* _LINEAR_STRUCTURED_OUTPUT_MACHINE__H__ */
