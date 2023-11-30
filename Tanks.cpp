#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h>

#define WIDTH 130
#define HEIGHT 40
#define MAX_PLAYERS 5
#define M_PI 3.14159265358979323846
#define GRAVITY 30

// Struktura reprezentující tank v hře
typedef struct Tank {
    char name[20];
    int xPosition;
    int yPosition;
    bool isHit;
} Tank;

// Deklarace funkcí

// Zobrazí hlavní menu hry.
void displayMainMenu();

// Spustí hlavní smyčku hry.
void runGameLoop();

// Zahájí novou hru s určeným počtem hráčů.
void startGame(int playerCount, Tank* players);

// Nastaví počet hráčů podle uživatelova výběru.
void setNumberOfPlayers(int* playerCount);

// Umožní hráčům zadat svá jména.
void enterPlayerNames(Tank* players, int playerCount);

// Vygeneruje výškovou mapu pro herní terén.
void generateHeightMap(int* heightMap, int columns);

// Vytvoří herní pole s určeným počtem řádků a sloupců a využije výškovou mapu pro terén.
char** createGameField(int rows, int columns, int* heightMap);

// Inicializuje herní pole, umístí hráče a nastaví počáteční stav.
void initializeGameField(char** matrix, int rows, int columns, int playerCount, Tank* players, int* heightMap);

// Vytiskne herní pole a aktuální stav hry.
void printGameField(char** matrix, int rows, int columns, Tank* players, int playerCount);

// Aktualizuje herní pole po každém tah hráče.
void updateGameField(char** matrix, Tank* players, int playerCount);

// Tah jednoho hráče.
void playerTurn(Tank* currentPlayer, char** matrix, int playerCount, Tank* players);

// Hráč vystřelí projektil s určitou silou a úhlem.
void fireProjectile(Tank* tank, char** matrix, int playerCount, Tank* players);

// Zničí terén na určité pozici v okolí výbuchu.
void destroyTerrain(char** matrix, int x, int y, int rows, int columns);

// Kontroluje a pohybuje tanky, pokud se mohou pohybovat dolů.
void checkAndMoveTanks(char** matrix, Tank* players, int playerCount, int rows, int columns);

// Animuje explozi na dané pozici.
void animateExplosion(char** matrix, int x, int y, int rows, int columns, Tank* players, int playerCount);

// Kontroluje, zda byl zásah na dané pozici a aktualizuje stav hráčů.
bool checkForHits(int x, int y, Tank* players, int playerCount);

// Ukáže výsledky hry a vyhlásí vítěze.
void endGame(Tank* players, int playerCount);

int main() {
    runGameLoop();
    return 0;
}

void runGameLoop() {

    printf("Welcome to the Tank Game!\n");
    printf("Use the menu to set up and start the game.\n");

    while (1) {
        displayMainMenu();
        system("cls"); 
    }
}

