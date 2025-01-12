#include <SFML/Graphics.hpp>
#include <math.h>
#include <vector>
#include <iostream>
#include "../include/positionControl.h"

// Fonction pour générer une courbe à partir des points fournis
void plotCurve(const std::vector<float>& points, float timeStep) {
    const int width = 800;  // Largeur de la fenêtre
    const int height = 600; // Hauteur de la fenêtre

    // Créer une fenêtre SFML
    sf::RenderWindow window(sf::VideoMode(width, height), "Courbe");

    // Définir les axes
    sf::VertexArray axes(sf::Lines, 4);
    axes[0].position = sf::Vector2f(50, height - 50); // Origine (axe des X)
    axes[1].position = sf::Vector2f(width - 50, height - 50);
    axes[2].position = sf::Vector2f(50, height - 50); // Origine (axe des Y)
    axes[3].position = sf::Vector2f(50, 50);

    // Ajouter des couleurs aux axes
    for (int i = 0; i < 4; ++i) {
        axes[i].color = sf::Color::White;
    }

    // Échelle pour afficher les points correctement
    float xScale = (width - 100) / (timeStep * (points.size() - 1));
    float yScale = (height - 100) / (*std::max_element(points.begin(), points.end()));

    // Créer les points de la courbe
    sf::VertexArray curve(sf::LineStrip, points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        float x = 50 + i * xScale;
        float y = height - 50 - points[i] * yScale;
        curve[i].position = sf::Vector2f(x, y);
        curve[i].color = sf::Color::Green;
    }

    // Boucle principale
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Effacer l'écran
        window.clear(sf::Color::Black);

        // Dessiner les axes et la courbe
        window.draw(axes);
        window.draw(curve);

        // Afficher le contenu de la fenêtre
        window.display();
    }
}

int main() {
    // Exemple de points : une sinusoïde
    std::vector<float> points;
    float timeStep = 0.1f; // Pas de temps entre les points

    positionControl positionTest;
    positionTest.reset(10);

    for (int i = 0; i < 1000; ++i) {
        points.push_back(50 * sin(i * timeStep)); // Exemple avec une amplitude de 50
    }

    // Afficher la courbe
    plotCurve(points, timeStep);

    return 0;
}
