#include "Neuron.h"
Neuron::Neuron(): bias(0.0f), output(0.0f), gradient(0.0f) {
	
    // Initialize the visual appearance of the neuron
    shape.setRadius(8.f);
    shape.setOrigin(shape.getPosition().x + shape.getRadius(), shape.getPosition().y + shape.getRadius());
	shape.setFillColor(sf::Color::Green);
	shape.setOutlineColor(sf::Color::Red);
	shape.setOutlineThickness(0.f); // Outline shown when active
}
sf::FloatRect Neuron::getBounds() {
	return shape.getGlobalBounds(); // Return clickable bounds
}
float Neuron::getRadius() {
    return shape.getRadius();
}
bool Neuron::isSelected(sf::Event& event, GUI& window) {

    // Handle mouse interaction to toggle selection
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !wasPressed) {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        sf::Vector2f mouse_world = window.mapPixelToCoords(mouse_pos);
        if (this->getBounds().contains(mouse_world)) {
            isActive = !isActive;
            wasPressed = true;
            if (isActive) {
                this->setActive(isActive); // Update visual outline
                return true;
            }
            else {
                this->setActive(isActive);
                return false;
            }
        }
    }
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        wasPressed = false;
    }
}
void Neuron::setPosition(float x, float y) {
    // Place the neuron shape (circle) on the screen
    float xPos = x + shape.getRadius();
    float yPos = y + shape.getRadius();
    shape.setPosition(x, y);
}
void Neuron::draw(sf::RenderWindow& window) {
    window.draw(shape); // Render neuron to the screen
}
bool Neuron::checkActive() {
    return isActive;
}
sf::Vector2f Neuron::getPosition() {
    return shape.getPosition();
}

void Neuron::setActive(bool value) {
    isActive = value;
    shape.setOutlineThickness(value ? 5.f : 0.f); // Toggle outline
}

float Neuron::activationFunc(float input) {
    // ReLU activation function
    return std::max(0.0f, input);
}

void Neuron::initializeWeights(int inputSize) {
    // He initialization for ReLU networks
    std::default_random_engine generator(std::random_device{}());
    float stddev = std::sqrt(2.0f / static_cast<float>(inputSize));
    std::normal_distribution<float> distribution(0.0f, stddev);

    weights.resize(inputSize);
    for (size_t i = 0; i < inputSize; ++i) {
        weights[i] = distribution(generator); // Random weight per input
    }

    bias = 0.0f;
}

float Neuron::run(const std::vector<float>& inputs, bool useReLU = true) {
    // Forward pass: weighted sum + activation
    float sum = bias;
    for (size_t i = 0; i < inputs.size(); ++i) {
        sum += inputs[i] * weights[i];
    }

    if (useReLU) {
        output = activationFunc(sum); 
    }
    else {
        output = sum; // Identity function(used for softmax pre - activation)
    }
    return output;
}

float Neuron::getBias() const {
    return bias;
}

void Neuron::setBias(float b) {
    bias = b;
}

void Neuron::updateWeights(int i, float newVal) {
    // Update specific weight (used in backprop)
    weights[i] = newVal;
}

float Neuron::getOutput() {
    return output;
}

void Neuron::setGradient(float f) {
    gradient = f;
}

std::vector<float> Neuron::getWeights() {
    return weights;
}

float Neuron::getGradient() {
    return gradient;
}

void Neuron::setActivationPreReLU(float value) { 
    activationPreReLU = value;
}
float Neuron::getActivationPreReLU() const { 
    return activationPreReLU; 
}
void Neuron::setOutput(float out) {
    output = out;
}