#include <easyx.h>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <random>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <graphics.h>
#include <conio.h>
#include <windows.h>
#define MINIAUDIO_IMPLEMENTATION
#include "third_party/miniaudio.h"
namespace fs = std::filesystem;
using namespace std;


#define SAVE_FOLDER   "save"
#define INITIAL_HP 600
#define MOVE_SPEED 7
#define TIMER_BEGIN 0
#define TIMER_ALL 20
#define TIMER_DIE 19
#define TIMER_HP 1
#define TIMER_ATTACK 3
#define BLOCK_LENGTH 60
#define BLOCK_SPEED 3
#define BLOCK_WIDTH 3
#define END_TEXT_COLOR WHITE
#define END_TEXT_SIZE 28
#define END_TEXT_X 430
#define MODE_ENDLESS 1
#define TIME_1 (690+100)
#define TIME_2 (TIME_1+270+200)
#define TIME_3 (TIME_2+1700)
#define TIME_REPEAT (TIME_3+1000)
#define TIME_TEST 0
#define BLOCK_NUMBER 20


typedef struct aBone {
    IMAGE image;
    int x, y;
    aBone* next;
} aBone;
typedef struct {
    IMAGE image;
    int x, y;
    float face;
} aDie;
typedef struct {
    IMAGE image, image1;
    int x, y;
    double face;
} aCannon;
typedef struct {
    int x, y;
    bool active;
} aBlock;
typedef struct {
    int x,y;
    bool active;
} bBone;
typedef struct {
    int maxScores;
    ExMessage msg;
    int page;
    int gameMode;
    int switchingToPage0;
    int isAttacking;
    int isPaused;
    int playerX, playerY;
    int point1CurrentX, point1CurrentY;
    int playerHp;
    int bgX;
    int buttonX;
    int pointX, pointY;
    int aBoundaryX, aBoundaryY, bBoundaryX, bBoundaryY;
    int rightBlockY,leftBlockY;
    int yesY;
    int blackToPage1X;
    int timer[21];
    int isDie;
    int mode;
    int scores;
    int randomList[50];
    string name;
    aBone* boneList;
    aCannon cannon[20];
    aBlock rightBlock[BLOCK_NUMBER];
    aBlock leftBlock[BLOCK_NUMBER];
    aDie corpse[10];
    bBone boneB[10];
    IMAGE start, history, login, player, bg, decorate, hp, attack[6], miss, fight, blackToDie, blackToPage1, yes, back, warn;
} gameData;
typedef struct Player {
    string name;
    int score;
}Player;

bool compareScore(const Player& a, const Player& b);

string getRankPlayer(int n);

int random(int a,int b);

void load(gameData* data);

void reLoad(gameData* data);

fs::path getHistoryFilePath(gameData* data);

void saveHistory(gameData* data,const string& content);

void clearHistory(gameData* data);

int getMaxScores(gameData* data);

void showIFO(gameData* data);

void playerHpDown(gameData* data, int n);

void updateDie(gameData* data);

int buttonInGame(gameData* data, int aX, int aY, int bX, int bY);

int button(gameData* data, int aX, int aY, int bX, int bY);

void pointMove(gameData* data);

void move(gameData* data);

void boundary(gameData* data);

// 创建节点
aBone* createBone(int x, int y, IMAGE img);

// 尾插
void addBone(aBone** head, aBone* newBone);

// 删除指定节点
void removeBone(aBone** head, aBone* target);

// 清空链表
void clearAllBones(aBone** head);

void spawnBoneA(gameData* data, int time, int idx);

void spawnBonesA(gameData* data, int time);

void spawnYLine(gameData* data, int time, int y);

// 生成骨龙炮 [face]0~2*3.14,为0时面向右,顺时针转 [wait]至少为50供炮台就位, wait时间结束后激光还有50帧 [speed]越高越慢
void spawnCannon(gameData* data, int time, int x, int y, double face, int wait, int speed, int idx);

void spawnRightBlock(gameData* data, int time, int y);

void spawnRightBlocks(gameData* data, int time,int y, int number, int interval,int h=5);

void updateRightBlock(gameData* data);

void spawnLeftBlock(gameData* data, int time, int y);

void spawnLeftBlocks(gameData* data, int time,int y, int number, int interval,int h=5);

void updateLeftBlock(gameData* data);

void changeBoundary(gameData* data, int time, int aX, int aY, int bX, int bY, int speed = 20);

void attack(gameData* data,int time);

void blueTime(gameData* data, int time);

void redTime(gameData* data, int time);

void updateTimer(gameData* data);

void pause(gameData *data);

void switchToPage0(gameData* data);

void switchToPage1(gameData* data);

void switchToPage2(gameData* data);

void switchToPage3(gameData* data);

// 务必<100字符;timeB采用计时器TIMER_BEGIN
void showText(gameData *data,int timeB, char *text, int x, int y,COLORREF color=WHITE,int size=12);

void login(gameData* data);

static ma_engine g_engine;
static ma_sound   g_loopSound;
static bool       g_engineInited = false;
static bool       g_soundLoaded  = false;

void playSound(const char* path);

void playSoundLoop(const char* path);

void stopSoundLoop();


