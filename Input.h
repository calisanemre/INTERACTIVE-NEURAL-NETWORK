#pragma once
#include "GUI.h"
#include "Layer.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <array>

// Constants defining grid size and cell/brush dimensions
const float BRUSH_SIZE = 18.f;
const int GRID_COUNT = 28;
const float CELL_SIZE = 10.f;
class GUI;

// Input class handles the 28x28 grid drawing area where the user writes digits.
// It captures mouse strokes and provides the normalized grid values for prediction.
class Input
{
private: 
	sf::RectangleShape shape; // Outer container of the grid
	std::vector<sf::VertexArray> pointList; // Stores drawn strokes
	std::array<std::array<sf::RectangleShape, GRID_COUNT>, GRID_COUNT> grid; // 28x28 drawable cells
	std::vector<float> gridValues; // Normalized pixel values from user input
	bool canDrawable = true; // Drawing enabled flag
	bool readyToPredict = false; // Indicates if drawing is ready to be fed to the model

public:
	Input(); // Constructor
	void setPosition(GUI* window, float x = 50.f, float inc = 50.f); // Set position of the input grid in the GUI window
	void draw(GUI& window); // Draw grid outline and contents
	std::vector<sf::VertexArray> takeInput(sf::Event& event, GUI& window); // Capture strokes based on mouse events
	void drawGrid(GUI& window); // Calculating output from pointlist in order to set color intensity
	std::vector<float> getData(); // Get normalized data from current drawing
	void showInGridArr(std::vector<float> arr);// Display a digit from dataset as a grayscale grid
	void clearGrid(); // Reset grid to white
	std::vector<float> getGridValues(); // Get the last predicted input
	bool shouldPredict() const; // Whether a new prediction should be triggered
	void resetPredictFlag(); // Reset the prediction flag
}; 

