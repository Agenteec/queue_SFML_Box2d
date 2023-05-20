#include <iostream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
using namespace std;
#define Grav 10.f
float PTM_RATIO = 30.f;
float BlockSize = 50.f;
sf::Font font;
void Render(sf::RenderWindow* window);
template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        b2Body* body;
        sf::Color color;
        Node(const T& data) : data(data), next(nullptr), body(nullptr),color(sf::Color::Green){}
    };
    Node* front;
    Node* rear;
    size_t cou;

public:
    b2World* scene;
    Queue() : front(nullptr), rear(nullptr), cou(0){
        // Create the Box2D world
        b2Vec2 gravity(0.f, Grav);
        bool doSleep = true;
        scene = new b2World(gravity);
        scene->SetAllowSleeping(doSleep);
    }
    ~Queue() {
        while (front != nullptr) {
            Node* temp = front;
            front = front->next;
            delete temp;
        }
        rear = nullptr;
        delete scene;
    }
    void enqueue(const T& data) {
        Node* newNode = new Node(data);
        if (isEmpty()) {
            front = rear = newNode;
        }
        else {
            rear->next = newNode;
            rear = newNode;
        }
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(500.f,500.f -cou*50.0f-10.f);
        b2PolygonShape shape;
        shape.SetAsBox(BlockSize/2, BlockSize / 2);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 1.f;
        fixtureDef.friction = 0.3f;
        fixtureDef.restitution = 0.5f;
        newNode->body = scene->CreateBody(&bodyDef);
        newNode->body->CreateFixture(&fixtureDef);
        newNode->color = sf::Color((rand() % (255 - 0 + 1) + 0), (rand() % (255 - 0 + 1) + 0), (rand() % (255 - 0 + 1) + 0));
        cou++;
    }

    T dequeue() {
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty");
            
        }
        Node* temp = front;
        T data = temp->data;
        front = front->next;
        if (front == nullptr) {
            rear = nullptr;
        }
        scene->DestroyBody(temp->body);
        delete temp;

        cou--;
        return data;
    }

    bool isEmpty() const {
        return front == nullptr;
    }

    void printQueue() const {
        Node* current = front;
        while (current != nullptr) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }

    void merge(Queue& other) {
        if (other.isEmpty()) {
            return;
        }
        if (isEmpty()) {
            front = other.front;
            rear = other.rear;
            other.front = nullptr;
            other.rear = nullptr;
            return;
        }
        Node* current1 = front;
        Node* current2 = other.front;
        Node* prev = nullptr;
        while (current1 != nullptr && current2 != nullptr) {
            if (current2->data < current1->data) {
                Node* temp = current2->next;
                current2->next = current1;
                if (prev == nullptr) {
                    front = current2;
                }
                else {
                    prev->next = current2;
                }
                prev = current2;
                current2 = temp;
            }
            else {
                prev = current1;
                current1 = current1->next;
            }
        }
        if (current2 != nullptr) {
            prev->next = current2;
            rear = other.rear;
        }
        other.front = nullptr;
        other.rear = nullptr;
    }

    void draw(sf::RenderWindow* window)
    {
        Node* current = front;
        while (current != nullptr) {

            b2Vec2 position = current->body->GetPosition();
            T value = current->data;
            // получаем размеры тела Box2D (прямоугольника)
            sf::RectangleShape rect(sf::Vector2f(50,50));
            rect.setPosition(position.x, position.y);
            rect.setOrigin(rect.getSize().x/2, rect.getSize().y / 2);
            int randomNumber = rand() % (255 - 0 + 1) + 0;
            rect.setFillColor(current->color);


            sf::Text text(std::to_string(value), font, 14);
            text.setPosition(rect.getPosition().x-4,rect.getPosition().y);


            window->draw(rect);
            window->draw(text);

            current = current->next;
        }
    }


    Queue operator+(Queue& other) {
        Queue result = *this;
        result.merge(other);
        return result;
    }
};
int main()
{
    font.loadFromFile("arial.ttf");
    sf::RenderWindow* window;
    window = new sf::RenderWindow(sf::VideoMode(1024, 800), "Queue", sf::Style::Close);
    window->setFramerateLimit(120);
    Render(window);
    return 0;
}
void Render(sf::RenderWindow* window)
{

    srand(static_cast<unsigned int>(time(nullptr)));

    int min = 0;
    int max = 1000;

    Queue<int> queue;
    Queue<int> queue2;
    for (int i = 0; i < 5; i++)
    {
        int randomNumber = rand() % (max - min + 1) + min;
        queue.enqueue(randomNumber);
    }
    for (int i = 0; i < 4; i++)
    {
        int randomNumber = rand() % (max - min + 1) + min;
        queue2.enqueue(randomNumber);
    }
    //queue = queue + queue2;
    b2BodyDef wallBodyDef1;
    wallBodyDef1.position.Set(0.0f, window->getSize().y);
    b2BodyDef wallBodyDef2;
    wallBodyDef2.position.Set(450.0f, 0);
    b2BodyDef wallBodyDef3;
    wallBodyDef3.position.Set(550.0f, 0);

    b2PolygonShape wallShape1;
    wallShape1.SetAsBox(window->getSize().x*100, 1.0f);
    b2FixtureDef wallFixtureDef1;
    wallFixtureDef1.shape = &wallShape1;
    wallFixtureDef1.density = 0.0f; 
    wallFixtureDef1.friction = 0.0f;
    wallFixtureDef1.restitution = 0.5f;
    b2PolygonShape wallShape2;
    wallShape2.SetAsBox(1.f, window->getSize().y*100);
    b2FixtureDef wallFixtureDef2;
    wallFixtureDef2.shape = &wallShape2;
    wallFixtureDef2.density = 0.0f;
    wallFixtureDef2.friction = 0.0f;
    wallFixtureDef2.restitution = 0.5f;
    b2PolygonShape wallShape3;
    wallShape3.SetAsBox(1.f, window->getSize().y * 100);
    b2FixtureDef wallFixtureDef3;
    wallFixtureDef3.shape = &wallShape3;
    wallFixtureDef3.density = 0.0f;
    wallFixtureDef3.friction = 0.0f;
    wallFixtureDef3.restitution = 0.5f;
    b2Body* wallBody1 = queue.scene->CreateBody(&wallBodyDef1);
    b2Body* wallBody2 = queue.scene->CreateBody(&wallBodyDef2);
    b2Body* wallBody3 = queue.scene->CreateBody(&wallBodyDef3);
    wallBody1->CreateFixture(&wallFixtureDef1);
    wallBody2->CreateFixture(&wallFixtureDef2);
    wallBody3->CreateFixture(&wallFixtureDef3);
    sf::RectangleShape buttonAdd(sf::Vector2f(50, 50));
    buttonAdd.setPosition(10, 10);
    buttonAdd.setFillColor(sf::Color::Green);
    sf::RectangleShape buttonDel(sf::Vector2f(50, 50));
    buttonDel.setPosition(65, 10);
    buttonDel.setFillColor(sf::Color::Red);
#pragma region Render
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {

            switch (event.type)
            {
            case sf::Event::Closed:
                window->close();
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left &&
                    buttonAdd.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    // кнопка была нажата
                    buttonAdd.setFillColor(sf::Color::Yellow);
                }
                if (event.mouseButton.button == sf::Mouse::Left &&
                    buttonDel.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    buttonDel.setFillColor(sf::Color::Yellow);
                }
                break;

            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left &&
                    buttonAdd.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    int randomNumber = rand() % (max - min + 1) + min;
                    queue.enqueue(randomNumber);
                    buttonAdd.setFillColor(sf::Color::Green);
                }
                if (event.mouseButton.button == sf::Mouse::Left &&
                    buttonDel.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                {
                    if(!queue.isEmpty())
                    queue.dequeue();
                    buttonDel.setFillColor(sf::Color::Red);
                }
                break;

            default:
                break;
            }
        }
        window->clear(sf::Color(66, 135, 245));
        queue.draw(window);
        queue.scene->Step(1.f / 60.0f, 8, 1);
        window->draw(buttonAdd);
        window->draw(buttonDel);
        window->display();
    }
#pragma endregion
}