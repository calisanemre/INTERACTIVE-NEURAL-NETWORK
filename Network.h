#pragma once
#include "Input.h"
#include "GUI.h"
#include "Layer.h"
#include <random>

// Represents a feedforward neural network connected to the GUI layer structure.
// This class handles weight initialization, forward pass, backpropagation, and prediction.
class Network
{
private:
	std::vector<Layer*> layerList; // Layer pointers representing the neural network structure
	float learning_rate; // Learning rate for gradient descent
	int epochs; // Number of training epochs
	GUI* window; // Reference to the GUI for layer information
	std::vector<float> output; // Output from the last forward pass

public: 
	Network(float learning_rate, int epochs, GUI* window); // Constructor
	std::vector<float> forwardPass(const std::pair<int, std::vector<float>>& input); // Performs forward propagation through all layers
	void backPropagation(std::pair<int, std::vector<float>> input); // Performs backpropagation using cross-entropy + softmax loss
	void initializeWeights(); // Randomly initializes weights of neurons based on layer structure
	int getEpoch(); // Get training epoch count
	float computeLoss(int trueLabel, const std::vector<float>& prediction); // Computes cross-entropy loss for classification
	int predict(std::vector<float>& out); // Returns predicted class index based on output vectors
};

