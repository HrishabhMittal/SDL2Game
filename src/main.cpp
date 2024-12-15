#include "drawing.cpp"
#include "onewayplat.cpp"
#include "player.cpp"
#include "rbc.cpp"
#include "ptd.cpp"
#include "lvlmaker.cpp"
#include <SDL2/SDL_events.h>
#define WIDTH 1000
#define HEIGHT 600
int main() {
    Window window("Game",WIDTH,HEIGHT);
    std::unordered_map<SDL_Keycode,int> keymap;
    std::vector<RigidBoxCollider*> rbcs;
    std::vector<PassThroughDetectors*> ptds;
    std::vector<OneWayPlat*> owps;
    Player p(window);
    TextureManager texture(window);
    LevelMaker lm("testlevel",window,p,rbcs,ptds,owps,texture);
    bool running=1;
    SDL_Event e;
    int some=0,mousePressed=0,mousex,mousey;
    while (running) {
        //events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) running=0;
            if (e.type == SDL_KEYDOWN) {
                keymap[e.key.keysym.sym]=1;
            } else if (e.type == SDL_KEYUP) {
                keymap[e.key.keysym.sym]=0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                mousePressed=1;
            }
            if (e.type == SDL_MOUSEBUTTONUP) {
                mousePressed=0;
            }
        }

        //updates
        for (int i=0;i<rbcs.size();i++) {
            rbcs[i]->update();
        }
        for (int i=0;i<owps.size();i++) {
            owps[i]->update();
        }
        p.climbing=0;
        for (int i=0;i<ptds.size();i++) {
            ptds[i]->update();
        }
        p.handleInput(keymap,mousePressed);
        p.update();
        p.setOffsets();


        //render
        window.drawBg(texture.getTexture("background"));
        window.drawScrollingTexture(texture.getTexture("mountains"),40);
        for (int i=0;i<ptds.size();i++) {
            ptds[i]->render();
        }
        for (int i=0;i<rbcs.size();i++) {
            rbcs[i]->render();
        }
        for (int i=0;i<owps.size();i++) {
            owps[i]->render();
        }
        p.render();
        window.present();
        if (p.y>2000||p.hp<=0) {
            p.hp=10;
            p.x=400;
            p.y=10;
            p.climbing=0;
            p.onplat=0;
        }
        //fps
        setFrameRate(100);
    }

}
































































