#include <SFML/Graphics.hpp>
#include<iostream>
#include<vector>
#include"map.h"
#include"view.h"
#include<list>

using namespace sf;

class Entity {
public:
    float dx, dy, x, y, speed;
    int w, h;
    bool life, isMove;
    Texture texture;
    Sprite sprite;
    String name;
    Entity(Image& image, String Name, float X, float Y, int W, int H) {
        x = X; y = Y; w = W; h = H; name = Name;
        speed = 0; dx = 0; dy = 0;
        life = true; isMove = false;
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        sprite.setOrigin(w / 2, h / 2);
    }

    virtual void update(float time) = 0;

};

class Player :public Entity {
public:
    enum { left, right, up, down } state;
    bool isShoot = false;

    Player(Image& image, String Name, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
        state = up;
        if (name == "Player") {
            sprite.setTextureRect(IntRect(0, 0, w, h));
            sprite.setColor(Color(0, 128, 0));
        }
    }

    void control() {
        if (Keyboard::isKeyPressed) {
            if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) {
                state = left; speed = 1; sprite.setTextureRect(IntRect(96, 0, w, h));
            }
            if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) {
                state = right; speed = 1; sprite.setTextureRect(IntRect(32, 0, w, h));
            }

            if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) {
                state = up; speed = 1;  sprite.setTextureRect(IntRect(0, 0, w, h));
            }

            if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) {
                state = down; speed = 1; sprite.setTextureRect(IntRect(64, 0, w, h));
            }
            if (Keyboard::isKeyPressed(Keyboard::Space)) {
                isShoot = true;
            }
        }
    }

    void checkCollisionWithMap(float Dx, float Dy)
    {
        for (int i = y / 64; i < (y + h) / 64; i++)
            for (int j = x / 64; j < (x + w) / 64; j++)
            {
                if (TileMap[i][j] == '0')
                {
                    if (Dy > 0) { y = i * 64 - h; }
                    if (Dy < 0) { y = i * 64 + 64; }
                    if (Dx > 0) { x = j * 64 - w; }
                    if (Dx < 0) { x = j * 64 + 64; }
                }
                if (TileMap[i][j] == '1')
                {
                    speed = 0.3;
                }
            }
    }

    void update(float time)
    {
        control();
        checkCollisionWithMap(dx, 0);
        switch (state)
        {
        case right:dx = speed; dy = 0;  break;
        case left:dx = -speed; dy = 0;  break;
        case up: dy = -speed;  dx = 0;  break;
        case down: dy = speed; dx = 0;  break;
        }
        x += dx;
        y += dy;
        checkCollisionWithMap(0, dy);
        sprite.setPosition(x + 16, y + 16);
        if (!isMove) { speed = 0; }
        if (life) { getPlayerCoordinateForView(x, y); }
    }
};

class Bullet :public Entity {
public:
    int direction;

    Bullet(Image& image, String Name, float X, float Y, int W, int H, int dir) :Entity(image, Name, X, Y, W, H) {
        x = X;
        y = Y;
        direction = dir;
        speed = 3;
        life = true;
        if (Name == "Bullet")
        {
            sprite.setTextureRect(IntRect(0, 0, 8, 8));
        }
        switch (dir)
        {
        case 0: x += w / 2 + 12; y += h / 2 + 13; break;
        case 1: x += w / 2 + 12; y += h / 2 + 13; break;
        case 2: x += h / 2 + 12; y += h / 2 + 12; break;
        case 3: x += h / 2 + 12; y += h / 2 + 12; break;
        }

    }

    void checkCollisionWithMap(float Dx, float Dy)
    {
        for (int i = (y - 12) / 64; i < y / 64; i++)
            for (int j = (x - 12) / 64; j < x / 64; j++)
            {
                if (TileMap[i][j] == '0')
                {
                    life = false;
                }
            }
    }

    void update(float time)
    {
        switch (direction)
        {
        case 0: dx = -speed; dy = 0;   break;
        case 1: dx = speed; dy = 0;    break;
        case 2: dx = 0; dy = -speed;   break;
        case 3: dx = 0; dy = speed;   break;
        }
        x += dx;
        checkCollisionWithMap(x, 0);
        y += dy;
        checkCollisionWithMap(0, y);
        sprite.setPosition(x, y);
    }
};


int main()
{
    RenderWindow window(VideoMode(800, 800), "Tanks");
    view.reset(FloatRect(0, 0, 640, 480));

    Image playerImage;
    playerImage.loadFromFile("Tanks.png");
    playerImage.createMaskFromColor(Color(0, 0, 0));
    Player p(playerImage, "Player", 1200, 1200, 32, 32);

    Image bulletImage;
    bulletImage.loadFromFile("Bullet.png");
    bulletImage.createMaskFromColor(Color(255, 255, 255));

    Texture maptexture;
    maptexture.loadFromFile("Map.png");
    Sprite mapsprite;
    mapsprite.setTexture(maptexture);

    std::list<Entity*> entities;
    std::list<Entity*>::iterator it;

    Clock clock;
    float reloadTime = 0;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time = time / 500;
        reloadTime += time;

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if ((p.isShoot == true) && (reloadTime > 1000)) { reloadTime = 0; entities.push_back(new Bullet(bulletImage, "Bullet", p.x, p.y, p.w, p.h, p.state)); }
            p.isShoot = false;
        }

        for (it = entities.begin(); it != entities.end();)
        {
            Entity* b = *it;
            b->update(time);
            if (b->life == false) { it = entities.erase(it); delete b; }
            else it++;

        }
        p.update(time);
        window.setView(view);
        window.clear();

        for (int i = 0; i < H_MAP; i++)
        {
            for (int j = 0; j < W_MAP; j++)
            {
                if (TileMap[i][j] == '0') { mapsprite.setTextureRect(IntRect(0, 448, 64, 64)); }
                if (TileMap[i][j] == ' ') { mapsprite.setTextureRect(IntRect(0, 0, 64, 64)); }
                if (TileMap[i][j] == 's') { mapsprite.setTextureRect(IntRect(0, 64, 64, 64)); }
                if (TileMap[i][j] == 'q') { mapsprite.setTextureRect(IntRect(0, 128, 64, 64)); }
                if (TileMap[i][j] == 'w') { mapsprite.setTextureRect(IntRect(0, 192, 64, 64)); }
                if (TileMap[i][j] == 'e') { mapsprite.setTextureRect(IntRect(0, 256, 64, 64)); }
                if (TileMap[i][j] == 'r') { mapsprite.setTextureRect(IntRect(0, 320, 64, 64)); }
                if (TileMap[i][j] == 't') { mapsprite.setTextureRect(IntRect(0, 384, 64, 64)); }
                if (TileMap[i][j] == '1') { mapsprite.setTextureRect(IntRect(0, 512, 64, 64)); }
                mapsprite.setPosition(j * 64, i * 64);
                window.draw(mapsprite);
            }
        }
        for (it = entities.begin(); it != entities.end(); it++)
        {
            window.draw((*it)->sprite);
        }
        window.draw(p.sprite);
        window.display();
    }
    return 0;
}