int main() {
    initgraph(400, 250);
    ActivateKeyboardLayout((HKL)0x04090409, KLF_SETFORPROCESS);
    gameData data;
    load(&data);
    initgraph(1000, 800, EX_NOMINIMIZE | EX_DBLCLKS);
    const clock_t fps = 1000 / 60;
    int startTime = 0;
    int freemTime = 0;
    while (1) {
        startTime = clock();
        BeginBatchDraw();
        cleardevice();
        switch (data.page) {
            case 0:
                settextcolor(RGB(255, 150, 0));settextstyle(20, 0, NULL);
                if (data.name=="Guest")outtextxy(150, 500,"You haven't logged in yet.");
                else {char text[100];sprintf(text,"Welcome, %s",data.name.c_str());outtextxy(data.buttonX, 500,text);}
                settextstyle(12, 0, NULL);settextcolor(WHITE);
                peekmessage(&data.msg, EX_MOUSE);
                putimage(data.buttonX, 200, &data.start);
                putimage(data.buttonX, 300, &data.history);
                putimage(data.buttonX, 400, &data.login);
                putimage(data.bgX, 0, &data.bg);
                if (button(&data, data.buttonX, 205, data.buttonX + 100, 235)) {
                    playSoundLoop("assets/audio/bgm.wav");
                    data.page = 1;
                }
                if (button(&data, data.buttonX, 305, data.buttonX + 140, 345)) data.page = 2;
                if (button(&data, data.buttonX, 400, data.buttonX + 100, 445)) data.page = 3;
                pointMove(&data);
                break;
            case 1:
                if (data.isPaused) {
                    outtextxy(400,400,"You Can Press Space To Continue");
                    pause(&data);
                }
                else {
                    move(&data);
                    boundary(&data);

                    spawnBonesA(&data, 0);
                    spawnYLine(&data, 300, 560);

                    spawnBonesA(&data, 260);
                    changeBoundary(&data, 440, 200, data.aBoundaryY, 500, data.bBoundaryY);

                    spawnBonesA(&data, 520);
                    changeBoundary(&data, 660, 501, data.aBoundaryY, 800, data.bBoundaryY);
                    spawnYLine(&data, 660, 560);
                    spawnYLine(&data, 670, 520);
                    spawnYLine(&data, 680, 480);
                    spawnYLine(&data, 690, 440);

                    changeBoundary(&data, 20+TIME_1, 400, data.aBoundaryY, 600, data.bBoundaryY);

                    spawnCannon(&data, 30+TIME_1, 200 - 35, 200, 3.14 / 4, 130, 30, 1);
                    spawnCannon(&data, 20+TIME_1, 400 - 35, 100, 3.14 / 3, 140, 30, 2);
                    spawnCannon(&data, 10+TIME_1, 600 - 35, 100, 3.14 / 3 * 2, 150, 30, 3);
                    spawnCannon(&data, TIME_1, 800 - 35, 200, 3.14 / 4 * 3, 160, 30, 4);

                    changeBoundary(&data, 150+TIME_1, 200, data.aBoundaryY, 800, data.bBoundaryY, 6);

                    spawnCannon(&data, 150+TIME_1, 210 - 35, 100, 3.14 / 2, 110, 20, 5);
                    spawnCannon(&data, 140+TIME_1, 310 - 35, 100, 3.14 / 2, 120, 20, 6);
                    spawnCannon(&data, 130+TIME_1, 410 - 35, 100, 3.14 / 2, 130, 20, 7);
                    spawnCannon(&data, 120+TIME_1, 510 - 35, 100, 3.14 / 2, 140, 20, 8);

                    spawnCannon(&data, 230+TIME_1, 600 - 35, 200, 3.14 / 3 * 2, 70, 10, 9);
                    spawnCannon(&data, 220+TIME_1, 700 - 35, 300, 3.14 / 3 * 2, 80, 10, 10);
                    spawnCannon(&data, 210+TIME_1, 800 - 35, 400, 3.14 / 3 * 2, 90, 10, 11);

                    spawnCannon(&data, TIME_1+270, 200, 400, 0, 70, 10, 1);
                    spawnCannon(&data, TIME_1+250, 200, 550, 0, 90, 10, 3);

                    if (data.gameMode!=MODE_ENDLESS)attack(&data,880);

                    blueTime(&data,TIME_2);

                    changeBoundary(&data, TIME_2, -2, 350, 1000, data.bBoundaryY, 6);

                    if (data.timer[TIMER_ALL]==TIME_2){for (int i = 0; i <= 10; i++) {data.randomList[i]=random(1,5);}}
                    spawnRightBlocks(&data,TIME_2,600-40,30,50,5);
                    spawnLeftBlocks(&data,TIME_2,600-40,30,50,5);
                    for (int i=0;i<10;i++) {spawnYLine(&data, TIME_2+100+i*150, 580);}
                    for (int i=0;i<10;i++) {spawnYLine(&data, TIME_2+175+i*150, 580-(data.randomList[i])*40);}
                    spawnCannon(&data, 1400+TIME_2, 900, 580, 3.14, 100, 30, 1);


                    changeBoundary(&data, TIME_3, 200, 400, 800, 600);

                    redTime(&data, TIME_3);

                    spawnBonesA(&data, TIME_3+50);
                    if (data.timer[TIMER_ALL]==TIME_3){for (int i = 0; i <= 30; i++) {data.randomList[i]=random(1,9);}}
                    for (int i=0;i<10;i++) {spawnYLine(&data, TIME_3+100+i*75, 600-(data.randomList[i*2])*20);spawnYLine(&data, TIME_3+100+i*75, 600-(data.randomList[i*2+1])*20);}
                    spawnCannon(&data, 510+TIME_3, 210 - 35, 50, 3.14 / 2, 110, 20, 5);
                    spawnCannon(&data, 520+TIME_3, 310 - 35, 50, 3.14 / 2, 110, 20, 6);
                    spawnCannon(&data, 530+TIME_3, 410 - 35, 50, 3.14 / 2, 110, 20, 7);
                    spawnCannon(&data, 540+TIME_3, 510 - 35, 50, 3.14 / 2, 110, 20, 8);
                    spawnCannon(&data, 550+TIME_3, 610 - 35, 50, 3.14 / 2, 110, 20, 9);
                    spawnCannon(&data, 560+TIME_3, 710 - 35, 50, 3.14 / 2, 110, 20, 10);
                    spawnCannon(&data, 570+TIME_3, 810 - 35, 50, 3.14 / 2, 110, 20, 11);


                    showText(&data,0,(char*)"You Can Press Space To Pause",10,750,WHITE,20);

                    updateLeftBlock(&data);
                    updateRightBlock(&data);
                    updateDie(&data);
                    updateTimer(&data);
                    switchToPage1(&data);
                    switchToPage0(&data);
                    pause(&data);
                }

                break;
            case 2:
                peekmessage(&data.msg, EX_MOUSE);
                char textRecord[100];
                char textName[100];
                sprintf(textName,"Your name: %s",data.name.c_str());
                sprintf(textRecord,"Your best record: %d",data.maxScores);
                showText(&data,0,textName,100,100,WHITE,20);
                showText(&data,40,textRecord,100,150,WHITE,20);
                showText(&data,0,(char*)"Rank",100,300,RGB(255, 150, 0),30);
                showText(&data,40,(char*)getRankPlayer(1).c_str(),100,350,WHITE,20);
                showText(&data,40,(char*)getRankPlayer(2).c_str(),100,400,WHITE,20);
                showText(&data,40,(char*)getRankPlayer(3).c_str(),100,450,WHITE,20);
                putimage(870, 750, &data.back);
                if (button(&data, 870, 750, 990, 790)) {
                    data.switchingToPage0=1;
                }
                updateTimer(&data);
                switchToPage2(&data);
                switchToPage0(&data);
                break;
            case 3:
                login(&data);
                reLoad(&data);
                initgraph(1000, 800, EX_NOMINIMIZE | EX_DBLCLKS);
                break;
            }
        EndBatchDraw();
        freemTime = clock() - startTime;
        if (fps > freemTime) Sleep(fps - freemTime);
    }
}


