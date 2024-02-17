#include <SFML/Graphics.hpp>
#include "imgui/imgui.h"
#include "imgui-sfml/imgui-SFML.h"
#include <iostream>


void handleEvents(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        ImGui::SFML::ProcessEvent(event);
        }}

void handleRectangleInteraction(sf::RenderWindow& window, sf::RectangleShape& rectangle) {
    static sf::Vector2f offset; // Static variable to track offset for dragging

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Check if the mouse is over the rectangle
                if (rectangle.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    // Calculate the offset from the top-left corner of the rectangle
                    offset = rectangle.getPosition() - sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                }
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                offset = sf::Vector2f(); // Reset the offset when releasing the mouse button
            }
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        // If the left mouse button is held down, update the position of the rectangle
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        if (offset != sf::Vector2f()) { // Check if dragging has started
            rectangle.setPosition(mousePosition.x + offset.x, mousePosition.y + offset.y);
        }
    }
}

void render(sf::RenderWindow& window, sf::RectangleShape& rectangle, sf::RectangleShape& floor) {
        window.clear();
        window.draw(rectangle);
        window.draw(floor);
        ImGui::SFML::Render(window);
        window.display();
}

void setupImGui(sf::RenderWindow& window, sf::RectangleShape& rectangle) {
    // Start the ImGui frame
    ImGui::SFML::Update(window, sf::seconds(1.0f / 60.0f));

    // Create a simple ImGui window
    ImGui::Begin("bHopper!");
    ImGui::Text("Change Player Color!");
    if (ImGui::Button("Magenta")) {
    rectangle.setFillColor(sf::Color::Magenta);
    }
    if (ImGui::Button("Blue")) {
    rectangle.setFillColor(sf::Color::Blue);
    }
    if (ImGui::Button("Cyan")) {
    rectangle.setFillColor(sf::Color::Cyan);
    }

    ImGui::End();
}

void gravity(sf::Vector2f& rectangleVelocity, sf::RectangleShape& rectangle) {
    const float Gravity = 0.5f;
    rectangleVelocity.y += Gravity;
    rectangle.move(rectangleVelocity);
}

bool collisionBottom(sf::RectangleShape& rectangle, sf::Vector2f& rectangleVelocity, sf::RectangleShape& floor) {
    sf::FloatRect playerBounds = rectangle.getGlobalBounds();
    sf::FloatRect floorBounds = floor.getGlobalBounds();

    // Check for collision between the bottom side of the player's rectangle and the top side of the floor
    if (playerBounds.top + playerBounds.height >= floorBounds.top && rectangleVelocity.y >= 0) {
        rectangle.setPosition(rectangle.getPosition().x, floorBounds.top - playerBounds.height);
        return true; // Collision detected
    }

    return false; // No collision
}




int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "could it be?", sf::Style::Default);
    window.setFramerateLimit(60);
    sf::RectangleShape rectangle;
    sf::Vector2f rectangleVelocity = {0.0f, 0.0f};
    rectangle.setSize(sf::Vector2f(200, 100)); // Set size (width, height)
    rectangle.setPosition(300, 200); // Set position (x, y)
    rectangle.setFillColor(sf::Color::Transparent); // Set fill color
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setOutlineThickness(2);

    sf::RectangleShape floor;
    floor.setSize(sf::Vector2f(5000, 50));
    floor.setPosition(0,600);
    floor.setFillColor(sf::Color::Transparent);
    floor.setOutlineColor(sf::Color::Blue);
    floor.setOutlineThickness(2);

    // Initialize ImGui
    ImGui::SFML::Init(window);



    while (window.isOpen()) {
        handleRectangleInteraction(window, rectangle);
        //gravity(rectangleVelocity, rectangle);
        handleEvents(window);
        setupImGui(window, rectangle); // Set up ImGui before rendering
        collisionBottom(rectangle, rectangleVelocity, floor);
        
        render(window, rectangle, floor);

        
        //sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        //std::cout << "Mouse Cursor Position: (" << mousePosition.x << ", " << mousePosition.y << ")" << std::endl;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            // Get the current position of the mouse cursor relative to the window
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

            // Output the mouse click position to the console
            std::cout << "Mouse Click Position: (" << mousePosition.x << ", " << mousePosition.y << ")" << std::endl;
        }
        

    }
    ImGui::SFML::Shutdown();
    return 0;
}


