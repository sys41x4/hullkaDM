#include "config/config.h"
#include "lib/cryptography.h"
#include "lib/security.h"
#include <locale.h>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;


/*
// TitleBar Item ID List

1. Power
2. Utilities
3. <DateTime>
4. Desktop Manager

*/

char desktopEnvironmentsSubItems[30][30];
int titleBarItemTree[2];

int availableDesktopManagerCount;

char desktopManagerSubItems[1][20];

WINDOW *mainScreenWin, *subItemListWindow, *authBox, *accountPicBox, *titleBar_subwin; // User Account Picture Box
WINDOW *messageBoxBorderWindow, *messageBox_msg;
int winMaxX, winMaxY;
int accountPicBoxMaxX, accountPicBoxMaxY, accountPicBoxMaxW, accountPicBoxMaxH;
int msgBoxMaxX, msgBoxMaxY, msgBoxMaxW, msgBoxMaxH;
int loginBoxMaxX, loginBoxMaxY;

char username[32];
char userpass[255]; // no password limit



// int usernameVisibilityConf[3] = {0, 0, 0}; // {[0..2], [0..1], [0..1]}Default username_visibility_config
// int userpassVisibilityConf[3] = {1, 0, 0}; // Default userpass_visibility_config

WINDOW *loginColourMatrixWin;
int loginColourMatrixConf[] = {}; // {COORDINATE-Y, COORDINATE-X, HEIGHT, WIDTH}


void setLoginMatrixWindow(WINDOW *win){
    loginColourMatrixWin = win;
}

void loginMatrixSetConfig(int y, int x, int h, int w){
    loginColourMatrixConf[0] = y;
    loginColourMatrixConf[1] = x;
    loginColourMatrixConf[2] = h;
    loginColourMatrixConf[3] = w;
}


void delete_window(WINDOW *win){
    delwin(win);
    free(win);
}

void initColor(){
   // must be called after initializing the frames

   /*

// COLORS MAPOUT //

0: [1-8] | [COLOR_BLACK, COLOR_RED, COLOR_YELLOW, COLOR_GREEN, COLOR_WHITE, COLOR_MAGENTA, COLOR_BLUE, COLOR_CYAN] : size:8 | Exclude the following for directly calling them in any case
1: [9-16/X] : size:8 | Pre Setup Colours | will be loaded from config file | minSize=8 | maxSize=USER_PREFERENCE
2: [<totalRandomizedColors>] | <randomize <totalRandomizedColors> colors> : size <totalRandomizedColors> | Randomization will be done only once on initial loading of the executable, to reduce memory consumpti>
3: [username visibility colourmap] : size:3 | will get colors from [9-16]
4: | [userpass visibility colourmap] : size:3 | will get colors from [9-16]
5: [Profile Pic] : size:(Relative)maxX*maxY | 1D-array | available colour : [1-8]+[9-16]+<totalRandomizedColors>
6: [Login Randomization colourMap] : size:(Relative)maxX*maxY | 1D-array | available colour : [1-8]+[9-16]+<totalRandomizedColors>


*/

   int fg, bg;
   int storedColorsStartIndex = 0;
   // int totalASCIIcolors = sizeof(asciiColors)/sizeof(asciiColors[0]);
   // int totalManualColors = 8;

   // Default Stored Colour blocks | fg==bg
   for(int i=1; i<=totalASCIIcolors; i++){
       init_pair(i, asciiColors[i+storedColorsStartIndex], asciiColors[i+storedColorsStartIndex]);
   }

   // Manual Colour Setup
   init_pair(9, COLOR_WHITE, COLOR_BLACK);
   init_pair(10, COLOR_WHITE, COLOR_RED);
   init_pair(11, COLOR_BLACK, COLOR_YELLOW);
   init_pair(12, COLOR_WHITE, COLOR_GREEN);
   init_pair(13, COLOR_BLACK, COLOR_WHITE);
   init_pair(14, COLOR_WHITE, COLOR_MAGENTA);
   init_pair(15, COLOR_WHITE, COLOR_BLUE);
   init_pair(16, COLOR_BLACK, COLOR_CYAN);

   // Initial Colour Randomization
   srand(time(NULL));
   for(int i=1; i<=totalRandomizedColors; i++){
       fg = asciiColors[rand() % 8];
       bg = asciiColors[rand() % 8];
       init_pair(i+totalASCIIcolors+totalManualColors, fg, bg);
   }


}