bool compareScore(const Player& a, const Player& b) {
    return a.score > b.score;
}

string getRankPlayer(int n) {
    vector<Player> allPlayers;  // 保存所有玩家的最高分
    string savePath = "save/";
    if (!fs::exists(savePath)) {return "";}

    for (const auto& entry : fs::directory_iterator(savePath)) {
        string playerName = entry.path().filename().string(); // 文件名 = 玩家名
        ifstream file(entry.path());
        if (!file) continue;

        int maxScore = 0;
        int s;
        while (file >> s) {if (s > maxScore) maxScore = s;}
        file.close();

        // 3. 加入总列表
        allPlayers.push_back({ playerName, maxScore });
    }
    sort(allPlayers.begin(), allPlayers.end(), compareScore);
    if (n < 1 || n > allPlayers.size()) {return "";}
    return "NO." + to_string(n) + "   " + allPlayers[n - 1].name + "   " + to_string(allPlayers[n-1].score);
}

int random(int a,int b){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> d(a, b);
    return d(gen);
}

void load(gameData* data) {
    data->name="Guest";
    data->gameMode=MODE_ENDLESS;
    data->maxScores=getMaxScores(data);
    data->msg = { 0 };
    data->page=0;
    data->switchingToPage0 = 0;
    data->isAttacking = 0;
    data->isPaused = 0;
    data->playerX = 170;
    data->playerY = -100;
    data->point1CurrentY = 900;
    data->point1CurrentX = 900;
    data->playerHp = INITIAL_HP;
    data->bgX = 500;
    data->buttonX = 200;
    data->pointY = -100;
    data->pointX = data->buttonX;
    data->aBoundaryX = 200;
    data->aBoundaryY = 400;
    data->bBoundaryX = 800;
    data->bBoundaryY = 600;
    data->yesY = 900;
    data->blackToPage1X=-1000;
    data->isDie = 0;
    data->mode=1;
    data->boneList = nullptr; // bone链表初始化
    for (int i = 0; i <= 10; i++) {data->randomList[i]=random(1,5);}
    for (int i = 0; i < 21; i++) {data->timer[i] = -1;}
    data->timer[TIMER_ALL] = TIME_TEST;
    data->timer[TIMER_BEGIN] = 0;
    loadimage(&data->start, "assets/images/A01_start.png", 100, 35);
    loadimage(&data->history, "assets/images/A05_history.png", 140, 40);
    loadimage(&data->login, "assets/images/A05_login.png", 100, 35);
    loadimage(&data->bg, "assets/images/A00.png", 550, 800);
    loadimage(&data->player, "assets/images/C00.png", 20, 20);
    loadimage(&data->hp, "assets/images/C01_HP.png", data->playerHp, 20);
    loadimage(&data->decorate, "assets/images/C02.png", 600, 40);
    // corpse
    for (int i = 0; i < 5; i++) {
        loadimage(&data->corpse[i].image, "assets/images/C50_die.png", 7, 6);
        data->corpse[i].x = -100;
        data->corpse[i].y = -100;
        data->corpse[i].face = (165 + i * 50) * 6.28 / 360;
    }
    // cannon
    for (int i = 0; i < 20; i++) {
        loadimage(&data->cannon[i].image, "assets/images/C11_cannon.png", 70, 70);
        data->cannon[i].x = -100;
        data->cannon[i].y = 400;
        data->cannon[i].face = 0;
    }
    // attack
    for (int i = 0; i < 6; i++) {
        char path[40];
        sprintf(path, "assets/images/C20_attack%d.png", i+1);
        loadimage(&data->attack[i], path, 590, 195);
    }
    // block
    for (int i = 0; i < BLOCK_NUMBER; i++) {
        data->rightBlock[i].x = -100;
        data->rightBlock[i].y = -100;
        data->rightBlock[i].active = false;
        data->leftBlock[i].x = -100;
        data->leftBlock[i].y = -100;
        data->leftBlock[i].active = false;
    }
    loadimage(&data->miss, "assets/images/C03_miss.png", 100, 27);
    loadimage(&data->fight, "assets/images/C04_fight.png", 115, 43);
    loadimage(&data->blackToDie, "assets/images/D00_black.png", 1000, 800);
    loadimage(&data->blackToPage1, "assets/images/D00_black.png", 1000, 800);
    loadimage(&data->yes, "assets/images/D01_yes.png", 150, 40);
    loadimage(&data->back, "assets/images/E01_back.png", 120, 40);
    loadimage(&data->warn, "assets/images/C14_warn.png", 7, 28);
}

