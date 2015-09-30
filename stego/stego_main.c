/*
-parseCmd
-openMsg
-compressMsg
-encryptMsg
-openAudio
-stego
-output
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../utils/wavtool.h"

typedef int bool;
#define true 1
#define false 0

bool isEncode = true;
char *msgFilename;
char *audioFilename;
char *outputFilename;
int pin = -1;

int parseCmd(int argc, char const *argv[])
{
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			isEncode = false;
			pin = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-e") == 0)
		{
			isEncode = true;
			pin = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-m") == 0)
			msgFilename = argv[++i];
		else if (strcmp(argv[i], "-a") == 0)
			audioFilename = argv[++i];
		else if (strcmp(argv[i], "-o") == 0)
			outputFilename = argv[++i];
		else
			return -1;
	}
	if (outputFilename == NULL)
		return -2;
	if (isEncode)
	{
		if (msgFilename == NULL || audioFilename == NULL || pin == -1)
			return -3;
	}
	else
	{
		if (audioFilename == NULL || pin == -1)
			return -4;
	}
	printf("%s\n", msgFilename);
	printf("%s\n", audioFilename);
	printf("%s\n", outputFilename);
	printf("%d\n", isEncode);
	printf("%d\n", pin);
	return 0;
}

int encodeCycle()
{

}

int main(int argc, char const *argv[])
{
	parseCmd(argc, argv);
	return 0;
}