#include <iostream>
#include <windows.h>
#include <clocale>
#include <conio.h>
#include "main.h"

using namespace std;

#define KEY_ARROW_DOWN 80
#define KEY_ARROW_UP 72
#define KEY_ARROW_LEFT 75
#define KEY_ARROW_RIGHT 77
#define KEY_ESCAPE 27
#define KEY_ENTER 13

HANDLE hStdOutM;
CONSOLE_SCREEN_BUFFER_INFO csbInfo;

extern COORD curspos = { 0,1 }; // координаты курсора
WORD workWindowAttributes = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN; // цвет рабочей области
WORD nonWorkWindowAttributes = BACKGROUND_INTENSITY; // цвет нерабочей области
extern WORD inactiveItemAttributes = BACKGROUND_INTENSITY; // невыделенный пункт меню
extern WORD activeItemAttributes = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;

enum class menuItems { MNUFILE, MNUHELP, MNUEXIT };
enum class FileOptionsItems { MNUEDIT, MNUCREATE, MNUOPEN, MNUSAVE, MNUSAVEAS };
enum class SaveErrorOptionsItems { MNUCREATE, MNUOPEN };
enum class exitItems { MNUYES, MNUNO };

const int numMenu = 3;
const int numFileOptions = 5;
const int numSaveErrorOptions = 2;
const int numExit = 2;

struct ITEM {
	int x, y; // столбец(x) и строка(y) консоли
	const char* str; // название пункта меню
};

ITEM menu[numMenu] = {
	{0,0," FILE "},
	{6,0," HELP "},
	{12,0," EXIT "}
};
ITEM fileOptions[numFileOptions] = {
	{0,1," EDIT "},
	{6,1," CREATE "},
	{14,1," OPEN "},
	{20,1," SAVE "},
	{26,1," SAVE AS "}
};
ITEM saveErrorOptions[numSaveErrorOptions] = {
	{50,1," CREATE "},
	{60,1," OPEN "}	
};
ITEM Exit[numExit] = {
	{50,1," YES "},
	{60,1," NO "}
};

void gotoxy(int x, int y)
{
	COORD cursorPos = { x, y };
	SetConsoleCursorPosition(hStdOutM, cursorPos);
}

void itemMenu(ITEM* item, int sel, bool activate)
{
	WORD itemAttributes;

	itemAttributes = (activate) ? activeItemAttributes : inactiveItemAttributes;
	gotoxy(item[sel].x, item[sel].y);
	SetConsoleTextAttribute(hStdOutM, itemAttributes);
	cout << item[sel].str;
}

void showCursor(bool visible)
{
	CONSOLE_CURSOR_INFO ccInfo;
	ccInfo.bVisible = visible;
	ccInfo.dwSize = 20;
	SetConsoleCursorInfo(hStdOutM, &ccInfo);
}

void getCursorPosition()
{
	GetConsoleScreenBufferInfo(hStdOutM, &csbInfo);
	curspos = csbInfo.dwCursorPosition;
}

