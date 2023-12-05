#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <conio.h> // Pouze pro Windows!

#define WIDTH 140
#define HEIGHT 40
#define MAX_PLAYERS 5
#define M_PI 3.14159265358979323846
#define GRAVITY 10
#define TERRAIN_CHAR (char)178    
#define BORDER_CHAR (char)219  
#define AIR_CHAR (char)176
#define TANK_CHAR 'T' 
#define EXPLOSION_CHAR 'X' 
#define RADIUS_OF_EXPLOSION 1




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

// Tah jednoho hráče.
void playerTurn(Tank* currentPlayer, char** matrix, int playerCount, Tank* players);

// Hráč vystřelí projektil s určitou silou a úhlem.
void fireProjectile(Tank* tank, char** matrix, int playerCount, Tank* players);

// Kontroluje a pohybuje tanky, pokud se mohou pohybovat dolů.
void checkAndMoveTanks(char** matrix, Tank* players, int playerCount);

// Animuje explozi na dané pozici.
void animateExplosion(char** matrix, int x, int y, int rows, int columns, Tank* players, int playerCount);

// Ukáže výsledky hry a vyhlásí vítěze.
void endGame(Tank* players, int playerCount);

int main() {
    runGameLoop();
    return 0;
}

void runGameLoop() {

    printf("Welcome to the Tank Game!\n");
    printf("Use the menu to set up and start the game.\n");

    printf("System loading...\n");


    while (1) {
        displayMainMenu();
        system("cls"); 
    }
}

