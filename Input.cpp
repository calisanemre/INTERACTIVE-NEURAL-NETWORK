#include "Input.h"

Input::Input() {
    // Initialize input area and grid layout
	shape.setSize(sf::Vector2f(280.f, 280.f));
	shape.setOutlineColor(sf::Color::Black);
	shape.setOutlineThickness(3.f); 
    // Create 28x28 white squares
    for (size_t i = 0; i < GRID_COUNT; ++i) {
        for (size_t j = 0; j < GRID_COUNT; ++j) {
            grid[i][j].setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            grid[i][j].setFillColor(sf::Color::White);
        }
    }
}
void Input::setPosition(GUI* window, float x, float inc) {
    // Center the input grid vertically
    float yPos = window->getSize().y/2 - shape.getSize().y/2 - inc;
	shape.setPosition(x, yPos);
    // Set grid cell positions
    for (size_t i = 0; i < GRID_COUNT; ++i) {
        for (size_t j = 0; j < GRID_COUNT; ++j) {
            grid[i][j].setPosition(shape.getPosition().x + i * CELL_SIZE, shape.getPosition().y + j * CELL_SIZE);
        }
    }
 }

void Input::draw(GUI& window) {
	window.draw(shape);
    this->drawGrid(window);
}

void Input::drawGrid(GUI& window) {
    // For each cell, calculate how much user drawing overlaps with it
    int row = 0, col = 0;
    auto tempGrid = grid;
    while (row < GRID_COUNT) {
        sf::FloatRect cellBound = grid[row][col].getGlobalBounds();
        float ratio = 0.f;
        for (auto& point : pointList) {
            sf::FloatRect pointBound = point.getBounds();
            if (cellBound.intersects(pointBound)) {
                // Calculate overlapping area and boost grayscale intensity
                float leftCoord = std::max(cellBound.left, pointBound.left);
                float rightCoord = std::min(cellBound.left + cellBound.width, pointBound.left + pointBound.width);
                float upCoord = std::max(cellBound.top, pointBound.top);
                float downCoord = std::min(cellBound.top + cellBound.height, pointBound.top + pointBound.height);
                float intersectArea = std::abs(rightCoord - leftCoord) * std::abs(upCoord - downCoord);
                ratio += 5.f * (intersectArea / (cellBound.height * cellBound.width));
            }
        }
        if (ratio > 0.f) {
            int intensity = 255.f - std::min(255.f, ratio * 255.f);
            grid[row][col].setFillColor(sf::Color(intensity, intensity, intensity));
        }
        window.draw(grid[row][col]);
        col++;
        if (col == GRID_COUNT) {
            col = 0;
            row++;
        }
    }
}

std::vector<sf::VertexArray> Input::takeInput(sf::Event& event, GUI& window) {
    // Handle user drawing input via mouse events
    static bool isDrawing = false;
    static bool hasDrawn = false;
    static sf::Vector2f lastPosition;

    // On mouse press: start drawing
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        for (auto& row : grid) for (auto& cell : row) cell.setFillColor(sf::Color::White);
        pointList.clear();
        isDrawing = true;
        hasDrawn = false;
        readyToPredict = false;
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        lastPosition = window.mapPixelToCoords(mouse_pos);
    }

    // While moving the mouse with left-click: record strokes
    if (event.type == sf::Event::MouseMoved && isDrawing) {
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (isDrawing && hasDrawn) {
                isDrawing = false;
                gridValues = this->getData();
                readyToPredict = true;
            }
            isDrawing = false;
            return pointList;
        }
        if (!canDrawable) {
            std::cout << "Work in progress!\n";
            isDrawing = false;
            return {};
        }

        // Ignore tiny mouse movements to avoid noise
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        sf::Vector2f mouse_world = window.mapPixelToCoords(mouse_pos);
        if (std::abs(mouse_world.x - lastPosition.x) < 1.0f &&
            std::abs(mouse_world.y - lastPosition.y) < 1.0f) {
            return pointList;
        }

        lastPosition = mouse_world;

        // If within grid bounds, draw a brush square
        if (shape.getGlobalBounds().contains(mouse_world)) {
            sf::VertexArray point(sf::Quads, 4);
            point[0] = sf::Vector2f(mouse_world.x - BRUSH_SIZE / 2.f, mouse_world.y - BRUSH_SIZE / 2.f);
            point[1] = sf::Vector2f(mouse_world.x + BRUSH_SIZE / 2.f, mouse_world.y - BRUSH_SIZE / 2.f);
            point[2] = sf::Vector2f(mouse_world.x + BRUSH_SIZE / 2.f, mouse_world.y + BRUSH_SIZE / 2.f);
            point[3] = sf::Vector2f(mouse_world.x - BRUSH_SIZE / 2.f, mouse_world.y + BRUSH_SIZE / 2.f);
            for (int i = 0; i < point.getVertexCount(); i++) {
                float top = abs(shape.getGlobalBounds().top - mouse_world.y);
                float bottom = abs(mouse_world.y - shape.getGlobalBounds().top - shape.getGlobalBounds().height);
                float left = abs(mouse_world.x - shape.getGlobalBounds().left);
                float right = abs(shape.getGlobalBounds().left + shape.getGlobalBounds().width - mouse_world.x);
                if (top < BRUSH_SIZE / 2) {
                    point[i].position.y = top;
                }
                if (bottom < BRUSH_SIZE / 2) {
                    point[i].position.y = bottom;
                }
                if (left < BRUSH_SIZE / 2) {
                    point[i].position.x = left;
                }
                if (right < BRUSH_SIZE / 2) {
                    point[i].position.x = right;
                }
                point[i].color = sf::Color::Transparent;
            }
            pointList.push_back(point);
            hasDrawn = true;
        }
    }

    // On mouse release, finalize drawing and set prediction flag
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        if (isDrawing) {
            isDrawing = false;
            if (hasDrawn) {
                gridValues = this->getData();
                readyToPredict = true;
            }
        }
    }
    return pointList;
}

void Input::showInGridArr(std::vector<float> arr) {
    // Display a digit from a vector of floats (e.g., dataset sample)
    int i = 0;
    for (size_t row = 0; row < GRID_COUNT; ++row) {
        for (size_t col = 0; col < GRID_COUNT; ++col) {
            int color = static_cast<int>((1.f - arr[i++]) * 255.f);
            grid[col][row].setFillColor(sf::Color(color, color, color));
        }
    }
}


std::vector<float> Input::getData() {
    // Convert grid pixels to float array in [0, 1] range (normalized inverse grayscale)
    std::vector<float> out(GRID_COUNT * GRID_COUNT);
    for (size_t col = 0; col < GRID_COUNT; ++col) {
        for (size_t row = 0; row < GRID_COUNT; ++row) {
            int index = row * GRID_COUNT + col;
            out[index] = static_cast<float>(255 - grid[col][row].getFillColor().r) / 255.f;
        }
    }
    return out;
}

// Utility: clears the grid and resets prediction status
void Input::clearGrid() {
    for (auto& row : grid) {
        for (auto& cell : row) {
            cell.setFillColor(sf::Color::White);
        }
    }
}

std::vector<float> Input::getGridValues() {
    return gridValues;
}

bool Input::shouldPredict() const {
    return readyToPredict;
}
void Input::resetPredictFlag() {
    readyToPredict = false;
}