void gen_randColorMap(WINDOW *win, int y, int x, int h, int w){
    // Draw Random bitmap
    int randColorID=1;
    int totalAvailableColour = totalASCIIcolors+totalManualColors+totalRandomizedColors;

    srand(time(NULL));
    for (int i = y; i < h; i++) {
        wmove(win, i, x);
        for (int j = 0; j < w; j++) {
              randColorID = rand()%totalAvailableColour;
   //         int fg = asciiColors[rand() % 8] + rand()%7;
   //         int bg = asciiColors[rand() % 8] * rand()%15;
   //         init_pair(randColorID, fg, bg);
            // wattron(win, COLOR_PAIR(color));
            wattron(win, COLOR_PAIR(randColorID));
            waddch(win, 'A' + rand() % 26);
            wattroff(win, COLOR_PAIR(randColorID));
            // wattroff(win, COLOR_PAIR(color));
        }
    }
    wrefresh(win);
}

void show_datetime(WINDOW *win, int y, int x){
    wmove(win, y, x);
    FILE *pp;
    cbreak();

    if ((pp = popen("/usr/bin/date | tr -s '\n' ' '", "r")) != 0) {
        char buffer[BUFSIZ];
        while (fgets(buffer, sizeof(buffer), pp) != 0) {
            waddstr(win, buffer);
        }
        wrefresh(win);
        pclose(pp);
    }
}


void drawCMDStr(WINDOW *win, int y, int x, int alignX, int is_cmd, int colorID, const char* msg){
    // wmove(win, y, x);
    FILE *pp;
    cbreak();
    // int strLen;
    /* if alignX==0 : left align
          alignX==1 : centre align
          alignX==2 : right align
    */

    if(is_cmd==0){
        if(alignX==1){
            wmove(win, y, x-(strlen(msg)/2));
        }
        else if(alignX==1){
            wmove(win, y, x-strlen(msg));
        }
        wattron(win, COLOR_PAIR(colorID));
        waddstr(win, msg);
        wattroff(win, COLOR_PAIR(colorID));

    }
    else{

        if ((pp = popen(msg, "r")) != 0) {
            char buffer[BUFSIZ];
            while (fgets(buffer, sizeof(buffer), pp) != 0) {

                if(alignX==1){
                    wmove(win, y, x-(strlen(buffer)/2));
                }
                else if(alignX==1){
                    wmove(win, y, x-strlen(buffer));
                }
                wattron(win, COLOR_PAIR(colorID));
                waddstr(win, buffer);
                wattroff(win, COLOR_PAIR(colorID));
            }
            pclose(pp);
        }
    }
}



void updateRequestedUSRENV(){
    FILE *pp;
    cbreak();
    char defaultEnv[] = "Default";
    char cmd[100] = "sudo cat /var/lib/AccountsService/users/";
    strcat(cmd, username);
    strcat(cmd, " 2>/dev/null | grep 'XSe*' | cut -d '=' -f 2");
    if ((pp = popen(cmd, "r")) != 0) {
        char buffer[BUFSIZ];
        while (fgets(buffer, sizeof(buffer), pp) != 0) {
            for(int i=5; i<strlen(buffer)+4; i++){titleBarItems[2][i]=buffer[i-5];};
        }
        pclose(pp);
    }
}

void genProfilePicture(int h, int w, int y, int x){
    // Draw Random bitmap
    int randColorID=1;
    int totalAvailableColour = totalASCIIcolors+totalManualColors+totalRandomizedColors;

    box(accountPicBox, 0, 0);
    srand(time(NULL));
    for (int i = y; i < h; i++) {
        wmove(accountPicBox, i, x);
        for (int j = 0; j < w; j++) {
           randColorID = rand()%totalAvailableColour;

            wattron(accountPicBox, COLOR_PAIR(randColorID));
            waddch(accountPicBox, 'A' + rand() % 26);
            wattroff(accountPicBox, COLOR_PAIR(randColorID));
        }
    }
    wrefresh(accountPicBox);
}


