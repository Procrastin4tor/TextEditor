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
#define KEY_DOWN 80
#define KEY_UP 72
#define KEY_LEFT 75
#define KEY_RIGHT 77

extern HANDLE hStdOut;
extern CONSOLE_SCREEN_BUFFER_INFO csbInfo;
extern WORD workWindowAttributes; 
ifstream file;

bool menuIsActive = true;
bool editIsActive = false, openIsActive = false, createIsActive = false,
saveIsActive = false, saveAsIsActive = false, specOptionsIsActive = false,
deleteIsActive = false, saveErrorIsActive = false;
bool firstEnter = true;
bool saveChangesForCreate = false, saveChangesForOpen = false;
bool CleanScreenBufferIsActive = false;
const int menuLine = 120;

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

void ClearMenuLines()
{
	SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);
	SetConsoleCursorPosition(hStdOutM, { 0,1 });
	for (int i = 0; i < menuLine; i++)
	{
		cout << " ";
	}
	SetConsoleCursorPosition(hStdOutM, { 0,1 });
}

void ClearScreenBuffer(const vector <string>& textFile)
{
	SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
	SetConsoleCursorPosition(hStdOutM, { 0,2 });
	
	system("cls");
	
	CleanScreenBufferIsActive = true;
	menuIsActive = true;
	
	DrawMenu();

	SetConsoleCursorPosition(hStdOutM, { 0,2 });
	SetConsoleTextAttribute(hStdOutM, workWindowAttributes);

	if (!textFile.empty())
	{
		for (int i = 0; i < textFile.size(); i++)
		{
			cout << textFile[i] << endl;
		}
	}
	CleanScreenBufferIsActive = false;
}

void cursorOffset(const vector <string>& textFile, COORD& cursorPos, const int offset)
{
	if (textFile[cursorPos.Y - 2].length() > textFile[cursorPos.Y - offset].length() && cursorPos.X > textFile[cursorPos.Y - offset].length())
	{
		cursorPos.X = textFile[cursorPos.Y - offset].length();
	}
}

void moveKeyArrows(const HANDLE& hStdOut, const vector <string>& textFile, COORD& cursorPos)
{
	switch (_getch())
	{
	case KEY_DOWN:
		if (cursorPos.Y <= textFile.size())
		{
			cursorOffset(textFile, cursorPos, 1);
			cursorPos.Y++;
			SetConsoleCursorPosition(hStdOut, cursorPos);
		}
		break;

	case KEY_UP:
		if (cursorPos.Y > 2)
		{
			cursorOffset(textFile, cursorPos, 3);
			cursorPos.Y--;
			SetConsoleCursorPosition(hStdOut, cursorPos);
		}
		break;

	case KEY_RIGHT:
		if (!textFile.empty())
		{
			if (cursorPos.X < textFile[cursorPos.Y - 2].length())
			{
				cursorPos.X++;
			}
			else if (cursorPos.X == textFile[cursorPos.Y - 2].length() && cursorPos.Y <= textFile.size())
			{
				cursorPos.X = 0;
				cursorPos.Y++;
			}
			SetConsoleCursorPosition(hStdOut, cursorPos);
		}
		break;

	case KEY_LEFT:
		if (cursorPos.X > 0)
		{
			cursorPos.X--;
		}
		else if (cursorPos.X == 0 && cursorPos.Y != 2)
		{
			cursorPos.X = textFile[cursorPos.Y - 3].length();
			cursorPos.Y--;
		}
		SetConsoleCursorPosition(hStdOut, cursorPos);
		break;
	}
}

void editFile(const HANDLE& hStdOut,vector <string>& textFile, char& key,
	string& text, COORD& cursorPos)
{
	SetConsoleCursorPosition(hStdOut, cursorPos);

	key = _getch();

	if (isprint(key))
	{

		if (cursorPos.X < menuLine)
		{
			cursorPos.X++;
		}
		else
		{
			cursorPos.X = 0;
			cursorPos.Y++;
		}


		if (textFile.empty())
		{
			text.clear();
			text += key;
			textFile.push_back(text);
		}
		else
		{
			textFile[cursorPos.Y - 2].insert(cursorPos.X - 1, 1, key);
		}
		ClearScreenBuffer(textFile);
	}
	else if (iscntrl(key))
	{
		switch (key)
		{
		case KEY_ESCAPE:
			menuIsActive = true;
			editIsActive = false;
			return;

		case KEY_ENTER:
			if (textFile.empty())
			{
				text.clear();
				textFile.push_back(text);
			}
			cursorPos.Y++;
			cout << endl;
			text.clear();
			textFile.emplace(textFile.cbegin() + cursorPos.Y - 2, text);
			ClearScreenBuffer(textFile);
			cursorPos.X = textFile[cursorPos.Y - 2].length();
			return;

		case KEY_BACKSPACE:

			if (cursorPos.X == 0 && cursorPos.Y == 2 && textFile.empty())
			{
				return;
			}

			if (cursorPos.X > 0)
			{
				cursorPos.X--;
			}
			else
			{
				if (cursorPos.Y > 2)
				{
					cursorPos.X = textFile[cursorPos.Y - 3].length();
				}
				else
				{
					if (cursorPos.X != 0 && cursorPos.Y != 2)
					{
						cursorPos.X = textFile[cursorPos.Y - 1].length();
					}
				}

				if (textFile[cursorPos.Y - 2].length() == 0)
				{
					textFile.erase(textFile.cbegin() + cursorPos.Y - 2);
				}
				if (cursorPos.Y > 2)
				{
					cursorPos.Y--;
				}
				ClearScreenBuffer(textFile);
				return;
			}

			if (textFile[cursorPos.Y - 2].length() > 0)
			{
				textFile[cursorPos.Y - 2].erase(cursorPos.X, 1);
				ClearScreenBuffer(textFile);
			}
			return;
		default:
			break;
		}
	}
	else
	{
		moveKeyArrows(hStdOut, textFile, cursorPos);
	}
}

