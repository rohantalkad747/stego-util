#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Provides text-encoding and decoding capabilities for images. Due to the modifications,
 * regular image editors are sometimes unable to view the image. However, all BMP files will
 * retain their appearance, and hence BMP is the suggested image type to be used.
 * Both the length of the message and the message are stored contiguously
 * as LSBs starting the 24th bit, although this can be modified. The length is stored
 * over 32 bits and the actual message is stored after that.
 *
 * Simply move an executable verson file into usr/local/bin.
 *
 * Application Programming Interface:
 *
 * stego encode --msg <your message to encode> --src <your source image> --dest <the destination location>
 *
 * stego decode <your source image>
 *
 *
 */

/* Index of bit where we start encoding the length and message */
#define OFFSET 24

typedef unsigned char BYTE;

BYTE* stob(char* str);
BYTE* itob(int num);
void encode(BYTE* strb, BYTE* img, int offset);
char* decode(BYTE* img);
int get_num_bytes(FILE* f);
void encode_driver(char* str, char* filename, char* destname);
void decode_driver(char* filename);
void fcpy(FILE *, FILE *);

int main(int argc, char* argv[]) {
    encode_driver("my text", "port.png", "temp.png");
}

int get_num_bytes(FILE* f) {
    int bytes = 0;
    while (fgetc(f) != EOF)
        bytes++;
    return bytes;
}

void fcpy(FILE *ipf, FILE *opf)
{
    int c;
    while ((c = fgetc(ipf)) != EOF)
        fputc(c, opf);
}

void decode_driver(char* filename) {
//    FILE* fsrc = fopen(filename, "rb");
//    decode(fsrc);
//    fclose(fsrc);
}


void encode_driver(char* str, char* filename, char* destname) {
    FILE* fsrc = fopen(filename, "rb");
    FILE* fdest = fopen(destname, "wb");

    fcpy(fsrc, fdest);

    fclose(fsrc);

    int num_bytes = get_num_bytes(fdest);

    printf("Encode target file with size %d b ", num_bytes);

    BYTE* img_bytes = (BYTE *) malloc (sizeof(BYTE) * num_bytes);
    fread(img_bytes, sizeof(img_bytes),1,fdest);

    int len = strlen(str);
    BYTE* len_bytes = itob(len);
    encode(len_bytes, img_bytes, OFFSET);

    BYTE* str_bytes = stob(str);
    encode(str_bytes, img_bytes, OFFSET + 32);

    fclose(fdest);
}

void encode(BYTE* strb, BYTE* img, int offset) {
    int len = sizeof (strb);
    for (int i = 0; i < len; i++) {
        int addition = strb[i];
        for (int b = 7; b >= 0; b--, offset++) {
            int additionBit = (addition >> b) & 1;
            if ((additionBit != img[offset]) & 1)
                img[offset] = (img[offset] & 0xFE) | additionBit;
        }
    }
}

BYTE* itob(int num) {
    BYTE* bytes = (BYTE*) malloc(sizeof(BYTE) * 4);
    bytes[0] = (num & 0xFF000000) >> 24;
    bytes[1] = (num & 0x00FF0000) >> 16;
    bytes[2] = (num & 0x0000FF00) >> 8;
    bytes[3] = num & 0x000000FF;
    return bytes;
}

BYTE* stob(char* str) {
    BYTE* bytes = (BYTE*) malloc(sizeof(BYTE) * strlen(str));
    if (bytes == NULL) {
        printf("Cannot allocate memory for byte array conversion!");
        exit(1);
    }
    int i = -1;
    while (str[++i] != '\0')
        bytes[i] = str[i];
    return bytes;
}

char* decode(BYTE* img) {
    int msg_length = 0;
    int end_bit = OFFSET + 32;
    for (int i = OFFSET; i < end_bit; i++)
        msg_length = (msg_length << 1) | (img[i] & 1);
    BYTE* str_bytes = (BYTE*) malloc(sizeof(BYTE) * msg_length);
    if (str_bytes == NULL) {
        printf("Failed to allocate memory while decoding!");
        exit(1);
    }
    for (int j = 0; j < msg_length; j++)
        for (int k = 0; k < 8; k++, end_bit++)
            str_bytes[j] = (str_bytes[j] << 1) | (img[end_bit] & 1);
    char* str = (char*) malloc(sizeof(char) * msg_length);
    while (str_bytes != NULL)
        *str++ = (char) *str_bytes++;
    free(str_bytes);
    return str;
}