void reLoad(gameData* data) {
    data->gameMode=MODE_ENDLESS;
    data->maxScores=getMaxScores(data);
    data->msg = { 0 };
    data->page=0;
    data->switchingToPage0 = 0;
    data->isAttacking = 0;
    data->isPaused = 0;
    data->playerX = 170;
    data->playerY = -100;
    data->playerHp = INITIAL_HP;
    data->bgX = 500;
    data->buttonX = 200;
    data->pointY = -100;
    data->pointX = data->buttonX;
    data->aBoundaryX = 200;
    data->aBoundaryY = 400;
    data->bBoundaryX = 800;
    data->bBoundaryY = 600;
    data->yesY = 900;
    data->blackToPage1X=-1000;
    data->isDie = 0;
    data->mode=1;
    data->boneList = nullptr; // bone链表初始化
    for (int i = 0; i <= 10; i++) {data->randomList[i]=random(1,5);}
    for (int i = 0; i < 21; i++) {data->timer[i] = -1;}
    data->timer[TIMER_ALL] = TIME_TEST;
    data->timer[TIMER_BEGIN] = 0;
    // corpse
    for (int i = 0; i < 5; i++) {
        data->corpse[i].x = -100;
        data->corpse[i].y = -100;
        data->corpse[i].face = (165 + i * 50) * 6.28 / 360;
    }
    // cannon
    for (int i = 0; i < 20; i++) {
        loadimage(&data->cannon[i].image, "assets/images/C11_cannon.png", 70, 70);
        data->cannon[i].x = -100;
        data->cannon[i].y = 400;
        data->cannon[i].face = 0;
    }
    // block
    for (int i = 0; i < BLOCK_NUMBER; i++) {
        data->rightBlock[i].x = -100;
        data->rightBlock[i].y = -100;
        data->rightBlock[i].active = false;
        data->leftBlock[i].x = -100;
        data->leftBlock[i].y = -100;
        data->leftBlock[i].active = false;
    }
    loadimage(&data->player, "assets/images/C00.png", 20, 20);
    loadimage(&data->hp, "assets/images/C01_HP.png", data->playerHp, 20);
}

void playSound(const char* path) {
    static ma_engine engine;
    static bool isInited = false;
    if (!isInited) {
        ma_engine_init(NULL, &engine);
        isInited = true;
    }
    ma_engine_play_sound(&engine, path, NULL);
}

fs::path getHistoryFilePath(gameData* data) {
    fs::path path = SAVE_FOLDER;
    if (!fs::exists(path))
        fs::create_directory(path);
    return path / data->name;
}

void saveHistory(gameData* data,const string& content) {
    ofstream ofs(getHistoryFilePath(data), ios::app);
    if (ofs) {
        ofs << content << endl;
        ofs.close();
    }
}

void clearHistory(gameData* data) {
    fs::path file_path = getHistoryFilePath(data);
    if (fs::exists(file_path)) {
        fs::remove(file_path);
        std::cout << "clear!\n";
    } else {
        std::cout << "notfound\n";
    }
}

int getMaxScores(gameData* data) {
    fs::path file_path = getHistoryFilePath(data);
    if (!fs::exists(file_path)) return 0;
    std::ifstream ifs(file_path);
    std::vector<int> numbers;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;

        try {
            int num = std::stoi(line);
            numbers.push_back(num);
        } catch (...) {
            continue;
        }
    }
    ifs.close();
    if (numbers.empty()) return 0;
    return *std::max_element(numbers.begin(), numbers.end());
}

void showIFO(gameData* data) {
    char text1[30];
    ExMessage msg;
    peekmessage(&msg, EX_MOUSE);
    sprintf(text1, "Timer:%d  Switching:%d", data->timer[TIMER_BEGIN],data->switchingToPage0);
    outtextxy(0, 0, text1);
}

void playerHpDown(gameData* data, int n) {
    data->playerHp -= n;
    loadimage(&data->hp, "assets/images/C01_HP.png", data->playerHp, 20);
    data->timer[TIMER_HP] = 0;
    playSound("assets/audio/hit.wav");
}

int button(gameData* data, int aX, int aY, int bX, int bY) {
    if (data->msg.x >= aX && data->msg.x <= bX && data->msg.y >= aY && data->msg.y <= bY) {
        data->pointY = aY + 5;
        data->pointX = aX - 30;
        if (data->msg.message == WM_LBUTTONUP) {
            return 1;
        }
    }
    return 0;
}

int buttonInGame(gameData* data, int aX, int aY, int bX, int bY) {
    ExMessage msg;
    if (peekmessage(&msg, EX_MOUSE) && msg.message == WM_LBUTTONUP) {
        if (msg.x >= aX && msg.x <= bX && msg.y >= aY && msg.y <= bY) return 1;
    }
    return 0;
}

void pointMove(gameData* data) {
    if (data->timer[0] > 50) return;
    if (data->playerY != data->pointY) {
        if (data->playerY > data->pointY) data->playerY += (data->pointY - data->playerY) / 10 - 1;
        else data->playerY += (data->pointY - data->playerY) / 10 + 1;
    }
    if (data->playerX != data->pointX) {
        if (data->playerX > data->pointX) data->playerX += (data->pointX - data->playerX) / 10 - 1;
        else data->playerX += (data->pointX - data->playerX) / 10 + 1;
    }
    putimage(data->playerX, data->playerY, &data->player);
}