void fileExtension(string& path)
{
	if (path.find(".txt") == string::npos)
	{
		path += ".txt";
	}
}

void saveErrorCheck(string& path)
{
	ClearMenuLines();
	
	if (openIsActive)
	{
		cout << "File doesn't exist! Please, try again.";
		openIsActive = true;
		saveChangesForOpen = false;
	}	
	else if (createIsActive)
	{
		cout << "Can't create a new file! Please, try again.";
		createIsActive = true;
		saveChangesForCreate = false;
	}
	else
	{
		if (saveIsActive)
		{
			cout << "Can't save a file! Please, try again.";
			saveIsActive = true;
		}
		else
		{
			cout << "Can't save a new file! Please, try again.";
			saveAsIsActive = true;
		}
	}
	path.clear();
	Sleep(2000);
}

void openFile(const HANDLE& hStdOut,vector <string>& textFile, string& text, 
	COORD& cursorPos, string& path)
{
	fileExtension(path);
	
	SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
	ifstream file(path);

	if (!file.is_open())
	{
		saveErrorCheck(path);
		return;
	}
	else
	{
		textFile.clear();
		ClearScreenBuffer(textFile);
		char ch;
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
				textFile.push_back(text);
				text.clear();
				cout << endl;
				cursorPos.X = 0;
				cursorPos.Y++;
				continue;
			}
			if (cursorPos.X < menuLine)
			{
				cursorPos.X++;
			}
			else
			{
				textFile.push_back(text);
				text.clear();
				cursorPos.X = 0;
				cursorPos.Y++;
			}

			text += ch;
			cout << ch;
		}

	}
	textFile.push_back(text);
	file.close();
}

void createFile(string& path)
{
	fileExtension(path);
	
	ofstream file;
	file.open(path);
	if (!file.is_open())
	{
		saveErrorCheck(path);
		return;
	}
	file.close();
}

void saveFile(vector <string>& textFile, string& path)
{
	ofstream file;
	file.open(path);
	if (!file.is_open())
	{
		saveErrorCheck(path);
		return;
	}
	else
	{
		for (int i = 0; i < textFile.size(); i++)
		{
			file << textFile[i] << endl;
		}
		file.close();
	}
}

void saveChanges(vector <string>& textFile, string& path)
{
	if (saveChangesForCreate || saveChangesForOpen)
	{
		if (saveChangesForCreate)
		{
			createIsActive = true;
			saveChangesForCreate = false;
		}
		else
		{
			openIsActive = true;
			saveChangesForOpen = false;
		}
		menuIsActive = false;
		path.clear();
		textFile.clear();
	}
}

void unsavedDataCheck(vector <string>& textFile, string& path)
{
		menuIsActive = true;
		ClearMenuLines();
		
		if (!path.empty())
		{
			cout << "There is unsaved data. Do you want to save changes in " << path << " ?";
			saveIsActive = true;
		}
		else
		{
			cout << "There is unsaved data. Do you want to save it as a new file?";
			saveAsIsActive = true;
		}
		drawSaveChanges();
		if (!saveChangesForOpen || !saveChangesForCreate)
		{
			path.clear();
			textFile.clear();
		}
}