void messageBoxWindow(int h, int w, int y, int x, int is_cmd, const char* title, const char* msg){

   /* If is_cmd==1,  then the supplied char array, will be eecuted and the output will be printed in the message box
      else if is_cmd==0, then the message will directly be printed in the message box
   */

    FILE *pp;
    int ch;

    messageBoxBorderWindow = newwin(h, w, y, x);
    messageBox_msg = newwin(h-6, w-4, y+4, x+2);

    box(messageBoxBorderWindow, 0, 0);

    // Draw Message Box Title

    wattron(messageBoxBorderWindow, COLOR_PAIR(12));
    mvwprintw(messageBoxBorderWindow, 2, (w-(sizeof(title)/sizeof(title[0])))/2, title);
    wattroff(messageBoxBorderWindow, COLOR_PAIR(12));

    wrefresh(messageBoxBorderWindow);

    noecho();
    keypad(messageBox_msg, TRUE);

    wmove(messageBox_msg, 0, 0);

    if(is_cmd==1){

        if ((pp = popen(msg, "r")) != 0) {
            char buffer[BUFSIZ];
            while (fgets(buffer, sizeof(buffer), pp) != 0) {

                waddstr(messageBox_msg, buffer);
            }
            wrefresh(messageBox_msg);
            pclose(pp);
        }
    }
    else{
        mvwprintw(messageBox_msg, 0, 0, msg);
        wrefresh(messageBox_msg);
    }


    do{
        ch = wgetch(messageBox_msg);     /* refresh, accept single keystroke of input */
        if((ch == KEY_ESCAPE) || (ch == '\n') || (ch == 'q') || (ch == KEY_BACKSPACE) || (ch == 'w') || (ch == 'a') || (ch == 's') || (ch == 'd') || (ch == '4') || (ch == '8') || (ch == '2') || (ch == '6') || (ch == '5') || (ch == KEY_HOME) || (ch == KEY_EXIT)){ // If Enter is pressed

            wclear(messageBox_msg);
            werase(messageBox_msg);
            // delwin(messageBox_msg);

            wclear(messageBoxBorderWindow);
            werase(messageBoxBorderWindow);
            // delwin(messageBoxBorderWindow);

            wrefresh(messageBox_msg);
            wrefresh(messageBoxBorderWindow);

            // delwin(messageBox_msg);
            // delwin(messageBoxBorderWindow);

            genProfilePicture(accountPicBoxMaxH-1, accountPicBoxMaxW-4, 1, 2);
            break;
        }
        else if(ch == '\t'){
            // login_passField(win, (loginBoxMaxY/2), (loginBoxMaxX/4)+14);
        }
        else{}
    }while(1);
}



void list_available_desktop_environments(WINDOW *win, int y, int x){
    wmove(win, y, x);
    FILE *pp;
    cbreak();

    int i=0;
    int j=0;

    if ((pp = popen("ls /usr/share/xsessions | rev | cut -d '.' -f 2 | rev | tr -s '\n' '\7'", "r")) != 0) {
        char buffer[BUFSIZ];
        while (fgets(buffer, sizeof(buffer), pp) != 0) {
            for(int k=0;k<sizeof(buffer)/sizeof(buffer[0]);k++){
                if(buffer[k]=='\7'){i++;k++;j=0;}
                else if(buffer[k]=='\0'){break;}
                desktopEnvironmentsSubItems[i][j] = buffer[k];
                j++;
            }
        }
        pclose(pp);
    }
}


