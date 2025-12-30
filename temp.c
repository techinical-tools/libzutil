#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "include/utlz/maths.h"

#define WIDTH 10000
#define HEIGHT 10000
#define NUM_STARS 500 // 500 massive 100x100 suns

void draw_massive_star(unsigned char* img, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    int size = 100;
    int half = size / 2;
    
    for (int dy = -half; dy <= half; dy++) {
        for (int dx = -half; dx <= half; dx++) {
            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                size_t idx = ((size_t)ny * WIDTH + nx) * 3;
                
                float dist = sqrtf((float)(dx * dx + dy * dy));
                float normalized_dist = dist / half;

                if (normalized_dist <= 1.0f) {
                    float alpha = 1.0f - powf(normalized_dist, 0.4f); 
                    float core_glow = powf(alpha, 5.0f);
                    
                    // Mixing colors into the buffer
                    for(int c=0; c<3; c++) {
                        unsigned char color = (c==0) ? r : (c==1) ? g : b;
                        int val = img[idx+c] + (int)(color * alpha + 255 * core_glow);
                        img[idx+c] = (val > 255) ? 255 : (unsigned char)val;
                    }
                }
            }
        }
    }
}

int main() {
    printf("Allocating 300MB for the void...\n");
    unsigned char* image = (unsigned char*)calloc(WIDTH * (size_t)HEIGHT * 3, 1);
    if (!image) return 1;

    srandom(1337);

    printf("Igniting %d suns...\n", NUM_STARS);
    for (int i = 0; i < NUM_STARS; i++) {
        uint32_t x = random(PRNG) % WIDTH;
        uint32_t y = random(PRNG) % HEIGHT;
        
        unsigned char r = (random(PRNG) % 155) + 100;
        unsigned char g = (random(PRNG) % 155) + 100;
        unsigned char b = (random(PRNG) % 155) + 100;
        
        draw_massive_star(image, x, y, r, g, b);
        if (i % 50 == 0) printf("Progress: %d%%\n", (i * 100) / NUM_STARS);
    }

    printf("Writing Starfield_Gigantic.ppm (this will take a moment)...\n");
    FILE *f = fopen("starfield_gigantic.ppm", "wb");
    fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
    fwrite(image, 1, WIDTH * (size_t)HEIGHT * 3, f);
    fclose(f);

    free(image);
    printf("Done! Prepare your image viewer for 100 megapixels.\n");
    return 0;
}