void move(gameData* data) {
    if (data->timer[TIMER_BEGIN] <= 50) return;
    if (data->mode == 1) {
        int dx = 0, dy = 0;
        if (GetAsyncKeyState('A') & 0x8000 || GetAsyncKeyState(VK_LEFT) & 0x8000) {
            dx = -1;
        }
        if (GetAsyncKeyState('D') & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000) {
            dx = 1;
        }
        if (GetAsyncKeyState('W') & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000) {
            dy = -1;
        }
        if (GetAsyncKeyState('S') & 0x8000 || GetAsyncKeyState(VK_DOWN) & 0x8000) {
            dy = 1;
        }
        float speed = MOVE_SPEED;
        if (dx != 0 && dy != 0) {
            speed = MOVE_SPEED / sqrt(2);
        }
        int newX = data->playerX + dx * speed;
        int newY = data->playerY + dy * speed;

        if (newX >= data->aBoundaryX && newX <= data->bBoundaryX - 20) {
            data->playerX = newX;
        } else if (newX < data->aBoundaryX) {
            data->playerX = data->aBoundaryX;
        } else {
            data->playerX = data->bBoundaryX - 20;
        }
        if (newY >= data->aBoundaryY && newY <= data->bBoundaryY - 20) {
            data->playerY = newY;
        } else if (newY < data->aBoundaryY) {
            data->playerY = data->aBoundaryY;
        } else {
            data->playerY = data->bBoundaryY - 20;
        }
    }
    else if (data->mode == 2) {
        const float gravity = 0.8;
        const float jumpPower = -10.5;
        const float moveSpeed = MOVE_SPEED;

        static float vY = 0.0;
        static bool isOnGround = false;

        int dx = 0;
        if (GetAsyncKeyState('A') & 0x8000 || GetAsyncKeyState(VK_LEFT) & 0x8000) dx = -1;
        if (GetAsyncKeyState('D') & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000) dx = 1;
        if ((GetAsyncKeyState('W') & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000) && isOnGround) {
            vY = jumpPower;
            isOnGround = false;
        }

        vY += gravity;

        float newX = data->playerX + dx * moveSpeed;
        float newY = data->playerY + vY;
        for (int i = 0; i < BLOCK_NUMBER; i++) {
            if (data->rightBlock[i].active) {
                if (vY > 0&&abs(newY + 20 - data->rightBlock[i].y) <= 6 && newX + 20 > data->rightBlock[i].x && newX < data->rightBlock[i].x + BLOCK_LENGTH) {
                    newY = data->rightBlock[i].y - 20;
                    vY = 0;
                    isOnGround = true;
                    newX-=BLOCK_SPEED;
                    break;
                }
            }
            if (data->leftBlock[i].active) {
                if (vY > 0&&abs(newY + 20 - data->leftBlock[i].y) <= 6 && newX + 20 > data->leftBlock[i].x && newX < data->leftBlock[i].x + BLOCK_LENGTH) {
                    newY = data->leftBlock[i].y - 20;
                    vY = 0;
                    isOnGround = true;
                    newX+=BLOCK_SPEED;
                    break;
                }
            }
        }
        // 左右边界
        if (newX < data->aBoundaryX) newX = data->aBoundaryX;
        if (newX > data->bBoundaryX - 20) newX = data->bBoundaryX - 20;
        // 上下边界+落地判定
        if (newY >= data->bBoundaryY - 20) {
            newY = data->bBoundaryY - 20;
            vY = 0;
            isOnGround = true;
        }
        if (newY < data->aBoundaryY) {
            newY = data->aBoundaryY;
            vY = 0;
        }
        data->playerX = (int)newX;
        data->playerY = (int)newY;
    }
    // 显示角色
    if (data->isDie == 0 && (data->timer[TIMER_HP] < 0 || data->timer[TIMER_ALL] % 4 == 0 || data->timer[TIMER_ALL] % 4 == 1))putimage(data->playerX, data->playerY, &data->player);
    if (data->timer[TIMER_HP] > 4)data->timer[TIMER_HP] = -1;
}

void boundary(gameData* data) {
    rectangle(data->aBoundaryX, data->aBoundaryY, data->bBoundaryX, data->bBoundaryY);
    putimage(200, data->bBoundaryY + 20, &data->hp);
    putimage(200, data->bBoundaryY + 60, &data->decorate);
}

aBone* createBone(int x, int y, IMAGE img) {
    aBone* node = new aBone();
    node->image = img;
    node->x = x;
    node->y = y;
    node->next = nullptr;
    return node;
}

void addBone(aBone** head, aBone* newBone) {
    if (*head == nullptr) {
        *head = newBone;
        return;
    }
    aBone* cur = *head;
    while (cur->next != nullptr) cur = cur->next;
    cur->next = newBone;
}

void removeBone(aBone** head, aBone* target) {
    if (*head == nullptr || target == nullptr) return;
    if (*head == target) {
        *head = target->next;
        delete target;
        return;
    }
    aBone* cur = *head;
    while (cur != nullptr && cur->next != target) cur = cur->next;
    if (cur != nullptr) {
        cur->next = target->next;
        delete target;
    }
}

void clearAllBones(aBone** head) {
    aBone* cur = *head;
    while (cur != nullptr) {
        aBone* temp = cur;
        cur = cur->next;
        delete temp;
    }
    *head = nullptr;
}

void spawnBoneA(gameData* data, int time, int idx) {
    aBone* cur = data->boneList;
    int step = 0;
    while (cur != nullptr && step < idx) {
        cur = cur->next;
        step++;
    }
    if (cur == nullptr) {
        IMAGE tempImg;
        loadimage(&tempImg, "assets/images/C10_aBone.png", 40, 40);
        cur = createBone(200 + idx * 50, 0, tempImg);
        addBone(&data->boneList, cur);
    }
    if (cur->y > 1100) return;
    putimage(cur->x, cur->y, &cur->image, SRCINVERT);
    if (abs(cur->x - data->playerX) <= 30 && abs(cur->y - data->playerY) <= 15)
        playerHpDown(data, 10);
    if (cur->y <= 10) {
        cur->y += 10;
    } else if (data->timer[TIMER_ALL] - time >= idx * 7 + 100) {
        cur->y += 10 * sqrt(cur->y / 5) / 5;
    }
}

void spawnBonesA(gameData* data, int time) {
    if (data->timer[TIMER_ALL] - time >= 260) return;
    if (data->timer[TIMER_ALL] == time) clearAllBones(&data->boneList);
    for (int i = 0; i < 12; i++) {
        if (data->timer[TIMER_ALL] - time >= i * 5)
            spawnBoneA(data, time, i);
    }
}

void spawnYLine(gameData* data, int time, int y) {
    if (data->timer[TIMER_ALL] - time > -60 && data->timer[TIMER_ALL] - time <= -30) {
        if ((data->timer[TIMER_ALL] - time)%9>=-4) {
            putimage(10, y-14, &data->warn);
        }
    }
    if (data->timer[TIMER_ALL] - time > -30 && data->timer[TIMER_ALL] - time < 0) {
        putimage(10, y-14, &data->warn);
    }
    if (data->timer[TIMER_ALL] >= time && data->timer[TIMER_ALL] <= time + 60) {
        setlinecolor(RED);
        setlinestyle(PS_SOLID, 3);
        line((data->timer[TIMER_ALL] - 50 - time) * 100, y, (data->timer[TIMER_ALL] - time) * 100, y);
        setlinecolor(WHITE);
        if (data->playerY - y <= 0 && data->playerY - y >= -20 && data->playerX + 20 > (data->timer[TIMER_ALL] - 50 - time) * 100 && data->playerX < (data->timer[TIMER_ALL] - time) * 100 && data->timer[TIMER_ALL] % 2 == 1) {
            playerHpDown(data, 7);
        }
    }
}