void subItemListWin(int maxY, int maxX, int minY, int minX, char **charArray){
    // Generate New Window dynamically, on exit delete/free the window before breaking out from the function, to free up sys resource


    if ((maxY != 0) && (maxX != 0)){
        int ch='\0';
        maxY+=2;
        maxX+=2;
        noecho();

        do{
        subItemListWindow = newwin(maxY, maxX, minY, minX);
        keypad(subItemListWindow, TRUE);

        box(subItemListWindow, 0, 0);

            for(int i=0; i<(maxY-2); i++){
                if(i==titleBarItemTree[1]){wattron(subItemListWindow, COLOR_PAIR(13));}
                mvwprintw(subItemListWindow, i+1, 1, charArray[i]);
                if(i==titleBarItemTree[1]){wattroff(subItemListWindow, COLOR_PAIR(13));}

            }


            wrefresh(subItemListWindow);


                ch = wgetch(subItemListWindow);     /* refresh, accept single keystroke of input */
                if((ch == KEY_ESCAPE) || (ch==KEY_LEFT) || (ch=='a') || (ch == 'q') || (ch == KEY_HOME) || (ch == KEY_EXIT)){ // If Enter is pressed
                    for (int i = 0; i < maxY-2; i++) {
                        free(charArray[i]);
                    }
                    free(charArray);
                    wclear(subItemListWindow);
                    werase(subItemListWindow);
                    wrefresh(subItemListWindow);
                    break;
                }
                else if((ch == '\n') || (ch==KEY_RIGHT) || (ch == 'd') || (ch=='6') || (ch=='5')){
                    // Execute Message Window
                    messageBoxWindow(msgBoxMaxH, msgBoxMaxW, msgBoxMaxY, msgBoxMaxX, 1, charArray[titleBarItemTree[1]], subItemsCmd[titleBarItemTree[0]][titleBarItemTree[1]]);

                }
                else if((ch == '\t') || (ch==KEY_DOWN) || (ch == 's') || (ch=='2')){
                    wclear(subItemListWindow);
                    werase(subItemListWindow);

                    if(titleBarItemTree[1]>=(maxY-3)){titleBarItemTree[1]=0;}
                    else{titleBarItemTree[1]++;}
                }
                else if((ch == KEY_BACKSPACE) || (ch==KEY_UP) || (ch == 'w') || (ch=='8')){
                    wclear(subItemListWindow);
                    werase(subItemListWindow);

                    if(titleBarItemTree[1]<=0){titleBarItemTree[1]=(maxY-3);}
                    else{titleBarItemTree[1]--;}

                }
                else{}
            }while(1);
    }

}

// void draw_titlebar(WINDO *titlebar=titleBar_subwin, int itemID=-1);

void draw_titlebar(WINDOW *titlebar, int itemID=-1)
{

    int ch, titlebarCoordX, titlebarCoordY;
    int positionCoordX;
    int spacingX = 3;
    int titleBarItemCount = sizeof(titleBarItems)/sizeof(titleBarItems[0]);
    noecho();
    titleBarItemTree[0]=itemID;
    do{
        keypad(titlebar, TRUE);
        getmaxyx(titlebar, titlebarCoordY, titlebarCoordX);
        positionCoordX=0;

        if(titleBarItemTree[0]==0){wattron(titlebar, COLOR_PAIR(13));}
        wmove(titlebar, 1, positionCoordX+spacingX);
        waddstr(titlebar,titleBarItems[0]);
        if(titleBarItemTree[0]==0){wattroff(titlebar, COLOR_PAIR(13));}

        positionCoordX+=strlen(titleBarItems[0]);

        if(titleBarItemTree[0]==1){wattron(titlebar, COLOR_PAIR(13));}
        wmove(titlebar, 1, positionCoordX+(spacingX)*2);
        waddstr(titlebar, titleBarItems[1]);
        if(titleBarItemTree[0]==1){wattroff(titlebar, COLOR_PAIR(13));}

        if(titleBarItemTree[0]==2){wattron(titlebar, COLOR_PAIR(13));}
        wmove(titlebar, 1, titlebarCoordX-(strlen(titleBarItems[2])+spacingX));
        waddstr(titlebar, titleBarItems[2]);
        if(titleBarItemTree[0]==2){wattroff(titlebar, COLOR_PAIR(13));}


        wmove(titlebar, 1, titlebarCoordX/2);
        show_datetime(titlebar, 1, (titlebarCoordX/2)-16); // Show DateTime

        wrefresh(titlebar);

        ///////////////////////////

        if(titleBarItemTree[0]==-1){break;}

        ch = wgetch(titlebar);
        if((ch=='w') || (ch=='q') || (ch==KEY_UP) || (ch==KEY_ESCAPE) || (ch=='8')){
            titleBarItemTree[0]=-1;
        }
        else if((ch=='\t') || (ch==KEY_RIGHT) || (ch==' ') || (ch=='d') || (ch=='6')){
                if(titleBarItemTree[0]>=(titleBarItemCount-1)){titleBarItemTree[0]=0;}
                else{titleBarItemTree[0]++;}
        }
        else if((ch==KEY_BACKSPACE) || (ch==KEY_LEFT) || (ch=='a') || (ch=='4')){
                if(titleBarItemTree[0]<=0){titleBarItemTree[0]=titleBarItemCount-1;}
                else{titleBarItemTree[0]--;}
        }
        else if((ch=='\n') || (ch==KEY_DOWN) || (ch=='s') || (ch=='5') || (ch=='2')){
            if(titleBarItemTree[0]==0){
                titleBarItemTree[1]=0;
                int rows = sizeof(powerSubItems)/sizeof(powerSubItems[0]);
                int cols = sizeof(powerSubItems[0])/sizeof(powerSubItems[0][0]);
                char** arr = static_cast<char**>(std::malloc(rows * sizeof(char*)));

                for (int i = 0; i < rows; i++) {
                    arr[i] = static_cast<char*>(std::malloc(cols * sizeof(char)));
                    for (int j = 0; j < cols; j++) {
                        arr[i][j] = powerSubItems[i][j];
                    }
                }

                 subItemListWin(rows, cols, titlebarCoordY, spacingX, arr);


            }
            else if(titleBarItemTree[0]==1){
                titleBarItemTree[1]=0;
                int rows = sizeof(utilitiesSubItems)/sizeof(utilitiesSubItems[0]);
                int cols = sizeof(utilitiesSubItems[0])/sizeof(utilitiesSubItems[0][0]);
                char** arr = static_cast<char**>(std::malloc(rows * sizeof(char*)));

                for (int i = 0; i < rows; i++) {
                    arr[i] = static_cast<char*>(std::malloc(cols * sizeof(char)));
                    for (int j = 0; j < cols; j++) {
                        arr[i][j] = utilitiesSubItems[i][j];
                    }
                }

                 subItemListWin(rows, cols, titlebarCoordY, spacingX, arr);
            }
            else if(titleBarItemTree[0]==2){
                titleBarItemTree[1]=0;
                int rows = sizeof(desktopEnvironmentsSubItems)/sizeof(desktopEnvironmentsSubItems[0]);
                int cols = sizeof(desktopEnvironmentsSubItems[0])/sizeof(desktopEnvironmentsSubItems[0][0]);
                char** arr = static_cast<char**>(std::malloc(rows * sizeof(char*)));

                for (int i = 0; i < rows; i++) {
                    arr[i] = static_cast<char*>(std::malloc(cols * sizeof(char)));
                    for (int j = 0; j < cols; j++) {
                        arr[i][j] = desktopEnvironmentsSubItems[i][j];
                    }
                }

                 subItemListWin(rows, cols, titlebarCoordY, spacingX, arr);
            }
        }

    }while(1);
}


