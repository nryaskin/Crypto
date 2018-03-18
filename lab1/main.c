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

void generate_keys(uint64_t, block* ,uint8_t); 
void feisteil(block*);
uint32_t str_to_num32(unsigned char*);
uint64_t str_to_num64(unsigned char*);


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
                flag = -1;
                //decrypt(&tmp);
            }
            else {
                flag = 1;
                //encrypt(&tmp);
            }
            uint64_t num_key =  str_to_num64(key);
            generate_keys(num_key, &tmp, flag);
         while(!feof(fin)){
            //fread(&tmp.left, sizeof( uint32_t ), 1, fin);
            //fread(&tmp.right, sizeof( uint32_t ), 1, fin);
            // gfets returns NULL if read nothing
            if(fgets(tmp.left,SUBBLOCK_LENGTH, fin) != NULL){
            fgets(tmp.right, SUBBLOCK_LENGTH, fin);
            feisteil(&tmp);
            fprintf(fout, "%s", tmp.left);
            fprintf(fout, "%s", tmp.right);
            }
            else 
                break;
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
    return (mask | str_to_num32(str)) << 32 | str_to_num32(str + 4);
}

uint32_t str_to_num32(unsigned char* str) {
    const uint32_t mask = 0;
    return ((mask | str[0]) << 24) | ((mask | str[1]) << 16) |((mask | str[2]) << 8) | str[3];
}

uint64_t left_shift64(uint64_t value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value << count) | (value >> (-count & mask));
}

uint64_t right_shift64(uint64_t value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value >> count) | (value << (-count & mask));
}

uint32_t left_shift32(uint32_t value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value << count) | (value >> (-count & mask));
}

uint32_t right_shift32(uint32_t value, unsigned int count) {
    const unsigned int mask = CHAR_BIT * sizeof(value) - 1;
    count &= mask;
    return (value >> count) | (value << (-count & mask));
}


void generate_keys(uint64_t key, block *b,uint8_t flag) {
    int j = 0;
    uint32_t* result_keys = malloc(sizeof( uint32_t ) * ROUNDS);
    const uint32_t mask = 0;
    uint32_t key_32;
    if(flag == 1) {

        for(j = 0; j < ROUNDS; j++) {
            key = right_shift64(key, j * 3);

            key_32 = ( mask | key);
            b->keys[j] = key_32;
        }

    } else {
        for(j = 0; j < ROUNDS; j++) {

            key = right_shift64(key, j * 3);
            key_32 = ( mask | key);
            b->keys[ROUNDS - j - 1] = key;           
        } 
    
    }
}

void bits_to_str(uint32_t in, char* out_str) {
    unsigned char mask = 0;
    out_str[0] =(unsigned char) ((in >> 24) | mask);
    out_str[1] = (unsigned char)((in >> 16) | mask);
    out_str[2] = (unsigned char)((in >> 8) | mask);
    out_str[3] = (unsigned char)(in | mask);
}

void feisteil(block *b) {
    uint8_t i;
    uint32_t left = 0, right = 0, tmp = 0;
    tmp = str_to_num32(b->left);
    right = str_to_num32(b->right); 
    for(i = 0; i < ROUNDS; i++) {
       left = right^((left_shift32(tmp, 9)) ^ (~(right_shift32(b->keys[i], 11) ^ tmp) ));
       right = tmp;
       tmp = left;
    }
    bits_to_str(right, b->left);
    bits_to_str(left, b->right);
}
