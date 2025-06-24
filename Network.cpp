#include "Network.h"


// Constructor: fetches layers from GUI and initializes network weights
Network::Network(float learning_rate, int epochs, GUI* window)
    : learning_rate(learning_rate), epochs(epochs), window(window) {
    layerList.clear();
    for (Layer* guiLayer : window->getLayerList()) {
        layerList.push_back(guiLayer);
    }

    std::cout << "Final layer count: " << layerList.size() << std::endl;

    this->initializeWeights();
}

// Initialize weights and biases for all neurons in all layers
void Network::initializeWeights() {
    std::cout << "Initializing weights for network with " << layerList.size() << " layers" << std::endl;
    for (size_t i = 0; i < layerList.size(); ++i) {
        size_t inputSize = (i == 0) ? 784 : layerList[i - 1]->getNeuronCount();

        std::cout << "Layer " << i << " expects " << inputSize << " inputs" << std::endl;

        Layer* currentLayer = layerList[i];
        size_t neuronCount = currentLayer->getNeuronCount();
        std::cout << "Initializing weights for " << neuronCount << " neurons in layer " << i << std::endl;

        for (size_t j = 0; j < neuronCount; ++j) {
            Neuron* neuron = currentLayer->getNeuronList()[j];
            neuron->initializeWeights(inputSize);
            if (neuron->getWeights().size() != inputSize) {
                std::cerr << "ERROR: Neuron " << j << " in layer " << i << " has " << neuron->getWeights().size()
                    << " weights, but expected " << inputSize << std::endl;
            }
        }
    }

    std::cout << "Weight initialization complete" << std::endl;
}

// Forward pass through all layers with ReLU (hidden) and softmax (output)
std::vector<float> Network::forwardPass(const std::pair<int, std::vector<float>>& input) {
    
    if (input.second.size() != 784) {
        std::cout << "WARNING: Input size (" << input.second.size()
            << ") does not match expected input size (784)" << std::endl;
    }

    const std::vector<float>* currentActivations = &input.second; 
    std::vector<float> layerOutputs;
    std::vector<float> nextLayerInputs;

    output.clear();
    for (size_t i = 0; i < layerList.size(); ++i) {
        Layer* currentLayer = layerList[i];
        bool isOutputLayer = (i == layerList.size() - 1);
        size_t neuronCount = currentLayer->getNeuronCount();
        layerOutputs.clear();
        layerOutputs.resize(neuronCount);
        for (size_t j = 0; j < neuronCount; ++j) {
            Neuron* neuron = currentLayer->getNeuronList()[j];
            const std::vector<float>& weights = neuron->getWeights();
            float z = neuron->getBias();
            if (weights.size() != currentActivations->size()) {
                std::cerr << "Size mismatch in forwardPass: weights(" << weights.size()
                    << "), input(" << currentActivations->size() << ") at layer "
                    << i << ", neuron " << j << "\n";
                if (i > 0 && j == 0) { 
                    std::cout << "Attempting to fix weights for layer " << i << " to match input size "
                        << currentActivations->size() << std::endl;
                }

                // Weighted sum: z = w·x + b
                for (size_t k = 0; k < currentActivations->size(); ++k) {
                    z += neuron->getWeights()[k] * (*currentActivations)[k];
                }
            }
            else {
                for (size_t k = 0; k < weights.size(); ++k) {
                    z += weights[k] * (*currentActivations)[k];
                }
            }
            // Save pre-activation for backprop
            neuron->setActivationPreReLU(z);

            // Apply activation: ReLU for hidden, identity for output
            float activation = (!isOutputLayer) ? std::max(0.0f, z) : z;
            neuron->setOutput(activation);
            layerOutputs[j] = activation;
        }

        // Apply softmax only at output layer
        if (isOutputLayer) {
            float maxLogit = *std::max_element(layerOutputs.begin(), layerOutputs.end());
            float sumExp = 0.f;

            for (float& val : layerOutputs) {
                val = std::exp(val - maxLogit);
                sumExp += val;
            }

            for (size_t j = 0; j < neuronCount; ++j) {
                float softmaxVal = layerOutputs[j] / sumExp;
                currentLayer->getNeuronList()[j]->setOutput(softmaxVal);
                layerOutputs[j] = softmaxVal;
            }
        }
        nextLayerInputs = layerOutputs;
        currentActivations = &nextLayerInputs;
    }
    output = *currentActivations;
    return output;
}

