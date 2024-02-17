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
    }
}

void handleRectangleInteraction(sf::RenderWindow& window, sf::RectangleShape& rectangle) {
    static sf::Vector2f offset; // Static variable to track offset for dragging
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Convert mouse position to world coordinates
                sf::Vector2f worldMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                // Check if the mouse is over the rectangle
                if (rectangle.getGlobalBounds().contains(worldMousePos)) {
                    // Calculate the offset from the top-left corner of the rectangle
                    offset = rectangle.getPosition() - worldMousePos;
                }
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                offset = sf::Vector2f(); // Reset the offset when releasing the mouse button
            }
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        // Convert mouse position to world coordinates
        sf::Vector2f worldMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        // If the left mouse button is held down, update the position of the rectangle
        if (offset != sf::Vector2f()) { // Check if dragging has started
            rectangle.setPosition(worldMousePos + offset);
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

    ImGui::Text("Change Player Color:");

    // Color picker for the rectangle fill color
    static ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Initial color (white)
    if (ImGui::ColorEdit3("Color", (float*)&color)) {
        rectangle.setFillColor(sf::Color(color.x * 255, color.y * 255, color.z * 255)); // Convert ImGui color to SFML color
    }

    ImGui::End();
}

void cameraView(sf::RenderWindow& window, sf::RectangleShape& rectangle, sf::View& view) {

    view.reset(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    float x = rectangle.getPosition().x;

    view.setCenter(x, view.getCenter().y);

    window.setView(view);
}


/*void gravity(sf::Vector2f& rectangleVelocity, sf::RectangleShape& rectangle) {
    const float Gravity = 0.5f;
    rectangleVelocity.y += Gravity;
    rectangle.move(rectangleVelocity);
}*/

void gravity(sf::Vector2f& rectangleVelocity, sf::RectangleShape& rectangle, float& dtSeconds) {
    const float Gravity = 2000.0f; // Adjust this value to control the strength of gravity
    rectangleVelocity.y += Gravity * dtSeconds;
    rectangle.move(rectangleVelocity * dtSeconds);
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



/*void controller(sf::Vector2f& rectangleVelocity, sf::RectangleShape& rectangle) {

    float moveSpeed = 0.15f;
    rectangle.move(rectangleVelocity);
    rectangleVelocity *= 0.95f;

    // Set threshold for velocity
    const float velocityThreshold = 0.1f;
    if (std::abs(rectangleVelocity.x) < velocityThreshold) {
        rectangleVelocity.x = 0.0f;
    }

    //Move the Player Horizontally
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        // Move left
        rectangleVelocity.x -= moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        // Move right
        rectangleVelocity.x += moveSpeed; 
    }

    if (rectangleVelocity.x != 0) {
        std::cout << rectangleVelocity.x << std::endl;
    }
}*/


void controller(sf::Vector2f& rectangleVelocity, sf::RectangleShape& rectangle, float dtSeconds) {
    // Adjust movement speed based on delta time
    float moveSpeed = 800.0f * dtSeconds; // Adjust this value for faster movement

    // Apply movement
    rectangle.move(rectangleVelocity * dtSeconds);

    // Apply friction
    rectangleVelocity *= 0.9f;

    // Set threshold for velocity
    const float velocityThreshold = 0.1f;
    if (std::abs(rectangleVelocity.x) < velocityThreshold) {
        rectangleVelocity.x = 0.0f;
    }

    // Handle player input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        // Move left
        rectangleVelocity.x -= moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        // Move right
        rectangleVelocity.x += moveSpeed; 
    }

    // Debug output
    if (rectangleVelocity.x != 0) {
        std::cout << rectangleVelocity.x << std::endl;
    }
}









int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "could it be?", sf::Style::Default);
    sf::View view;
    window.setFramerateLimit(60);
    sf::RectangleShape rectangle;
    sf::Vector2f rectangleVelocity = {0.0f, 0.0f};
    rectangle.setSize(sf::Vector2f(50, 50)); // Set size (width, height)
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

    // deltaTime
    sf::Clock clock;



    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart(); // Restart the clock and get the time elapsed since the last restart
        float dtSeconds = deltaTime.asSeconds(); // Convert the delta time to seconds (float)

        cameraView(window, rectangle, view);

        controller(rectangleVelocity, rectangle, dtSeconds);
        handleRectangleInteraction(window, rectangle);
        gravity(rectangleVelocity, rectangle, dtSeconds);
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
            std::cout << dtSeconds << std::endl;
        }
        

    }
    ImGui::SFML::Shutdown();
    return 0;
}