void spawnCannon(gameData* data, int time, int x, int y, double face, int wait, int speed, int idx) {
    if (data->timer[TIMER_ALL] < time || data->timer[TIMER_ALL] - time > wait+100) return;
    if (data->timer[TIMER_ALL] - time == wait+100) {
        loadimage(&data->cannon[idx].image, "assets/images/C11_cannon.png", 70, 70);
        data->cannon[idx].x = -100;
        data->cannon[idx].y = 400;
        data->cannon[idx].face = 0;
        return;
    }
    // 炮台移动
    if (data->timer[TIMER_ALL] >= time && data->timer[TIMER_ALL] < time + wait) {
        rotateimage(&data->cannon[idx].image1, &data->cannon[idx].image, data->cannon[idx].face);
        putimage(data->cannon[idx].x, data->cannon[idx].y, &data->cannon[idx].image1, SRCINVERT);

        if (data->cannon[idx].x != x) {
            data->cannon[idx].x += (data->cannon[idx].x > x) ?
                (x - data->cannon[idx].x) / speed - 1 :
                (x - data->cannon[idx].x) / speed + 1;
        }
        if (data->cannon[idx].y != y) {
            data->cannon[idx].y += (data->cannon[idx].y > y) ?
                (y - data->cannon[idx].y) / speed - 1 :
                (y - data->cannon[idx].y) / speed + 1;
        }
        if (data->cannon[idx].face != 1.57 - face) {
            data->cannon[idx].face += (data->cannon[idx].face > 1.57 - face) ?
                (1.57 - face - data->cannon[idx].face) / 17 - 0.01 :
                (1.57 - face - data->cannon[idx].face) / 17 + 0.01;
        }
    }
    // 发射激光
    else {
        int laserTime = data->timer[TIMER_ALL] - (time + wait);
        // 激光持续50帧
        if (laserTime >= 0 && laserTime <= 50) {
            setlinestyle(PS_SOLID,
                (laserTime <= 10) ? 4 * laserTime :
                (laserTime < 20) ? 40 - (laserTime - 10) :
                (laserTime < 30) ? 30 + (laserTime - 20) :
                (laserTime < 50) ? 40 - 2 * (laserTime - 30) : 0
            );
            double dx = x + 15 - laserTime * 6 * cos(face);
            double dy = y + 15 - laserTime * 6 * sin(face);
            double len = laserTime * 100;
            line(
                dx, dy,
                dx + len * cos(face),
                dy + len * sin(face)
            );
            setlinestyle(PS_SOLID, 1);
            double dis = abs(sin(face) * (data->playerX + 10) - cos(face) * (data->playerY + 10)
                     + (y + 15) * cos(face) - (x + 15) * sin(face));
            if (dis <= 30) {
                playerHpDown(data, 7);
            }
        }
        rotateimage(&data->cannon[idx].image1, &data->cannon[idx].image, 1.57 - face);
        putimage(data->cannon[idx].x, data->cannon[idx].y, &data->cannon[idx].image1, SRCINVERT);
        data->cannon[idx].x -= 6 * cos(face);
        data->cannon[idx].y -= 6 * sin(face);
    }
}

void spawnRightBlock(gameData* data, int time, int y) {
    if (data->timer[TIMER_ALL] == time) {
        for (int i = 0; i < BLOCK_NUMBER; i++) {
            if (!data->rightBlock[i].active) {
                data->rightBlock[i].x = 1000;
                data->rightBlock[i].y = y;
                data->rightBlock[i].active = true;
                break;
            }
        }
    }
}

void spawnRightBlocks(gameData* data, int time,int y, int number, int interval,int h) {
    if (data->timer[TIMER_ALL] < time||data->timer[TIMER_ALL] - time>=interval*number+400) return;
    if (data->timer[TIMER_ALL] == time) data->rightBlockY=y;
    int r;
    if ((data->timer[TIMER_ALL]-TIME_2)%interval==0) {
        for (int i=0;i<number;i++) {
            spawnRightBlock(data,i*interval+time,data->rightBlockY);
        }
        r=random(0,12);
        if (r>=8) {
            if (data->rightBlockY>y-(h-1)*40)data->rightBlockY-=40;
            else data->rightBlockY+=40;
        }else if (r<=4) {
            if (data->rightBlockY<y)data->rightBlockY+=40;
            else data->rightBlockY-=40;
        }
    }
}

void updateRightBlock(gameData* data){
    setlinestyle(PS_SOLID, BLOCK_WIDTH);
    setlinecolor(LIGHTBLUE);
    for (int i = 0; i < 20; i++) {
        if (data->rightBlock[i].active) {
            data->rightBlock[i].x -= BLOCK_SPEED;
            line(data->rightBlock[i].x, data->rightBlock[i].y, data->rightBlock[i].x + BLOCK_LENGTH, data->rightBlock[i].y);
            if (data->rightBlock[i].x < -BLOCK_LENGTH) {
                data->rightBlock[i].active = false;
            }
        }
    }
    setlinecolor(WHITE);
    setlinestyle(PS_SOLID, 1);
}

void spawnLeftBlock(gameData* data, int time, int y) {
    if (data->timer[TIMER_ALL] == time) {
        for (int i = 0; i < BLOCK_NUMBER; i++) {
            if (!data->leftBlock[i].active) {
                data->leftBlock[i].x = -BLOCK_LENGTH;
                data->leftBlock[i].y = y;
                data->leftBlock[i].active = true;
                break;
            }
        }
    }
}

void spawnLeftBlocks(gameData* data, int time,int y, int number, int interval,int h) {
    if (data->timer[TIMER_ALL] < time||data->timer[TIMER_ALL] - time>=interval*number+400) return;
    if (data->timer[TIMER_ALL] == time) data->leftBlockY=y;
    int r;
    if ((data->timer[TIMER_ALL]-TIME_2)%interval==0) {
        for (int i=0;i<number;i++) {
            spawnLeftBlock(data,i*interval+time,data->leftBlockY);
        }
        r=random(0,12);
        if (r>=8) {
            if (data->leftBlockY>y-(h-1)*40)data->leftBlockY-=40;
            else data->leftBlockY+=40;
        }else if (r<=4) {
            if (data->leftBlockY<y)data->leftBlockY+=40;
            else data->leftBlockY-=40;
        }
    }
}

