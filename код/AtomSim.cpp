#include <iostream>
#include <SFML/Graphics.hpp>
#include <list>
#include <random>

int w = sf::VideoMode::getDesktopMode().width;
int h = sf::VideoMode::getDesktopMode().height;

sf::RenderWindow window(sf::VideoMode(w, h), "molnya", sf::Style::Fullscreen);

struct atom
{
public:
    float x;
    float y;
    float vx = 0;
    float vy = 0;

    int type;//тип
};
std::list<atom> atms;//атом лист

void start(int types, int n, int a, int b)
{
    //types - типы кол-во
    //n - кол-во частиц
    //a, b - ширина и длина прямоугольника где появляются частицы

    atom atm;

    for (int i = 0; i < n; i++)
    {
        atm.x = ((float)rand() / (float)RAND_MAX - 0.5) * a;
        atm.y = ((float)rand() / (float)RAND_MAX - 0.5) * b;
        atm.type = int((float)rand() / (float)RAND_MAX * types);

        atms.push_back(atm);
    };
};
void render(float rd, int** colmat, int types)
{
    sf::CircleShape atm_r;
    atm_r.setRadius(rd);

    window.clear();
    for (auto atm : atms)
    {
        atm_r.setFillColor(sf::Color::Color(colmat[atm.type][0], colmat[atm.type][1], colmat[atm.type][2]));

        atm_r.setPosition(sf::Vector2f(atm.x + w / 2, atm.y + h / 2));

        window.draw(atm_r);
    }
    window.display();
};
void itt(float* ms, int types, float* qs, float k, float dt, float a, float b)
{
    float r;

    atom atm;
    for (auto atmz = std::begin(atms); atmz != std::end(atms); atmz++)
    {
        atm = *atmz;
        for (auto atm2 : atms)
        {
            //отталкивание
            if (atm2.x != atm.x || atm2.y != atm.y)
            {
                r = (k * qs[atm.type] * qs[atm2.type]) / (ms[atm.type] * (pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)));
                atm.vx += (atm2.x - atm.x) / sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) * r;
                atm.vy += (atm2.y - atm.y) / sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) * r;
            }
        };

        //скорость
        atm.x += atm.vx * dt;
        atm.y += atm.vy * dt;

        //закольцовка
        atm.x = fmod((atm.x + a/2), a) - a/2;
        atm.y = fmod((atm.y + b/2), b) - b/2;
        
        //запись
        atmz = atms.erase(atmz);
        atms.insert(atmz, atm);
    };
}
int main()
{
    //параметры
    int types = 2;//кол-во типов атомов
    int a = w;//ширина пространства игры(x)
    int b = h;//длинна(y)
    int n = 50;//кол-во атомов при старте
    float rd = 10.;//радиус атома
    float dt = 1.;//время шага симуляции
    float k = 100;//коэффициент отталкивания
    float rs = 5.;//расстояние связи - радиус * 2

    int** colmat = new int*[types];//цвета типов
    float* ms = new float[types];//масса типов
    float* qs = new float[types];//заряд типов

    for (int i = 0; i < types; i++)
    {
        colmat[i] = new int[3];
    }

    colmat[0][0] = 0;
    colmat[0][1] = 191;
    colmat[0][2] = 255;

    colmat[0][0] = 102;
    colmat[0][1] = 255;
    colmat[0][2] = 0;

    ms[0] = 1;
    ms[1] = 1.5;
    
    qs[0] = 1.;
    qs[1] = 1.2;
    /*
    esc - выход
    */
    start(types, n, a, b);

    window.clear();

    srand(time(NULL));

    while (window.isOpen())
    {
        sf::Event evnt;

        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::KeyPressed)
            {
                if (evnt.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }
        
        itt(ms, types, qs, k, dt, a, b);
        render(rd, colmat, types);
    }
}