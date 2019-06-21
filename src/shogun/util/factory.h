/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Heiko Strathmann, Fernando Iglesias
 */
#ifndef FACTORY_H_
#define FACTORY_H_

#include <shogun/base/class_list.h>
#include <shogun/classifier/svm/SVM.h>
#include <shogun/converter/Converter.h>
#include <shogun/distance/Distance.h>
#include <shogun/evaluation/DifferentiableFunction.h>
#include <shogun/evaluation/Evaluation.h>
#include <shogun/evaluation/EvaluationResult.h>
#include <shogun/evaluation/MachineEvaluation.h>
#include <shogun/evaluation/SplittingStrategy.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/features/DenseSubsetFeatures.h>
#include <shogun/io/CSVFile.h>
#include <shogun/io/LibSVMFile.h>
#include <shogun/io/SGIO.h>
#include <shogun/kernel/CustomKernel.h>
#include <shogun/kernel/Kernel.h>
#include <shogun/labels/DenseLabels.h>
#include <shogun/lib/observers/ParameterObserver.h>
#include <shogun/loss/LossFunction.h>
#include <shogun/machine/Machine.h>
#include <shogun/machine/Pipeline.h>
#include <shogun/machine/gp/Inference.h>
#include <shogun/machine/gp/LikelihoodModel.h>
#include <shogun/machine/gp/MeanFunction.h>
#include <shogun/multiclass/MulticlassStrategy.h>
#include <shogun/multiclass/ecoc/ECOCDecoder.h>
#include <shogun/multiclass/ecoc/ECOCEncoder.h>
#include <shogun/neuralnets/NeuralLayer.h>
#include <shogun/transformer/Transformer.h>

namespace shogun
{

	std::shared_ptr<CDistance> distance(const std::string& name);
	std::shared_ptr<CEvaluation> evaluation(const std::string& name);
	std::shared_ptr<CKernel> kernel(const std::string& name);
	std::shared_ptr<CMachine> machine(const std::string& name);
	std::shared_ptr<CMulticlassStrategy> multiclass_strategy(const std::string& name);
	std::shared_ptr<CECOCEncoder> ecoc_encoder(const std::string& name);
	std::shared_ptr<CECOCDecoder> ecoc_decoder(const std::string& name);
	std::shared_ptr<CTransformer> transformer(const std::string& name);
	std::shared_ptr<CNeuralLayer> layer(const std::string& name);
	std::shared_ptr<CSplittingStrategy> splitting_strategy(const std::string& name);
	std::shared_ptr<CMachineEvaluation> machine_evaluation(const std::string& name);
	std::shared_ptr<CSVM> svm(const std::string& name);
	std::shared_ptr<CFeatures> features(const std::string& name);
	std::shared_ptr<CLikelihoodModel> gp_likelihood(const std::string& name);
	std::shared_ptr<CMeanFunction> gp_mean(const std::string& name);
	std::shared_ptr<CDifferentiableFunction> differentiable(const std::string& name);
	std::shared_ptr<CInference> gp_inference(const std::string& name);
	std::shared_ptr<CLossFunction> loss(const std::string& name);
	std::shared_ptr<ParameterObserver> parameter_observer(const std::string& name);

#define BASE_CLASS_FACTORY(T, factory_name)                                    \
	std::shared_ptr<T> factory_name(const std::string& name)                   \
	{                                                                          \
		return create_object<T>(name.c_str());                                 \
	}                                                                          \
	std::shared_ptr<T> as_## factory_name(std::shared_ptr<CSGObject> obj)      \
	{                                                                          \
		return obj->as<T>();                             					   \
	}
	BASE_CLASS_FACTORY(CEvaluation, evaluation)
	BASE_CLASS_FACTORY(CDistance, distance)
	BASE_CLASS_FACTORY(CKernel, kernel)
	BASE_CLASS_FACTORY(CMachine, machine)
	BASE_CLASS_FACTORY(CMulticlassStrategy, multiclass_strategy)
	BASE_CLASS_FACTORY(CECOCEncoder, ecoc_encoder)
	BASE_CLASS_FACTORY(CECOCDecoder, ecoc_decoder)
	BASE_CLASS_FACTORY(CTransformer, transformer)
	BASE_CLASS_FACTORY(CNeuralLayer, layer)
	BASE_CLASS_FACTORY(CSplittingStrategy, splitting_strategy)
	BASE_CLASS_FACTORY(CMachineEvaluation, machine_evaluation)
	BASE_CLASS_FACTORY(CSVM, svm)
	BASE_CLASS_FACTORY(CFeatures, features)
	BASE_CLASS_FACTORY(CLikelihoodModel, gp_likelihood)
	BASE_CLASS_FACTORY(CMeanFunction, gp_mean)
	BASE_CLASS_FACTORY(CInference, gp_inference)
	BASE_CLASS_FACTORY(CDifferentiableFunction, differentiable)
	BASE_CLASS_FACTORY(CLossFunction, loss)
	BASE_CLASS_FACTORY(ParameterObserver, parameter_observer)
	BASE_CLASS_FACTORY(CEvaluationResult, evaluation_result)