void updateLeftBlock(gameData* data){
    setlinestyle(PS_SOLID, BLOCK_WIDTH);
    setlinecolor(YELLOW);
    for (int i = 0; i < 20; i++) {
        if (data->leftBlock[i].active) {
            data->leftBlock[i].x += BLOCK_SPEED;
            line(data->leftBlock[i].x, data->leftBlock[i].y, data->leftBlock[i].x + BLOCK_LENGTH, data->leftBlock[i].y);
            if (data->leftBlock[i].x > 1000+BLOCK_LENGTH) {
                data->leftBlock[i].active = false;
            }
        }
    }
    setlinecolor(WHITE);
    setlinestyle(PS_SOLID, 1);
}

void changeBoundary(gameData* data, int time, int aX, int aY, int bX, int bY, int speed) {
    if (data->timer[TIMER_ALL] < time || data->timer[TIMER_ALL] > time + 120) return;
    if (data->aBoundaryX != aX) {
        if (data->aBoundaryX > aX) data->aBoundaryX += (aX - data->aBoundaryX) / speed - 1;
        else data->aBoundaryX += (aX - data->aBoundaryX) / speed + 1;
    }
    if (data->aBoundaryY != aY) {
        if (data->aBoundaryY > aY) data->aBoundaryY += (aY - data->aBoundaryY) / speed - 1;
        else data->aBoundaryY += (aY - data->aBoundaryY) / speed + 1;
    }
    if (data->bBoundaryX != bX) {
        if (data->bBoundaryX > bX) data->bBoundaryX += (bX - data->bBoundaryX) / speed - 1;
        else data->bBoundaryX += (bX - data->bBoundaryX) / speed + 1;
    }
    if (data->bBoundaryY != bY) {
        if (data->bBoundaryY > bY) data->bBoundaryY += (bY - data->bBoundaryY) / speed - 1;
        else data->bBoundaryY += (bY - data->bBoundaryY) / speed + 1;
    }
}

void attack(gameData* data, int time) {
    if (data->timer[TIMER_ALL]-time<0||data->timer[TIMER_ALL]-time>50||data->timer[TIMER_DIE]>=0) return;// bug终于得到解决: attack的时停效果干扰了死亡
    if (data->timer[TIMER_ALL]-time==0) {
        if (data->isAttacking==0)data->timer[TIMER_ATTACK]=0;
        data->timer[TIMER_ATTACK]+=1;
        switch (data->timer[TIMER_ATTACK]) {
            case 1:outtextxy(202,402,"* ");break;
            case 2:outtextxy(202,402,"* It");break;
            case 3:outtextxy(202,402,"* It's");break;
            case 4:outtextxy(202,402,"* It's yo");break;
            case 5:outtextxy(202,402,"* It's your");break;
            case 6:outtextxy(202,402,"* It's your tu");break;
            case 7:outtextxy(202,402,"* It's your turn");break;
            case 8:outtextxy(202,402,"* It's your turn to ");break;
            case 9:outtextxy(202,402,"* It's your turn to a");break;
            case 10:outtextxy(202,402,"* It's your turn to atta");break;
            default:outtextxy(202,402,"* It's your turn to attack.");break;
        }
        putimage(200,660,&data->fight);
        data->isAttacking=1;
        if (buttonInGame(data,200,659,315,700)) {
            data->timer[TIMER_ALL]+=1;
            data->isAttacking=0;
            data->timer[TIMER_ATTACK]=0;
        }
    }else {
        if (data->timer[TIMER_ALL]-time>11) {
            putimage(204,402,&data->attack[0]);
        }else {
            putimage(204,402,&data->attack[(data->timer[TIMER_ALL]-time)/2]);
        }
        if (data->timer[TIMER_ALL]-time>20&&data->timer[TIMER_ALL]%2==1) {
            putimage(450,100,&data->miss);
        }
    }
}

void blueTime(gameData* data, int time) {
    if (data->timer[TIMER_ALL]-time!=0) return;
    data->mode=2;
    loadimage(&data->player, "assets/images/C00_blue.png", 20, 20);
}

void redTime(gameData* data, int time) {
    if (data->timer[TIMER_ALL]-time!=0) return;
    data->mode=1;
    loadimage(&data->player, "assets/images/C00.png", 20, 20);
}

void updateDie(gameData* data) {
    if (data->playerHp <= 0 && data->isDie == 0) {
        for (int i = 0; i < 5; i++) {
            data->corpse[i].x = data->playerX;
            data->corpse[i].y = data->playerY;
        }
        data->scores=data->timer[TIMER_BEGIN];
        char text[50];
        sprintf(text, "%d", data->timer[TIMER_BEGIN]);
        saveHistory(data,text);
        data->timer[TIMER_DIE] = 0;
        data->isDie = 1;
    }
    if (data->switchingToPage0==1) {
        data->timer[TIMER_ALL] = -1;
        data->timer[TIMER_BEGIN] = -1;
        putimage(0,0,&data->blackToDie);
        putimage(425,data->yesY,&data->yes);

        char text[100];
        sprintf(text, "Your Scores: %d", data->scores);
        settextcolor(END_TEXT_COLOR);
        settextstyle(END_TEXT_SIZE, 0, NULL);
        outtextxy(END_TEXT_X,300,text);
        settextcolor(WHITE);
        settextstyle(12, 0, NULL);
        return;
    }
    if (data->timer[TIMER_DIE] >= 0) {
        for (int i = 0; i < 5; i++) {
            data->corpse[i].x += cos(data->corpse[i].face) * 4;
            data->corpse[i].y += sin(data->corpse[i].face) * 4 - 10 + data->timer[TIMER_DIE];
            putimage(data->corpse[i].x, data->corpse[i].y, &data->corpse[i].image, SRCINVERT);
        }

        if (data->timer[TIMER_DIE]<100) {
            putimage(-1000+10*data->timer[TIMER_DIE],0,&data->blackToDie);
        }else {
            putimage(0,0,&data->blackToDie);
        }

        if (data->timer[TIMER_DIE]>=100){
            char text[100];
            sprintf(text, "Your Scores: %d", data->scores);
            showText(data,data->timer[TIMER_BEGIN]-data->timer[TIMER_DIE]+100,text,END_TEXT_X,300,END_TEXT_COLOR,END_TEXT_SIZE);
        }

        if (data->timer[TIMER_DIE]>=130) {
            if (data->yesY > 400) data->yesY -= (data->yesY-400) / 20 + 1;
            putimage(425,data->yesY,&data->yes);
        }

        if (data->timer[TIMER_DIE]>=150 && buttonInGame(data,425,400,575,440)) {
            data->switchingToPage0=1;
        }
    }
}

