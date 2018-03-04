#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "defines.h"

typedef struct block
{
    unsigned char left[SUBBLOCK_LENGTH];
    unsigned char right[SUBBLOCK_LENGTH];
    uint32_t keys[ROUNDS];
} block;

void feisteil(block* b);
uint32_t str_to_num32(unsigned char* str);
uint64_t str_to_num64(unsigned char* str) {


void main(int argc, char* argv[]) {
   

    FILE *fin, *fout, *fkey;
    unsigned char in_str[SUBBLOCK_LENGTH], key[KEY_LENGTH];
    uint8_t flag;
    block tmp;
    if(argc != 5) {
        printf("Incorrect ussage: was expected ./feistel <flag: d|e> <in_file_path> <out_file_path> <key>\n");
        exit(1);
    }

    if((fkey = fopen(argv[4], "r")) == NULL) {
        printf("Cannot read key file: %s", argv[3]);
        exit(1);
    } else {
        if( fgets(key, KEY_LENGTH, fkey) == NULL ) {
            printf("No key in key file : %s", argv[3]);
            exit(1);
        }
    }
    if((fout = fopen(argv[3], "wb")) == NULL) {
        printf("Cannot wirte to file: %s", argv[2]);
        exit(1);
    } else {

    }   
    if((fin = fopen(argv[2], "rb")) == NULL) {
        printf("Cannot read input file: %s", argv[1]);
        exit(1);        
    } else {
            if(argv[1][0] == 'd') {
                flag = 1;
                //decrypt(&tmp);
            }
            else {
                flag = -1;
                //encrypt(&tmp);
            }
            
            tmp.keys = generate_keys(str_to_num64(key), flag);
         while(!feof(fin)){
            //fread(&tmp.left, sizeof( uint32_t ), 1, fin);
            //fread(&tmp.right, sizeof( uint32_t ), 1, fin);
            // gfets returns NULL if read nothing
            fgets(tmp.left,SUBBLOCK_LENGTH, fin);
            fgets(tmp.right, SUBBLOCK_LENGTH, fin);

            //fwrite(&tmp.left, sizeof( uint32_t ), 1, fout);
            //fwrite(&tmp.right, sizeof( uint32_t), 1, fout);
        }
   }

    fclose(fin);
    fclose(fout);
    fclose(fkey);
    printf("Hello, world!");

}

uint64_t str_to_num64(unsigned char* str){
    const uint64_t mask = 0;
    return (mask | str_to_num32(str)) << 31 | str_to_num32(str + 4);
}

uint32_t str_to_num32(unsigned char* str) {
    const uint32_t mask = 0;
    return (mask | str[0]) << 24 | (mask | str[1]) << 16 | (mask | str[2]) << 8 | str[3];
}

void left_shift64(uint64_t value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value << count) | (value >> (-count & mask));
}

void right_shift64(uint64_t value; unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value >> count) | (value << (-count & mask));
}

void left_shift32(uint32_t value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value << count) | (value >> (-count & mask));
}

void right_shift32(uint32_t value; unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value >> count) | (value << (-count & mask));
}


uint32_t* generate_keys(uint64_t key, uint8_t flag) {
    size_t key_size = strlen(key);
    int j = 0;
    uint32_t* result_keys = malloc(sizeof( uint32_t ) * ROUNDS);
    const uint32_t mask = 0;
    if(flag > 1) {

        for(j = 0; j < ROUNDS; j++) {
            uint32_t key = ( mask | right_shift64(key, j * 3));
            result_keys[j] = key;
        }

    } else {
        for(j = 0; j < ROUNDS; j++) {
            uint32_t key = ( mask | right_shift64(key, j * 3));
            result_keys[ROUNDS - j - 1] = key;           
        } 
    
    }

    return result_keys;
}

void feisteil(block *b, char **keys) {


}
