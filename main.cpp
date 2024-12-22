#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <Windows.h>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void draw_ui(){
    system("cls");
    std::cout <<  '\n';
    std::cout << "         XhhhhhhhhhhhhhhhhhhX  " << '\n';      
    std::cout << "        fhhkhkhkhk//khkhkhkhhf " << '\n';      
    std::cout << "        fhhhhhhh/h  h/hhhhhkhf " << '\n';      
    std::cout << "        fhkhkv <hh  kh< vkhhhf " << '\n';      
    std::cout << "        fhhkh zhhhhhhkhz hhhhf " << '\n';      
    std::cout << "        fhhhk! JhkhkhhJ !hkhkf " << '\n';      
    std::cout << "        fhkhhhL> '//' <Chhhhhf " << '\n';      
    std::cout << "        fhhhkhhhhhhhhhhhkhkhhf " << '\n';      
    std::cout << "         !phhhkhhkhhhkhhhhhp!  " << '\n';  
    std::cout << '\n';
    std::cout << " ----------------------------------";
    std::cout << " |Start warp      |Stop warp      |";
    std::cout << " |service (admin) |service (admin)|";
    std::cout << " ----------------------------------"<<'\n';

}


int connection_status(){
    std::string stats = exec("warp-cli status");
    if (stats == ""){
        draw_ui();
        return 2;
    }
    else if (stats.find("Disconnected") != std::string::npos) {
        return 0;
    }
    else{
        return 1;
    }
}

void disable_cmd_selection(){
    HANDLE hInput;
    hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prev_mode;
    GetConsoleMode(hInput, &prev_mode); 
    SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | 
    (prev_mode & ~ENABLE_QUICK_EDIT_MODE));
}
void lock_scrollbar(){
     // get handle to the console window
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // retrieve screen buffer info
    CONSOLE_SCREEN_BUFFER_INFO scrBufferInfo;
    GetConsoleScreenBufferInfo(hOut, &scrBufferInfo);

    // current window size
    short winWidth = scrBufferInfo.srWindow.Right - scrBufferInfo.srWindow.Left + 1;
    short winHeight = scrBufferInfo.srWindow.Bottom - scrBufferInfo.srWindow.Top + 1;

    // current screen buffer size
    short scrBufferWidth = scrBufferInfo.dwSize.X;
    short scrBufferHeight = scrBufferInfo.dwSize.Y;        
    // to remove the scrollbar, make sure the window height matches the screen buffer height
    COORD newSize;
    newSize.X = scrBufferWidth;
    newSize.Y = winHeight;
    // set the new screen buffer dimensions
    SetConsoleScreenBufferSize(hOut, newSize);
}

int main(){
    HWND hwnd = GetConsoleWindow();
    if( hwnd != NULL ){ MoveWindow(hwnd ,1600,730 ,315,340 ,TRUE); }
    lock_scrollbar();
    disable_cmd_selection();
    int connectivity = connection_status();
    std::string con_status[3] ={"Disconnected","Connected","  Error"};
    std::string service_result;
    draw_ui();
    std::cout << "\r            "<<con_status[connectivity]<<"          ";
    //button cordinates
    int x1 = 60, y1 = 15;
    int x2 = 235, y2 = 150;

    int start_x1 = 4 , start_y1 = 185;
    int start_x2 = 143 , start_y2 = 230;

    int stop_x1 = 150 , stop_y1 = 185;
    int stop_x2 = 275 , stop_y2 = 230;
    POINT p;
    while(true){
        Sleep(1);
     if (GetAsyncKeyState(VK_LBUTTON)&1){
        if (GetCursorPos(&p)){
            if (ScreenToClient(hwnd, &p)){ //translates screen cords to window
                //std::cout << " x " << p.x << " y: " << p.y;
                if (p.x>x1 & p.x<x2 & p.y>y1 & p.y<y2){
                        if (connectivity){
                            exec("warp-cli disconnect"); 
                        }
                        else{
                            exec("warp-cli connect");
                        }
                        std::cout << "\r                 "<<"..."<<"              ";
                        Sleep(1500);
                }

                if (p.x>start_x1 & p.x<start_x2 & p.y>start_y1 & p.y<start_y2){
                    service_result = exec("net start \"Cloudflare WARP\"");
                    if (service_result == ""){
                        draw_ui();
                        std::cout << "\r            "<<con_status[2]<<"          ";
                    }
                }
                if (p.x>stop_x1 & p.x<stop_x2 & p.y>stop_y1 & p.y<stop_y2){ 
                    service_result = exec("net stop \"Cloudflare WARP\"");
                    if (service_result == ""){
                        draw_ui();
                        std::cout << "\r            "<<con_status[2]<<"          ";
                    }
                }
                connectivity = connection_status();//double check
                std::cout << "\r            "<<con_status[connectivity]<<"          ";
            }
        }
     }
    }
}
