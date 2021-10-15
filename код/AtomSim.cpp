#include <iostream>
#include <SFML/Graphics.hpp>
#include <list>
#include <random>

float pi = 3.141592653589793238462643;

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
    std::list<int> svz;//связи частицы (номера связанных частиц)
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
        //частица
        atm_r.setFillColor(sf::Color::Color(colmat[atm.type][0], colmat[atm.type][1], colmat[atm.type][2]));
        atm_r.setPosition(sf::Vector2f(atm.x + w / 2 - rd / 2, atm.y + h / 2 - rd / 2));

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

            sf::ConvexShape svz1;
            svz1.setPointCount(4);
            svz1.setFillColor(sf::Color::Color(colmat[atm.type][0], colmat[atm.type][1], colmat[atm.type][2]));

            float xm = (atm2.x - atm.x) / sqrt(pow(atm.x - atm2.x,2) + pow(atm2.y - atm.y,2));
            float ym = (atm2.y - atm.y) / sqrt(pow(atm.x - atm2.x, 2) + pow(atm2.y - atm.y, 2));

            sf::VertexArray lines(sf::Lines, 2);
            lines;

            lines[0].position = sf::Vector2f(atm.x + w / 2, atm.y + h / 2);
            lines[1].position = sf::Vector2f(atm2.x + w / 2, atm2.y + h / 2);

            lines[0].color = sf::Color::Color(colmat[atm.type][0], colmat[atm.type][1], colmat[atm.type][2]);
            lines[1].color = sf::Color::Color(colmat[atm2.type][0], colmat[atm2.type][1], colmat[atm2.type][2]);

            window.draw(lines);
        }
    }
    window.display();
};
void itt(float* ms, int types, float** qs, float k, float dt, float a, float b, float rd, float rs, float endurance_s, float ed, float edst)
{
    float r;

    atom atm;
    int n1 = -1;
    for (auto atmz = std::begin(atms); atmz != std::end(atms); atmz++)
    {
        n1++;
        atm = *atmz;
        int n0 = -1;
        for (auto atm2 : atms)
        {
            bool svz = false;//наличие связи

            n0++;
            for (auto n : atm.svz)
            {
                if (n0 == n)
                {
                    svz = true;
                    break;
                };
            };

            r = (k * qs[atm.type][atm2.type]) / (ms[atm.type] * (pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)));
            //отталкивание
            if (atm2.x != atm.x || atm2.y != atm.y)
            {
                atm.vx -= dt * (atm2.x - atm.x) / sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) * r;
                atm.vy -= dt * (atm2.y - atm.y) / sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) * r;
            }

            //образование связей
            if (sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) <= (rd * 2 + rs) && (!svz) && (atm2.x != atm.x || atm2.y != atm.y))
            {
                std::cout << n0 << " " << n1 << " " << "\n";
                atm.svz.push_back(n0);
            } 

            //притяжение связей
            if (svz)
            {
                atm.vx += dt * (atm2.x - atm.x) / sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) * endurance_s;
                atm.vy += dt * (atm2.y - atm.y) / sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) * endurance_s;
            }
            
            //столкновения
            if ((atm2.x != atm.x || atm2.y != atm.y) && sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) <= rd * 2)
            {
                atm.vx -= ed * dt * (atm2.x - atm.x) / sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) / (sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) - rd);
                atm.vy -= ed * dt * (atm2.y - atm.y) / sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) / (sqrt(pow(atm.x - atm2.x, 2) + pow(atm.y - atm2.y, 2)) - rd);
            }
        };

        //запись
        atmz = atms.erase(atmz);
        atms.insert(atmz, atm);
    };
    n1 = -1;
    for (auto atmz = std::begin(atms); atmz != std::end(atms); atmz++)
    {
        n1++;
        atm = *atmz;

        //скорость
        atm.x += atm.vx * dt;
        atm.y += atm.vy * dt;

        //ограничение
        if (atm.x >= w / 2)
        {
            atm.vx += -dt * edst;
        }
        else if (atm.x <= -w / 2)
        {
            atm.vx = dt * edst;
        }
        if (atm.y >= h / 2)
        {
            atm.y = -dt * edst;
        }
        else if (atm.y <= -h / 2)
        {
            atm.vy = dt * edst;
        }

        //запись
        atmz = atms.erase(atmz);
        atms.insert(atmz, atm);
    }
}
int main()
{
    //параметры
    int types = 3;//кол-во типов атомов
    int a = w;//ширина пространства игры(x)
    int b = h;//длинна(y)
    int n = 50;//кол-во атомов при старте
    float rd = 10.;//радиус атома
    float dt = 1.;//время шага симуляции
    float k = 100;//коэффициент отталкивания
    float rs = 5.;//расстояние связи - радиус * 2
    float endurance_s = 0.1;//прочность связи
    float ed = 1.;//сопротивление атома проникновению внутрь себя
    float edst = 1.;;//сопротивление стенки проникновению внутрь себя

    int** colmat = new int*[types];//цвета типов
    float* ms = new float[types];//масса типов
    float** qs = new float*[types];//сила отталкивания

    for (int i = 0; i < types; i++)
    {
        colmat[i] = new int[3];
    }
    for (int i = 0; i < types; i++)
    {
        qs[i] = new float[types];
    }

    colmat[0][0] = 0;
    colmat[0][1] = 191;
    colmat[0][2] = 255;

    colmat[1][0] = 102;
    colmat[1][1] = 255;
    colmat[1][2] = 0;

    colmat[2][0] = 220;
    colmat[2][1] = 20;
    colmat[2][2] = 60;

    ms[0] = 1;
    ms[1] = 1.5;
    ms[2] = 1.2;
    
    qs[0][0] = 0;//отталкивание частиц № 1 и 1
    qs[0][1] = 1;//отталкивание частиц № 1 и 2
    qs[0][2] = 0;//отталкивание частиц № 1 и 3

    qs[1][0] = 1;
    qs[1][1] = 0;
    qs[1][2] = 2;

    qs[2][0] = 0;
    qs[2][1] = 2;
    qs[2][2] = 1;
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
        
        itt(ms, types, qs, k, dt, a, b, rd, rs, endurance_s, ed, edst);
        render(rd, colmat, types);
    }
}