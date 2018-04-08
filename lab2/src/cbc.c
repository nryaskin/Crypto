#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include "defines.h"
#include "mt64.h"

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
void prepare_block(block* , block);
void cp_block(block*, block);
uint64_t prepare_iv(void);
void bits_to_str(uint32_t, unsigned char*);
void read_fp(unsigned char*, unsigned int ,FILE*);

void main(int argc, char* argv[]) {
    FILE *fin, *fout, *fkey, *fiv;
    unsigned char in_str[SUBBLOCK_LENGTH], key[KEY_LENGTH];
    uint8_t flag;
    uint64_t init_vector = 0;
    block plain, cypher, swap;
    if(argc != 6) {
        printf("Incorrect ussage: was expected ./cbc <flag: d|e> <in_file_path> <out_file_path> <key> <init_vector>\n");
        exit(1);
    }

    if((fkey = fopen(argv[4], "rb")) == NULL) {
        printf("Cannot read key file: %s", argv[4]);
        exit(1);
    } else {
        if( fgets(key, KEY_LENGTH, fkey) == NULL ) {
            printf("No key in key file : %s", argv[4]);
            exit(1);
        }
    }
    if((fout = fopen(argv[3], "wb")) == NULL) {
        printf("Cannot wirte to file: %s", argv[3]);
        exit(1);
    }

    flag = (argv[1][0] == 'e');        
    if(flag){
        // init iv here then write it to file
        init_vector = prepare_iv();
        if((fiv = fopen(argv[5], "wb")) == NULL) {
	    printf("Cannot wirte to file: %s", argv[5]);
            exit(1);
        }
        else {
	    char iv[9];
	    uint32_t mask = 0;
            bits_to_str((uint32_t)(mask | init_vector), swap.right);
            bits_to_str((uint32_t)(mask | init_vector >> 32), swap.left);	
	    fwrite(swap.left, SUBBLOCK_LENGTH - 1, 1, fiv);
	    fwrite(swap.right, SUBBLOCK_LENGTH - 1, 1, fiv);
        }   
    }
    else 
    {
	if((fiv = fopen(argv[5], "rb")) == NULL) {
	    printf("Cannot wirte to file: %s", argv[5]);
            exit(1);
        }
        else 
	{
            read_fp(swap.left,SUBBLOCK_LENGTH, fiv);
	    read_fp(swap.right, SUBBLOCK_LENGTH, fiv);
	}	
    }

    cp_block(&cypher, swap);
    
    if((fin = fopen(argv[2], "rb")) == NULL) {
        printf("Cannot read input file: %s", argv[1]);
        exit(1);        
    } else {
            uint64_t num_key =  str_to_num64(key);
            generate_keys(num_key, &plain, flag);
	    do {
		read_fp(plain.left,SUBBLOCK_LENGTH, fin);
	        read_fp(plain.right, SUBBLOCK_LENGTH, fin);
			    if (flag) {
				prepare_block(&plain, cypher); 
			    }
			    else {
				cp_block(&swap, plain);
			    }
			    feisteil(&plain);
			    if(!flag) {
				prepare_block(&plain, cypher);
			    }
			    fwrite(plain.left, SUBBLOCK_LENGTH - 1, 1, fout);
			    fwrite(plain.right, SUBBLOCK_LENGTH - 1, 1, fout);
			    if(flag) {
				cp_block(&cypher, plain);
			    }
			    else {
				cp_block(&cypher, swap);
			    }
	    } while(!feof(fin));
   }

    fclose(fin);
    fclose(fout);
    fclose(fkey);
}

void read_fp(unsigned char* out, unsigned int length, FILE* fp) {
        char* result = NULL;
        unsigned char ch;
	uint32_t tmp = 0;
	int c = 0;
	for(int i = 0; i < length; i++){
            out[i] = '\0';
	}
	fread(out, length - 1, 1, fp);  
}

uint64_t prepare_iv(void) {
    uint64_t seed = 7;
    init_genrand64(seed);
    return genrand64_int64();
}


void cp_block(block *to, block from) {
   for(int i = 0; i < SUBBLOCK_LENGTH; i++) {
       	to->left[i] = from.left[i];
        to->right[i] = from.right[i];
   }
}

void prepare_block(block *b, block t) {
   for(int i = 0; i < SUBBLOCK_LENGTH - 1; i++ ) {
        b->left[i] ^= t.left[i];
	b->right[i] ^= t.right[i];
    } 
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
    if(flag) {

        for(j = 0; j < ROUNDS; j++) {
            key = right_shift64(key, j * 3);

            key_32 = ( mask | key);
            b->keys[j] = key_32;
        }

    } else {
        for(j = 0; j < ROUNDS; j++) {

            key = right_shift64(key, j * 3);
            key_32 = ( mask | key);
            b->keys[ROUNDS - j - 1] = key_32;           
        } 
    
    }
}

void bits_to_str(uint32_t in,unsigned char* out_str) {
    unsigned char mask = 0;
    out_str[0] =(unsigned char) ((in >> 24) | mask);
    out_str[1] = (unsigned char)((in >> 16) | mask);
    out_str[2] = (unsigned char)((in >> 8) | mask);
    out_str[3] = (unsigned char)(in | mask);
    out_str[4] = '\0';
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
