#pragma once

extern bool editIsActive, openIsActive, createIsActive, saveIsActive, saveAsIsActive, specOptionsIsActive, deleteIsActive;
extern bool menuIsActive;
extern bool CleanScreenBufferIsActive;
extern bool saveChangesForCreate, saveChangesForOpen;
extern const int menuLine;

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
void drawSaveChanges();
