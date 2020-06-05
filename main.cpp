#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <windows.h>
#include <vector>
#include "main.h"

using namespace std;

#define KEY_ESCAPE 27
#define KEY_ENTER 13
#define KEY_BACKSPACE 8

extern HANDLE hStdOut;
extern CONSOLE_SCREEN_BUFFER_INFO csbInfo;
extern WORD workWindowAttributes; 
ifstream file;

bool menuIsActive = true;
bool editIsActive = false, openIsActive = false, createIsActive = false, saveIsActive = false, saveAsIsActive = false;
bool firstEnter = true;

bool menu()
{
	if (firstEnter == true)
	{
		DrawMenu();
		firstEnter = false;
	}
	else
	{
		drawFileOptions();
	}
	
	return false;
}

void ClearConsoleLetter(const HANDLE& hStdOut, const COORD cursorPos)
{
	SetConsoleCursorPosition(hStdOut, cursorPos);
	cout << " ";
	SetConsoleCursorPosition(hStdOut, cursorPos);
}

void ClearMenuLines()
{
	SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);
	SetConsoleCursorPosition(hStdOutM, { 0,1 });
	for (int i = 0; i < 120; i++)
	{
		cout << " ";
	}
	SetConsoleCursorPosition(hStdOutM, { 0,1 });
}

void openFile(const HANDLE& hStdOut, vector <uint8_t>& lineEndPosition, string& text, COORD& cursorPos, const string& path)
{
	SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
	ifstream file(path);

	if (!file.is_open())
	{
		
		SetConsoleCursorPosition(hStdOutM, { 0, 1 });
		SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);	
		ClearMenuLines();
		SetConsoleCursorPosition(hStdOutM, { 0, 1 });
		cout << "File doesn't exist! Please, try again.";
		Sleep(2000);
		return;
	}
	else
	{
		char ch;
		lineEndPosition.clear();
		text.clear();
		cursorPos.X = 0;
		cursorPos.Y = 2;
		SetConsoleCursorPosition(hStdOut, cursorPos);

		while (file.good())
		{
			ch = file.get();

			if (file.eof())
			{
				break;
			}

			if (ch == '\n' || ch == '\r')
			{
				text += '\n';
				lineEndPosition.push_back(cursorPos.X);
				cout << endl;
				cursorPos.X = 0;
				cursorPos.Y++;
				continue;
			}
			if (cursorPos.X < 120)
			{
				cursorPos.X++;
			}
			else
			{
				text += '\n';
				cursorPos.X = 0;
				cursorPos.Y++;
			}

			text += ch;
			cout << ch;
		}
	}

	file.close();
}

void createFile(const string& path)
{
	ofstream file;
	file.open(path);
	file.close();
}

void saveFile(const string& text, string& path)
{
	ofstream file;
	file.open(path);
	file << text;
	file.close();
}

void saveFileAs(const string& text, string& path)
{
	ifstream fileIn;
	fileIn.open(path);
	if (fileIn.is_open())
	{
		cout << "file is alre";
	}
	else
	{
		ofstream file;
		file.open(path);
		file << text;
		file.close();
	}
}

int main()
{
	hStdOutM = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdOutM, &csbInfo);
	SetConsoleTextAttribute(hStdOutM, workWindowAttributes); // фон
	system("cls");

	char key;
	vector <uint8_t> lineEndPosition;
	string path;
	string text;
	HANDLE hStdOut;
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursorPos;
	cursorPos.X = 0;
    cursorPos.Y = 2;

	while (true)
	{
		if (menuIsActive)
		{
			menuIsActive = menu();
		}
		else
		{
			if (editIsActive == true || openIsActive == true || createIsActive == true || saveIsActive == true || saveAsIsActive == true)
			{
				if (openIsActive)
				{
					menuIsActive = true;
					openIsActive = false;
					ClearMenuLines();
					cout << "Choose file directory or file's name:";
					cin >> path;
					SetConsoleTextAttribute(hStdOutM, workWindowAttributes);		
					for (int i = 0; i < 3360; i++)
					{
						cout << " ";
					}			
					openFile(hStdOut, lineEndPosition, text, cursorPos, path);
				}			
				if (editIsActive == true)
				{		
					SetConsoleCursorPosition(hStdOut, cursorPos);

					key = _getch();

					switch (key)
					{
					case KEY_ESCAPE:
						menuIsActive = true;
						editIsActive = false;
						continue;

					case KEY_ENTER:
						lineEndPosition.push_back(cursorPos.X);
						cursorPos.X = 0;
						cursorPos.Y++;
						cout << endl;
						text += '\n';
						continue;

					case KEY_BACKSPACE:
						if (cursorPos.X > 0)
						{
							cursorPos.X--;
							ClearConsoleLetter(hStdOut, cursorPos);
						}
						else
						{
							if (!lineEndPosition.empty())
							{
								cursorPos.X = lineEndPosition.back();
								lineEndPosition.erase(lineEndPosition.end() - 1);
								cursorPos.Y--;
							}
						}
						if (text.length() > 2)
						{
							text.erase(text.length() - 1);
						}
						else
						{
							text.erase(0);
						}
						continue;

					default:
						if (cursorPos.X < 120)
						{
							cursorPos.X++;
						}
						else
						{
							cursorPos.X = 0;
							cursorPos.Y++;
						}
					}
					text += key;
					cout << key;
				}
				if (createIsActive)
				{
					menuIsActive = true;
					createIsActive = false;
					ClearMenuLines();
					cout << "Choose file's name:";
					cin >> path;
					createFile(path);
					path.clear();
					SetConsoleCursorPosition(hStdOutM, { 0,1 });
					cout << "File was created successfully!!!";
					Sleep(2000);
				}
				if (saveIsActive)
				{

					if (path.empty())
					{
						menuIsActive = true;
						saveIsActive = false;
						ClearMenuLines();
						cout << "File doesn't open! What do you want to do:";
						drawSaveError();	
					}
					else
					{
						menuIsActive = true;
						saveIsActive = false;
						ClearMenuLines();
						cout << "File was saved successfully!!!";
						Sleep(1000);
						saveFile(text, path);
					}
					file.close();							
				}
				if (saveAsIsActive)
				{
					menuIsActive = true;
					saveAsIsActive = false;
					ClearMenuLines();
					cout << "Choose a new file's name:";
					cin >> path;
					saveFileAs(text, path);
					ClearMenuLines();
					cout << "New file was saved successfully!!!";
					Sleep(2000);
				}
			}
		}
	}
	return 0;
}