// Backpropagation using cross-entropy loss and gradient descent
void Network::backPropagation(std::pair<int, std::vector<float>> input) {
    int trueLabel = input.first;
    int numLayers = layerList.size();
    Layer* outputLayer = layerList[numLayers - 1];
    int outputSize = outputLayer->getNeuronCount();

    // Output layer: compute initial gradient (dL/dz) = predicted - target
    for (int i = 0; i < outputSize; ++i) {
        Neuron* neuron = outputLayer->getNeuronList()[i];
        float output = neuron->getOutput();
        float target = (i == trueLabel) ? 1.0f : 0.0f;
        float error = output - target;
        neuron->setGradient(error);
    }

    // Update weights/biases for output layer (if >1 layers)
    if (numLayers > 1) { 
        Layer* prevLayer = layerList[numLayers - 2];
        int prevLayerSize = prevLayer->getNeuronCount();
        for (int i = 0; i < outputSize; ++i) {
            Neuron* neuron = outputLayer->getNeuronList()[i];
            float gradient = neuron->getGradient();
            for (int j = 0; j < prevLayerSize; ++j) {
                float prevActivation = prevLayer->getNeuronList()[j]->getOutput();
                float dw = gradient * prevActivation;
                float w = neuron->getWeights()[j];
                float updatedW = w - learning_rate * dw;
                neuron->updateWeights(j, updatedW);
            }
            float db = gradient;
            float b = neuron->getBias();
            float updatedB = b - learning_rate * db;
            neuron->setBias(updatedB);
        }
    }

    // Special case: single-layer network
    else if (numLayers == 1) {
        for (int i = 0; i < outputSize; ++i) {
            Neuron* neuron = outputLayer->getNeuronList()[i];
            float gradient = neuron->getGradient();
            for (int j = 0; j < input.second.size(); ++j) {
                float x = input.second[j];
                float dw = gradient * x;
                float w = neuron->getWeights()[j];
                float updatedW = w - learning_rate * dw;
                neuron->updateWeights(j, updatedW);
            }
            float db = gradient;
            float b = neuron->getBias();
            float updatedB = b - learning_rate * db;
            neuron->setBias(updatedB);
        }
    }

    // Backpropagate through hidden layers
    for (int l = numLayers - 2; l >= 0; --l) {
        Layer* currentLayer = layerList[l];
        Layer* nextLayer = layerList[l + 1];
        int currentLayerSize = currentLayer->getNeuronCount();
        int nextLayerSize = nextLayer->getNeuronCount();
        for (int i = 0; i < currentLayerSize; ++i) {
            Neuron* neuron = currentLayer->getNeuronList()[i];

            // Accumulate error from next layer
            float preActivation = neuron->getActivationPreReLU();
            float errorSignal = 0.0f;
            for (int j = 0; j < nextLayerSize; ++j) {
                Neuron* nextNeuron = nextLayer->getNeuronList()[j];
                errorSignal += nextNeuron->getGradient() * nextNeuron->getWeights()[i];
            }

            // Derivative of ReLU
            float gradient = (preActivation > 0) ? errorSignal : 0.0f;
            neuron->setGradient(gradient);

            // Update weights and bias
            if (l > 0) {
                Layer* prevLayer = layerList[l - 1];
                int prevLayerSize = prevLayer->getNeuronCount();

                for (int j = 0; j < prevLayerSize; ++j) {
                    float prevActivation = prevLayer->getNeuronList()[j]->getOutput();
                    float dw = gradient * prevActivation;
                    float w = neuron->getWeights()[j];
                    float updatedW = w - learning_rate * dw;
                    neuron->updateWeights(j, updatedW);
                }
            }
            else {
                for (int j = 0; j < input.second.size(); ++j) {
                    float x = input.second[j];
                    float dw = gradient * x;
                    float w = neuron->getWeights()[j];
                    float updatedW = w - learning_rate * dw;
                    neuron->updateWeights(j, updatedW);
                }
            }
            float db = gradient;
            float b = neuron->getBias();
            float updatedB = b - learning_rate * db;
            neuron->setBias(updatedB);
        }
    }
}

int Network::getEpoch() {
    return epochs;
}

// Computes cross-entropy loss for softmax
float Network::computeLoss(int trueLabel, const std::vector<float>& prediction) {
    if (trueLabel < 0 || trueLabel >= static_cast<int>(prediction.size())) return 0.f;
    return -std::log(std::max(1e-6f, prediction[trueLabel]));
}

// Returns index of highest activation (argmax), i.e., predicted class
int Network::predict(std::vector<float>& out){
    if (out.empty()) { 
        std::cout << "Error while prediction!" << std::endl; 
        return -1; 
    }
    float max_elem = out.front();
    int max_index = 0;
    for (int i = 0; i < out.size(); i++) {
        if (max_elem >= out[i]) continue;
        max_elem = out[i];
        max_index = i;
    }
    return max_index;
}