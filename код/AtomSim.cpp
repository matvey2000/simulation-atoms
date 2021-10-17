#include <iostream>
#include <SFML/Graphics.hpp>
#include <list>
#include <random>

float pi = 3.141592653589793238462643;

int w = sf::VideoMode::getDesktopMode().width;
int h = sf::VideoMode::getDesktopMode().height;

sf::RenderWindow window(sf::VideoMode(w, h), "molnya", sf::Style::Fullscreen);

struct type
{
    int col[3];//цвет

    float m;//масса
    float q;//заряд
    float kr;//кристаллохимический радиус
};
struct atom
{
public:
    float x;
    float y;
    float vx = 0;
    float vy = 0;
    float ax = 0;
    float ay = 0;

    type tpe;//тип
};
struct substr
{
    int a;//ширина пространства игры(x)
    int b;//длинна(y)
    int n;//кол-во атомов при старте
    float rd;//радиус атома
    float dt;//время шага симуляции
    float qo;//коэффицент притяжения

    float g;//ускорение свободного падения
};

std::list<atom> atms;//атом лист
std::list<type> tpes;
substr sbs;

void start()
{
    //types - типы кол-во
    //n - кол-во частиц
    //a, b - ширина и длина прямоугольника где появляются частицы

    atom atm;
    int len = 0;
    for (auto tp : tpes)
    {
        len++;
    }

    for (int i = 0; i < sbs.n; i++)
    {
        atm.x = ((float)rand() / (float)RAND_MAX - 0.5) * sbs.a;
        atm.y = ((float)rand() / (float)RAND_MAX - 0.5) * sbs.b;

        int rnd = int((float)rand() / (float)RAND_MAX * len);
        int n = -1;
        for (auto tpe : tpes)
        {
            n++;
            if (rnd == n)
            {
                atm.tpe = tpe;
                break;
            };
        }
        
        atms.push_back(atm);
    };
};
void render()
{
    sf::CircleShape atm_r;
    atm_r.setRadius(sbs.rd);

    window.clear();
    for (auto atm : atms)
    {
        //частица
        atm_r.setFillColor(sf::Color::Color(atm.tpe.col[0], atm.tpe.col[1], atm.tpe.col[2]));
        atm_r.setPosition(sf::Vector2f(atm.x + w / 2 - sbs.rd / 2, atm.y + h / 2 - sbs.rd / 2));

        window.draw(atm_r);
    }
    //VertexArray
    window.display();
};
void itt()
{
    atom atm;

    std::list<atom> atms_c;
    for (auto atmz = atms.begin(); atmz != atms.end(); ++atmz)
    {
        atm = *atmz;

        atm.ax = 0;
        atm.ay = 0;

        for (auto atm2 : atms)
        {
            if (atm2.x == atm.x || atm.y == atm2.y)
            {
                continue;
            }

            //Леннард-Джонс
            float ar = (atm.tpe.kr + atm2.tpe.kr) / sqrt((atm.x - atm2.x)* (atm.x - atm2.x) + (atm.y - atm2.y)* (atm.y - atm2.y));
            float md = (12 * atm.tpe.q * atm2.tpe.q / (atm.tpe.kr + atm2.tpe.kr) * (pow(ar,13) - pow(ar, 7))) / atm.tpe.m * sbs.qo;
            
            atm.ax -= sbs.dt * (atm.x - atm2.x) / sqrt((atm.x - atm2.x) * (atm.x - atm2.x) + (atm.y - atm2.y) * (atm.y - atm2.y)) * md;
            atm.ay -= sbs.dt * (atm.y - atm2.y) / sqrt((atm.x - atm2.x) * (atm.x - atm2.x) + (atm.y - atm2.y) * (atm.y - atm2.y)) * md;
        }

        //стенки
        if ((atm.x > (w / 2 - sbs.rd)) || (atm.x < -(w / 2 - sbs.rd)))
        {
            atm.vx = -atm.vx;
        }
        if ((atm.y > (h / 2 - sbs.rd)) || (atm.y < -(h / 2 - sbs.rd)))
        {
            atm.vy = -atm.vy;
        }

        //гравитация
        atm.ay += sbs.g * sbs.dt;

        //запись
        atms_c.push_back(atm);
    };
    atms = atms_c;
    atms_c.clear();

    for (auto atmz = atms.begin(); atmz != atms.end(); ++atmz)
    {
        atm = *atmz;

        //a,v -> X
        atm.x += atm.vx * sbs.dt + atm.ax * sbs.dt * sbs.dt;
        atm.y += atm.vy * sbs.dt;

        //a -> v
        atm.vx += atm.ax * sbs.dt / 2;
        atm.vy += atm.ay * sbs.dt / 2;

        //запись
        atms_c.push_back(atm);
    }

    atms = atms_c;
}
int main()
{
    //параметры

    int types = 3;//кол-во типов
    sbs.a = w;
    sbs.b = h;
    sbs.dt = 0.1;
    sbs.n = 50;
    sbs.rd = 10.;
    sbs.qo = pow(10, 6);
    sbs.g = 9.8;

    type tpe;

    tpe.col[0] = 0;
    tpe.col[1] = 191;
    tpe.col[2] = 255;
    tpe.m = 1.;
    tpe.q = 6;
    tpe.kr = 1.;

    tpes.push_back(tpe);

    tpe.col[0] = 102;
    tpe.col[1] = 255;
    tpe.col[2] = 0;
    tpe.m = 1.5;
    tpe.q = 3;
    tpe.kr = 1.5;

    tpes.push_back(tpe);

    tpe.col[0] = 220;
    tpe.col[1] = 20;
    tpe.col[2] = 60;
    tpe.m = 1.2;
    tpe.q = 2;
    tpe.kr = 2.;

    tpes.push_back(tpe);
    /*
    esc - выход
    */
    start();

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
        
        itt();
        render();
    }
}