void drawSaveError()
{
	if (menuIsActive == false)
	{
		return;
	}

	SaveErrorOptionsItems sel = SaveErrorOptionsItems::MNUCREATE;

	SetConsoleTextAttribute(hStdOutM, inactiveItemAttributes);

	for (int i = 0; i < numSaveErrorOptions; i++)
	{
		gotoxy(saveErrorOptions[i].x, saveErrorOptions[i].y);
		cout << saveErrorOptions[i].str;
	}
	itemMenu(saveErrorOptions, (int)sel, true);

	fflush(stdin);

	int iKey = 67;
	
	while (1)
	{
		if (menuIsActive == false)
		{
			return;
		}
		if (_kbhit())
		{
			iKey = _getch();

			switch (iKey)
			{
			case KEY_ARROW_RIGHT:
				if ((int)sel < numSaveErrorOptions - 1)
				{
					itemMenu(saveErrorOptions, (int)sel, false);
					sel = (SaveErrorOptionsItems)((int)sel + 1);
					itemMenu(saveErrorOptions, (int)sel, true);
				}
				else
				{
					itemMenu(saveErrorOptions, (int)sel, false);
					sel = SaveErrorOptionsItems::MNUCREATE;
					itemMenu(saveErrorOptions, (int)sel, true);
				}
				showCursor(false);
				break;

			case KEY_ARROW_LEFT:
				if ((int)sel > 0)
				{
					itemMenu(saveErrorOptions, (int)sel, false);
					sel = (SaveErrorOptionsItems)((int)sel - 1);
					itemMenu(saveErrorOptions, (int)sel, true);
				}
				else
				{
					itemMenu(saveErrorOptions, (int)sel, false);
					sel = SaveErrorOptionsItems::MNUOPEN;
					itemMenu(saveErrorOptions, (int)sel, true);
				}
				showCursor(false);
				break;

			case KEY_ENTER:
				gotoxy(curspos.X, curspos.Y);
				SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
				showCursor(true);

				switch (sel)
				{
				case SaveErrorOptionsItems::MNUCREATE:
					createIsActive = true;
					menuIsActive = false;
					SetConsoleCursorPosition(hStdOutM, { 0,1 });
					SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);
					for (int i = 0; i < 120; i++)
					{
						cout << " ";
					}
					return;

				case SaveErrorOptionsItems::MNUOPEN:
					openIsActive = true;
					menuIsActive = false;
					SetConsoleCursorPosition(hStdOutM, { 0,1 });
					SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);
					for (int i = 0; i < 120; i++)
					{
						cout << " ";
					}
					return;
				}
				break;
			}
		}
	}


}

void drawFileOptions()
{

	SetConsoleCursorPosition(hStdOutM, { 0,1 });
	SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);

	for (int i = 0; i < 120; i++)
	{
		cout << " ";
	}

	FileOptionsItems sel = FileOptionsItems::MNUEDIT;

	SetConsoleTextAttribute(hStdOutM, inactiveItemAttributes);

	for (int i = 0; i < numFileOptions; i++)
	{
		gotoxy(fileOptions[i].x, fileOptions[i].y);
		cout << fileOptions[i].str;
	}
	itemMenu(fileOptions, (int)sel, true);

	fflush(stdin);

	int iKey = 67;

	while (1)
	{
		
		if (_kbhit())
		{
			iKey = _getch();

			switch (iKey)
			{
			case KEY_ESCAPE:
				menuIsActive = true;
				DrawMenu();
				return;

			case KEY_ARROW_RIGHT:
				if ((int)sel < numFileOptions - 1)
				{
					itemMenu(fileOptions, (int)sel, false);
					sel = (FileOptionsItems)((int)sel + 1);
					itemMenu(fileOptions, (int)sel, true);
				}
				else
				{
					itemMenu(fileOptions, (int)sel, false);
					sel = FileOptionsItems::MNUEDIT;
					itemMenu(fileOptions, (int)sel, true);
				}
				showCursor(false);
				break;

			case KEY_ARROW_LEFT:
				if ((int)sel > 0)
				{
					itemMenu(fileOptions, (int)sel, false);
					sel = (FileOptionsItems)((int)sel - 1);
					itemMenu(fileOptions, (int)sel, true);
				}
				else
				{
					itemMenu(fileOptions, (int)sel, false);
					sel = FileOptionsItems::MNUSAVEAS;
					itemMenu(fileOptions, (int)sel, true);
				}
				showCursor(false);
				break;

			case KEY_ENTER:
				gotoxy(curspos.X, curspos.Y);
				SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
				showCursor(true);

				switch (sel)
				{
				case FileOptionsItems::MNUEDIT:	
					menuIsActive = false;
					editIsActive = true;
					return;
				
				case FileOptionsItems::MNUOPEN:
					menuIsActive = false;
					openIsActive = true;
					return;
					
				case FileOptionsItems::MNUCREATE:
					menuIsActive = false;
					createIsActive = true;
					return;
					
				case FileOptionsItems::MNUSAVE:
					menuIsActive = false;
					saveIsActive = true;
					return;
					
				case FileOptionsItems::MNUSAVEAS:	
					menuIsActive = false;
					saveAsIsActive = true;
					return;
				}
				break;
			}
		}
	}
	SetConsoleCursorPosition(hStdOutM, { 0,0 });
}

