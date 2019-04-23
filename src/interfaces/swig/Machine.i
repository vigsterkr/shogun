/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Heiko Strathmann, Sergey Lisitsyn
 */

/*%warnfilter(302) apply;
%warnfilter(302) apply_generic;*/

%rename(Machine) CMachine;
%rename(PipelineBuilder) CPipelineBuilder;
%rename(Pipeline) CPipeline;

#if defined(SWIGPYTHON) || defined(SWIGOCTAVE) || defined(SWIGRUBY) || defined(SWIGLUA) || defined(SWIGR)

%define APPLY_MULTICLASS(CLASS)
    %extend CLASS
    {
        std::shared_ptr<shogun::CMulticlassLabels> apply(std::shared_ptr<shogun::CFeatures> data=NULL)
        {
            return $self->apply_multiclass(data);
        }
    }
%enddef

%define APPLY_BINARY(CLASS)
    %extend CLASS
    {
        std::shared_ptr<shogun::CBinaryLabels> apply(std::shared_ptr<shogun::CFeatures> data=NULL)
        {
            return $self->apply_binary(data);
        }
    }
%enddef

%define APPLY_REGRESSION(CLASS)
    %extend CLASS
    {
        std::shared_ptr<shogun::CRegressionLabels> apply(std::shared_ptr<shogun::CFeatures> data=NULL)
        {
            return $self->apply_regression(data);
        }
    }
%enddef

%define APPLY_STRUCTURED(CLASS)
    %extend CLASS
    {
        std::shared_ptr<shogun::CStructuredLabels> apply(std::shared_ptr<shogun::CFeatures> data=NULL)
        {
            return $self->apply_structured(data);
        }
    }
%enddef

%define APPLY_LATENT(CLASS)
    %extend CLASS
    {
        std::shared_ptr<shogun::CLatentLabels> apply(std::shared_ptr<shogun::CFeatures> data=NULL)
        {
            return $self->apply_latent(data);
        }
    }
%enddef

APPLY_MULTICLASS(shogun::CMulticlassMachine);
APPLY_MULTICLASS(shogun::CKernelMulticlassMachine);
APPLY_MULTICLASS(shogun::CLinearMulticlassMachine);
APPLY_MULTICLASS(shogun::CDistanceMachine);

APPLY_BINARY(shogun::CLinearMachine);
APPLY_BINARY(shogun::CKernelMachine);
#ifdef USE_GPL_SHOGUN
APPLY_BINARY(shogun::CWDSVMOcas);
#endif //USE_GPL_SHOGUN
APPLY_BINARY(shogun::CPluginEstimate);
#ifdef USE_GPL_SHOGUN
APPLY_BINARY(shogun::CGaussianProcessClassification);
#endif //USE_GPL_SHOGUN

#if USE_SVMLIGHT
APPLY_REGRESSION(shogun::CSVRLight);
#endif //USE_SVMLIGHT
APPLY_REGRESSION(shogun::CMKLRegression);
#ifdef HAVE_LAPACK
APPLY_REGRESSION(shogun::CLeastSquaresRegression);
APPLY_REGRESSION(shogun::CLeastAngleRegression);
#endif
#ifdef USE_GPL_SHOGUN
APPLY_REGRESSION(shogun::CGaussianProcessRegression);
#endif //USE_GPL_SHOGUN

APPLY_STRUCTURED(shogun::CStructuredOutputMachine);
APPLY_STRUCTURED(shogun::CLinearStructuredOutputMachine);
APPLY_STRUCTURED(shogun::CKernelStructuredOutputMachine);
#ifdef USE_MOSEK
APPLY_STRUCTURED(shogun::CPrimalMosekSOSVM);
#endif
#ifdef USE_GPL_SHOGUN
APPLY_STRUCTURED(shogun::CDualLibQPBMSOSVM);
APPLY_LATENT(shogun::CLatentSVM);
#endif //USE_GPL_SHOGUN

%rename(apply_generic) CMachine::apply(std::shared_ptr<CFeatures> data=NULL);
%rename(apply_generic) CMulticlassMachine::apply(std::shared_ptr<CFeatures> data=NULL);
%rename(apply_generic) CKernelMulticlassMachine::apply(std::shared_ptr<CFeatures> data=NULL);
%rename(apply_generic) CLinearMulticlassMachine::apply(std::shared_ptr<CFeatures> data=NULL);
%rename(apply_generic) CCDistanceMachineMachine::apply(std::shared_ptr<CFeatures> data=NULL);
%rename(apply_generic) CLinearMachine::apply(std::shared_ptr<CFeatures> data=NULL);
%rename(apply_generic) CKernelMachine::apply(std::shared_ptr<CFeatures> data=NULL);
#ifdef USE_GPL_SHOGUN
%rename(apply_generic) CWDSVMOcas::apply(std::shared_ptr<CFeatures> data=NULL);
#endif //USE_GPL_SHOGUN
%rename(apply_generic) CPluginEstimate::apply(std::shared_ptr<CFeatures> data=NULL);
#ifdef USE_SVMLIGHT
%rename(apply_generic) CSVRLight::apply(std::shared_ptr<CFeatures> data=NULL);
#endif //USE_SVMLIGHT
%rename(apply_generic) CMKLRegression::apply(std::shared_ptr<CFeatures> data=NULL);
#ifdef HAVE_LAPACK
%rename(apply_generic) CLeastSquaresRegression::apply(std::shared_ptr<CFeatures> data=NULL);
%rename(apply_generic) CLeastAngleRegression::apply(std::shared_ptr<CFeatures> data=NULL);
#endif
%rename(apply_generic) CGaussianProcessRegression::apply(std::shared_ptr<CFeatures> data=NULL);

%rename(apply_generic) CStructuredOutputMachine::apply(std::shared_ptr<CFeatures> data=NULL);
%rename(apply_generic) CLinearStructuredOutputMachine::apply(std::shared_ptr<CFeatures> data=NULL);
%rename(apply_generic) CKernelStructuredOutputMachine::apply(std::shared_ptr<CFeatures> data=NULL);
#ifdef USE_MOSEK
%rename(apply_generic) CPrimalMosekSOSVM::apply(std::shared_ptr<CFeatures> data=NULL);
#endif

#undef APPLY_MULTICLASS
#undef APPLY_BINARY
#undef APPLY_REGRESSION
#undef APPLY_STRUCTURED
#undef APPLY_LATENT

#endif

%include <shogun/machine/Machine.h>
%include <shogun/machine/Pipeline.h>
