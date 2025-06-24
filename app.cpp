#include "GUI.h"
#include "Button.h"
#include "Layer.h"
#include "Network.h"
#include <iostream>
#include <fstream>
#include <sstream>
/*
################################################################

After several test some of the ideal parameters in order to get best results:
-> Learning rate: 0.001

Number of epochs can increased but each epoch lasts for approximately 30 seconds to finish
(Model with maximum layer count (5) and maximum neuron count (58) lasts 38 seconds per epoch)

################################################################
*/

float learning_rate = 0.001f; // Learning rate for gradient descent
int epochs = 10; // Number of epochs for training

// This function initializes button positions and checks their pressed state
void initializeButtons(Button* buttonList[MAX_BUTTONS], GUI& window, sf::Event& event, int padding = 10) {
    int lastX = padding;
    sf::Vector2f windowSize = static_cast<sf::Vector2f>(window.getSize());
    for (int i = 0; i < MAX_BUTTONS; i++) {
        buttonList[i]->setPosition(lastX, windowSize.y - buttonList[i]->getBounds().height - padding);
        lastX += buttonList[i]->getBounds().width + padding;
        buttonList[i]->draw(window);
        buttonList[i]->isPressed(event, window);
    }
}

// This function loads the dataset from a CSV file and normalizes pixel values
std::vector<std::pair<int, std::vector<float>>> loadDataset(const std::string& filename) {
    std::vector<std::pair<int, std::vector<float>>> dataset;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error loading dataset " << filename << std::endl;
        return dataset;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<char> buffer(line.begin(), line.end());
        buffer.push_back('\0');
        char* next_token;
        const char* delim = ",";
        char* token = strtok_s(buffer.data(), delim, &next_token);
        if (!token) continue;
        int label = std::atoi(token);
        std::vector<float> pixels;
        token = strtok_s(nullptr, delim, &next_token);
        while (token != nullptr) {
            float value = std::atof(token) / 255.0f;
            pixels.push_back(value);
            token = strtok_s(nullptr, delim, &next_token);
        }
        if (pixels.size() == GRID_COUNT * GRID_COUNT) {
            dataset.emplace_back(label, pixels);
        }
    }

    return dataset;
}
bool neuron_flag = false; // Used to track if a neuron is selected
bool buildPressed = false; // True if build button was pressed
bool addNeuronPressed = false; // True if add neuron button was pressed
bool trainPressed = false; // True if train button was pressed
Layer* prevLayer = nullptr; // Tracks the previously selected layer
Network* network = nullptr; // Pointer to the neural network instance
bool trainingMode = false; // Indicates if the network is in training mode
int currentEpoch = 0; // Current epoch during training
int sampleIndex = 0; // Index of current training sample
std::vector<std::pair<int, std::vector<float>>> dataset; // Training or test dataset
static float epochLoss = 0.f; // Accumulates loss over one epoch

