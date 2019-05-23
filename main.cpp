#include <algorithm>
#include <array>
#include <queue>
#include <random>
#include <vector>

class Board {
public:
    static const int WIDTH = 10;
    static const int HEIGHT = 15;
    static std::mt19937 gen(std::random_device());

    typedef std::array<std::array<int, HEIGHT>, WIDTH> board_t;

    board_t board{};

    Board() {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(1, 3);
        for(auto& col : board) {
            for(auto& v : col) {
                v = dis(gen);
            }
        }
    }

    struct coord_t {
        coord_t(int row, int col) : row(row), col(col) {}
        int row;
        int col;
    };

    int clear(int row, int col) {
        auto group = board[col][row];
        if(group == 0) {
            return 0;
        }

        int count = 0;
        std::queue<coord_t> waiting;
        waiting.emplace(row, col);
        while(!waiting.empty()) {
            auto p = waiting.front();
            if(board[p.col][p.row] == group) {
                board[p.col][p.row] = 0;
                ++count;
                if(p.row != 0       ) waiting.emplace(p.row-1, p.col  );
                if(p.row != HEIGHT-1) waiting.emplace(p.row+1, p.col  );
                if(p.col != 0       ) waiting.emplace(p.row  , p.col-1);
                if(p.col != WIDTH-1 ) waiting.emplace(p.row  , p.col+1);
            }
            waiting.pop();
        }

        if(count == 1) {
            board[col][row] = group;
            return 0;
        }

        for(auto& c : board) {
            std::stable_partition(c.begin(), c.end(), [](int v){return v;});
        }
        std::stable_partition(board.begin(), board.end(), [](auto c){return c[0];});

        return count*count;
    }
};

#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    Board board;

    sf::RenderWindow window(sf::VideoMode(200, 300), "Blockd");
    sf::Color colors[] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    board.clear((299-event.mouseButton.y) / 20, event.mouseButton.x / 20);
                }
            }
        }

        window.clear();
        for(int i=0; i<board.board.size(); ++i) {
            auto& c = board.board[i];
            for(int j=0; j<c.size(); ++j) {
                auto v = c[j];
                if(v) {
                    sf::RectangleShape rect({20, 20});
                    rect.setFillColor(colors[v-1]);
                    rect.setPosition(i*20, 280-j*20);
                    window.draw(rect);
                }
            }
        }
        window.display();
    }

    return 0;
}