	template <class T>
	std::shared_ptr<CFeatures> features(SGMatrix<T> mat)
	{
		return std::make_shared<CDenseFeatures<T>>(mat);
	}

	std::shared_ptr<CFeatures> features(std::shared_ptr<CFile> file, machine_int_t primitive_type = PT_FLOAT64)
	{
		REQUIRE(file, "No file provided.\n");
		std::shared_ptr<CFeatures> result = nullptr;

		switch (primitive_type)
		{
		case PT_FLOAT64:
			result = std::make_shared<CDenseFeatures<float64_t>>();
			break;
		case PT_FLOAT32:
			result = std::make_shared<CDenseFeatures<float32_t>>();
			break;
		case PT_FLOATMAX:
			result = std::make_shared<CDenseFeatures<floatmax_t>>();
			break;
		case PT_UINT8:
			result = std::make_shared<CDenseFeatures<uint8_t>>();
			break;
		case PT_UINT16:
			result = std::make_shared<CDenseFeatures<uint16_t>>();
			break;
		default:
			SG_SNOTIMPLEMENTED
		}
		result->load(file);
		return result;
	}

	std::shared_ptr<CFeatures> string_features(
	    std::shared_ptr<CFile> file, machine_int_t alphabet_type = DNA,
	    machine_int_t primitive_type = PT_CHAR)
	{
		REQUIRE(file, "No file provided.\n");

		switch (primitive_type)
		{
		case PT_CHAR:
			return std::make_shared<CStringFeatures<char>>(file, static_cast<EAlphabet>(alphabet_type));
			break;
		default:
			SG_SNOTIMPLEMENTED
		}

		return nullptr;
	}

	/** Create embedded string features from string char features.
	 * The new features has the same alphabet as the original features. Data of
	 * the new features is obtained by calling CStringFeatures::obtain_from_char
	 * with the given features and other arguments of this factory method.
	 *
	 * @param features StringCharFeatures
	 * @param start start
	 * @param p_order order
	 * @param gap gap
	 * @param rev reverse
	 * @param primitive_type primitive type of the string features
	 * @return new instance of string features
	 */
	std::shared_ptr<CFeatures> string_features(
	    std::shared_ptr<CFeatures> features, int32_t start, int32_t p_order, int32_t gap,
	    bool rev, machine_int_t primitive_type)
	{

		REQUIRE_E(features, std::invalid_argument, "No features provided.\n");
		REQUIRE_E(
		    features->get_feature_class() == C_STRING &&
		        features->get_feature_type() == F_CHAR,
		    std::invalid_argument, "Given features must be char-based StringFeatures, "
		                           "provided (%s) have feature class (%d), feature type "
		                           "(%d) and class name.\n",
		                           features->get_name(),
								   features->get_feature_class(),
								   features->get_feature_type());

		auto string_features = std::dynamic_pointer_cast<CStringFeatures<char>>(features);

		switch (primitive_type)
		{
		case PT_UINT16:
		{
			auto result =
			    std::make_shared<CStringFeatures<uint16_t>>(string_features->get_alphabet());
			bool success = result->obtain_from_char(
			    string_features, start, p_order, gap, rev);
			REQUIRE(success, "Failed to obtain from string char features.\n");
			return result;
		}
		default:
			SG_SNOTIMPLEMENTED
		}

		return nullptr;
	}

