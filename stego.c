#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Provides text-encoding and decoding capabilities for images. Due to the nature of the algo (edits to bits),
 * regular image editors may be unable to view the encoded image. However, most BMP image will
 * retain their appearance, and hence BMP is the suggested image type to be used if appearance matters.
 * Both the length of the message and the message are stored contiguously
 * as LSBs starting the 24th bit. The length is stored over 32 bits and the actual message is stored after that.
 *
 * Instructions:
 *
 * Add an executable of stego.c to the PATH of your machine.
 *
 * Application Programming Interface:
 *
 * stego encode --msg <your message to encode> --src <your source image> --dest <the destination location>
 *
 * stego decode <your source image>
 *
 * Rohan Talkad
 */

/* Index of bit where we start encoding the length and message */
#define OFFSET 24

typedef unsigned char BYTE;

/**
 * @return the byte form of the given file.
 */
BYTE *ftob(FILE *str);

/**
 * @return the byte form of the given string.
 */
BYTE *stob(char *str);

/**
 * @return the byte form of the given integer.
 */
BYTE *itob(int num);

/**
 * Encodes the byte representation of a string into the image.
 * Note the 32 bit length limit to the target text.
 */
void encode(BYTE *strb, BYTE *img, int offset);

/**
 * @return the string encoded in the given image, if it exists.
 */
char *decode(BYTE *img);

/**
 * @return the number of bytes in this file.
 */
int get_num_bytes(FILE *f);

/**
 * Copies the bytes from fsrc to fdest.
 */
void fcpy(FILE *fsrc, FILE *fdest);

void encode_driver(char *str, char *filename, char *destname);

void decode_driver(char *filename);

int get_msg_length(const BYTE *img, int end_bit);

void load_str_bytes(const BYTE *img, int msg_length, int end_bit, BYTE *str_bytes);

int main(int argc, char *argv[])
{
    encode_driver("my text", "port.png", "temp.png");
    decode_driver("temp.png");
}

int get_num_bytes(FILE *f)
{
    int bytes = 0;
    while ( fgetc(f) != EOF)
    {
        bytes++;
    }
    printf("File with size %d b \n", bytes);
    return bytes;
}

void fcpy(FILE *ipf, FILE *opf)
{
    int c;
    while ((c = fgetc(ipf)) != EOF)
    {
        fputc(c, opf);
    }
}

void decode_driver(char *filename)
{
    FILE *fsrc = fopen(filename, "rb");
    BYTE* bytes = ftob(fsrc);
    char* str = decode(bytes);
    fclose(fsrc);
    printf("%s", str);
}


void encode_driver(char *str, char *filename, char *destname)
{
    FILE *fsrc = fopen(filename, "rb");
    int num_bytes = get_num_bytes(fsrc);

    FILE *fdest = fopen(destname, "wb");

    fseek(fsrc, 0L, SEEK_SET);
    fcpy(fsrc, fdest);
    fclose(fsrc);

    BYTE *img_bytes = (BYTE *) malloc(sizeof(BYTE) * num_bytes);

    fread(img_bytes, sizeof(img_bytes), 1, fdest);

    BYTE *len_bytes = itob(strlen(str));

    printf("%d \n", len_bytes[3]);
    printf("%d \n", len_bytes[4]);


    encode(len_bytes, img_bytes, OFFSET);

    BYTE *str_bytes = stob(str);
    encode(str_bytes, img_bytes, OFFSET + 32);

    fclose(fdest);
}

void encode(BYTE *strb, BYTE *img, int offset)
{
    int len = sizeof(strb);
    for ( int i = 0; i < len; i++ )
    {
        int addition = strb[i];
        for ( int b = 7; b >= 0; b--, offset++ )
        {
            int additionBit = (addition >> b) & 1;
            if ((additionBit != img[offset]) & 1 )
                img[offset] = (img[offset] & 0xFE) | additionBit;
        }
    }
}

BYTE *itob(int num)
{
    BYTE *bytes = (BYTE *) malloc(sizeof(BYTE) * 4);
    bytes[0] = (num & 0xFF000000) >> 24;
    bytes[1] = (num & 0x00FF0000) >> 16;
    bytes[2] = (num & 0x0000FF00) >> 8;
    bytes[3] = num & 0x000000FF;
    return bytes;
}

BYTE *ftob(FILE *file)
{
    int num_bytes = get_num_bytes(file);
    BYTE *bytes = (BYTE *) malloc(num_bytes * sizeof(BYTE));
    if ( bytes == NULL)
    {
        printf("Cannot allocate memory for byte array conversion!");
        exit(1);
    }
    for ( int i = 0, c; (c = fgetc(file)) != EOF; i++ )
    {
        printf("%d", c);
        bytes[i] = c;
    }
    return bytes;
}

BYTE *stob(char *str)
{
    BYTE *bytes = (BYTE *) malloc(sizeof(BYTE) * strlen(str));
    if ( bytes == NULL)
    {
        printf("Cannot allocate memory for byte array conversion!");
        exit(1);
    }
    int i = -1;
    while ( str[++i] != '\0' )
    {
        bytes[i] = str[i];
    }
    return bytes;
}

char *decode(BYTE *img)
{
    int end_bit = OFFSET + 32;
    int msg_length = get_msg_length(img, end_bit);
    BYTE *str_bytes = (BYTE *) malloc(sizeof(BYTE) * msg_length);
    if ( str_bytes == NULL)
    {
        printf("Failed to allocate memory while decoding!");
        exit(1);
    }
    load_str_bytes(img, msg_length, end_bit, str_bytes);
    char *str = (char *) malloc(sizeof(char) * msg_length);
    if ( str == NULL)
    {
        printf("Cannot allocate memory for decoded string!");
        exit(1);
    }
    while ( str_bytes != NULL)
    {
        *str++ = (char) *str_bytes++;
    }
    free(str_bytes);
    return str;
}

void load_str_bytes(const BYTE *img, int msg_length, int end_bit, BYTE *str_bytes)
{
    for ( int j = 0; j < msg_length; j++ )
    {
        for ( int k = 0; k < 8; k++, end_bit++ )
        {
            str_bytes[j] = (str_bytes[j] << 1) | (img[end_bit] & 1);
        }
    }
}

int get_msg_length(const BYTE *img, int end_bit)
{
    int msg_length = 0;
    for ( int i = OFFSET; i < end_bit; i++ )
    {
        msg_length = (msg_length << 1) | (img[i] & 1);
    }
    return msg_length;
}