void user_pass_visibility(WINDOW *win, int y, int x){

    /*
     ------------------------------------------------------------------------------------------------------------------

     0 : COLOR_BLACK -> NONE | Common

     ----------------------------------
     1: COLOR_RED -> show chr
     2: COLOR_YELLOW -> mask chr | with *
     3: COLOR_GREEN -> hide chr
     ----------------------------------

     ----------------------------------
     4: COLOR_WHITE -> input chr == output chr && output chr selection is not randomized
     5: COLOR_MAGENTA -> input chr != output chr && output chr selection is randomized
     ----------------------------------

     ----------------------------------
     6: COLOR_BLUE -> assuming the length of output string == length of input string && output string may/may not be masked
     7: COLOR_CYAN -> assuming the length of output string != length of input string && output string may/may not be masked
     ----------------------------------
     -------------------------------------------------------------------------------------------------------------------

     MAX COLOR PIXEL LENGTH -> 3
     MIN COLOR PIXEL LENGTH -> 1

     -------------------------------------------------------------------------------------------------------------------

     <output_string_type> : visible chr/masked chr/hidden chr [COLOR_RED/COLOR_YELLOW/COLOR_GREEN]
     <output_string_randomization> : true/false [COLOR_MAGENTA/COLOR_WHITE]
     <output_string_length_randomization> : true/false [COLOR_CYAN/COLOR_BLUE]

     serial: <output_string_type><output_string_randomization><output_string_length_randomization>

     -------------------------------------------------------------------------------------------------------------------
    */

    wmove(win, y, x);
    int j=1;
    for(int i=0; i<sizeof(userpassVisibilityConf)/sizeof(userpassVisibilityConf[0]); i++){
        wattron(win, COLOR_PAIR(userpassVisibilityConf[i]+j));
        waddch(win, userpassVisibilityConf[i]);
        wattroff(win, COLOR_PAIR(userpassVisibilityConf[i]+j));
        j+=2;

    }
    wrefresh(win);

}


