#include "color.hpp"
#include "window.hpp"
#include <random>

using namespace games;

class GameOfLife
{
  public:
    static constexpr int N = 100;
    static constexpr int M = 100;

  private:
    bool cells[N][M];
    bool next[N][M];
    rgb color;
    int generation = 0;

  public:
    GameOfLife(rgb color) : color(color)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < M; ++j)
            {
                cells[i][j] = dis(gen);
            }
        }
    }

    void step()
    {
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < M; ++j)
            {
                int cnt = 0;
                for (int x = -1; x <= 1; ++x)
                {
                    for (int y = -1; y <= 1; ++y)
                    {
                        int ni = i + x;
                        int nj = j + y;
                        if (unsigned(ni) < unsigned(N) && unsigned(nj) < unsigned(M))
                            cnt += cells[ni][nj];
                    }
                }
                next[i][j] = cnt == 3 || (cnt == 4 && cells[i][j]);
            }
        }
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < M; ++j)
            {
                cells[i][j] = next[i][j];
            }
        }
        ++generation;
    }

    void draw(RawCanvas &canvas, int cell_size)
    {
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < M; ++j)
            {
                if (cells[i][j])
                {
                    for (int x = 0; x < cell_size; ++x)
                    {
                        for (int y = 0; y < cell_size; ++y)
                        {
                            canvas.set_pixel(i * cell_size + x, j * cell_size + y, color.r, color.g, color.b);
                        }
                    }
                }
            }
        }
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
    Application app;
    const int cell_size = 5;
    const int width = GameOfLife::N * cell_size;
    const int height = GameOfLife::M * cell_size;
    RawCanvas canvas(width, height);
    auto size_policy = SizePolicy::fixed(width, height);
    MainWindow window(size_policy, canvas, 60);
    if (!window.init(L"Game of Life", width, height))
    {
        return 0;
    }
    std::thread t1(
        [&canvas, cell_size]
        {
            GameOfLife game(rgb::black());
            auto background = rgb::light_white();
            while (true)
            {
                canvas.beginpaint();
                canvas.fill(background.r, background.g, background.b);
                game.step();
                game.draw(canvas, cell_size);
                canvas.endpaint();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    window.show(nCmdShow);
    app.exec();
    return 0;
}