void displayMainMenu() {
    int choice = 0;
    int playerCount = 2;
    Tank players[MAX_PLAYERS];

    while (1) {
        printf("Main Menu\n");
        printf("1. Set Number of Players (Currently %d)\n", playerCount);
        printf("2. Enter Player Names\n");
        printf("3. Start Game\n");
        printf("4. Exit\n");
        printf("Select an option: ");
        scanf("%d", &choice);
        while (getchar() != '\n'); // Vyčistit buffer po scanf

        switch (choice) {
        case 1:
            setNumberOfPlayers(&playerCount);
            break;
        case 2:
            enterPlayerNames(players, playerCount);
            break;
        case 3:
            printf("Hello there %d:", playerCount);

            startGame(playerCount, players);
            break; 
        case 4:
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}

void setNumberOfPlayers(int* playerCount) {
    printf("Enter number of players (2-5): ");
    scanf("%d", playerCount);
    if (*playerCount < 2 || *playerCount > 5) {
        printf("Invalid number of players. Please choose between 2 and 5.\n");
        *playerCount = 2;
    }
    system("cls");

}

void enterPlayerNames(Tank* players, int playerCount) {
    for (int i = 0; i < playerCount; i++) {
        printf("Enter name for Player %d: ", i + 1);
        scanf("%s", players[i].name);
    }
    system("cls");

}

void startGame(int playerCount, Tank* players) {

    int heightMap[WIDTH];
    generateHeightMap(heightMap, WIDTH); // Generování výškové mapy

    char** gameField = createGameField(HEIGHT, WIDTH, heightMap); // Vytvoření herního pole s výškovou mapou
    initializeGameField(gameField, HEIGHT, WIDTH, playerCount, players, heightMap);

    bool isGameRunning = true;
    char input; 
    printf("Hello there");

    while (isGameRunning) {
        printGameField(gameField, HEIGHT, WIDTH, players, playerCount);
        for (int i = 0; i < playerCount; i++) {
            if (!players[i].isHit) {
                playerTurn(&players[i], gameField, playerCount, players);
            }
        }
        printf("Press 'q' to quit the game...\n");
        while ((input = getchar()) != 'q' && input != '\n'); 
        if (input == 'q') {
            isGameRunning = false;
            system("cls");
        }
    }

    for (int i = 0; i < HEIGHT; i++) {
        free(gameField[i]);
    }
    free(gameField);
}

void generateHeightMap(int* heightMap, int columns) {
    int minHeight = 5;  // Minimální počáteční výška
    int maxHeight = 15; // Maximální počáteční výška

    // Nastavíme první hodnotu
    heightMap[0] = rand() % (maxHeight - minHeight + 1) + minHeight;

    for (int i = 1; i < columns; i++) {
        // Vypočítáme rozdíl výšky pro následující sloupec
        int heightDifference = (rand() % 5) - 2; // Náhodný rozdíl mezi -1 až +1

        // Nastavíme výšku s omezením na rozsah
        heightMap[i] = heightMap[i - 1] + heightDifference;

        // Ujistíme se, že výška zůstává v požadovaném rozmezí
        if (heightMap[i] < minHeight) {
            heightMap[i] = minHeight;
        }
        else if (heightMap[i] > maxHeight) {
            heightMap[i] = maxHeight;
        }
    }
}

char** createGameField(int rows, int columns, int* heightMap) {
    char** matrix = (char**)malloc(rows * sizeof(char*));
    if (!matrix) {
        perror("Allocation failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        matrix[i] = (char*)malloc(columns * sizeof(char));
        if (!matrix[i]) {
            perror("Allocation failed");
            for (int k = 0; k < i; k++) {
                free(matrix[k]);
            }
            free(matrix);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            // Horní a dolní okraj
            if (i == 0 || i == rows - 1) {
                matrix[i][j] = '#';
            }
            // Levý a pravý okraj
            else if (j == 0 || j == columns - 1) {
                matrix[i][j] = '#';
            }
            // Terén a vzduch
            else {
                if (i < heightMap[j]) {
                    matrix[i][j] = '*'; // Terén
                }
                else {
                    matrix[i][j] = ' '; // Vzduch
                }
            }
        }
    }

    return matrix;
}


void initializeGameField(char** matrix, int rows, int columns, int playerCount, Tank* players, int* heightMap) {
    srand((unsigned int)time(NULL)); // Initialize random seed
    for (int i = 0; i < playerCount; i++) {
        bool placed = false;
        int attempts = 0;

        while (!placed && attempts < 1000) {
            int x = rand() % (columns - 2) + 1; // Vybereme náhodnou x-pozici

            // Nastavíme y-pozici tanku přímo nad terénem
            int y = heightMap[x];

            // Kontrola, zda je místo volné (je třeba zvážit, jak je terén reprezentován)
            if (matrix[y][x] == ' ' && matrix[y + 1][x] != 'T') {
                matrix[y + 1][x] = 'T'; // Umístíme tank hned nad terén
                players[i].xPosition = x;
                players[i].yPosition = y + 1;
                players[i].isHit = false;
                placed = true;
            }
            attempts++;
        }
        if (!placed) {
            printf("Failed to place tank after 1000 attempts\n");
        }
    }
}


void printGameField(char** matrix, int rows, int columns, Tank* players, int playerCount) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    static char** prevMatrix = NULL; // Statická proměnná pro uchování předchozího stavu herního pole
    static int prevRows = 0, prevColumns = 0; // Velikosti předchozího herního pole

    // Inicializace prevMatrix poprvé
    if (prevMatrix == NULL) {
        prevMatrix = (char**)malloc(rows * sizeof(char*));
        for (int i = 0; i < rows; i++) {
            prevMatrix[i] = (char*)malloc(columns * sizeof(char));
        }
        prevRows = rows;
        prevColumns = columns;
    }

    system("cls"); // Vyčištění konzole

    // Porovnání a vykreslení změn
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            char currentCell = matrix[i][j];
            char prevCell = prevMatrix[i][j];

            // Pokud je buňka odlišná od předchozího stavu, vykreslíme ji
            if (currentCell != prevCell) {
                SetConsoleCursorPosition(hConsole, (COORD) { .X = j, .Y = HEIGHT - i });
                if (currentCell == 'T') {
                    // Vykreslení tanku hráče
                    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                    printf("T");
                }
                else if (currentCell == '*') {
                    // Vykreslení terénu
                    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    printf("#");
                }
                else {
                    // Vykreslení vzduchu nebo jiného znaku
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                    printf("%c", currentCell);
                }

                // Aktualizujeme předchozí buňku na aktuální hodnotu
                prevMatrix[i][j] = currentCell;
            }
        }
        printf("\n");
    }

    // Resetování barvy textu na výchozí
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}



