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

extern COORD curspos = { 0,1 };
WORD workWindowAttributes = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN; 
WORD nonWorkWindowAttributes = BACKGROUND_INTENSITY; 
extern WORD inactiveItemAttributes = BACKGROUND_INTENSITY; 
extern WORD activeItemAttributes = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;

enum class menuItems { MNUFILE, MNUHELP, MNUEXIT };
enum class FileOptionsItems { MNUEDIT,MNUSPECOPTIONS, MNUCREATE,MNUDELETE, MNUOPEN, MNUSAVE, MNUSAVEAS };
enum class SaveErrorOptionsItems { MNUCREATE, MNUOPEN, MNUSAVEAS };
enum class saveChangesItems { MNUYES, MNUNO };
enum class helpItems {MNUHOWTO, MNUDELWORD, MNUDELLINE, MNUOK };
enum class exitItems { MNUYES, MNUNO };

const int numMenu = 3;
const int numFileOptions = 7;
const int numSaveErrorOptions = 3;
const int numSaveChanges = 2;
const int numHelp = 4;
const int numExit = 2;


FileOptionsItems sel = FileOptionsItems::MNUEDIT;

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
	{6,1," SPEC.OPTIONS "},
	{20,1," CREATE "},
	{28,1," DELETE "},
	{36,1," OPEN "},
	{42,1," SAVE "},
	{48,1," SAVE AS "}
};
ITEM saveErrorOptions[numSaveErrorOptions] = {
	{50,1," CREATE "},
	{60,1," OPEN "},
	{67,1," SAVE AS "}
};
ITEM saveChanges[numSaveChanges] = {
	{80,1," YES "},
	{90,1," NO "}
};
ITEM help[numHelp] = {
	{50,4,"How to use Special Options?"},
	{40,6,"Press \"D\" + \"W\" for deleting a word in a line" },
	{40,7,"Press \"D\" + \"L\" for deleting a line" },
	{60,10," OK " }
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

				itemMenu(saveErrorOptions, (int)sel, false);

				if ((int)sel < numSaveErrorOptions - 1)
				{
					sel = (SaveErrorOptionsItems)((int)sel + 1);
				}
				else
				{
					sel = SaveErrorOptionsItems::MNUCREATE;
				}
				itemMenu(saveErrorOptions, (int)sel, true);
				showCursor(false);
				break;

			case KEY_ARROW_LEFT:

				itemMenu(saveErrorOptions, (int)sel, false);

				if ((int)sel > 0)
				{
					sel = (SaveErrorOptionsItems)((int)sel - 1);
				}
				else
				{
					sel = SaveErrorOptionsItems::MNUSAVEAS;
				}
				itemMenu(saveErrorOptions, (int)sel, true);
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
					ClearMenuLines();
					return;

				case SaveErrorOptionsItems::MNUSAVEAS:
					saveAsIsActive = true;
					menuIsActive = false;
					ClearMenuLines();
					return;
				}
				break;
			}
		}
	}
}

void drawSaveChanges()
{
	if (menuIsActive == false)
	{
		return;
	}
	saveChangesItems sel = saveChangesItems::MNUNO;

	SetConsoleTextAttribute(hStdOutM, inactiveItemAttributes);

	for (int i = 0; i < numSaveChanges; i++)
	{
		gotoxy(saveChanges[i].x, saveChanges[i].y);
		cout << saveChanges[i].str;
	}
	itemMenu(saveChanges, (int)sel, true);

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

				itemMenu(saveChanges, (int)sel, false);

				if ((int)sel < numSaveChanges - 1)
				{
					sel = (saveChangesItems)((int)sel + 1);
				}
				else
				{
					sel = saveChangesItems::MNUYES;
				}
				itemMenu(saveChanges, (int)sel, true);
				showCursor(false);
				break;

			case KEY_ARROW_LEFT:
				
				itemMenu(saveChanges, (int)sel, false);
				
				if ((int)sel > 0)
				{
					sel = (saveChangesItems)((int)sel - 1);
				}
				else
				{
					sel = saveChangesItems::MNUNO;		
				}
				itemMenu(saveChanges, (int)sel, true);
				showCursor(false);
				break;

			case KEY_ENTER:

				gotoxy(curspos.X, curspos.Y);
				SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
				showCursor(true);

				switch (sel)
				{
				case saveChangesItems::MNUYES:
					
					menuIsActive = false;
					
					if (createIsActive)
					{
						createIsActive = false;
						saveChangesForCreate = true;
					}
					else if (openIsActive)
					{
						openIsActive = false;
						saveChangesForOpen = true;
					}
					return;
					
				case saveChangesItems::MNUNO:	
					
					menuIsActive = false;
					saveIsActive = false;
					saveAsIsActive = false;
					return;
				}
				break;
			}
		}
	}
}

