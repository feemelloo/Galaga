#include "myLib.h"

void drawRect(int row, int col, int height, int width, unsigned short color) {
    for(int r = 0; r < height; r++) {
        DMA[3].src =  &color;
        DMA[3].dst =  &videoBuffer[OFFSET(row+r, col, 240)];
        DMA[3].cnt = (width) | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED;
    }
}

void drawSquare(int row, int col, int size, unsigned short color) {
    drawRect(row, col, size, size, color);
}

void drawImage(int row, int col, int width, int height, const unsigned short* image) {
    for (int x = 0; x < height; x++) {
        DMA[3].src = &image[OFFSET(x, 0, width)];
        DMA[3].dst = &videoBuffer[OFFSET(row + x, col, 240)];
        DMA[3].cnt = (width) | DMA_ON;
    }
}

void drawChar(int row, int col, char ch, unsigned short color) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 6; c++) {
            if (font[OFFSET(r, c, 6) + ch * 48] == 1) {
                videoBuffer[OFFSET(r + row, c + col, 240)] = color;
            }
        }
    }
}

void drawString(int row, int col, char str[], unsigned short color) {
    while (*str) {
        drawChar(row, col, *str++, color);
        col += 6;
    }
}

void fillScreen(unsigned short color) {
    DMA[3].src =  &color;
    DMA[3].dst =  &videoBuffer[0];
    DMA[3].cnt = (240 * 160) | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED;
}

void waitForVblank() {
    while (SCANLINECOUNTER > 160);
    while (SCANLINECOUNTER < 160);
}