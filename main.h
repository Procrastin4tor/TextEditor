#pragma once

extern bool editIsActive, openIsActive, createIsActive, saveIsActive, saveAsIsActive;
extern bool menuIsActive;

extern HANDLE hStdOutM;
extern WORD workWindowAttributes;
extern WORD nonWorkWindowAttributes; 
extern COORD cursorPos;
extern WORD inactiveItemAttributes; 
extern WORD activeItemAttributes;

void DrawMenu();
void drawFileOptions();
void drawSaveError();
void ClearMenuLines();
