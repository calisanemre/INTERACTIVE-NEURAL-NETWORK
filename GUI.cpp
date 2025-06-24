#include "GUI.h"

// Constructor that initializes the window with given width (x), height (y), window title (name),
// and optionally sets initial button and layer counts
GUI::GUI(int x, int y, sf::String name, int b, int l) : sf::RenderWindow(sf::VideoMode(x, y), name) {
    this->setFramerateLimit(60);
    buttonCount = b;
    layerCount = l;
    Input* inp = new Input;
    input = inp;
    input->setPosition(this);
}

// Adds a button to the GUI's button list if the limit is not exceeded
void GUI::addButton(const Button& button) {
    if (buttonList.size() < MAX_BUTTONS) {
        buttonList.push_back(button);
        buttonCount++;
    }
    else {
        std::cout << "Max Buttons Reached!\n";
    }
}

// Repositions all layers horizontally with a fixed padding between them
void GUI::repositionLayers(int padding) {
    for (int i = 0; i < layerList.size(); i++) {
        Layer* layer = layerList[i];
        float xPos = this->getSize().x - (MAX_LAYERS - i) * (layer->getBounds().width + padding);
        layer->setPosition(xPos, 50);
        repositionNeurons(layer);
    }
}

// Adds a new Layer to the GUI if the limit is not reached; auto-positions it using padding
void GUI::addLayer(int padding) {
    if (layerCount < MAX_LAYERS) {
        Layer* layer = new Layer();
        layerList.push_back(layer);
        layerCount++;
        float xPos = this->getSize().x - (MAX_LAYERS - layerCount + 1) * (layer->getBounds().width + padding);
        layer->setPosition(xPos, 50);
    }
    else {
        std::cout << "Max Layers Reached!\n";
    }
}

// Draws all layer rectangles onto the window
void GUI::drawLayers() {
    for (auto& layer : layerList) {
        layer->draw(*this);
    }
}

// Deletes the given Layer and its associated neurons from the GUI
void GUI::deleteLayer(Layer* layer) {
    auto it = std::find(layerList.begin(), layerList.end(), layer);
    if (it != layerList.end()) {
        for (auto& neuron : layer->getNeuronList()) {
            delete neuron;
        }
        layer->getNeuronList().clear();
        delete* it;
        layerList.erase(it);
        layerCount--;
    }
}

// Returns a reference to the list of all layers
std::vector<Layer*>& GUI::getLayerList() {
    return layerList;
}

// Adds a neuron to the specified layer if the limit is not exceeded
void GUI::addNeuron(Layer* layer) {
    std::vector<Neuron*>& neuronList = layer->getNeuronList();
    if (neuronList.size() < MAX_NEURONS) {
        Neuron* neuron = new Neuron();
        neuronList.push_back(neuron);
        layer->setNeuronCount(1);
        repositionNeurons(layer);
    }
    else {
        std::cout << "Max Neurons Reached!\n";
    }
}

// Removes the given neuron from its layer and updates layout
void GUI::deleteNeuron(Layer* layer, Neuron* neuron) {
    auto it = std::find(layer->getNeuronList().begin(), layer->getNeuronList().end(), neuron);
    if (it != layer->getNeuronList().end() && !layer->getNeuronList().empty()) {
        delete* it;
        layer->getNeuronList().erase(it);
        layer->setNeuronCount(-1);
    }
}

// Draws all neurons within a specific layer
void GUI::drawNeurons(Layer* layer){
    for (auto& neuron : layer->getNeuronList()) {
        neuron->draw(*this);
    }
}

// Recalculates and repositions all neurons vertically within the layer
void GUI::repositionNeurons(Layer* layer) {
    auto& neuronList = layer->getNeuronList();
    if (neuronList.empty()) return;
    Neuron* sampleNeuron = neuronList[0];
    float neuronRadius = sampleNeuron->getRadius();
    float neuronDiameter = 2 * neuronRadius;
    float layerHeight = layer->getBounds().height;
    float maxTotalNeuronHeight = MAX_NEURONS * neuronDiameter;
    float availableSpace = layerHeight - maxTotalNeuronHeight ;
    float padding = availableSpace / (MAX_NEURONS + 1);

    for (int i = 0; i < neuronList.size(); i++) {
        Neuron* neuron = neuronList[i];

        float xPos = layer->getPosition().x + (layer->getBounds().width / 2);
        float yPos = layer->getPosition().y + padding * (i + 1) + (i * neuronDiameter);
        neuron->setPosition(xPos, yPos);
    }
}

// Returns a list of line segments connecting neurons between consecutive layers
std::vector <std::array<sf::Vertex, 2>> GUI::drawLines() {
    auto layerList = this->getLayerList();
    std::vector <std::array<sf::Vertex, 2>> lineList;
    auto it = layerList.begin();
    if (layerList.empty() || layerList.size() == 1) return lineList; 
    while (it != layerList.end()) {
        Layer* layer = *it;
        if (layer->getNeuronCount() == 0) { std::cout << "Make sure that every layer have at least one neuron inside!\n"; return lineList; }        
        ++it;
    }
    it = layerList.begin();
    Layer* prevLayer = *it;
    while (it !=layerList.end()) {
        Layer* currLayer = *it;
        if (it == layerList.begin()) {
            prevLayer = currLayer;
            ++it;
            continue;
        }

        for (auto& currNeuron : currLayer->getNeuronList()) {
            for (auto& prevNeuron : prevLayer->getNeuronList()) {
                std::array<sf::Vertex, 2> line = {
                    sf::Vertex(prevNeuron->getPosition()),
                    sf::Vertex(currNeuron->getPosition())
                };
                line.begin()->color = sf::Color::Black;
                line.back().color = sf::Color::Black;
                lineList.push_back(line);
            }
        }
        prevLayer = currLayer;
        ++it;
    }
    return lineList;
}

// Draws the input grid (used to receive user-drawn digits)
void GUI::drawInput() {
    input->draw(*this);
}

// Returns a pointer to the input grid 
Input* GUI::getInput() {
    return input;
}