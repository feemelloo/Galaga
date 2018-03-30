#include <stdio.h>
#include "myLib.h"

unsigned short *videoBuffer = (unsigned short *) 0x6000000;

int main() {
    REG_DISPCTL = MODE3 | BG2_ENABLE;

    // ENTITIES
    struct Spaceship s = {16, 140, 88, 88};

    struct RedEnemy r[4];
    for (int i = 0; i < 4; i++) {
        RedEnemy re;
        re.size = 16;
        re.row = -15;
        re.col = 45 + 44 * i;
        re.oldrow = -15;
        r[i] = re;
    }

    struct YellowEnemy y[2];
    for (int i = 0; i < 2; i++) {
        YellowEnemy ye;
        ye.size = 16;
        ye.row = -15;
        ye.col = 89 + 44 * i;
        ye.oldrow = -15;
        ye.oldcol = 89 + 44 * i;
        ye.direction = 1 - 2 * i;
        y[i] = ye;
    }

    struct Bullet b = {5, 3, 160, 0, 160};

    struct Life l1 = {8, 5, 205};
    struct Life l2 = {8, 5, 216};
    struct Life l3 = {8, 5, 227};

    // MARKERS
    int counter = 0;
    int lives = 3;
    int hit = 0;
    int score = 0;
    char buffer[41];

    while (1) {
        waitForVblank();

        if (score == 0 || KEY_DOWN_NOW(BUTTON_SELECT)) {    // START
            score = 0;

            for (int i = 0; i < 240 * 160; i++) {
                videoBuffer[i] = bg[i];
            }

            sprintf(buffer, "PRESS START");
            drawString(105, 78, buffer, YELLOW);

            while (!KEY_DOWN_NOW(BUTTON_START));

            sprintf(buffer, "%d", score - 1);
            drawString(5, 5, buffer, YELLOW);
            fillScreen(BLACK);
            drawRect(0, 40, 160, 160, BLUE);
            drawImage(l1.row, l1.col, l1.size, l1.size, life);
            drawImage(l2.row, l2.col, l2.size, l2.size, life);
            drawImage(l3.row, l3.col, l3.size, l3.size, life);

            score++;

            for (int i = 0; i < 4; i++) {
                r[i].row = -15;
                r[i].oldrow = -15;
                r[i].size = 16;
            }

            for (int i = 0; i < 2; i++) {
                y[i].row = -15;
                y[i].oldrow = -15;
                y[i].col = 89 + 44 * i;
                y[i].oldcol = y[i].col;
                y[i].size = 16;
            }
            
            s.col = 120;
            s.oldcol = s.col;
        } else if (KEY_DOWN_NOW(BUTTON_RIGHT) || KEY_DOWN_NOW(BUTTON_R)) {    // MOVE RIGHT
            s.col += 3;
        } else if (KEY_DOWN_NOW(BUTTON_LEFT) || KEY_DOWN_NOW(BUTTON_L)) {     // MOVE LEFT
            s.col -= 3;
        } else if (KEY_DOWN_NOW(BUTTON_A) && b.row == 160) {    // SHOOT
            b.row = s.row - 5;
            b.col = s.col + 6;
            drawRect(b.row, b.col, b.height, b.width, YELLOW);
        }

        if (b.row != 160 && counter % 3 == 0) {     //BULLET MOVEMENT
            b.oldrow = b.row;
            b.row -= 5;
            drawRect(b.oldrow, b.col, b.height, b.width, BLUE);
            drawRect(b.row, b.col, b.height, b.width, YELLOW);
        }

        if ((b.row < r[0].row + 16 && b.row > r[0].row - 10)) {     // BULLET HIT
            for (int i = 0; i < 4; i++) {
                if ((b.col > r[i].col && b.col < r[i].col + 16) && r[i].size != 0) {
                    drawSquare(r[i].row, r[i].col, r[i].size, BLUE);
                    r[i].size = 0;
                    hit = 1;
                    break;
                }
            }
            if (hit == 1) {
                drawRect(b.row, b.col, b.height, b.width, BLUE);
                b.row = 160;
                drawRect(0, 0, 20, 40, BLACK);
                score++;
                sprintf(buffer, "%d", score - 1);
                drawString(5, 5, buffer, YELLOW);
                hit = 0;
            }
        }

        if (counter % 8 == 0) {    // ENEMY MOVEMENT
            for (int i = 0; i < 4; i++) {    
                r[i].row += 3;
                drawSquare(r[i].oldrow, r[i].col, r[i].size, BLUE);
            }
        }

        // SCREEN BOUNDARIES
        if (r[0].row > 160) {
            for (int i = 0; i < 4; i++) {
                r[i].row = 0;
                r[i].oldrow = 0;
            }
        }

        if (y[0].row > 160 || y[1].row > 160) {
            for (int i = 0; i < 2; i++) {
                y[i].row = 0;
                y[i].oldrow = 0;
            }
        }

        if ((y[0].col < 45 || y[0].col > 195 - y[0].size) || (y[1].col < 45 || y[1].col > 195 - y[1].size)) {
            y[0].direction *= -1;
            y[1].direction *= -1;
        }

        if (s.col < 45) {
            s.col = 45;
        } else if (s.col > 195 - s.size) {
            s.col = 195 - s.size;
        }

        if (b.row < 15) {
            drawRect(b.row, b.col, b.height, b.width, BLUE);
            b.row = 160;
        }

        for (int i = 0; i < 4; i++) {   // DRAW RED ENEMIES
            drawImage(r[i].row, r[i].col, r[i].size, r[i].size, red);
            r[i].oldrow = r[i].row;
        }

        drawSquare(s.row, s.oldcol, s.size, BLUE);
        drawImage(s.row, s.col, s.size, s.size, spaceship);     // DRAW SPACESHIP
        s.oldcol = s.col;

        counter++;

        if ((r[0].row > 130 && r[0].row < 155) && ((s.col >= 29 && s.col <= 61 && r[0].size != 0)
            || (s.col >= 73 && s.col <= 105 && r[1].size != 0) || (s.col >= 117 && s.col <= 149 && r[2].size != 0)
            || (s.col >= 161 && s.col <= 193 && r[3].size != 0))) {     // COLLISION
            lives--;
            if (lives == 2) {
                drawSquare(l1.row, l1.col, l1.size, BLACK);
            } else if (lives == 1) {
                drawSquare(l2.row, l2.col, l2.size, BLACK);
            } else {
                drawSquare(l3.row, l3.col, l3.size, BLACK);
            }

            for (int i = 0; i < 4; i++) {    
                drawSquare(r[i].row, r[i].col, r[i].size, BLUE);
                r[i].row = -15;
                r[i].oldrow = -15;
            }
        }

        if (lives == 0) {      // GAME OVER
            fillScreen(BLACK);
            drawImage(10, 53, 133, 78, game_over);
            sprintf(buffer, "Score: %d", score - 1);
            drawString(100, 90, buffer, YELLOW);
            sprintf(buffer, "Press A to RESTART");
            drawString(120, 60, buffer, YELLOW);

            while(!KEY_DOWN_NOW(BUTTON_A));

            score = 0;
            lives = 3;
        }

        if (r[0].size == 0 && r[1].size == 0 && r[2].size == 0 && r[3].size == 0) {     // LEVEL 2
            if (counter % 5 == 0) {
                for (int i = 0; i < 2; i++) {
                    y[i].row += 2;
                    y[i].col += 2 * y[i].direction;
                    drawSquare(y[i].oldrow, y[i].oldcol, y[i].size, BLUE);
                }
                for (int i = 0; i < 2; i++) {   // DRAW YELLOW ENEMIES
                    drawImage(y[i].row, y[i].col, y[i].size, y[i].size, yellow);
                    y[i].oldrow = y[i].row;
                    y[i].oldcol = y[i].col;
                }
            }

            if ((b.row < y[0].row + 16 && b.row > y[0].row - 10)) {     // BULLET HIT
                for (int i = 0; i < 2; i++) {
                    if ((b.col > y[i].col && b.col < y[i].col + 16) && y[i].size != 0) {
                        drawSquare(y[i].row, y[i].col, y[i].size, BLUE);
                        y[i].size = 0;
                        hit = 1;
                        break;
                    }
                }
                if (hit == 1) {
                    drawRect(b.row, b.col, b.height, b.width, BLUE);
                    b.row = 160;
                    drawRect(0, 0, 20, 40, BLACK);
                    score += 2;
                    sprintf(buffer, "%d", score - 1);
                    drawString(5, 5, buffer, YELLOW);
                    hit = 0;
                }
            }

            if ((y[0].row > 130 && y[0].row < 155) && ((s.col > y[0].col - 16 && s.col < y[0].col + 16 && y[0].size != 0)
                || (s.col > y[1].col - 16 && s.col < y[1].col + 16 && y[1].size != 0))) {     // COLLISION
                lives--;
                if (lives == 2) {
                    drawSquare(l1.row, l1.col, l1.size, BLACK);
                } else if (lives == 1) {
                    drawSquare(l2.row, l2.col, l2.size, BLACK);
                } else {
                    drawSquare(l3.row, l3.col, l3.size, BLACK);
                }

                for (int i = 0; i < 2; i++) {    
                    drawSquare(y[i].row, y[i].col, y[i].size, BLUE);
                    y[i].row = -15;
                    y[i].oldrow = -15;
                    y[i].col = 89 + 44 * i;
                    y[i].oldcol = y[i].col;
                }
            }
        }

        if (y[0].size == 0 && y[1].size == 0) {    // VICTORY
            fillScreen(BLACK);
            drawImage(25, 4, 232, 34, victory);
            sprintf(buffer, "Score: %d", score - 1);
            drawString(100, 90, buffer, YELLOW);
            sprintf(buffer, "Press A to RESTART");
            drawString(120, 60, buffer, YELLOW);

            while(!KEY_DOWN_NOW(BUTTON_A));

            score = 0;
            lives = 3;
        }
    }
}