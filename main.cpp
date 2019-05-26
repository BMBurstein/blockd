#include <algorithm>
#include <array>
#include <queue>
#include <random>
#include <string>
#include <vector>

class Board {
public:
    static const int WIDTH = 10;
    static const int HEIGHT = 15;
    static std::mt19937 gen(std::random_device());

    typedef std::array<std::array<int, HEIGHT>, WIDTH> board_t;

    board_t board{};
    int score = 0;

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

    int clear(int row, int col) {
        auto group = board[col][row];
        if(group == 0) {
            return 0;
        }

        struct coord_t {
            coord_t(int row, int col) : row(row), col(col) {}
            int row;
            int col;
        };

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

        score += count * count;
        return count * count;
    }

    bool hasMore() {
        for(auto& c : board) {
            for(int i=1; i<c.size(); ++i) {
                if(c[i] == 0)
                    break;
                if(c[i] == c[i-1])
                    return true;
            }
        }
        for(int i=0; i<HEIGHT; ++i) {
            for(int j=1; j<WIDTH; ++j) {
                if(board[j][i] != 0 && board[j][i] == board[j-1][i])
                    return true;
            }
        }
        return false;
    }

    int countBlocks() {
        int count = 0;
        for(auto& c : board) {
            for(auto& v : c) {
                if(v)
                    ++count;
            }
        }
        return count;
    }
};

#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    Board board;

    sf::RenderWindow window(sf::VideoMode(200, 300), "Blockd");
    sf::Color colors[] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};

    sf::Event::MouseButtonEvent mouseDown;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                mouseDown = event.mouseButton;
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left
                 && mouseDown.button == sf::Mouse::Left
                 && mouseDown.x == event.mouseButton.x
                 && mouseDown.y == event.mouseButton.y)
                {
                    auto coord = window.mapPixelToCoords(sf::Vector2i{mouseDown.x, mouseDown.y});
                    board.clear((299-coord.y) / 20, coord.x / 20);
                    window.setTitle("Blockd - " + std::to_string(board.score) + " " + std::to_string(board.countBlocks())
                        + '/' + std::to_string(board.HEIGHT * board.WIDTH));
                    if(!board.hasMore()) window.close();
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
