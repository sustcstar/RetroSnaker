#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#include<conio.h>

#define WIDE 60
#define HIGH 20
#define SCOREOFFSET 15
// 1 设置地图范围 宽 高
// 2 初始化蛇
// 3 将蛇显示在屏幕上
// 4 蛇的移动（通过wasd按键控制蛇的移动方向）
/*
    a> 蛇碰到墙
    b> 蛇碰到身体
    c> 蛇碰到障碍物
    d> 蛇碰到事务（蛇的身体增长一节，原食物消失，生成新的食物，增加分数）
*/
// 5 蛇的死亡，显示积分

typedef struct _body{
    int x;
    int y;
}BODY;

typedef struct snake{
    BODY list[WIDE * HIGH]; // 身体的每一节坐标
    int size; // 蛇的身体长度
    BODY food; // 食物的坐标
    COORD coord; // 光标位置
    int dx; // 蛇x轴移动的方向
    int dy; // 蛇y轴移动的方向
    int score; // 游戏得分
    BODY tail;
}SNAKE;

void init_ui(){
    for(int i = 0; i < HIGH; i++){
        for(int j = 0; j < WIDE; j++){
            printf("#");
        }
        printf("\n");
    }
}

// TODO：万一蛇的身体和食物重叠怎么办？
void init_food(SNAKE *snake){
    srand(time(NULL));
    snake->food.x = rand()%(WIDE);
    snake->food.y = rand()%(HIGH);
}

void init_snake(SNAKE *snake){
    // 初始化蛇头坐标
    snake->list[0].x = (WIDE/2);
    snake->list[0].y = (HIGH/2);
    // 初始化蛇尾坐标
    snake->list[1].x = (WIDE/2) - 1;
    snake->list[1].y = (HIGH/2);
    // 初始化蛇的身体长度
    snake->size = 2;
    // 初始化食物坐标
    init_food(snake);
    // 初始化蛇的移动方向
    snake->dx = 1;
    snake->dy = 0;
    // 初始化分数
    snake->score = 0;
}

void show_ui(SNAKE *snake){
    // 显示蛇 注意：每次显示蛇或食物，都要设置显示的位置（光标）
    for(int i = 0; i < snake->size; i++){
        snake->coord.X = snake->list[i].x;
        snake->coord.Y = snake->list[i].y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), snake->coord);
        if(0 == i){
            printf("@");
        }
        else{
            printf("*");
        }
    }
    // 显示食物
    snake->coord.X = snake->food.x;
    snake->coord.Y = snake->food.y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), snake->coord);
    printf("#");
    // 将蛇原尾巴的位置更新为空格
    snake->coord.X = snake->tail.x;
    snake->coord.Y = snake->tail.y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), snake->coord);
    printf(" ");
}

void move_snake(SNAKE *snake){
    // 记录尾巴的坐标
    snake->tail.x = snake->list[snake->size-1].x;
    snake->tail.y = snake->list[snake->size-1].y;
    // 更新蛇的身体坐标，除蛇头
    for(int i = snake->size - 1; i > 0; i--){
        snake->list[i] = snake->list[i-1]; // 将数的前一节的坐标给后一节
    }
    // 更新蛇头
    snake->list[0].x += snake->dx;
    snake->list[0].y += snake->dy;
}

void control_snake(SNAKE *snake){
    char key = 0; // 必须初始化为0，当做是这个函数使用的特性就好
    while(_kbhit()){ // 判断是否有按键被按下, 按下为真
        key = _getch();
    }
    switch(key){
        case 'a':
            snake->dx = -1;
            snake->dy = 0;
            break;
        case 'w':
            snake->dx = 0;
            snake->dy = -1;
            break;
        case 's':
            snake->dx = 0;
            snake->dy = 1;
            break;
        case 'd':
            snake->dx = 1;
            snake->dy = 0;
            break;
        default:
            ;
    }
}

void end_game(SNAKE *snake){
    snake->coord.X = WIDE/2 - SCOREOFFSET;
    snake->coord.Y = HIGH + 2;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), snake->coord);
    printf("Game Over, your score is %d\n", snake->score);
    Sleep(3000);
    exit(0); // 退出进程
}

void snake_eat_body(SNAKE *snake){
    // 如果蛇头和身体任意一节坐标相同，则吃到自己
    for(int i = 1; i < snake->size; i++){
        if(snake->list[0].x == snake->list[i].x &&
            snake->list[0].y == snake->list[i].y){
            end_game(snake);
        }
    }
}

void snake_eat_food(SNAKE *snake){
    // 如果蛇头和食物坐标重合，吃到自己
    if(snake->list[0].x == snake->food.x &&
        snake->list[0].y == snake->food.y){
        // 原食物消失，生成新的食物
        // 由于原食物已经被蛇头覆盖，不需要清除原食物
        init_food(snake); // 生成新的食物
        // 蛇的身体增长一节
        snake->size++; // 蛇尾的坐标不用给，move_snake函数会更新
        // 分数增加
        snake->score += 10;
    }
}

void start_game(SNAKE *snake){
    while(snake->list[0].x < WIDE && snake->list[0].x > 0 &&    // 判断蛇是否碰到墙
        snake->list[0].y < HIGH && snake->list[0].y > 0){
        // 控制蛇的方向
        control_snake(snake);
        // 更新蛇的坐标
        move_snake(snake);
        // 蛇移动 (显示更新后的蛇)
        show_ui(snake);
        // 蛇是否碰到自己
        snake_eat_body(snake);
        // 蛇是否碰到食物 （碰到食物，原食物消失，产生新的食物，积分增加，蛇变长）
        snake_eat_food(snake);
        Sleep(100); // 延时0.5s TODO: 没有多线程，所以控制模块由于延时的存在产生延迟
    }
    end_game(snake);
}

void init_wall(){
    for(int i = 0; i <= HIGH; i++){
        for(int j = 0; j <= WIDE; j++){
            if(i ==0 || i == HIGH || j == 0 || j == WIDE){
                printf("+");
            }
            else{
                printf(" ");
            }
        }
        printf("\n");
    }
}

void hide_cursor(){
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = sizeof(cci);
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
}

int main(){
    // init_ui();
    hide_cursor(); // 隐藏光标
    system("cls"); // 清屏
    init_wall(); // 初始化墙壁
    SNAKE *snake = (SNAKE *)malloc(sizeof(SNAKE)); // 申请蛇的结构体
    init_snake(snake); // 初始化蛇和食物
    show_ui(snake); // 显示界面信息
    start_game(snake);
}