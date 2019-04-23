/* Remove C Prefix */
%rename(ConvolutionalFeatureMap) CConvolutionalFeatureMap;
%rename(NeuralLayer) CNeuralLayer;
%rename(RBM) CRBM;
%rename(DeepBeliefNetwork) CDeepBeliefNetwork;
%rename(Autoencoder) CAutoencoder;
%rename(DeepAutoencoder) CDeepAutoencoder;

/* Include Class Headers to make them visible from within the target language */
%include <shogun/neuralnets/NeuralLayer.h>
%include <shogun/neuralnets/ConvolutionalFeatureMap.h>
%include <shogun/neuralnets/RBM.h>
%include <shogun/neuralnets/DeepBeliefNetwork.h>
%include <shogun/neuralnets/Autoencoder.h>
%include <shogun/neuralnets/DeepAutoencoder.h>