void login_passField(WINDOW *win, int y, int x){
    wmove(win, y, x);
    int userpassChrCount = 0;
    int userpassLengthMax = sizeof(userpass)/sizeof(userpass[0]);
    int ch;
    // char* visible_userpass;
    char visible_userpass[50]={'\0'};
    // int finish=0;
    // cbreak();
    noecho();
    keypad(win, TRUE);

    do{
         wmove(win, y, x);

        if(userpassVisibilityConf[0]!=2 && strlen(userpass)>0){
            // visible_userpass = mask_authInput(0, userpassVisibilityConf, userpass);
            // maskStr2(visible_userpass, strlen(userpass), '*');
            halkadm_security.mask_authInput(0, userpassVisibilityConf, userpass, visible_userpass);
            wprintw(win, visible_userpass);
            userpassChrCount=strlen(userpass);
        }
        wrefresh(win);
        ch = wgetch(win);     /* refresh, accept single keystroke of input */
        if ((ch == '\n') || (ch == '\t')){ // If Enter is pressed
            genProfilePicture(accountPicBoxMaxH-1, accountPicBoxMaxW-4, 1, 2);
            break;
        }
        else if((userpassChrCount==0) && (ch == KEY_BACKSPACE)){}
        else if(((userpassChrCount>=0) && (userpassChrCount<userpassLengthMax)) || ((userpassChrCount == userpassLengthMax) && (ch == KEY_BACKSPACE))){
            gen_randColorMap(loginColourMatrixWin, loginColourMatrixConf[0], loginColourMatrixConf[1], loginColourMatrixConf[2], loginColourMatrixConf[3]);

            if(ch == KEY_BACKSPACE){ // If backspace is pressed
                userpassChrCount-=1;
                userpass[userpassChrCount] = '\0';
                mvwprintw(win, y, x+userpassChrCount, " ");
            }
            else{
                userpass[userpassChrCount] = ch;
                userpassChrCount++;
            }
        }
    }while (1);
}

void login_userField(WINDOW *win, int y, int x){
    wmove(win, y, x);
    int usernameChrCount = 0;
    int usernameLengthMax = sizeof(username)/sizeof(username[0]);
    int ch;
//    char* visible_username;
    char visible_username[50]={'\0'};
    noecho();
    keypad(win, TRUE);

    do{
        wmove(win, y, x);

        if(usernameVisibilityConf[0]!=2 && strlen(username)>0){
            halkadm_security.mask_authInput(0, usernameVisibilityConf, username, visible_username);
            wprintw(win, visible_username);
            usernameChrCount=strlen(username);
        }

        wrefresh(win);
        ch = wgetch(win);     /* refresh, accept single keystroke of input */
        if((ch == '\n') || (ch == '\t')){ // If Enter is pressed
            genProfilePicture(accountPicBoxMaxH-1, accountPicBoxMaxW-4, 1, 2);
            updateRequestedUSRENV();
            draw_titlebar(titleBar_subwin, -1);
            break;
        }
        else if((usernameChrCount==0) && (ch == KEY_BACKSPACE)){}
        else if(((usernameChrCount>=0) && (usernameChrCount<usernameLengthMax)) || ((usernameChrCount == usernameLengthMax) && (ch == KEY_BACKSPACE))){
            gen_randColorMap(loginColourMatrixWin, loginColourMatrixConf[0], loginColourMatrixConf[1], loginColourMatrixConf[2], loginColourMatrixConf[3]);

            if(ch == KEY_BACKSPACE){ // If backspace is pressed
                usernameChrCount-=1;
                username[usernameChrCount] = '\0';
                // wmove(win, y, x+usernameChrCount);
                // waddch(win, ' ');
                mvwprintw(win, y, x+usernameChrCount, " ");
            }
            else{
                username[usernameChrCount] = ch;
                usernameChrCount++;
            }
        }

        /* process the command keystroke */
    }while (1);
}


void drawAuthBox(int maxY, int maxX, int minY, int minX){

    // Draw Window
    // authBox = subwin(win, winMaxY/8, winMaxX/2, winMaxY*0.75,winMaxX/4);
    /*loginBoxMinX = winMaxX/4;
    loginBoxMaxX = winMaxX/2;
    loginBoxMinY = winMaxY*0.75;
    loginBoxMaxY = winMaxY/8;*/

    // authBox = newwin(loginBoxMaxY, loginBoxMaxX, loginBoxMinY, loginBoxMinX);
    box(authBox, 0, 0);

    // Draw Auth Map
    gen_randColorMap(authBox, loginColourMatrixConf[0], loginColourMatrixConf[1], loginColourMatrixConf[2], loginColourMatrixConf[3]);

    // Auth Section //

    // Username Field
    mvwprintw(authBox, (loginBoxMaxY/2)-1, (loginBoxMaxX/4)+5, "USER : ");

    // Userpass Field
    mvwprintw(authBox, (loginBoxMaxY/2), (loginBoxMaxX/4)+5, "PASS : ");

    // Login Button
    mvwprintw(authBox, loginBoxMaxY-1, loginBoxMaxX-10, "LOGIN");


    wrefresh(authBox);
}



