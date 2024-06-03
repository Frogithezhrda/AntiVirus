/*********************************
* Class: MAGSHIMIM C2			 *
* Week:                			 *
* Name:                          *
* Credits:                       *
**********************************/
#include "dirent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Calling the Functions
int readContentPrecent(int fileSize, FILE* file, FILE* virusFile, int virusSize, int isStart);
int dirFiles(char* dirPath, char* virusPath, FILE* log, int isQuick);
int openFile(char* filePath, char* virusPath, FILE* log, int isQuick);
int readFile(FILE* file, FILE* virusFile);
void sortFiles(int friendAmount, char** fileNames);
void swap(char** firstStr, char** secondStr);
int handleLog(char* dirPath, char* virusPath);
int quickScan(FILE* file, FILE* virusFile);

// Decalring CONSTS
#define AMOUNT_TO_READ 1
#define START_FILE 0
#define MAX_ARGS 3
#define DIRECTORY_PATH 1
#define VIRUS_SIGN 2
#define FALSE 0
#define TRUE 1
#define NORMAL_SCAN '0'
#define FIRST 20
#define LAST -20
#define ERROR -1
#define GET_FILE_DIVISION 5

int main(int argc, char** argv)
{
	//cgecking if the amount of args is valid
	if (argc == MAX_ARGS)
	{
		//if it is sending it to the log
		handleLog(argv[DIRECTORY_PATH], argv[VIRUS_SIGN]);
	}
	else
	{
		//else printing error message
		printf("Usage: ThisFile.exe <folder path> <virus sign path>");
	}
	//stopping the buffer
	getchar();
	return 0;
}
/*
Creating the log path and allocating memory and adding the virusLog.txt to the string printing the menu and creating the log
file and reseting the buffer calling the dirFiles with the userInput of the user in the end closing the file and reseting the buffer
input: directory Path, Virus Path
output: if there is an error return ERROR(bar ive done this because of the outfeedback system)
*/
int handleLog(char* dirPath, char* virusPath)
{
	FILE* log = NULL; //declaring varaibles
	char* logPath = NULL;
	char userInput = ' ';
	int c = 0;
	logPath = (char*)malloc(sizeof(char) * strlen(dirPath) + strlen("AntiVirusLog.txt") + 2); //allocating memory for the file name and the dirPath and / and null terminator
	strcpy(logPath, dirPath);
	strcat(logPath, "/AntiVirusLog.txt"); //creating the log path
	log = fopen(logPath, "w"); //opening the log using w mode
	if (log == NULL)
	{
		printf("Error opening directory!\n");
		fprintf(log, "Error opening directory!\n");
		return ERROR;
	} //printing which foleder to scan and the options for the users
	printf("\n\nFolder to scan: %s\nVirus signature: %s\n\nPress 0 for normal scan any other for quick scan:", dirPath, virusPath);
	fprintf(log, "\n\nFolder to scan: %s\nVirus signature: %s\n\nPress 0 for normal scan any other for quick scan:", dirPath, virusPath);
	userInput = getchar(); //getting user input
	while ((c = getchar()) != '\n' && c != EOF)  //reseting buffer
	{
		// Discard characters until newline or end of file
	}
	printf("Scanning....\n"); 
	fprintf(log, "Scanning....\n");
	userInput == NORMAL_SCAN ? dirFiles(dirPath, virusPath, log, FALSE) : dirFiles(dirPath, virusPath, log, TRUE); //calling the function by the answer
	printf("\nScan Completed.\n");
	fprintf(log, "\nScan Completed.\n");
	printf("See log for results: %s", logPath); //printing the log results path closing the log file and freeing the memory
	free(logPath);
	fclose(log);
}
/*
opening the directory getting its length using dirLength
allocating the memory for the file names by using the ** array reading directory putting the file names into the strings arrays
and fixing the paths of the files sorting the files a-z opening each file and freeing the memory
input: the directory path, virus signature path, log file, and if its quick scan or not
output: if error return error
*/
int dirFiles(char* dirPath, char* virusPath, FILE* log, int isQuick)
{
	int i = 0; //declaring variables
	char** fileNames = NULL;
	int directoryLength = 0;
	DIR* directory = 0;
	struct dirent* dir = 0;
	directory = opendir(dirPath); //opening the directory
	directoryLength = dirLength(directory); //getting the length
	if (directory == NULL)
	{
		printf("Error opening directory!\n");
		fprintf(log, "Error opening directory!\n");
		return ERROR;
	}

	fileNames = (char**)malloc(sizeof(char*) * (directoryLength + 1)); //allocating the amount of file names
	if (fileNames == NULL)
	{
		printf("Memory allocation failed!\n");
		fprintf(log, "Memory allocation failed!\n");
		closedir(directory);
		return ERROR;
	}
	directory = opendir(dirPath); //opening again because we closed it earler in dirLength
	while ((dir = readdir(directory)) != NULL)
	{
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..") && dir->d_name != DT_DIR) //checking if not directory or . or ..
		{
			fileNames[i] = (char*)malloc(sizeof(char) * (strlen(dirPath) + 1 + strlen(dir->d_name) + 1)); //allocating memory
			strcpy(fileNames[i], dirPath);
			strcat(fileNames[i], "/");
			strcat(fileNames[i], dir->d_name); //getting fullPath
			i++;
		}
	}
	sortFiles(directoryLength, fileNames); //sorting the files using sortFiles
	for (i = 0; i < directoryLength; i++) //opening all of the files
	{
		openFile(fileNames[i], virusPath, log, isQuick);
	}
	if (fileNames != NULL) //freeing the files and closing the directory
	{
		for (i = 0; i < directoryLength; i++)
		{
			free(fileNames[i]);
		}
		free(fileNames);
	}
	closedir(directory);
}