void deleteWord(vector <string>& textFile, COORD& cursorPos)
{
	if (cursorPos.X == 0 && cursorPos.Y == 2)
	{
		return;
	}
	char SPACE = ' ';
	while (cursorPos.X > 0 && textFile[cursorPos.Y - 2].at(cursorPos.X - 1) != SPACE)
	{	
			textFile[cursorPos.Y - 2].erase(cursorPos.X - 1, 1);          //
				cursorPos.X--;												//
	}
	while (cursorPos.X > 0 && textFile[cursorPos.Y - 2].at(cursorPos.X - 1) == SPACE)
	{
		textFile[cursorPos.Y - 2].erase(cursorPos.X - 1, 1);
		cursorPos.X--;
	}
	if (textFile[cursorPos.Y - 2].length()  == 0 && cursorPos.Y > 2)
	{
		cursorPos.X = textFile[cursorPos.Y - 3].length();					//
		textFile.erase(textFile.cbegin() + cursorPos.Y - 2);				//
		cursorPos.Y--;
	}
}

void deleteLine(vector <string>& textFile, COORD& cursorPos)
{
	if (!textFile.empty())
	{
		if (cursorPos.Y > 2)
		{
			cursorPos.X = textFile[cursorPos.Y - 3].length();
			textFile.erase(textFile.cbegin() + cursorPos.Y - 2);
			cursorPos.Y--;
		}
		else
		{
			cursorPos.X = textFile[cursorPos.Y - 1].length();
			textFile.erase(textFile.cbegin() + cursorPos.Y - 2);
		}
	}
}

void deleteFile(string& path)
{
	fileExtension(path);
	
	ClearMenuLines();
	SetConsoleCursorPosition(hStdOutM, { 0, 1 });
	
	if (remove(path.c_str()) != 0)
	{
		cout << "Error. Could find the file with this name";
	}
	else
	{
		cout << "File was deleted successfully!!!";
	}
}

void specOptions(HANDLE& hStdOut, vector <string>& textFile, 
	char& key, COORD& cursorPos)
{
	SetConsoleCursorPosition(hStdOut, cursorPos);

	key = _getch();

	if (isprint(key))
	{
		int hotKey = _getch();

		if ((key == 'D' && hotKey == 'W') || (key == 'd' && hotKey == 'w'))
		{
			deleteWord(textFile, cursorPos);
		}
		else if ((key == 'D' && hotKey == 'L') || (key == 'd' && hotKey == 'l'))
		{
			deleteLine(textFile, cursorPos);
		}
		ClearScreenBuffer(textFile);
	}
	else if (iscntrl(key))
	{
		switch (key)
		{
		case KEY_ESCAPE:
			menuIsActive = true;
			specOptionsIsActive = false;
			return;

		default:
			break;
		}
	}
	else
	{
		moveKeyArrows(hStdOut, textFile, cursorPos);
	}
}

int main()
{
	hStdOutM = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdOutM, &csbInfo);
	SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
	system("cls");

	char key;
	vector <string> textFile;
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
			if (openIsActive)
				{
					if (!textFile.empty() || !path.empty())
					{
						unsavedDataCheck(textFile, path);
						continue;
					}				
					menuIsActive = true;
					editIsActive = true;
			
					ClearMenuLines();
					cout << "Choose file directory or file's name:";
					cin >> path;
					SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
					openFile(hStdOut,textFile, text, cursorPos, path);
					if (!saveErrorIsActive)
					{
						openIsActive = false;
					}
					continue;
				}			
			if (editIsActive)
				{	
					editFile(hStdOut, textFile, key, text, cursorPos);
				}
			if (specOptionsIsActive)
				{
					specOptions(hStdOut, textFile, key, cursorPos);
				}
			if (createIsActive)
			{							
				if (!textFile.empty() || !path.empty())
				{
					unsavedDataCheck(textFile, path);
					continue;
				}				
					menuIsActive = true;
					editIsActive = true;
					ClearMenuLines();
					cout << "Choose file's name:";
					cin >> path;
					createFile(path);
					if (!saveErrorIsActive)
					{
						createIsActive = false;
					}
					openFile(hStdOut, textFile, text, cursorPos, path);
					SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);
					SetConsoleCursorPosition(hStdOutM, { 0,1 });
					ClearMenuLines();
					cout << "File was created successfully!!!";
					Sleep(1000);
					menuIsActive = true;
				}
			if (deleteIsActive)
			{
				menuIsActive = true;
				deleteIsActive = false;
				ClearMenuLines();
				cout << "Choose file's name that will be deleted:";
				cin >> path;
				deleteFile(path);
				path.clear();
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
					ClearMenuLines();
					saveFile(textFile, path);
					if (!saveErrorIsActive)
					{
						saveIsActive = false;
					}
					cout << "File was saved successfully!!!";
					Sleep(1000);
				}
				file.close();		
				saveChanges(textFile, path);
			}
			if (saveAsIsActive)
			{
				menuIsActive = true;
				ClearMenuLines();
				cout << "Choose a new file's name:";
				cin >> path;
				saveFile(textFile, path);
				if (!saveErrorIsActive)
				{
					saveAsIsActive = false;
				}
				ClearMenuLines();
				cout << "New file was saved successfully!!!";
				Sleep(2000);
				saveChanges(textFile, path);
			}
		}
	}
	return 0;
}