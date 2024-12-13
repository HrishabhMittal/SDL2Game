#ifndef PLAYER 
#define PLAYER
#include <unordered_map>
#include "drawing.cpp"
#include <SDL2/SDL_keycode.h>
class Player {
public:
    int x,y,w,h;
    SDL_Color color;
    int xspeed=0;
    float yspeed=0.0f;
    bool onplat=0,ponplat=0;
    Window& window;
    int frame=0;
    Player(int x,int y,int w,int h,SDL_Color c,Window& ww): x(x),y(y),w(w),h(h),color(c),window(ww)  {
        window.setOffsets(-x+window.Width()/4,-y+window.Height()*3/4);
    }
    void render(int idle,int rightrun,int leftrun,int jump) {
        int f=frame/20;
        if (onplat && xspeed==0) window.drawTexture(idle,{(f%4)*192,0,192,192},{x-5,y-h,w+10,h*2});
        else if (onplat) window.drawTexture(xspeed>0?rightrun:leftrun,{(f%4)*192,0,192,192},{x-2,y-h,w+10,h*2});
        else if (!onplat) {
            if (ponplat) frame=0,f=0;
            window.drawTexture(jump,{(f%5)*192,0,192,192},{x-5,y-h,w+10,h*2});
        }
        frame++;
        if (frame==1000) frame=0;
    }
    void setOffsets() {
        int dx=window.Width()/4-x-window.offsetx;
        int dy=window.Height()*3/4-y-window.offsety;
        window.offsetx+=dx*0.05f;
        window.offsety+=dy*0.05f;
    }
    void handleInput(std::unordered_map<SDL_Keycode,int>& km) {
        xspeed=0;
        if (km[SDLK_RIGHT]==1 || km[SDLK_d]==1) {
            xspeed=5;
        } else if (km[SDLK_LEFT]==1 || km[SDLK_a]==1) {
            xspeed=-5;
        }
        if (km[SDLK_w]==1 || km[SDLK_UP]==1 || km[SDLK_SPACE]==1) {
            if (onplat) {
                yspeed=-10;
                onplat=0;
            }
        }
    }
    void update() {
        ponplat=onplat;
        if (!onplat) {
            if (yspeed<20.0f) yspeed+=0.3f;
        } else yspeed=0.0f;
        y+=(int)yspeed;
        x+=xspeed;
    }
};
#endif