void initWindow(){
    // Allocate Memory Space

    // initscr(); // Initialize Curses Screen Base

    getmaxyx(stdscr, winMaxY, winMaxX);


    // Account Pic coordinate and oeiention settings
    accountPicBoxMaxH = winMaxY*0.375;
    accountPicBoxMaxW = winMaxX/4;
    accountPicBoxMaxY = winMaxY/8;
    accountPicBoxMaxX = winMaxX*0.375;

    msgBoxMaxY=winMaxY/4; // Message Box Coordinates Y
    msgBoxMaxX=winMaxX/4; // Message Box Coordinates X
    msgBoxMaxH=winMaxY/2; //  Message Box Length H
    msgBoxMaxW=winMaxX/2; // Message Box Length W

    // Allocate Windows and subWindows //
    // Create Main Window
    mainScreenWin = newwin(winMaxY, winMaxX, 0, 0);
    accountPicBox = subwin(mainScreenWin, accountPicBoxMaxH, accountPicBoxMaxW, accountPicBoxMaxY, accountPicBoxMaxX); // Account Picture Box
    messageBox_msg = newwin(msgBoxMaxH-6, msgBoxMaxW-4, msgBoxMaxY+4, msgBoxMaxX+2);
    authBox = subwin(mainScreenWin, winMaxY/8, winMaxX/2, winMaxY*0.75,winMaxX/4);
    titleBar_subwin = subwin(mainScreenWin, 3, winMaxX,0,0);

    getmaxyx(authBox, loginBoxMaxY, loginBoxMaxX);


    // Setup Colours
    start_color();
    initColor();

    drawCMDStr(mainScreenWin, winMaxY-2, winMaxX-(strlen(package)+2), 0, 0, 13, package);
    drawCMDStr(mainScreenWin, winMaxY/2, winMaxX/2, 1, 1, 13, "cat /etc/os-release | grep -w -E 'NAME=|VERSION=' | cut -d '=' -f 2 | cut -d '\"' -f 2 | tr -s '\n' ' '");
    drawCMDStr(mainScreenWin, (winMaxY/2)+1, winMaxX/2, 1, 1, 13, "uname -n -o");

    refresh();

    box(titleBar_subwin, 0, 0);
    box(mainScreenWin, 0, 0);

    draw_titlebar(titleBar_subwin, -1);

    // List Available Desktop Managers
    list_available_desktop_environments(mainScreenWin, winMaxY/2, winMaxX/2);

    //// UserName Visibility colorMap
    user_pass_visibility(authBox, (loginBoxMaxY/2)-1, loginBoxMaxX-10);

    //// UserPass Visibility colorMap
    user_pass_visibility(authBox, (loginBoxMaxY/2), loginBoxMaxX-10);


    // Set Login Matrix Config
    setLoginMatrixWindow(authBox); // The LoginMatrix to be shown in the provided window
    loginMatrixSetConfig(1, 1, loginBoxMaxY-1, (loginBoxMaxX/4)-2);


    // Draw Auth Box
    drawAuthBox(winMaxY/8, winMaxX/2, winMaxY*0.75,winMaxX/4);

    // refreshing the window
    wrefresh(mainScreenWin);
    wrefresh(titleBar_subwin);
    wrefresh(accountPicBox);

}

int main(int argc, char **argv)
{
    initscr();
    initWindow();

    do{
        genProfilePicture(accountPicBoxMaxH-1, accountPicBoxMaxW-4, 1, 2);
        login_userField(authBox, (loginBoxMaxY/2)-1, (loginBoxMaxX/4)+14);
        login_passField(authBox, (loginBoxMaxY/2), (loginBoxMaxX/4)+14);
        draw_titlebar(titleBar_subwin, 0);
    }while(1);

    getch();
    endwin();
    return 0;
}