void displayMainMenu() {
    int choice = 0;
    int playerCount = 2;
    Tank players[MAX_PLAYERS];

    for (int i = 0; i <= MAX_PLAYERS - 1; i++){
        players[i].name[0] = 'P';
        players[i].name[1] = 49 + i;
        players[i].name[2] = '\0';
    }

    while (1) {
        Sleep(2000);
        system("cls");
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
            startGame(playerCount, players);
            break; 
        case 4:
            exit(0);
        default:
            system("cls");
            printf("Invalid choice. Please try again.\n");
            Sleep(1500);
            system("cls");
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
    int currentPlayerIndex = 0; // Index pro sledování, čí je tah

    while (isGameRunning) {
        system("cls"); // Vyčistíme konzoli pro nový tah  

        // Tah současného hráče
        if (players[currentPlayerIndex].isHit == false) {
            playerTurn(&players[currentPlayerIndex], gameField, playerCount, players);
        }
        for (int i = 0; i < playerCount; i++) {
            if (gameField[players[i].yPosition][players[i].xPosition] == TANK_CHAR)
            {
                players[i].isHit = false;
            }
            else
            {
                players[i].isHit = true;
            }
            
        }
        // Kontrola, zda zůstal pouze jeden tank
        int aliveTanks = 0;
        for (int i = 0; i < playerCount; i++) {
            if (!players[i].isHit) {
                aliveTanks++;
            }
        }
        if (aliveTanks <= 1) {
            isGameRunning = false;
            endGame(players, playerCount); // Volání funkce endGame, pokud hra skončila
        }

        // Přechod na dalšího hráče
        currentPlayerIndex = (currentPlayerIndex + 1) % playerCount;

        // Krátká pauza před přechodem na dalšího hráče
        Sleep(1000);
    }

    // Uvolnění herního pole
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
        int heightDifference = (rand() % 5) - 2; // Náhodný rozdíl mezi -2 až +2

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
                matrix[i][j] = BORDER_CHAR;
            }
            // Levý a pravý okraj
            else if (j == 0 || j == columns - 1) {
                matrix[i][j] = BORDER_CHAR;
            }
            // Terén a vzduch
            else {
                if (i < heightMap[j]) {
                    matrix[i][j] = TERRAIN_CHAR; // Terén
                }
                else {
                    matrix[i][j] = AIR_CHAR; // Vzduch
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

            // Kontrola, zda je místo volné 
            if (matrix[y][x] == AIR_CHAR && matrix[y + 1][x] != TANK_CHAR) {
                matrix[y][x] = TANK_CHAR; // Umístíme tank hned nad terén
                players[i].xPosition = x;
                players[i].yPosition = y;
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
    COORD cursorPos;

    // Procházení herního pole odspodu nahoru
    for (int i = rows - 1; i >= 0; i--) {
        for (int j = 0; j < columns; j++) {
            cursorPos.X = j;
            cursorPos.Y = (rows - 1) - i;  // Obrátíme osu Y
            SetConsoleCursorPosition(hConsole, cursorPos);

            if (matrix[i][j] == TANK_CHAR) {
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                printf("%c", TANK_CHAR);
            }
            else if (matrix[i][j] == TERRAIN_CHAR) {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                printf("%c", TERRAIN_CHAR);
            }
            else {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                printf("%c", matrix[i][j]);
            }
        }
        printf("\n");
    }

    // Resetování barvy textu na výchozí po dokončení vykreslování
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    cursorPos.X = 0;
    cursorPos.Y = rows;
    SetConsoleCursorPosition(hConsole, cursorPos);
}

void playerTurn(Tank* currentPlayer, char** matrix, int playerCount, Tank* players) {
    // Hráč vystřelí projektil
    fireProjectile(currentPlayer, matrix, playerCount, players);
}

void fireProjectile(Tank* tank, char** matrix, int playerCount, Tank* players) {
    int power;
    float angle;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPos;

    checkAndMoveTanks(matrix, players, playerCount);
    printGameField(matrix, HEIGHT, WIDTH, players, playerCount);

    printf("\nPlayer %s's turn.\n", tank->name);

    bool validInput = false;
    // Ošetření vstupu pro 'power'
    while (!validInput) {
        printf("Enter power (1-5): ");
        if (scanf("%d", &power) == 1) { // Kontrola, zda byl vstup správně načten
            if (power >= 1 && power <= 5) {
                validInput = true; // Vstup je platný
            }
            else {
                printf("Invalid input. Please enter a number between 1 and 5.\n");
            }
        }
        else {
            printf("Invalid input. Please enter a numeric value.\n");
        }
        while (getchar() != '\n'); // Vyčistit buffer
    }

    validInput = false; // Resetovat indikátor platnosti pro další vstup

    // Ošetření vstupu pro 'angle'
    while (!validInput) {
        printf("Enter angle (0-180): ");
        if (scanf("%f", &angle) == 1) { // Kontrola, zda byl vstup správně načten
            if (angle >= 0.0f && angle <= 180.0f) {
                validInput = true; // Vstup je platný
            }
            else {
                printf("Invalid input. Please enter a number between 0 and 180.\n");
            }
        }
        else {
            printf("Invalid input. Please enter a numeric value.\n");
        }
        while (getchar() != '\n'); // Vyčistit buffer
    }

    float radian = angle * (M_PI / 180);

    // Výpočet rychlosti ve směrech X a Y
    float xVelocity = 0;
    float yVelocity = 0;

    switch (power) {
    case 1:
        xVelocity = 10 * power * cos(radian);
        yVelocity = 10 * power * -sin(radian);
        break;
    case 2:
        xVelocity = 8 * power * cos(radian);
        yVelocity = 8 * power * -sin(radian);
        break;
    case 3:
        xVelocity = 8 * power * cos(radian);
        yVelocity = 8 * power * -sin(radian);
        break;
    case 4:
        xVelocity = 7 * power * cos(radian);
        yVelocity = 7 * power * -sin(radian);
    case 5:
        xVelocity = 7 * power * cos(radian);
        yVelocity = 7 * power * -sin(radian);
    }

    float time = 0.0;

    // Upravit počáteční pozici projektilu
    int offsetX = cos(radian); // Offset na základě úhlu
    int offsetY = sin(radian); // Offset na základě úhlu
    int puvX = tank->xPosition;
    int puvY = tank->yPosition + 2;
    int prevX = tank->xPosition; // Projektil začíná na stejné x-pozici jako tank
    int prevY = tank->yPosition + 2; // Projektil začíná výše než tank

    while (true) {
        time += 0.05;
        int nextX = puvX + (int)(xVelocity * time);
        int nextY = puvY - (int)(yVelocity * time) - (- 0.5 * -GRAVITY * time * time);

        // Zajištění, že pozice projektilu jsou v rámci herního pole
        if (nextX < 1 || nextX >= WIDTH - 1 || nextY < 1 || nextY >= HEIGHT - 1) {
            // Projektil je mimo hranice herního pole, ukončí smyčku a tah hráče
            break;
        }

        // Kontrola, zda střela zasáhla hranici herního pole, terén nebo tank
        if (matrix[nextY][nextX] == BORDER_CHAR || matrix[nextY][nextX] == TERRAIN_CHAR || matrix[nextY][nextX] == TANK_CHAR) {
            // V případě zásahu terénu nebo tanku provedeme výbuch
            animateExplosion(matrix, nextX, nextY, HEIGHT, WIDTH, players, playerCount);
            break; // Ukončí smyčku a tah hráče
        }

        // Vymazání projektilu z předchozí pozice, pokud se přesunul
        if ((nextX != prevX || nextY != prevY) && matrix[prevY][prevX] != TANK_CHAR) {
            // Pouze pokud na předchozí pozici nebyl tank
            matrix[prevY][prevX] = AIR_CHAR;
        }

        // Vykreslení projektilu na nové pozici
        cursorPos.X = nextX;
        cursorPos.Y = HEIGHT - 1 - nextY;
        SetConsoleCursorPosition(hConsole, cursorPos);
        printf("%c", TERRAIN_CHAR);

        // Aktualizace pro další iteraci
        prevX = nextX;
        prevY = nextY;

        Sleep(20);
    }

    // Po skončení smyčky vymažeme střelu
    cursorPos.X = prevX;
    cursorPos.Y = HEIGHT - 1 - prevY;
    SetConsoleCursorPosition(hConsole, cursorPos);
    printf("%c", AIR_CHAR);

    // Resetování barvy textu
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void checkAndMoveTanks(char** matrix, Tank* players, int playerCount) {
    for (int i = 0; i < playerCount; i++) {
        while (matrix[players[i].yPosition - 1][players[i].xPosition] == AIR_CHAR) {
            matrix[players[i].yPosition][players[i].xPosition] = AIR_CHAR;
            players[i].yPosition--;
            matrix[players[i].yPosition][players[i].xPosition] = TANK_CHAR;
        }
    }
}

void animateExplosion(char** matrix, int x, int y, int rows, int columns, Tank* players, int playerCount) {
    const int radius = RADIUS_OF_EXPLOSION; // Rozsah výbuchu
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPos;
    for (int i = y - radius; i <= y + radius; i++) {
        for (int j = x - radius; j <= x + radius; j++) {
            if (i >= 0 && i < rows && j >= 0 && j < columns) {
                if (matrix[i][j] != BORDER_CHAR) {

               
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                cursorPos.X = j;
                cursorPos.Y = HEIGHT- i - 1;
                SetConsoleCursorPosition(hConsole,cursorPos);
                printf("%c", EXPLOSION_CHAR);
                }
            }
        }
    }
    
    Sleep(500); // Pauza pro animaci výbuchu

    // Resetování původního stavu herního pole
    for (int i = y - radius; i <= y + radius; i++) {
        for (int j = x - radius; j <= x + radius; j++) {
            if (i >= 0 && i < rows && j >= 0 && j < columns) {
                if (matrix[i][j] != BORDER_CHAR) {
                    matrix[i][j] = AIR_CHAR;

                }
            }
        }
    }

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
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
        system("cls");
        SetCursorPos(0, 0);
        printf("Game over, player %s wins!\n", winner->name);
    }
}
