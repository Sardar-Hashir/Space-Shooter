#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <list>

using namespace std;

const int SCREEN_WIDTH = 50;
const int SCREEN_HEIGHT = 25;
const char PLAYER_CHAR = 'A';
const char ENEMY_CHAR = 'V';
const char WALL_CHAR = '#';
const int INITIAL_HEALTH = 5;
const int ENEMY_SPAWN_RATE = 15;
const int ENEMY_MOVE_SPEED = 1;
const int GAME_SPEED = 100;

struct GameObject {
    int x, y;
    char displayChar;
};

void gotoxy(int x, int y);
void setup(GameObject& player, int& score, int& health, list<GameObject>& enemies);
void drawBorder();
void drawPlayer(const GameObject& player);
void drawEnemies(const list<GameObject>& enemies);
void drawUI(int score, int health);
void handleInput(GameObject& player);
void updateEnemies(list<GameObject>& enemies, int& spawnTimer, int& moveTimer);
void checkCollisions(GameObject& player, list<GameObject>& enemies, int& score, int& health);
void clearScreen();
bool isPositionOccupied(int x, int y, const list<GameObject>& enemies);
void showLandingScreen();

HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

void showLandingScreen() {
    system("cls");

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);

    gotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 2);
    cout << "=============================";

    gotoxy(SCREEN_WIDTH / 2 - 11, SCREEN_HEIGHT / 2);
    cout << "     SPACE SHOOTER GAME     ";

    gotoxy(SCREEN_WIDTH / 2 - 9, SCREEN_HEIGHT / 2 + 2);
    cout << "       By Sardar Hashir      ";

    gotoxy(SCREEN_WIDTH / 2 - 12, SCREEN_HEIGHT / 2 + 4);
    cout << "   Press any key to start... ";

    gotoxy(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 + 6);
    cout << "=============================";

    _getch();
    system("cls");
}

int main() {
    srand(time(0));
    showLandingScreen();
    GameObject player;
    int score = 0;
    int health = INITIAL_HEALTH;
    list<GameObject> enemies;
    int enemySpawnTimer = 0;
    int enemyMoveTimer = 0;
    bool gameOver = false;

    setup(player, score, health, enemies);

    while (!gameOver) {
        if (_kbhit()) {
            handleInput(player);
        }

        updateEnemies(enemies, enemySpawnTimer, enemyMoveTimer);
        checkCollisions(player, enemies, score, health);

        if (health <= 0) {
            gameOver = true;
        }

        clearScreen();
        drawBorder();
        drawPlayer(player);
        drawEnemies(enemies);
        drawUI(score, health);

        Sleep(GAME_SPEED);
    }

    clearScreen();
    gotoxy(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2);
    cout << "GAME OVER!";
    gotoxy(SCREEN_WIDTH / 2 - 8, SCREEN_HEIGHT / 2 + 1);
    cout << "Final Score: " << score;
    gotoxy(0, SCREEN_HEIGHT + 1);

    return 0;
}

void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(consoleHandle, pos);
}

void clearScreen() {
    system("cls");
}

void setup(GameObject& player, int& score, int& health, list<GameObject>& enemies) {
    player.x = SCREEN_WIDTH / 2;
    player.y = SCREEN_HEIGHT - 2;
    player.displayChar = PLAYER_CHAR;
    score = 0;
    health = INITIAL_HEALTH;
    enemies.clear();

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

void drawBorder() {
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        gotoxy(i, 0); cout << WALL_CHAR;
        gotoxy(i, SCREEN_HEIGHT - 1); cout << WALL_CHAR;
    }
    for (int i = 1; i < SCREEN_HEIGHT - 1; ++i) {
        gotoxy(0, i); cout << WALL_CHAR;
        gotoxy(SCREEN_WIDTH - 1, i); cout << WALL_CHAR;
    }
}

void drawPlayer(const GameObject& player) {
    gotoxy(player.x, player.y);
    cout << player.displayChar;
}

void drawEnemies(const list<GameObject>& enemies) {
    for (const auto& enemy : enemies) {
        gotoxy(enemy.x, enemy.y);
        cout << enemy.displayChar;
    }
}

void drawUI(int score, int health) {
    gotoxy(1, SCREEN_HEIGHT);
    cout << "Score: " << score << "  Health: ";
    for (int i = 0; i < health; ++i) cout << "<3 ";
    for (int i = health; i < INITIAL_HEALTH; ++i) cout << "   ";
}

void handleInput(GameObject& player) {
    char key = _getch();
    if (key == -32 || key == 0 || key == 224) {
        key = _getch();
        switch (key) {
        case 75:
            if (player.x > 1) player.x--;
            break;
        case 77:
            if (player.x < SCREEN_WIDTH - 2) player.x++;
            break;
        }
    }
    else {
        switch (tolower(key)) {
        case 'a':
            if (player.x > 1) player.x--;
            break;
        case 'd':
            if (player.x < SCREEN_WIDTH - 2) player.x++;
            break;
        case 27:
            break;
        }
    }
}

void updateEnemies(list<GameObject>& enemies, int& spawnTimer, int& moveTimer) {
    moveTimer++;
    if (moveTimer >= ENEMY_MOVE_SPEED) {
        moveTimer = 0;
        for (auto it = enemies.begin(); it != enemies.end();) {
            it->y++;
            if (it->y >= SCREEN_HEIGHT - 1) {
                it = enemies.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    spawnTimer++;
    if (spawnTimer >= ENEMY_SPAWN_RATE) {
        spawnTimer = 0;
        int spawnX = rand() % (SCREEN_WIDTH - 2) + 1;
        int spawnY = 1;
        if (!isPositionOccupied(spawnX, spawnY, enemies)) {
            enemies.push_back({ spawnX, spawnY, ENEMY_CHAR });
        }
    }
}

bool isPositionOccupied(int x, int y, const list<GameObject>& enemies) {
    for (const auto& enemy : enemies) {
        if (enemy.x == x && enemy.y == y) {
            return true;
        }
    }
    return false;
}

void checkCollisions(GameObject& player, list<GameObject>& enemies, int& score, int& health) {
    for (auto it = enemies.begin(); it != enemies.end();) {
        if (player.x == it->x && player.y == it->y) {
            score += 10;
            health--;
            it = enemies.erase(it);
        }
        else {
            ++it;
        }
    }
}
