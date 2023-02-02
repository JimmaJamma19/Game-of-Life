#include <iostream>
#include <time.h>
#include <windows.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int main()
{	
	clock_t startTime, endTime;

	startTime = clock();

	//https://docs.microsoft.com/en-us/answers/questions/651933/c-how-to-change-the-size-of-text-and-change-text-p.html
	static CONSOLE_FONT_INFOEX fontex;
	fontex.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetCurrentConsoleFontEx(hOut, 0, &fontex);
	fontex.dwFontSize.X = 2;
	fontex.dwFontSize.Y = 2;
	SetCurrentConsoleFontEx(hOut, NULL, &fontex);
	
	//https://stackoverflow.com/questions/4053554/running-a-c-console-program-in-full-screen
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

	unsigned char UIOutput = 219; //Used to be able to display rectangles instead of 1's and 0's

	//https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //To be able to add colour to the output to indentify fish or sharks

	int arrayRows = 150, arrayColums = 625, spawnChance, generations = 10000;//Setting up integer variabls such as the arrays and generation counter
	

	int OldArray[150 + 2][625 + 2], NewArray[150 + 2][625 + 2]; //initialise the two arrays used in this code

	srand((unsigned int)time(NULL));

	//initial population of "OldArray"

	for (int r = 1; r <= arrayRows; r++)
	{
		for (int c = 1; c <= arrayColums; c++)
		{
			spawnChance = rand() % 10;
			if (spawnChance >= 0 && spawnChance <= 4) { OldArray[r][c] = (rand() % 10) + 1; }
			else if (spawnChance >= 5 && spawnChance <= 8) { OldArray[r][c] = ((rand() % 20) + 1) * -1; }
			else { OldArray[r][c] = 0; }
		}
	}


	do {

		//Making the ghost cells
		//Creating the corner ghost cells
		OldArray[0][0] = OldArray[arrayRows][arrayColums];
		OldArray[0][arrayColums + 1] = OldArray[arrayRows][1];
		OldArray[arrayRows + 1][0] = OldArray[1][arrayColums];
		OldArray[arrayRows + 1][arrayColums + 1] = OldArray[1][1];


		//making the left and right ghost cells
		for (int r = 1; r <= arrayRows; r++)
		{
			OldArray[r][0] = OldArray[r][arrayColums];
			OldArray[r][arrayColums + 1] = OldArray[r][1];
		}

		//making the top and bottom ghost cells
		for (int c = 1; c <= arrayColums; c++)
		{
			OldArray[0][c] = OldArray[arrayRows][c];
			OldArray[arrayRows + 1][c] = OldArray[1][c];
		}
		
		//---------------------------------------------------------------------------------------------------------
#pragma omp parallel num_threads(15)
		{
#pragma omp for schedule(dynamic)
			for (int r = 1; r <= arrayRows; r++)
			{
				for (int c = 1; c <= arrayColums; c++)
				{

					int numFish = 0, numSharks = 0, numBFish = 0, numBSharks = 0;
					int SurroundingCells[8] = { OldArray[r + 1][c], OldArray[r + 1][c + 1], OldArray[r + 1][c - 1],
												OldArray[r][c + 1], OldArray[r][c - 1],
												OldArray[r - 1][c], OldArray[r - 1][c + 1], OldArray[r - 1][c - 1]
					};


					for (int i = 0; i < 8; i++)
					{
						if (SurroundingCells[i] < 0)
						{
							numSharks++;
							if (SurroundingCells[i] <= -3) { numBSharks++; }
						}
						else if (SurroundingCells[i] > 0)
						{
							numFish++;
							if (SurroundingCells[i] >= 2) { numBFish++; }
						}

					}

					//Breeding Rule
					if (OldArray[r][c] == 0)
					{
						if (numSharks >= 4 && numBSharks >= 3 && numFish < 4) { NewArray[r][c] = -1; }
						else if (numFish >= 4 && numBFish >= 3 && numSharks < 4) { NewArray[r][c] = 1; }
						else { NewArray[r][c] = 0; }
					}

					//Fish Rule
					else if (OldArray[r][c] > 0)
					{
						if (numSharks >= 5 || numFish == 8 || OldArray[r][c] >= 10) { NewArray[r][c] = 0; }
						else { NewArray[r][c] = OldArray[r][c] + 1; }
					}

					//Shark Rules
					else if (OldArray[r][c] < 0)
					{
						if (OldArray[r][c] <= -20 || (numSharks >= 6 && numFish == 0) || (rand() % 32) == 1) { NewArray[r][c] = 0; }
						else { NewArray[r][c] = OldArray[r][c] - 1; }
					}
				}
			}
		}
		


		//Display
		//Display at chosen generations

		//----------------------------------------------------------------------------------------------------------------

		if(generations == 10000 || generations == 9975 || generations == 9950 || generations == 9925 || generations == 9900 || generations == 9750 || generations == 9500)
		{
			for (int r = 1; r <= arrayRows; r++)
			{
				for (int c = 1; c <= arrayColums; c++)
				{
					if (OldArray[r][c] > 0)
					{
						SetConsoleTextAttribute(hConsole, 6);
						//cout << UIOutput;
						cout << UIOutput;
					}
					else if (OldArray[r][c] < 0)
					{
						SetConsoleTextAttribute(hConsole, 4);
						cout << UIOutput;
					}
					else
					{

						cout << " ";
					}
				}
				cout << endl;
			}
			SetConsoleTextAttribute(hConsole, 10);
			cout << endl << endl;
			system("pause");
		}

		for (int r = 1; r <= arrayRows; r++)
		{
			for (int c = 1; c <= arrayColums; c++)
			{
				OldArray[r][c] = NewArray[r][c];
			}
		}
		
		//----------------------------------------------------------------------------------------------------------------

		//Display every Generation
		//----------------------------------------------------------------------------------------------------------------
		
		/*for (int r = 1; r <= arrayRows; r++)
		{
			for (int c = 1; c <= arrayColums; c++)
			{
				if (OldArray[r][c] > 0)
				{
					SetConsoleTextAttribute(hConsole, 6);
					cout << UIOutput;
				}
				else if (OldArray[r][c] < 0)
				{
					SetConsoleTextAttribute(hConsole, 4);
					cout << UIOutput;
				}
				else
				{

					cout << " ";
				}
				OldArray[r][c] = NewArray[r][c];
			}
			cout << endl;
		}
		SetConsoleTextAttribute(hConsole, 10);
		cout << endl << endl;
		system("pause");*/

		//----------------------------------------------------------------------------------------------------------------

		generations--;

	} while (generations != 0);
	
	endTime = clock();
	float processTime(((float)endTime - (float)startTime) / 1000);
	cout << "Process time :  " << processTime << " (in s)" << endl;
}