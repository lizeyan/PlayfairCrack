/*
 * Written by Nate Stewart
 * Program to crack a playfair cipher using simulated annealing with quadgrams
 * 03/20/16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quadgram.h"
#include "playfairCrack.h"

extern float quadgram[];

int main(int argc, char **argv) {

	char cipher[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	FILE *fin = stdin;

	switch (argc) {
		case 2:
			fin = fopen(argv[1], "r");
			if (fin == NULL) {
				printf("Invalid file: %s. Unable to open for reading.\n", argv[3]);
				return -1;
			}
		case 1:
			if (!removeLetter(cipher, 'J')) {
				printf("Could not remove letter J from cipher key.\n");
				return -1;
			}
			break;
		case 4:
			fin = fopen(argv[3], "r");
			if (fin == NULL) {
				printf("Invalid file: %s. Unable to open for reading.\n", argv[3]);
				return -1;
			}
		case 3:
			if (strcmp(argv[1], "-r")) {
				printf("Optional parameter '-r' not found. '%s' found instead.\n", argv[1]);
				return -1;
			}
			if(!removeLetter(cipher, argv[2][0])) {
				printf("Could not remove letter %c from cipher key.\n", argv[2][0]);
				return -1;
			}
			break;
		default:
			printf("Invalid usage. See below for proper usage.\n");
			printf("\t./%s [ -r <character_to_remove> ] [ ciphertext_filepath ]\n", argv[0]);
			return -1;
	}

	char *m;
	m = readCipher(fin, INPUT_STEP_SIZE);

	if (validateText(m, strlen(m)) == 0)
		printf("Score: %lf\n", scoreText(m, strlen(m)));
	else
		printf("Invalid input.\n");
	free(m);

	// close the file as long as it is not stdin
	if (fin != stdin)
		fclose(fin);
	
	return 0;
}

bool removeLetter(char *cipher, char letter) {
	char *src, *dst;
	if (letter >= 'a' && letter <= 'z') {
		letter -= ' '; // make it capital letter 
	}
	bool found; // if the letter was found or not
	for (src = dst = cipher; *src != '\0'; src++) {
		*dst = *src;
		if (*dst != letter) 
			dst++;
		else
			found = true;
	}
	*dst = '\0';
	return found;
}

char *readCipher(FILE *fin, size_t size) {
	char *str;
	int currChar;
	size_t len = 0;
	str = malloc(sizeof(*str) * size); // Initially allocate str with provided size
	if (!str) return str; // If provided size is 0, or unable to allocate memory, return empty string
	
	while (EOF != (currChar = fgetc(fin)) && currChar != '\n') {
		str[len++] = currChar;
		if (len == size) {
			str = realloc(str, sizeof(*str) * (size += INPUT_STEP_SIZE));
			if (!str) return str; // If unable to realloc, return what we have so far
		}
	}

	// Add null terminator onto end of string
	str = realloc(str, sizeof(*str) * (++len));
	str[len - 1] = '\0';
	return str;
}

double scoreQuadgram(char *text) {
	int index[4];
	// Get a number associated with the index of each character
	index[0] = (*(text + 0) - 'A') * 17576;
	index[1] = (*(text + 1) - 'A') * 676;
	index[2] = (*(text + 2) - 'A') * 26;
	index[3] = (*(text + 3) - 'A') * 1;
	return quadgram[index[0] + index[1] + index[2] + index[3]];
}

double scoreText(char *text, int len) {
	int i;
	double score = 0.0;
	// Calculate all quadgrams in the text
	for (i = 0; i < len - 3; i++) {
		score += scoreQuadgram(text + i);
	}

	return score;
}

int validateText(char *text, int len) {
	int i;
	for (i = 0; i < len; i++)
		if (text[i] < 'A' || text[i] > 'Z')
			return -1;

	return 0;
}

