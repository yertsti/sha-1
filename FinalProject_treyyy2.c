/* SHA-1 I hope
 * Trey Yasunaga.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "printbits.h"

#define MAX_SIZE 1048576
#define DEBUG 1 //set to 1 for print statements of the hashing steps.
unsigned int readFile(unsigned char buffer[]);
void printArray(unsigned char buffer[], unsigned int);
unsigned int calculateBlocks(unsigned int sizeOfFileInBytes);
void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes);
void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount);
void computeMessageDigest(unsigned int message[], unsigned int blockCount);
unsigned int F(unsigned int t, unsigned int B, unsigned int C, unsigned int D);
unsigned int K(unsigned int t);
unsigned int rotateLeft(unsigned int x, unsigned int y);
void printNumArray(unsigned int message[], unsigned int);

int main() {
  //int i = 0;
  unsigned char buffer[MAX_SIZE];
  unsigned int message[MAX_SIZE];
  unsigned int sizeOfFileInBytes = readFile(buffer);

  if(sizeOfFileInBytes != -1) {
    unsigned int blockCount = calculateBlocks(sizeOfFileInBytes);
    printf("Block count is %d\n", blockCount);
    convertCharArrayToIntArray(buffer, message, sizeOfFileInBytes);
    addBitCountToLastBlock(message, sizeOfFileInBytes, blockCount);
    /*
    if (DEBUG == 1) {
      for(i = 0; i < blockCount * 16; i++) {
	printbits(message[i]);
      }
    }
    */
    computeMessageDigest(message, blockCount);
  }
 
  return 0;
}
//reads in a file and stores the chars into an array
unsigned int readFile(unsigned char buffer[]) {
    int i = 0;
    puts("Please enter the text you want to hash via input redirection on the command line.");
    puts("If you did not do the above enter ctrl-d to hash an empty string and exit.");
    char letter = getchar();
    while(letter != EOF) {
      if(i > MAX_SIZE - 1) {
	printf("Input file is too big\n");
	return -1;
      } else {
	buffer[i] = letter;
	letter = getchar();
	i++;
      }
    }
    //appends a 1 bit to the end of the input.
    buffer[i] = 0x80;
    printf("File is %d bytes\n", i);
    return i;
}
//Prints out an array
void printArray(unsigned char buffer[], unsigned int i) {
    int j = 0;
    for (j = 0; j < i; j++) {
        printf("%x\n", buffer[j]);
    }
}
//caluclulates the size of file in bytes
unsigned int calculateBlocks(unsigned int sizeOfFileInBytes) {
    unsigned int blockCount = 0;
    blockCount = (((8 * sizeOfFileInBytes) + 1) / 512) + 1;
    if((((8 * sizeOfFileInBytes) + 1) % 512) > (512 - 64)){
        blockCount = blockCount + 1;
    }
    return blockCount;
}
//Packs together 4 chars into 1 int and saves it into an unsigned int array
void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes) {
  int i = 0;
  int j = 0;
  for(i = 0; i <= sizeOfFileInBytes; i++) {
    //printf("\n%d i in conver char array\n", i);
    //printf("%c\n", buffer[i]);
    switch(i % 4) {
    case 0 : 
      message[j] = buffer[i] << 24;
      break;
    case 1 :
      message[j] = (buffer[i] << 16) | message[j];
      break;
    case 2 :
      message[j] = (buffer[i] << 8) | message[j];
      break;
    case 3 :
      message[j] = buffer[i] | message[j];
      j++;
      break;
    default : printf("wow");
      break; 
    } //end of switch
  }
}
//appends the number of bits in the input at the end of the last block
void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount) {
    unsigned int sizeOfTheFileInBits = sizeOfFileInBytes * 8;
    unsigned int indexOfEndOfLastBlock = (blockCount * 16) - 1;
    //printbits(message[indexOfEndOfLastBlock]);
    //printbits(sizeOfTheFileInBits);
    message[indexOfEndOfLastBlock] = (message[indexOfEndOfLastBlock] | sizeOfTheFileInBits);
}
//Does all the hashing black magic
void computeMessageDigest(unsigned int message[], unsigned int blockCount) {
  
    unsigned int h0 = 0x67452301;
    unsigned int h1 = 0xEFCDAB89;
    unsigned int h2 = 0x98BADCFE;
    unsigned int h3 = 0x10325476;
    unsigned int h4 = 0xC3D2E1F0;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned int e;
    unsigned int k;
    unsigned int f;
    
    unsigned int tempNum;
    unsigned int i = 0;
    unsigned int t = 0;
    unsigned int j = 0;

    //Breaks up each 16 int block with 80 spaces between each block.
    for(j = 0; j < blockCount; j++) {
      if(DEBUG == 1) printf("\nWords for block %d:\n", j + 1);
      for(i = 0; i < 16; i++) {
	message[i + (80 * j)] = message[i + (j * 16)];
	//printbits(message[i]);
	if(DEBUG == 1) printf("W[%d] = %08X\n", i, message[i + (80 * j)]); 
      }
    }

    //turns 16 ints into 80 per block.
    for(j = 0; j < blockCount; j++) {
      for(i = 16; i < 80; i++) {
	message[i + (80 * j)] = rotateLeft((message[i + (80 * j) - 3] ^ message[i + (80 * j) - 8] ^ 
					    message[i + (80 * j) - 14] ^ message[i + (80 * j) - 16]), 1);
      }
    }

    for(j = 0; j < blockCount; j++) {
      a = h0;
      b = h1;
      c = h2;
      d = h3;
      e = h4;
      if(DEBUG == 1) printf("\nBlock %d\n\t a\t  b\t   c\t    d\t     e\n", (j + 1));
      for(t = 0; t < 80; t++) {
	f = F(t, b, c, d);
	k = K(t);
	tempNum = rotateLeft(a, 5) + f + e + k + message[t + (80 * j)];
	e = d;
	d = c;
	c = rotateLeft(b, 30);
	b = a;
	a = tempNum;
	if(DEBUG == 1) {
	  printf("t = %d ", t);
	  printf("%08X %08X %08X %08X %08X\n", a, b, c, d, e);
	}
      }

      h0 = h0 + a;
      h1 = h1 + b;
      h2 = h2 + c;
      h3 = h3 + d;
      h4 = h4 + e;
      printf("Message digest for block %d:\n", (j + 1));
      printf("%08X %08X %08X %08X %08X\n", h0, h1, h2, h3, h4);
    }
    message[0] = h0;
    message[1] = h1;
    message[2] = h2;
    message[3] = h3;
    message[4] = h4;
    printf("\nMessage digest:\n");
    printf("%08X %08X %08X %08X %08X\n", message[0], message[1], message[2], message[3], message[4]);
    //BREAKS AT 312 because it adds a new block wiht only the 1 bit. block 6+ idk why
}

//performs some bit magic on b, c, d depending on t
unsigned int F(unsigned int t, unsigned int B, unsigned int C, unsigned int D) {
    if(0 <= t && t <= 19) {
      return (B & C) | ((~B) & D);
    }
    else if(20 <= t && t <= 39) {
      return B ^ C ^ D;
    }
    else if(40 <= t && t <= 59) {
        return (B & C) | (B & D) | (C & D);
    }
    else {
        return B ^ C ^ D;
    }
}
//returns k value based on t
unsigned int K(unsigned int t) {
    if(0 <= t && t <= 19) {
        return 0x5A827999;
    } else if(20 <= t && t <= 39) {
        return 0x6ED9EBA1;
    } else if(40 <= t && t <=59) {
        return 0x8F1BBCDC;
    } else {
        return 0xCA62C1D6;
    }
}

//Shifts x's bits left over to right side y times
unsigned int rotateLeft(unsigned int x, unsigned int y) {
    return (x << y) | (x >> (32 - y));
}
//prints number array
void printNumArray(unsigned int message[], unsigned int i) {
    int j = 0;
    for(j = 0; j < ceil((double)i/4); j++) {
        printf("%d\n",j);
        printbits(message[j]);
    }
}