void drawExit()
{
	if (menuIsActive == false)
	{
		return;
	}
	
	SetConsoleCursorPosition(hStdOutM, { 0,1 });
	SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);
	cout << "Do you really want to exit?";

	exitItems sel = exitItems::MNUNO;

	SetConsoleTextAttribute(hStdOutM, inactiveItemAttributes);

	for (int i = 0; i < numExit; i++)
	{
		gotoxy(Exit[i].x, Exit[i].y);
		cout << Exit[i].str;
	}
	itemMenu(Exit, (int)sel, true);

	fflush(stdin);

	int iKey = 67;

	while (1)
	{
		if (menuIsActive == false)
		{
			return;
		}
		if (_kbhit())
		{
			iKey = _getch();

			switch (iKey)
			{
			case KEY_ARROW_RIGHT:
				if ((int)sel < numExit - 1)
				{
					itemMenu(Exit, (int)sel, false);
					sel = (exitItems)((int)sel + 1);
					itemMenu(Exit, (int)sel, true);
				}
				else
				{
					itemMenu(Exit, (int)sel, false);
					sel = exitItems::MNUYES;
					itemMenu(Exit, (int)sel, true);
				}
				showCursor(false);
				break;

			case KEY_ARROW_LEFT:
				if ((int)sel > 0)
				{
					itemMenu(Exit, (int)sel, false);
					sel = (exitItems)((int)sel - 1);
					itemMenu(Exit, (int)sel, true);
				}
				else
				{
					itemMenu(Exit, (int)sel, false);
					sel = exitItems::MNUNO;
					itemMenu(Exit, (int)sel, true);
				}
				showCursor(false);
				break;

			case KEY_ENTER:
				gotoxy(curspos.X, curspos.Y);
				SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
				showCursor(true);

				switch (sel)
				{
				case exitItems::MNUYES:	
					exit(0);

				case exitItems::MNUNO:
					ClearMenuLines();
					return;
				}
				break;
			}
		}
	}

}

void DrawMenu()
{
	
	if (menuIsActive == false)
	{
		return;
	}

	SetConsoleCursorPosition(hStdOutM, { 0,0 });
	SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);

	for (int i = 0; i < 240; i++)
	{
		cout << " ";
	}
	
	menuItems sel = menuItems::MNUFILE;

	SetConsoleTextAttribute(hStdOutM, inactiveItemAttributes);

	for (int i = 0; i < numMenu; i++)
	{
		gotoxy(menu[i].x, menu[i].y);
		cout << menu[i].str;
	}
	itemMenu(menu, (int)sel, true);

	fflush(stdin);

	int iKey = 67;

	while (1)
	{
		if (menuIsActive == false)
		{
			return;
		}
		if (_kbhit())
		{
			iKey = _getch();

			switch (iKey)
			{
			case KEY_ARROW_RIGHT:
				if ((int)sel < numMenu - 1)
				{
					itemMenu(menu, (int)sel, false);
					sel = (menuItems)((int)sel + 1);
					itemMenu(menu, (int)sel, true);
				}
				else
				{
					itemMenu(menu, (int)sel, false);
					sel = menuItems::MNUFILE;
					itemMenu(menu, (int)sel, true);
				}
				showCursor(false);
				break;

			case KEY_ARROW_LEFT:
				if ((int)sel > 0)
				{
					itemMenu(menu, (int)sel, false);
					sel = (menuItems)((int)sel - 1);
					itemMenu(menu, (int)sel, true);
				}
				else
				{
					itemMenu(menu, (int)sel, false);
					sel = menuItems::MNUEXIT;
					itemMenu(menu, (int)sel, true);
				}
				showCursor(false);
				break;

			case KEY_ENTER:
				gotoxy(curspos.X, curspos.Y);
				SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
				showCursor(true);

				switch (sel)
				{
				case menuItems::MNUFILE:
					drawFileOptions();
					break;

				case menuItems::MNUHELP:
					//drawSettings();
					getCursorPosition();
					break;

				case menuItems::MNUEXIT:
					drawExit();	
					break;
				}
				break;
			}
		}
	}
	SetConsoleCursorPosition(hStdOutM, { 0,0 });	
}