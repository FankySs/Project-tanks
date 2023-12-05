// Tanks.h: Soubor k zahrnutí pro standardní systémové soubory k zahrnutí
// nebo soubory k zahrnutí specifické pro projekt.

#pragma once

#include <iostream>
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