void updateGameField(char** matrix, Tank* players, int playerCount) {
    for (int i = 0; i < playerCount; i++) {
        if (!players[i].isHit) {
            playerTurn(&players[i], matrix, playerCount, players);
        }
    }
}

void playerTurn(Tank* currentPlayer, char** matrix, int playerCount, Tank* players) {
    printf("Player %s's turn.\n", currentPlayer->name);
    fireProjectile(currentPlayer, matrix, playerCount, players);
}

void fireProjectile(Tank* tank, char** matrix, int playerCount, Tank* players) {
    int power;
    float angle;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    printf("Enter power (1-5): ");
    scanf("%d", &power);
    printf("Enter angle (0-180): ");
    scanf("%f", &angle);

    float radian = angle * M_PI / 180;
    float xVelocity = power * cos(radian);
    float yVelocity = power * sin(radian);
    float time = 0.0;
    int prevX = 0, prevY = 0;

    // Vypočítat dynamický interval spánku na základě síly střely
    // Power 1 = nejpomalejší, Power 5 = nejrychlejší
    int sleepDuration = 110 - (power * 20); // Můžete upravit koeficienty pro jemné ladění

    // Vykreslit střelu jednou před smyčkou, aby se zamezilo proklikávání
    SetConsoleCursorPosition(hConsole, (COORD) { .X = tank->xPosition, .Y = HEIGHT - tank->yPosition });
    printf("*");

    while (true) {
        time += 0.1;
        int currentX = tank->xPosition + (int)(xVelocity * time);
        int currentY = tank->yPosition + (int)(yVelocity * time - 0.5 * GRAVITY * time * time);

        if (currentX < 1 || currentX >= WIDTH - 1 || currentY < 1 || currentY >= HEIGHT - 1) {
            break;
        }

        // Pokud je nová pozice stejná jako stará, nemusíme nic dělat
        if (currentX == prevX && currentY == prevY) {
            continue;
        }

        // Vymazat starou pozici střely
        SetConsoleCursorPosition(hConsole, (COORD) { .X = prevX, .Y = HEIGHT - prevY });
        printf(" ");

        // Vykreslit novou pozici střely
        SetConsoleCursorPosition(hConsole, (COORD) { .X = currentX, .Y = HEIGHT - currentY });
        printf("*");

        prevX = currentX;
        prevY = currentY;
        Sleep(sleepDuration); // Interval spánku je nyní závislý na síle
    }

    // Vymazat poslední pozici střely
    SetConsoleCursorPosition(hConsole, (COORD) { .X = prevX, .Y = HEIGHT - prevY });
    printf(" ");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}



void destroyTerrain(char** matrix, int x, int y, int rows, int columns) {
    const int radius = 1;
    for (int i = y - radius; i <= y + radius; i++) {
        for (int j = x - radius; j <= x + radius; j++) {
            if (i >= 0 && i < rows && j >= 0 && j < columns) {
                matrix[i][j] = ' ';
            }
        }
    }
}

void checkAndMoveTanks(char** matrix, Tank* players, int playerCount, int rows, int columns) {
    for (int i = 0; i < playerCount; i++) {
        while (players[i].yPosition < rows - 1 && matrix[players[i].yPosition + 1][players[i].xPosition] == ' ') {
            players[i].yPosition++;
        }
    }
}


void animateExplosion(char** matrix, int x, int y, int rows, int columns, Tank* players, int playerCount) {
    const int radius = 2; // Rozsah výbuchu
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int i = y - radius; i <= y + radius; i++) {
        for (int j = x - radius; j <= x + radius; j++) {
            if (i >= 0 && i < rows && j >= 0 && j < columns) {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                matrix[i][j] = 'X'; 
            }
        }
    }

    printGameField(matrix, rows, columns, players, playerCount);
    Sleep(500); // Pauza pro animaci výbuchu

    // Resetování původního stavu herního pole
    for (int i = y - radius; i <= y + radius; i++) {
        for (int j = x - radius; j <= x + radius; j++) {
            if (i >= 0 && i < rows && j >= 0 && j < columns) {
                matrix[i][j] = ' ';
            }
        }
    }

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}




bool checkForHits(int x, int y, Tank* players, int playerCount) {
    for (int i = 0; i < playerCount; i++) {
        if (players[i].xPosition == x && players[i].yPosition == y && !players[i].isHit) {
            players[i].isHit = true;
            return true;
        }
    }
    return false;
}

void endGame(Tank* players, int playerCount) {
    int aliveTanks = 0;
    Tank* winner = NULL;
    for (int i = 0; i < playerCount; i++) {
        if (!players[i].isHit) {
            aliveTanks++;
            winner = &players[i];
        }
    }
    if (aliveTanks == 1) {
        printf("Game over, player %s wins!\n", winner->name);
    }
}