void drawFileOptions()
{
	ClearMenuLines();
	
	if (editIsActive)
	{
		sel = FileOptionsItems::MNUEDIT;
	}
	else if (specOptionsIsActive)
	{
		sel = FileOptionsItems::MNUSPECOPTIONS;
	}
	
	SetConsoleTextAttribute(hStdOutM, inactiveItemAttributes);

	for (int i = 0; i < numFileOptions; i++)
	{
		gotoxy(fileOptions[i].x, fileOptions[i].y);
		cout << fileOptions[i].str;
	}
	itemMenu(fileOptions, (int)sel, true);

	fflush(stdin);

	if (CleanScreenBufferIsActive)
	{
		return;
	}

	if (editIsActive)
	{
		SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
		return;
	}
	if (openIsActive)
	{
		return;
	}

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
				
				itemMenu(fileOptions, (int)sel, false);

				if ((int)sel < numFileOptions - 1)
				{
					sel = (FileOptionsItems)((int)sel + 1);
				}
				else
				{
					sel = FileOptionsItems::MNUEDIT;
				}
				itemMenu(fileOptions, (int)sel, true);
				showCursor(false);
				break;

			case KEY_ARROW_LEFT:

				itemMenu(fileOptions, (int)sel, false);

				if ((int)sel > 0)
				{
					sel = (FileOptionsItems)((int)sel - 1);
				}
				else
				{
					sel = FileOptionsItems::MNUSAVEAS;
				}
				itemMenu(fileOptions, (int)sel, true);
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

				case FileOptionsItems::MNUSPECOPTIONS:
					menuIsActive = false;
					specOptionsIsActive = true;
					return;
				
				case FileOptionsItems::MNUOPEN:
					menuIsActive = false;
					openIsActive = true;
					return;
					
				case FileOptionsItems::MNUCREATE:
					menuIsActive = false;
					createIsActive = true;
					return;

				case FileOptionsItems::MNUDELETE:
					menuIsActive = false;
					deleteIsActive = true;
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

void drawHelp()
{
	helpItems sel = helpItems::MNUOK;

	SetConsoleTextAttribute(hStdOutM, nonWorkWindowAttributes);
	int x = 38;
	int y = 3;

	for (int i = 0; i < 9; i++)
	{
		SetConsoleCursorPosition(hStdOutM, { (short)x, (short)y });
		
		for (int j = 0; j < 49; j++)
		{
			cout << " ";
		}
		y++;
	}

	for (int i = 0; i < numHelp; i++)
	{
		gotoxy(help[i].x, help[i].y);
		cout << help[i].str;
	}
	itemMenu(help, (int)sel, true);

	char okButton;

	okButton = _getch();

	switch (okButton)
	{
	case KEY_ESCAPE:
	case KEY_ENTER:
		SetConsoleTextAttribute(hStdOutM, workWindowAttributes);
		system("cls");
		DrawMenu();
	}
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
				
				itemMenu(Exit, (int)sel, false);
				
				if ((int)sel < numExit - 1)
				{	
					sel = (exitItems)((int)sel + 1);
				}
				else
				{
					sel = exitItems::MNUYES;
				}
				itemMenu(Exit, (int)sel, true);
				showCursor(false);
				break;

			case KEY_ARROW_LEFT:
				
				itemMenu(Exit, (int)sel, false);
				
				if ((int)sel > 0)
				{
					sel = (exitItems)((int)sel - 1);
				}
				else
				{
					sel = exitItems::MNUNO;
				}
				itemMenu(Exit, (int)sel, true);
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

	if (CleanScreenBufferIsActive)
	{
		menuIsActive = false;
		drawFileOptions();
		return;
	}

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

				itemMenu(menu, (int)sel, false);

				if ((int)sel < numMenu - 1)
				{
					sel = (menuItems)((int)sel + 1);
				}
				else
				{
					sel = menuItems::MNUFILE;
				}
				itemMenu(menu, (int)sel, true);
				showCursor(false);
				break;

			case KEY_ARROW_LEFT:

				itemMenu(menu, (int)sel, false);
				
				if ((int)sel > 0)
				{
					sel = (menuItems)((int)sel - 1);
				}
				else
				{
					sel = menuItems::MNUEXIT;
				}
				itemMenu(menu, (int)sel, true);
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
					drawHelp();
					
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