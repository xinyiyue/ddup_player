#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <iostream>
#include "sdl_window.h"
#include "sdl_button.h"
#include "sdl_layer.h"
#include "sdl_progress_bar.h"

using namespace std;

int sdl_renderer_test(void) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("局部更新示例", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Rect rects[4];
    rects[0] = {0, 0, 320, 240};
    rects[1] = {320, 0, 320, 240};
    rects[2] = {0, 240, 320, 240};
    rects[3] = {320, 240, 320, 240};

    SDL_Rect frect = {0,0,640,480};
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
    SDL_RenderFillRect(renderer, &(frect));
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point point;
        point.x = mouseX;
        point.y = mouseY;
        for (int i = 0; i < 4; i++) {
            if (SDL_PointInRect(&point, &(rects[i]))) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer, &(rects[i]));
            } else {
                //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                //SDL_RenderFillRect(renderer, &(rects[i]));
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// 事件监视器回调函数
int EventWatchCallback(void* userdata, SDL_Event* event) {
    // 获取事件类型
    SDL_EventType eventType = (SDL_EventType)event->type;

// 打印事件类型
switch (eventType) {
    case SDL_KEYDOWN:
        printf("Keydown event\n");
        break;
    case SDL_KEYUP:
        printf("Keyup event\n");
        break;
    case SDL_QUIT:
        printf("Quit event\n");
        break;
    default:
        printf("Unknown event\n");
        break;
}

// 返回0表示继续分派事件，返回1表示不分派事件
return 0;


}

int test2() {
    // 初始化SDL
    SDL_Init(SDL_INIT_EVERYTHING);

// 创建窗口
SDL_Window* window = SDL_CreateWindow("SDL AddEventWatch Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

// 添加事件监视器
SDL_AddEventWatch(EventWatchCallback, NULL);

// 事件循环
SDL_Event event;
int count = 0;
int quit = 0;
while (!quit) {
    cout << "poll start: count" << count << endl;
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = 1;
            break;
        }
    }
    count++;
    cout << "poll finish" << endl;
    // 在这里执行其他操作
}

#if 0
int count = 0;
int quit = 0;
while (!quit) {
    cout << "poll start: count" << count << endl;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = 1;
            break;
        }
    }
    count++;
    cout << "poll finish" << endl;
    // 在这里执行其他操作

    SDL_Delay(100);
}
#endif
// 销毁窗口
SDL_DestroyWindow(window);

// 退出SDL
SDL_Quit();

return 0;

}

void handle_bar_event(void *userdata, void* event) {
    action *a = (action *)event;
    //cout << "bar_event handler,state::" << a->state << ", seek_time:" << a->seek_time << endl;
}

int main(int argc, char* argv[]) {
    SdlWindow *win = new SdlWindow((char *)"win_test", 800, 480);
    win->create();
    SdlLayer *layer = new SdlLayer((char *)"button");
    layer->set_zorder(1);
    layer->set_show(true);
    SdlButton *pause_button = new SdlButton((char *)"pause_button", (char *)"pause.png", &win->m_array, &win->m_window, win->m_pRenderer);
    SdlButton *play_button = new SdlButton((char *)"play_button", (char *)"resume.png", &win->m_array, &win->m_window, win->m_pRenderer);
    SdlProgBar *prog_bar = new SdlProgBar((char *)"prog_bar", &win->m_window, win->m_pRenderer);
    prog_bar->set_action_callback(handle_bar_event, NULL);
    prog_bar->set_duration(1000);
    layer->add_widget(static_cast<Widget *>(pause_button));
    layer->add_widget(static_cast<Widget *>(play_button));
    layer->add_widget(static_cast<Widget *>(prog_bar));
    win->add_layer(static_cast<Layer *>(layer));
    win->show();
    delete play_button;
    delete pause_button;
    delete prog_bar;
    delete win;
    return 0;
}
