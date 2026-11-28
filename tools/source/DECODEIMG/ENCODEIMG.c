// ENCODE_IMG - Pokewalker image encoder
// (c) 2020 DmitryGR, modifications by BluRose. GPLv3 license.

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <png.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static size_t parseSize(const char *text)
{
    if (!isdigit((unsigned char)*text))
        return 0;
    char *end;
    errno = 0;
    uintmax_t value = strtoumax(text, &end, text[0] == '0' && (text[1] == 'x' || text[1] == 'X') ? 16 : 10);
    return errno || *end || value > SIZE_MAX || value % 2 ? 0 : (size_t)value;
}

// Both pointers are owned by the caller, including after a libpng read error.
struct Image {
    png_bytep pixels;
    png_bytepp rows;
    size_t width;
    size_t height;
    int indexed;
};

static int readImage(FILE *input, const char *path, size_t capacity, struct Image *image)
{
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
        return 0;
    png_infop info = png_create_info_struct(png);
    if (!info || setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        return 0;
    }
    png_init_io(png, input);
    png_read_info(png, info);
    image->width = png_get_image_width(png, info);
    image->height = png_get_image_height(png, info);
    int type = png_get_color_type(png, info);
    image->indexed = type == PNG_COLOR_TYPE_PALETTE;
    if ((!image->indexed && type != PNG_COLOR_TYPE_GRAY) || png_get_bit_depth(png, info) > 8)
        png_error(png, "Expected a grayscale or indexed PNG of at most 8 bits per pixel");
    if (image->height % 8 || image->width > SIZE_MAX / image->height
        || image->height > SIZE_MAX / sizeof(*image->rows)
        || image->width * image->height / 4 > capacity) {
        fprintf(stderr, "%s: dimensions do not fit the Pokewalker output.\n", path);
        png_destroy_read_struct(&png, &info, NULL);
        return 0;
    }
    png_set_packing(png);
    png_set_interlace_handling(png);
    png_read_update_info(png, info);
    image->pixels = malloc(image->width * image->height);
    image->rows = malloc(image->height * sizeof(*image->rows));
    if (!image->pixels || !image->rows)
        png_error(png, "Out of memory");
    for (size_t y = 0; y < image->height; y++)
        image->rows[y] = image->pixels + y * image->width;
    png_read_image(png, image->rows);
    png_read_end(png, NULL);
    png_destroy_read_struct(&png, &info, NULL);
    return 1;
}

static unsigned grayCode(unsigned pixel)
{
    switch (pixel) {
    case 255: return 0;
    case 192: return 1;
    case 128: return 2;
    case 0:   return 3;
    default:  return 4;
    }
}

static int writeImage(const struct Image *image, const char *source, const char *dest, size_t size)
{
    unsigned char *bytes = calloc(size, 1);
    if (!bytes)
        return 0;
    size_t offset = 0;
    for (size_t y = 0; y < image->height; y += 8) {
        for (size_t x = 0; x < image->width; x++, offset += 2) {
            for (unsigned bit = 0; bit < 8; bit++) {
                unsigned pixel = image->rows[y + bit][x];
                unsigned code = image->indexed ? pixel : grayCode(pixel);
                if (code > 3) {
                    fprintf(stderr, "%s: unsupported Pokéwalker pixel 0x%02X at (%zu, %zu).\n", source, pixel, x, y + bit);
                    free(bytes);
                    return 0;
                }
                bytes[offset] |= (code >> 1) << bit;
                bytes[offset + 1] |= (code & 1) << bit;
            }
        }
    }
    FILE *output = fopen(dest, "wb");
    if (!output) {
        perror(dest);
        free(bytes);
        return 0;
    }
    int ok = fwrite(bytes, 1, size, output) == size;
    free(bytes);
    if (fclose(output) != 0 || !ok) {
        perror(dest);
        struct stat status;
        if (stat(dest, &status) == 0 && S_ISREG(status.st_mode))
            remove(dest);
        return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    size_t size = argc == 4 ? parseSize(argv[1]) : 0;
    if (!size) {
        fprintf(stderr, "Usage: %s <even byte count> file.png file.pw2bpp\n", argv[0]);
        return EXIT_FAILURE;
    }
    FILE *input = fopen(argv[2], "rb");
    if (!input) {
        perror(argv[2]);
        return EXIT_FAILURE;
    }
    struct Image image = {0};
    int ok = readImage(input, argv[2], size, &image);
    fclose(input);
    if (ok)
        ok = writeImage(&image, argv[2], argv[3], size);
    free(image.rows);
    free(image.pixels);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
