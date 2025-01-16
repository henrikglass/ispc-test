#define HGL_PROFILE_IMPLEMENTATION
#include "hgl_profile.h"

#include <stdio.h>
#include <stdlib.h>

extern void mandelbrot_serial(float x0, float y0, float x1, float y1, int width, int height, int maxIterations, int output[]);
void mandelbrot_ispc(float x0, float y0, float x1, float y1, int width, int height, int maxIterations, int output[]);
void mandelbrot_ispc2(float x0, float y0, float x1, float y1, int width, int height, int maxIterations, int output[]);


/* Write a PPM image file with the image of the Mandelbrot set */
static void writePPM(int *buf, int width, int height, const char *fn) {
    FILE *fp = fopen(fn, "wb");
    if (!fp) {
        printf("Couldn't open a file '%s'\n", fn);
        exit(-1);
    }
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    for (int i = 0; i < width * height; ++i) {
        // Map the iteration count to colors by just alternating between
        // two greys.
        char c = (buf[i] & 0x1) ? (char)240 : 20;
        for (int j = 0; j < 3; ++j)
            fputc(c, fp);
    }
    fclose(fp);
    printf("Wrote image file %s\n", fn);
}

int main()
{
    printf("Hello World!\n");
    int n_iter = 100;

    unsigned int width = 768;
    unsigned int height = 512;
    float x0 = -2;
    float x1 = 1;
    float y0 = -1;
    float y1 = 1;

    int maxIterations = 256;
    int *buf = malloc(sizeof(int) * width * height);

    hgl_profile_begin("serial");
    for (int i = 0; i < n_iter; i++) {
        mandelbrot_serial(x0, y0, x1, y1, width, height, maxIterations, buf);
    }
    hgl_profile_end();

    writePPM(buf, width, height, "mandelbrot_serial.ppm");

    hgl_profile_begin("ispc single thread");
    for (int i = 0; i < n_iter; i++) {
        mandelbrot_ispc(x0, y0, x1, y1, width, height, maxIterations, buf);
    }
    hgl_profile_end();

    writePPM(buf, width, height, "mandelbrot_ispc.ppm");

    hgl_profile_begin("ispc multi-thread");
    for (int i = 0; i < n_iter; i++) {
        mandelbrot_ispc2(x0, y0, x1, y1, width, height, maxIterations, buf);
    }
    hgl_profile_end();

    writePPM(buf, width, height, "mandelbrot_ispc2.ppm");

    hgl_profile_report(HGL_PROFILE_TIME_LAST);

    double serial_ms     = hgl_profile_get("serial")->time_ns_last;
    double ispc_ms       = hgl_profile_get("ispc single thread")->time_ns_last;
    double ispc_multi_ms = hgl_profile_get("ispc multi-thread")->time_ns_last;
    
    printf("ispc vs serial = %f%% speedup.\n", 100.0 * serial_ms / ispc_ms);
    printf("ispc_multi_ms vs ispc = %f%% speedup.\n", 100.0 * ispc_ms / ispc_multi_ms);
    printf("ispc_multi_ms vs serial = %f%% speedup.\n", 100.0 * serial_ms / ispc_multi_ms);

}
