#ifndef PLAYER 
#define PLAYER
#include "drawing.cpp"
#include <SDL2/SDL_keycode.h>
class Player {
public:
    int hp=10,x,y,w,h,xspeed=0,frame=0,manax,manay,mt,beamon,inventory[8]={0,0,0,0,0,0,0,0};
    int idle,rightrun,leftrun,jump,hpbar,beam;
    float yspeed=0.0f;
    bool onplat=0,ponplat=0,climbing=0,down=0;
    Window& window;
    Player(Window& ww): window(ww)  {
    }
    void update() {
        manax+=(x-50-manax)/20;
        manay+=(y-50-manay)/20;
        ponplat=onplat;
        if (!onplat) {
            if (yspeed<20.0f) yspeed+=0.3f;
        } else yspeed=0.0f;
        y+=(int)yspeed;
        x+=xspeed;
    }
    void render() {
        int f=frame/20;
        if (beamon) {
            int mousex,mousey;
            SDL_GetMouseState(&mousex,&mousey);
            window.drawBeam(beam,100,x+window.offsetx+w/2,y+window.offsety+h/2,mousex,mousey);
        }
        window.drawHp(hpbar,hp,10);
        window.animate(mt,{manax,manay,50,50},10);
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
        int dx=window.Width()/2-w/2-x-window.offsetx;
        int dy=window.Height()/2-h/2-y-window.offsety;
        window.offsetx+=dx*0.05f;
        window.offsety+=dy*0.05f;
    }
    void handleInput(std::unordered_map<SDL_Keycode,int>& km,int mousedown) {
        xspeed=0;
        if (onplat) climbing=0;
        if (km[SDLK_RIGHT]==1 || km[SDLK_d]==1) {
            xspeed=climbing?3:5;
        } else if (km[SDLK_LEFT]==1 || km[SDLK_a]==1) {
            xspeed=climbing?-3:-5;
        }
        if (km[SDLK_s]==1 || km[SDLK_DOWN]==1) {
            if (climbing) {
                yspeed=3;
            }
            down=1;
        } else down=0;
        if (km[SDLK_w]==1 || km[SDLK_UP]==1 || km[SDLK_SPACE]==1) {
            if (onplat) {
                yspeed=-12;
                onplat=0;
            } else if (climbing) yspeed=-3;
        }
        beamon=mousedown;
    }
};
#endif