void updateTimer(gameData* data) {
    if (data->isAttacking)return;
    for (int i = 0; i < 21; i++) {if (data->timer[i]>=0)data->timer[i] += 1;}
    if (data->timer[TIMER_ALL] == TIME_REPEAT) data->timer[TIMER_ALL] = 440;
}

void pause(gameData *data) {
    static bool lastSpace = false;
    bool currSpace = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    if (currSpace && !lastSpace) {
        data->isPaused = !data->isPaused;
        // 待会加音效
    }
    lastSpace = currSpace;
}

void switchToPage0(gameData* data) {
    if (data->switchingToPage0==1) {
        if (data->bgX > 500) data->bgX += (500 - data->bgX) / 20 - 1;
        if (data->blackToPage1X < 0) data->blackToPage1X += (0 - data->blackToPage1X) / 20 + 1;
        if (data->buttonX < 200) data->buttonX += (200-data->buttonX) / 20 + 1;

        // 虽然稍后load也会初始化角色坐标, 但这里需要隐藏角色
        data->playerY = -100;
        data->playerX = 170;

        putimage(data->blackToPage1X,0,&data->blackToPage1);
        putimage(data->bgX, 0, &data->bg);
        putimage(data->buttonX, 200, &data->start);
        putimage(data->buttonX, 300, &data->history);
        putimage(data->buttonX, 400, &data->login);
        if (data->bgX >= 500&&data->blackToPage1X >= 0) {
            clearAllBones(&data->boneList);
            stopSoundLoop();
            reLoad(data);
        }
    }
}

void switchToPage1(gameData* data) {
    if (data->timer[TIMER_BEGIN]>50||data->switchingToPage0==1) return;
    if (data->bgX < 1100) {
        data->bgX += (1100 - data->bgX) / 20 + 2;
        putimage(data->bgX, 0, &data->bg);
    }
    if (data->buttonX > -200) {
        data->buttonX -= (data->buttonX + 200) / 20 + 2;
        putimage(data->buttonX, 200, &data->start);
        putimage(data->buttonX, 300, &data->history);
        putimage(data->buttonX, 400, &data->login);
    }
    data->pointY = 500;
    data->pointX = 500;
    pointMove(data);
}

void switchToPage2(gameData* data) {
    if (data->timer[TIMER_BEGIN]>50||data->switchingToPage0==1) return;
    if (data->bgX < 1100) {
        data->bgX += (1100 - data->bgX) / 20 + 2;
        putimage(data->bgX, 0, &data->bg);
    }
    if (data->buttonX > -200) {
        data->buttonX -= (data->buttonX + 200) / 20 + 2;
        putimage(data->buttonX, 200, &data->start);
        putimage(data->buttonX, 300, &data->history);
        putimage(data->buttonX, 400, &data->login);
    }
    data->pointY = -100;
    data->pointX = 170;
    pointMove(data);
}

void switchToPage3(gameData* data) {
    switchToPage2(data);
}

void showText(gameData *data,int timeB,char *text, int x, int y,COLORREF color,int size) {
    if (data->timer[TIMER_BEGIN]<timeB) return;
    settextcolor(color);
    settextstyle(size, 0, NULL);
    if (data->timer[TIMER_BEGIN]-timeB<100){
        char show[100];
        int i;
        for(i=0; i<(data->timer[TIMER_BEGIN]-timeB)/3 && text[i]!='\0'; i++){
            show[i] = text[i];
        }
        show[i] = '\0';

        outtextxy(x,y,show);
        settextcolor(WHITE);
        settextstyle(12, 0, NULL);

        return;
    }
    outtextxy(x,y,text);
    settextcolor(WHITE);
    settextstyle(12, 0, NULL);
}

void login(gameData* data) {
    while (_kbhit()) _getch();
    initgraph(400, 250);
    std::string inputName;
    while (1) {
        BeginBatchDraw();
        cleardevice();
        setbkmode(TRANSPARENT);
        settextcolor(WHITE);
        outtextxy(140, 30, "Input your name:");
        outtextxy(100, 50, "(use English letters & numbers)");

        setlinecolor(WHITE);
        rectangle(80, 90, 320, 140);

        settextcolor(RED);
        solidrectangle(85, 95, 315, 135);

        setfillcolor(RED);
        solidrectangle(160, 185, 240, 210);

        settextcolor(WHITE);
        outtextxy(185, 190, "YES");

        settextcolor(WHITE);
        outtextxy(95, 107, inputName.c_str());
        if (_kbhit())
        {
            wchar_t ch = _getwch();
            if (ch == L'\r') {
                data->name = inputName;
                if (data->name=="")data->name="Guest";
                closegraph();
                return;
            }else if (ch == L'\b'){
                if (!inputName.empty()) inputName.pop_back();
            }else if (ch >= 32 && inputName.size() < 18){
                inputName += ch;
            }
        }
        while (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                if (msg.x >= 160 && msg.x <= 240 && msg.y >= 185 && msg.y <= 210) {
                    data->name = inputName;
                    if (data->name=="")data->name="Guest";
                    closegraph();
                    return;
                }
            }
        }
        EndBatchDraw();
    }
}

void playSoundLoop(const char* path){
    if (!g_engineInited){
        ma_engine_init(NULL, &g_engine);
        g_engineInited = true;
    }

    if (g_soundLoaded){
        ma_sound_stop(&g_loopSound);
        ma_sound_uninit(&g_loopSound);
        g_soundLoaded = false;
    }

    ma_sound_init_from_file(&g_engine, path, MA_SOUND_FLAG_DECODE, NULL, NULL, &g_loopSound);
    ma_sound_set_looping(&g_loopSound, MA_TRUE);
    ma_sound_start(&g_loopSound);
    g_soundLoaded = true;
}

void stopSoundLoop(){
    if (g_soundLoaded)
    {
        ma_sound_stop(&g_loopSound);
        ma_sound_uninit(&g_loopSound);
        g_soundLoaded = false;
    }
}