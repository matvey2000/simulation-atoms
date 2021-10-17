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
    int v;//валентность
    float ao;//электротрицательность
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
    std::list<int> svz;
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
    float mmr;//max / min r связи
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

    for (auto atm : atms)
    {
        //частица
        atm_r.setFillColor(sf::Color::Color(atm.tpe.col[0], atm.tpe.col[1], atm.tpe.col[2]));
        atm_r.setPosition(sf::Vector2f(atm.x + w / 2 - sbs.rd / 2, atm.y + h / 2 - sbs.rd / 2));

        window.draw(atm_r);

        //связи
        for (auto svz : atm.svz)
        {
            atom atm2;
            int n = -1;
            for (auto atm3 : atms)
            {
                n++;
                if (n == svz)
                {
                    atm2 = atm3;
                    break;
                };
            }
            std::cout << atm2.x << " " << atm2.y << "\n";
            
            sf::VertexArray lines(sf::Lines, 2);
            lines[0].position = sf::Vector2f(atm.x + w / 2, atm.y + h / 2);
            lines[1].position = sf::Vector2f(atm2.x + w / 2, atm2.y + h / 2);
            lines[0].color = sf::Color::Color(atm.tpe.col[0], atm.tpe.col[1], atm.tpe.col[2]);
            lines[1].color = sf::Color::Color(atm2.tpe.col[0], atm2.tpe.col[1], atm2.tpe.col[2]);
            window.draw(lines);
        }
    }
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

        int n0 = -1;
        for (auto atm2 : atms)
        {
            ++n0;

            bool svz = false;//наличие связи
            for (auto n : atm.svz)
            {
                if (n0 == n)
                {
                    svz = true;
                    break;
                };
            };
            if (atm2.x == atm.x || atm.y == atm2.y)
            {
                continue;
            }

            float md = sbs.qo / ((atm.x - atm2.x) * (atm.x - atm2.x) + (atm.y - atm2.y) * (atm.y - atm2.y)) / atm.tpe.m;
            
            atm.ax += sbs.dt * (atm.x - atm2.x) / sqrt((atm.x - atm2.x) * (atm.x - atm2.x) + (atm.y - atm2.y) * (atm.y - atm2.y)) * md;
            atm.ay += sbs.dt * (atm.y - atm2.y) / sqrt((atm.x - atm2.x) * (atm.x - atm2.x) + (atm.y - atm2.y) * (atm.y - atm2.y)) * md;
            
            if ((!svz) && (pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2) <= pow(sbs.mmr, 2)))
            {
                atm.svz.push_back(n0);
            }
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

    int Stypes = 3;//кол-во типов
    sbs.a = w;
    sbs.b = h;
    sbs.dt = 0.1;
    sbs.n = 50;
    sbs.rd = 10.;
    sbs.qo = pow(10, 6);
    sbs.g = 9.8;
    sbs.mmr = 50;

    type tpe;

    tpe.col[0] = 0;
    tpe.col[1] = 191;
    tpe.col[2] = 255;
    tpe.m = 1.;
    tpe.v = 2;
    tpe.ao = 3.5;

    tpes.push_back(tpe);

    tpe.col[0] = 102;
    tpe.col[1] = 255;
    tpe.col[2] = 0;
    tpe.m = 1.5;
    tpe.v = 1;
    tpe.ao = 0.05;

    tpes.push_back(tpe);

    tpe.col[0] = 220;
    tpe.col[1] = 20;
    tpe.col[2] = 60;
    tpe.m = 1.2;
    tpe.v = 6;
    tpe.ao = 2.6;

    tpes.push_back(tpe);
    /*
    esc - выход
    f1 - режим "новая частица"
    f2 - режим "траектории"
    enter - установка частицы в режиме новая частица
    wheel - смена типа частицы
    space - пауза
    */
    start();

    window.clear();

    srand(time(NULL));
    bool rem = false;//отрисовка частицы у курсора
    int tpn = 0;//выбранный тип
    type tpen;

    int len = 0;
    for (auto tp : tpes)
    {
        len++;
    }

    bool go = true;
    bool f2 = true;
    while (window.isOpen())
    {
        float mx = sf::Mouse::getPosition().x;
        float my = sf::Mouse::getPosition().y;

        sf::Event evnt;

        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    window.close();
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
                {
                    if (rem)
                    {
                        rem = false;
                    }
                    else
                    {
                        rem = true;
                    }
                    tpn = 0;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                {
                    if (rem)
                    {
                        atom atm;

                        atm.tpe = tpen;
                        atm.x = mx - w / 2;
                        atm.y = my - h / 2;

                        atms.push_back(atm);
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                {
                    if (go)
                    {
                        go = false;
                    }
                    else
                    {
                        go = true;
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
                {
                    if (f2)
                    {
                        f2 = false;
                    }
                    else
                    {
                        f2 = true;
                    }
                }
            }
            else if (evnt.type == sf::Event::MouseWheelMoved)
            {
                tpn += evnt.mouseWheel.delta;

                tpn = (len + tpn) % len;

                int num = 0;
                for (auto tp : tpes)
                {
                    if (tpn == num++)
                    {
                        tpen = tp;
                        break;
                    }
                }
            }
        }

        if (f2)
        {
            window.clear();
        }
        
        if (go)
        {
            itt();
        }
        render();

        if (rem)
        {
            sf::CircleShape atm_r;
            atm_r.setRadius(sbs.rd);

            atm_r.setFillColor(sf::Color::Color(tpen.col[0], tpen.col[1], tpen.col[2]));
            atm_r.setPosition(sf::Vector2f(mx - sbs.rd / 2, my - sbs.rd / 2));

            window.draw(atm_r);
        }

        window.display();
    }
}