int main() {
    // Create the main application window
	GUI window(1000, 600, "Neural Network GUI");

    // Initialize buttons for GUI actions
    Button addLayerButton("Add Layer");
    Button addNeuronButton("Add Neuron");
    Button buildButton("Build");
    Button trainButton("Train");
    Button testButton("Test");

    // Button list for iteration
    Button *buttonList[MAX_BUTTONS] = {&addLayerButton, &addNeuronButton, &buildButton, &trainButton, &testButton};
    
    // Main application loop
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();

            // Layer addition
            if (addLayerButton.isPressed(event, window)) {
                window.addLayer();
            }            

            bool needToRestartLoop = false;
            auto& layerList = window.getLayerList();

            // Traverse layers to manage interactions
            for (auto it = layerList.begin(); it != layerList.end();) {
                if(needToRestartLoop) {
                    it = layerList.begin();
                    needToRestartLoop = false;
                    if (it == layerList.end()) break;
                }

                Layer* layer = *it;
                bool layer_flag = layer->checkActive();
                auto& neuronList = layer->getNeuronList();
                bool anyNeuronSelected = false;
                
                // Traverse neurons in layer
                if(!neuronList.empty()){
                    for (auto itt = neuronList.begin(); itt != neuronList.end();) {
                        Neuron* neuron = *itt;
                        neuron->isSelected(event, window);
                        neuron_flag = neuron->checkActive();

                        // Delete neuron with DELETE key
                        if (neuron_flag && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete) {
                            window.deleteNeuron(layer, neuron);
                            itt = neuronList.begin();
                            window.repositionNeurons(layer);
                            anyNeuronSelected = true;
                        }
                        else {
                            neuron_flag = neuron->checkActive();
                            ++itt;
                        }
                    }
                }

                // Handle layer selection
                layer->isSelected(event, window);
                if (layer_flag) {
                    if (prevLayer != nullptr && prevLayer != layer) {
                        if (prevLayer->checkActive()) {
                            prevLayer->setActive(false);
                        }
                    }
                    prevLayer = layer;
                }

                // Enforce output layer max 10 neurons
                if (layer == layerList.back() && layer->getNeuronCount() == 11) {
                    std::cout << "Last layer must have 10 neurons maximum\n";
                    window.deleteNeuron(layer, layer->getNeuronList().back());
                    needToRestartLoop = true;
                    continue;
                }

                // Delete layer if selected and DELETE key pressed
                if (layer_flag && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete && !anyNeuronSelected) {
                    prevLayer = nullptr;
                    window.deleteLayer(layer);
                    window.repositionLayers();
                    it = layerList.begin(); // restart iteration
                    needToRestartLoop = true;
                    continue;
                }
                else if (layer_flag && addNeuronButton.isPressed(event, window)) {
                    window.addNeuron(layer);
                    addNeuronPressed = true;
                }

                // Build the network if all layers are valid
                else if (buildButton.isPressed(event, window)) {
                    buildPressed = true;
                    if (!network) {
                        bool canBuild = true;
                        for (auto& layer : window.getLayerList()) {
                            if (layer->getNeuronList().empty()) {
                                std::cout << "Cannot build network! Each layer must have at least one neuron.\n";
                                canBuild = false;
                                break;
                            }
                        }
                        if (canBuild) {
                            network = new Network(learning_rate, epochs, &window);
                            std::cout << "Network created!" << std::endl;
                           
                        }
                    }
                }

                // Train the network
                else if (trainButton.isPressed(event, window)) {
                    if (!network) {
                        std::cout << "There is no built network!\n";
                    }
                    else {
                        dataset = loadDataset("assets/mnist_data_train.csv");
                        currentEpoch = 0;
                        sampleIndex = 0;
                        trainingMode = true;
                        std::cout << "Training started...\n";
                    }
                }

                // Test the network
                else if (testButton.isPressed(event, window)) {
                    if (!network) std::cout << "There is no built network!\n";
                    else {
                        int score = 0;
                        dataset = loadDataset("assets/mnist_data_test.csv");
                        for (auto& elem : dataset) {
                            auto out = network->forwardPass(elem);
                            int predictedLbl = network->predict(out);
                            if (predictedLbl == elem.first) score += 1;
                        }
                        float accuracy = static_cast<float>(score) / dataset.size() * 100.f;
                        std::cout << "Accuracy: " << accuracy << "%" << std::endl;
                    }
                }
                ++it;
                
            }
        }


        window.clear(sf::Color::White); // Clear the window with white background
        
        // Training loop
        if (trainingMode && !dataset.empty()) {
            if (currentEpoch < network->getEpoch()) {
                if (trainingMode && sampleIndex < dataset.size()) {
                    auto currentSample = dataset[sampleIndex];
                    auto prediction = network->forwardPass(currentSample);
                    epochLoss += network->computeLoss(currentSample.first, prediction);
                    network->backPropagation(currentSample);
                    ++sampleIndex;
                }
                else if (trainingMode) {

                    std::cout << "Epoch " << currentEpoch + 1 << " completed. Loss: " << epochLoss / dataset.size() << std::endl;
                    sampleIndex = 0;
                    ++currentEpoch;
                    epochLoss = 0.0f;
                    std::random_shuffle(dataset.begin(), dataset.end());
                    if (currentEpoch >= network->getEpoch()) {
                        trainingMode = false;
                        std::cout << "Training finished.\n";
                        window.getInput()->clearGrid();
                        window.drawInput();
                    }
                }
            }
            else {
                trainingMode = false;
                std::cout << "Training finished.\n";
                window.getInput()->clearGrid();
                window.drawInput();
            }
        }

        initializeButtons(buttonList, window, event); // Update buttons 
        window.drawLayers(); // Draw layers

        if (trainingMode && sampleIndex < dataset.size())
            window.getInput()->showInGridArr(dataset[sampleIndex].second); // Show sample image

        for (auto& layer : window.getLayerList()) {
            window.drawNeurons(layer); // Draw neurons
        }

        // Reset network if user edits after building
        if (buildPressed) {
            if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete) || addNeuronPressed) {
                buildPressed = !buildPressed;
                if (network) {
                    delete network;
                    network = nullptr;
                    std::cout << "Network reset please create another one." << std::endl;
                }
            }

            // Draw connections
            if (!window.drawLines().empty()) {
                for (auto& line : window.drawLines()) {
                    window.draw(line.data(), 2, sf::Lines);
                }
            }
            else {
                buildPressed = !buildPressed;
            }
        }


        window.drawInput(); // Draw the input grid
        std::vector<sf::VertexArray> inputPoints = window.getInput()->takeInput(event, window);
        
        // Prediction from grid input
        if (window.getInput()->shouldPredict()) {
            if (network) {
                std::vector<float> input = window.getInput()->getGridValues();
                if (!input.empty() && input.size() == GRID_COUNT * GRID_COUNT) {
                    std::pair<int, std::vector<float>> sampleData = { -1, input };
                    auto prediction = network->forwardPass(sampleData);
                    int predictedDigit = network->predict(prediction);
                    std::cout << "Predicted digit: " << predictedDigit << std::endl;
                    std::cout << "Confidence scores:" << std::endl;
                    for (int i = 0; i < prediction.size(); i++) {
                        std::cout << "  " << i << ": " << prediction[i] * 100.0f << "%" << std::endl;
                    }
                }
                else {
                    std::cout << "Debug: Invalid input size: " << input.size() << " (expected " << GRID_COUNT * GRID_COUNT << ")" << std::endl;
                }
                window.getInput()->resetPredictFlag();
            }
            else {
                std::cout << "Debug: Network doesn't exist, can't predict\n";
            }
        }
        
        for (auto& point : window.getInput()->takeInput(event, window)) {
            if (window.getInput()->shouldPredict() && network) {
                std::vector<float> input = window.getInput()->getGridValues();
                if (!input.empty() && input.size() == GRID_COUNT * GRID_COUNT) {
                    std::pair<int, std::vector<float>> sampleData = { -1, input };
                    auto prediction = network->forwardPass(sampleData);
                    int predictedDigit = network->predict(prediction);
                    std::cout << "Predicted digit: " << predictedDigit << std::endl;
                    std::cout << "Confidence scores:" << std::endl;
                    for (int i = 0; i < prediction.size(); i++) {
                        std::cout << "  " << i << ": " << prediction[i] * 100.0f << "%" << std::endl;
                    }
                }
                else {
                    std::cout << "Invalid input size: " << input.size() << " (expected " << GRID_COUNT * GRID_COUNT << ")" << std::endl;
                }
                window.getInput()->resetPredictFlag(); 
            }
        }
        window.display(); // Update the window
        addNeuronPressed= false; // Reset flag
    }
	return 0;
}
