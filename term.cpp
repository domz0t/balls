#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <sys/ioctl.h>

#include "term.h"

#define home() 			printf(ESC "[H") 
#define clrscr()		printf(ESC "[2J") 
#define gotoxy(x,y)		printf(ESC "[%d;%dH", y, x);
#define off_cursor_visibility() printf(ESC "[?25l");
#define on_cursor_visibility() printf(ESC "[?25h");

#define resetcolor() printf(ESC "[0m")
#define set_display_atrib(color) 	printf(ESC "[%dm",color)

void frame_draw (int width, int height) {
	home();
	set_display_atrib(B_WHITE);
    std::cout << "┌";
    for(int i = 0; i < width-2; i++)
        std::cout << "─";
    std::cout << "┐";
    std::cout << "\n";
    for(int y = 0; y < height - 2; y++)
    {
        std::cout << "│";
        for(int x = 0; x < width - 2; x++)
            std::cout << " ";
        std::cout << "│";
        std::cout << "\n";
    }
    std::cout << "└";
    for(int i = 0; i < width-2; i++)
        std::cout << "─";
    std::cout << "┘";
}

void signal_handler(int signal)
{
    resetcolor();
    on_cursor_visibility();
    clrscr();
    gotoxy(1, 1);
    exit(0);
}

class Ball
{
    public:
    int x;
    int y;
    int vx;
    int vy;
    int number;
    int collisions;
    int collision_number;
    
    public:
    Ball(int _x, int _y, int _vx, int _vy, int _number)
    : x(_x), y(_y), vx(_vx), vy(_vy), number(_number), collisions(0), collision_number(-1) {}
    void draw_ball()
    {
        set_display_atrib(30 + (number % 7));
        gotoxy(x, y);
        std::cout << "\u25EF" << std::endl;
    }
    void draw_void()
    {
        gotoxy(x, y);
        //set_display_atrib(40 + (number % 7));
        std::cout << " " << std::endl;
    }
    void define_speed(int width, int height, std::vector<Ball>& balls)
    {
        for(int i = 0; i < balls.size(); i++)
        {
            if(balls[i].number == number) continue;
            if(balls[i].collision_number == number) continue;
            if(
                (((balls[i].x + balls[i].vx) == (x + vx)) && ((balls[i].y + balls[i].vy) == (y + vy))) || 
                (
                    (abs(balls[i].x - x) <= 1) && (abs(balls[i].y - y) <= 1) &&
                    (
                        (((balls[i].x + balls[i].vx) == x) && (balls[i].x == (x + vx))) ||
                        (((balls[i].y + balls[i].vy) == y) && (balls[i].y == (y + vy)))
                    )
                )
            )
            {
                collisions++;
                balls[i].collisions++;
                if(collisions >= 2) break;
                collision_number = balls[i].number;
            }
        }
    }
    void move(int width, int height, std::vector<Ball>& balls)
    {
        if((collisions == 1) && (collision_number != -1))
        {
            Ball* temp_ball = &balls[collision_number];
            int temp_vx = vx;
            int temp_vy = vy;
            vx = temp_ball->vx;
            vy = temp_ball->vy;
            temp_ball->vx = temp_vx;
            temp_ball->vy = temp_vy;
        }
        if(((x + vx) == width) || ((x + vx) == 1) || (collisions >= 2))
            vx *= -1;
        if(((y + vy) == height) || ((y + vy) == 1) || (collisions >= 2))
            vy *= -1;
        x += vx;
        y += vy;
        collisions = 0;
        collision_number = -1;
    }
};

int random(int low, int high) { return low + (high - low) * (rand()/(RAND_MAX + 1.0)); }

int main (void) {
    signal(SIGINT, signal_handler);
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int width = w.ws_col;
    int height = w.ws_row;;
    std::vector<Ball> balls;
    for(int i = 0; i < 100; i++)
        balls.push_back(Ball(random(2, width-1), random(2, height-1), 1, 1, i));
	clrscr();
	frame_draw(width, height);
    off_cursor_visibility();
    while (true)
    {
        for(auto ball : balls)
            ball.draw_ball();
        usleep(60000);
        for(int i = 0; i < balls.size(); i++)
            balls.at(i).define_speed(width, height, balls);
        for(int i = 0; i < balls.size(); i++)
        {
            balls.at(i).draw_void();
            balls.at(i).move(width, height, balls);
        }
        fflush(stdout);
    }
	return 0;
}
