#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_TABLE 5

#define MAX_KEY 8

#define MAX_DATA 12

#define OFFSET 24

#define TRUE 1

#define FALSE 0

#define ENCODE "encode"

#define DECODE "decode"

#define MESSAGE "msg"

#define SOURCE "src"

#define DEST "dest"

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
void encode(BYTE *strb, int strlen, BYTE *img, int offset);

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

int get_msg_length(BYTE *img, int end_bit);

void load_str_bytes(BYTE *img, int msg_length, int end_bit, char *str_bytes);

void handle_fread_err(FILE *fsrc);

void encode_length_and_message(char *str, char *destname, int num_bytes, BYTE *img_bytes);

void parse_args(char *argv[]);

struct node
{
    char key[MAX_KEY];
    char *value;
    struct node *next;
};

typedef struct node Node;

Node *tb[MAX_TABLE];
char keys[MAX_DATA][MAX_KEY];
int values[MAX_DATA];

void init()
{
    for ( int i = 0; i < MAX_TABLE; ++i )
    {
        Node *cur = tb[i];
        Node *tmp;
        while ( cur != NULL)
        {
            tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        tb[i] = NULL;
    }
}

void my_str_cpy(char *dest, char *src)
{
    while ((*dest++ = *src++) != '\0' )
    {}
}

int my_str_cmp(char *str1, char *str2)
{
    while ( *str1 != '\0' && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

int hash(char *str)
{
    int hash = 401;
    while ( *str != '\0' )
    {
        hash = ((hash << 4) + (int) (*str)) % MAX_TABLE;
        str++;
    }
    return hash % MAX_TABLE;
}

void add(char *key, char *value)
{
    Node *new_node = (Node *) malloc(sizeof(Node));
    my_str_cpy(new_node->key, key);
    new_node->value = value;
    new_node->next = NULL;
    int index = hash(key);
    if ( tb[index] == NULL)
    {
        tb[index] = new_node;
    }
    else
    {
        Node *cur = tb[index];
        while ( cur != NULL)
        {
            if ( my_str_cmp(cur->key, key) == 0 )
            {
                cur->value = value;
                return;
            }
            cur = cur->next;
        }
        new_node->next = tb[index];
        tb[index] = new_node;
    }
}

char *find(char *key)
{
    int index = hash(key);
    Node *cur = tb[index];
    while ( cur != NULL)
    {
        if ( my_str_cmp(cur->key, key) == 0 )
        {
            return cur->value;
        }
        cur = cur->next;
    }

}

int main(int argc, char *argv[])
{
    char *stego_flag = argv[1];
    if ( strcmp(stego_flag, ENCODE) == 0 )
    {
        if ( argc != 8 )
        {
            printf("Expected eight arguments but got %d arguments", argc);
            exit(1);
        }
        parse_args(argv + 2);
        encode_driver(
                find(MESSAGE),
                find(SOURCE),
                find(DEST)
        );
    }
    else if ( strcmp(stego_flag, DECODE) == 0 )
    {
        if ( argc != 3 )
        {
            printf("Expected three arguments but got %d arguments", argc);
            exit(1);
        }
        decode_driver(argv[2]);
    }
    else
    {
        printf("Invalid command: %s", stego_flag);
        exit(1);
    }
}

void parse_args(char *argv[])
{
    init();
    char *c;
    printf("Parsed Arguments \n");
    while ((c = *argv++) != NULL)
    {
        if ( c[0] == '-' && c[1] == '-' )
        {
            add(c + 2, *argv++);
            printf("(%s => %s) \n", c + 2, find(c + 2));
        }
    }
}

int get_num_bytes(FILE *f)
{
    int bytes = 0;
    while ( fgetc(f) != EOF)
    {
        bytes++;
    }
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
    int num_bytes = get_num_bytes(fsrc);
    fseek(fsrc, 0L, SEEK_SET);
    BYTE *bytes = (BYTE *) malloc(num_bytes);
    fread(bytes, 1, num_bytes, fsrc);
    char *str = decode(bytes);
    printf("%s", str);
}


void encode_driver(char *str, char *filename, char *destname)
{
    if ( str == NULL || filename == NULL || destname == NULL)
    {
        printf("encode_driver parameters cannot be null!");
        exit(1);
    }
    FILE *fsrc = fopen(filename, "rb");
    int num_bytes = get_num_bytes(fsrc);
    fseek(fsrc, 0L, SEEK_SET);
    BYTE *img_bytes = (BYTE *) malloc(num_bytes);
    size_t ret_code = fread(img_bytes, 1, num_bytes, fsrc);
    if ( ret_code == num_bytes )
    {
        encode_length_and_message(str, destname, num_bytes, img_bytes);
    }
    else
    {
        handle_fread_err(fsrc);
    }
}

void encode_length_and_message(char *str, char *destname, int num_bytes, BYTE *img_bytes)
{
    int trgt_length = strlen(str);
    BYTE *len_bytes = itob(trgt_length);
    // Length of this encoding is 4 bytes because this is a 32 bit number
    encode(len_bytes, 4, img_bytes, OFFSET);
    BYTE *str_bytes = stob(str);
    encode(str_bytes, trgt_length, img_bytes, OFFSET + 32);
    FILE *fdest = fopen(destname, "wb");
    fwrite(img_bytes, 1, num_bytes, fdest);
}

void handle_fread_err(FILE *fsrc)
{
    if ( ferror(fsrc))
    {
        perror("Error reading file");
    }
    else if ( feof(fsrc))
    {
        perror("EOF found");
    }
    else
    {
        perror("Error opening file");
    }
}

void encode(BYTE *strb, int strlen, BYTE *img, int offset)
{
    printf("Encoding a message of %d bytes \n", strlen);
    for ( int i = 0; i < strlen; i++ )
    {
        int addition = strb[i];
        for ( int b = 7; b >= 0; b--, offset++ )
        {
            int additionBit = (addition >> b) & 1;
            if ((additionBit != img[offset]) & 1 )
            {
                img[offset] = (img[offset] & 0xFE) | additionBit;
            }
        }
    }
}

BYTE *itob(int num)
{
    BYTE *bytes = (BYTE *) malloc(4);
    bytes[0] = (num & 0xFF000000) >> 24;
    bytes[1] = (num & 0x00FF0000) >> 16;
    bytes[2] = (num & 0x0000FF00) >> 8;
    bytes[3] = num & 0x000000FF;
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
    char *str_bytes = (char *) malloc(msg_length);
    if ( str_bytes == NULL)
    {
        printf("Failed to allocate memory while decoding!");
        exit(1);
    }
    load_str_bytes(img, msg_length, end_bit, str_bytes);
    return str_bytes;
}

void load_str_bytes(BYTE *img, int msg_length, int end_bit, char *str_bytes)
{
    for ( int j = 0; j < msg_length; j++ )
    {
        for ( int k = 0; k < 8; k++, end_bit++ )
        {
            str_bytes[j] = (str_bytes[j] << 1) | (img[end_bit] & 1);
        }
    }
    str_bytes[msg_length] = '\0';
}

int get_msg_length(BYTE *img, int end_bit)
{
    int msg_length = 0;
    for ( int i = OFFSET; i < end_bit; i++ )
    {
        msg_length = (msg_length << 1) | (img[i] & 1);
    }
    return msg_length;
}