	/** Factory for CDenseSubsetFeatures.
	 * TODO: Should be removed once the concept of feature views has arrived
	 */
	std::shared_ptr<CFeatures> features_subset(std::shared_ptr<CFeatures> base_features, SGVector<index_t> indices,
			EPrimitiveType primitive_type = PT_FLOAT64)
	{
		REQUIRE(base_features, "No base features provided.\n");

		switch (primitive_type)
		{
		case PT_FLOAT64:
			return std::make_shared<CDenseSubsetFeatures<float64_t>>(
				std::dynamic_pointer_cast<CDenseFeatures<float64_t>>(base_features), indices);
			break;
		default:
			SG_SNOTIMPLEMENTED
		}

		return nullptr;
	}

	template <typename T, typename T2 = typename std::enable_if_t<
	                          std::is_floating_point<T>::value>>
	std::shared_ptr<CKernel> kernel(SGMatrix<T> kernel_matrix)
	{
		return std::make_shared<CCustomKernel>(kernel_matrix);
	}

#ifndef SWIG // SWIG should skip this part
	template <typename LT,
	          std::enable_if_t<
	              std::is_base_of<CDenseLabels, typename std::remove_pointer<
	                                                LT>::type>::value,
	              LT>* = nullptr>
	void try_labels(std::shared_ptr<CDenseLabels>& labels, const SGVector<float64_t>& data)
	{
		if (!labels)
		{
			auto l = std::make_shared<LT>();
			l->set_labels(data);
			if (l->is_valid())
				labels = l;
		}
	}
#endif // SWIG

	std::shared_ptr<CLabels> labels(std::shared_ptr<CFile> file)
	{
		REQUIRE(file, "No file provided.\n");

		// load label data into memory via any dense label specialization
		auto loaded = std::make_shared<CRegressionLabels>();
		loaded->load(file);
		auto labels = loaded->get_labels();


		std::shared_ptr<CDenseLabels> result = nullptr;
		auto csv_file = std::dynamic_pointer_cast<CCSVFile>(file);
		REQUIRE(
		    csv_file,
		    "Cannot load labels from %s(\"%s\").\n", file->get_name(),
		    file->get_filename());

		// try to interpret as all dense label types, from most restrictive to
		// least restrictive
		try_labels<CBinaryLabels>(result, labels);
		try_labels<CMulticlassLabels>(result, labels);
		try_labels<CRegressionLabels>(result, labels);
		REQUIRE(
		    result,
		    "Cannot load labels from %s(\"%s\") as any of dense labels.\n",
		    file->get_name(), file->get_filename());
		SG_SINFO(
		    "Loaded labels from %s(\"%s\") as %s\n", file->get_name(),
		    file->get_filename(), result->get_name())

		return result;
	}

	template <class T>
	std::shared_ptr<CLabels> labels(SGVector<T> labels)
	{
		std::shared_ptr<CDenseLabels> result = nullptr;
		// try to interpret as all dense label types, from most restrictive to
		// least restrictive
		try_labels<CBinaryLabels>(result, labels);
		try_labels<CMulticlassLabels>(result, labels);
		try_labels<CRegressionLabels>(result, labels);
		REQUIRE(
		    result, "Cannot interpret given labels as any of dense labels.\n");
		SG_SINFO("Interpreted labels as %s\n", result->get_name())
		return result;
	}

	std::shared_ptr<CFile> csv_file(std::string fname, char rw = 'r')
	{
		return std::make_shared<CCSVFile>(fname.c_str(), rw);
	}

	std::shared_ptr<CFile> libsvm_file(std::string fname, char rw = 'r')
	{
		return std::make_shared<CLibSVMFile>(fname.c_str(), rw);
	}

	/** Create a pipeline builder.
	 * See also CPipelineBuilder and CPipeline.
	 * @return new instance of CPipelineBuilder
	 */
	std::shared_ptr<CPipelineBuilder> pipeline()
	{
		return std::make_shared<CPipelineBuilder>();
	}
}
#endif // FACTORY_H_
