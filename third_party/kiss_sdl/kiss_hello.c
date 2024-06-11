#include "kiss_sdl.h"

void button_event(kiss_button *button, SDL_Event *e, int *draw,int *quit)
{
    if (kiss_button_event(button, e, draw)) 
	    *quit = 1;
    //printf("e.type:%x \n", e->type);
}

int main(int argc, char **argv)
{
    SDL_Renderer *renderer;
    SDL_Event e;
    kiss_array objects;
    kiss_window window;
    kiss_label label = {0};
    kiss_button button = {0};
    kiss_button button2 = {0};
    kiss_progressbar progressbar = {0};
    char message[KISS_MAX_LENGTH];
    int draw, quit;

    quit = 0;
    draw = 1;
    kiss_array_new(&objects);
    renderer = kiss_init("Hello kiss_sdl", &objects, 320, 120);
    if (!renderer)
        return 1;
    kiss_window_new(&window, NULL, 0, 0, 0, kiss_screen_width, kiss_screen_height);
    strcpy(message, "Hello World!");
    printf("window w:%d, h:%d \n", window.rect.w, window.rect.h);
    int label_w = window.rect.w/2 - strlen(message) *kiss_textfont.advance/2;
    int label_h = window.rect.h/2 - (kiss_textfont.fontheight +2 * kiss_normal.h)/2;
    printf("label x:%d, y:%d \n", label_w, label_h);
    kiss_label_new(&label,
                   &window,
                   message,
                   0,
                   0);
    label.textcolor.r = 255;
    printf("button1 x:%d, y:%d \n", window.rect.w / 2 - kiss_normal.w / 2, label.rect.y + kiss_textfont.fontheight + kiss_normal.h);
    kiss_button_new(&button,
                    &window,
                    "OK",
                    window.rect.w / 2 - kiss_normal.w / 2,
                    label.rect.y + kiss_textfont.fontheight + kiss_normal.h);
    printf("button1 x:%d, y:%d \n", window.rect.w / 2 - kiss_normal.w / 2 + 80, label.rect.y + kiss_textfont.fontheight + kiss_normal.h);
    kiss_button_new(&button2,
                    &window,
                    "Cancle",
                    window.rect.w / 2 - kiss_normal.w / 2 + 80,
                    label.rect.y + kiss_textfont.fontheight + kiss_normal.h);
    kiss_progressbar_new(&progressbar, &window, window.rect.x +10, window.rect.y + 80, 300);
    progressbar.fraction = 0.;
    progressbar.run = 1;
    window.visible = 1;
    int min = 0;
    int sec = 0;
    char buf[10];
    while (!quit) {
        SDL_Delay(500);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = 1;
            kiss_window_event(&window, &e, &draw);
            button_event(&button, &e, &draw, &quit);
        }
        kiss_progressbar_event(&progressbar, NULL, &draw);
        sec += 1;
	if (sec == 60) {
	    sec = 0;
	    min++;
	}
        if (!draw)
            continue;
        printf("draw... \n");
        snprintf(buf,10,"%d:%d",min,sec);
        kiss_string_copy(label.text, KISS_MAX_LABEL, buf, NULL);
        SDL_RenderClear(renderer);
        kiss_window_draw(&window, renderer);
        kiss_label_draw(&label, renderer);
        /* kiss_button_draw(&button, renderer);
         kiss_button_draw(&button2, renderer);*/
        kiss_progressbar_draw(&progressbar, renderer);
        SDL_RenderPresent(renderer);
        draw = 0;
    }
    kiss_clean(&objects);
    return 0;
}