/*
getting the directory reading the directoy and adding the files closing the directory and returning the amount
input: directory to get size
output: amount of files
*/
int dirLength(DIR* directory)
{
	int count = 0;
	struct dirent* file = 0;
	//reading the directory and adding the count by if its not another directory or . or ..
	while ((file = readdir(directory)) != NULL) 
	{
		if (strcmp(file->d_name, ".") && strcmp(file->d_name, "..") && file->d_name != DT_DIR)
		{
			count++;
		}
	}
	closedir(directory); //closing the directory and reutrning the count
	return count;
}
/*
opening the virus and the normal file if its quick scan you will quick scan it if its not in last and not
in first than scan it using readFile and if its normal scan just scan everything use readFile
in each one print and write to the log using fprintf if they are valid than just print the right result
input: the file path, virus path, log file to write int, and if its a quick scan or not

*/
int openFile(char* filePath, char* virusPath, FILE* log, int isQuick)
{
	int isVirus = 0; //declaring variables to read 
	FILE* virusFile = NULL;
	FILE* file = NULL;
	virusFile = fopen(virusPath, "rb");
	if (virusFile == NULL)
	{
		printf("Error opening virus file!\n");
		fprintf(log, "Error opening virus file!\n");
		return ERROR;
	}
	file = fopen(filePath, "rb");
	if (file == NULL)
	{
		printf("Error opening file file!\n");
		fprintf(log, "Error opening file!\n");
		return ERROR;
	}
	if (isQuick) //checking if quick scan
	{
		isVirus = quickScan(file, virusFile); //returning if its a virus or not in the first 20% or last
		isVirus == FIRST ? (printf("%s - Infected! (first 20%%)\n", filePath), fprintf(log, "%s - Infected! (first 20%%)\n", filePath)) :
		isVirus == LAST ? (printf("%s - Infected! (last 20%%)\n", filePath), fprintf(log, "%s - Infected! (last 20%%)\n", filePath)) : (isVirus = readFile(file, virusFile));
		if (isVirus != LAST && isVirus != FIRST) //if not either than just read normally
		{
			isVirus == TRUE ? printf("%s - Infected!\n", filePath), fprintf(log, "%s - Infected!\n", filePath) : printf("%s - Clean!\n", filePath), fprintf(log, "%s - Clean!\n", filePath);
		}
	}
	else
	{
		isVirus = readFile(file, virusFile); // read the file normally
		//if virus print infected else print clean
		isVirus == TRUE ? printf("%s - Infected!\n", filePath), fprintf(log, "%s - Infected!\n", filePath) : printf("%s - Clean!\n", filePath), fprintf(log, "%s - Clean!\n", filePath);
	}
	fclose(virusFile);
}
/*
reading the file getting each file size while its not the end of the file read each charater and write into the memory
if the char i read is the same has the virus file char if it is add to the file size 1 if the file size is equal to the virus file
than close the files and return that it is a virus if not go to the start of the virus file and reset the file size and close the file in the end 
and return FALSE
input: the file to scan, the virusFile
output: isvirus or not
*/
int readFile(FILE* file, FILE* virusFile) 
{
	char ch = ' '; //declaring variables
	char chVirus = ' ';
	int fileSize = 0;
	int virusSize = 0;
	fseek(file, START_FILE, SEEK_SET);
	fseek(virusFile, START_FILE, SEEK_END); //getting the file size
	virusSize = ftell(virusFile);
	fseek(virusFile, START_FILE, SEEK_SET);
	// as long its not the end of the file just read the character from the file
	while (!feof(file))
	{
		fread(&ch, sizeof(char), AMOUNT_TO_READ, file);
		fread(&chVirus, sizeof(char), AMOUNT_TO_READ, virusFile);
		if (ch == chVirus) //if the characters equal than add 1 to the fileSize
		{
			fileSize++;
			if (fileSize == virusSize) //if they are equal its the virus and return truee
			{
				fclose(file);
				fclose(virusFile);
				return TRUE;
			}
		}
		else
		{
			fseek(virusFile, START_FILE, SEEK_SET); //reset it back to the start of the virus file and reset the file size
			fileSize = 0;
		}
	}
	fclose(file); //close the file
	return FALSE;
}
/*
sorting the array by getting them by refrence and than it already changes in the memory
input: friends Amount, the fileNames
output: none
*/
void sortFiles(int friendAmount, char** fileNames)
{
	int i = 0;
	int j = 0;
	//sorting the array
	for (i = 0; i < friendAmount; i++)
	{
		for (j = i + 1; j < friendAmount; j++)
		{
			if (strcmp(fileNames[i], fileNames[j]) > 0)
			{
				swap(&fileNames[i], &fileNames[j]); //swapping the names
			}
		}
	}
}
/*
swapping the names uisng the pointers
input: the 2 names to swap
output: none
*/
void swap(char** firstStr, char** secondStr)
{
	char* temp = NULL;
	temp = *firstStr;
	*firstStr = *secondStr;
	*secondStr = temp;
}
/*
quickScan gets the file to quickScan the virusFile signature getting the virusSize
the fileSize and sending the read content if its not in the first 20% than send it to look for the last else just keep it as is
input: file to scan, singature virus file
output: if virus or not by quick scan
*/
int quickScan(FILE* file, FILE* virusFile)
{
	int fileSize = 0;
	int virusSize = 0;
	int isVirus = 0;
	//getting virus file size
	fseek(virusFile, START_FILE, SEEK_END);
	virusSize = ftell(virusFile);
	fseek(virusFile, START_FILE, SEEK_SET);
	//getting the file size
	fseek(file, START_FILE, SEEK_END);
	fileSize = ftell(file);
	fseek(file, START_FILE, SEEK_SET);
	//reading the contenet as the first 20%
	isVirus = readContentPrecent(fileSize, file, virusFile, virusSize, TRUE);
	isVirus != FIRST ? isVirus = readContentPrecent(fileSize, file, virusFile, virusSize, FALSE) : isVirus; //if its not the first scan as the last 20% and return if it found the 20 precent
	return isVirus; //returning isVirus if its the last than -20 if the first 20 and if not FALSE
}
/*
getting the file size the virus file size the dividng by 5 to get 20 precent the virus is equal to the first
if its the start we read throught the file checking each character if the size is the same return thats the first 20
else if its not the start than do the same from the last of the file if its LAST than isVirus equal to it
if its not a virus return FALSE
input: the file to scan, the virusFile, the size of both and if its thae start or not(what to scan start or last)
output: isvirus or not and if its in the 20%
*/
int readContentPrecent(int fileSize, FILE* file, FILE* virusFile, int virusSize, int isStart)
{
	int checkSize = 0; //declaring the variables
	int twentyFileSize = 0;
	char ch = ' ';
	char chVirus = ' ';
	int isVirus = 0;
	int i = 0;
	//getting thee size of the file divided to the 20% of the files
	twentyFileSize = fileSize / GET_FILE_DIVISION;
	isVirus = FIRST; //getting the isVirus to the first of the file
	if (isStart == FALSE) //if its the last
	{
		fseek(file, -twentyFileSize, SEEK_END);
		fseek(virusFile, START_FILE, SEEK_SET); //than get it to the last 20%
		isVirus = LAST; //and not the isVirus equal to -20
	}
	for (i = 0; i < twentyFileSize; i++) //reading the file like readFile
	{
		fread(&ch, sizeof(char), AMOUNT_TO_READ, file);
		fread(&chVirus, sizeof(char), AMOUNT_TO_READ, virusFile);
		if (ch == chVirus)
		{
			checkSize++;
			if (checkSize == virusSize)
			{
				fclose(file);
				fclose(virusFile);
				return isVirus; //returning if its virus or not
			}
		}
		else //if didnt read correctly reset
		{
			fseek(virusFile, START_FILE, SEEK_SET);
			checkSize = 0;
		}
	}
	return FALSE; //if none rturn false
}