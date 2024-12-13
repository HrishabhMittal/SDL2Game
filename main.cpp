#include "drawing.cpp"
#include "player.cpp"
#include "wall.cpp"
#include <unordered_map>
#include <vector>
#define WIDTH 1000
#define HEIGHT 600
int main() {
    Window window("Game",WIDTH,HEIGHT);
    Player p(100,10,75,40,{255,200,255,255},window);
    std::unordered_map<SDL_Keycode,int> keymap;
    std::vector<platform> plats;
    std::vector<box> boxes;
    plats.push_back(platform(900,400,192*5,192*2,p));    
    plats.push_back(platform(10,300,192*4,192*2,p));
    boxes.push_back(box(400,200,100,p));
    bool running=1;
    SDL_Event e;
    int some=0;
    int mountains=window.loadImageOntoTexture("/home/hrishabhmittal/Pictures/pixelart/mountains.png");
    int bg=window.loadImageOntoTexture("/home/hrishabhmittal/Pictures/pixelart/background.png");
    int idleslime=window.loadImageOntoTexture("/home/hrishabhmittal/Pictures/pixelart/slime.png");
    int rrunningslime=window.loadImageOntoTexture("/home/hrishabhmittal/Pictures/pixelart/running_slime.png");
    int lrunningslime=window.loadImageOntoTexture("/home/hrishabhmittal/Pictures/pixelart/left_running_slime.png");
    int jumpingslime=window.loadImageOntoTexture("/home/hrishabhmittal/Pictures/pixelart/jumping_slime.png");
    int grasstiles=window.loadImageOntoTexture("/home/hrishabhmittal/Pictures/pixelart/grass.png");
    int boxtile=window.loadImageOntoTexture("/home/hrishabhmittal/Pictures/pixelart/box.png");
    while (running) {

        //events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) running=0;
            if (e.type == SDL_KEYDOWN) {
                keymap[e.key.keysym.sym]=1;
            } else if (e.type == SDL_KEYUP) {
                keymap[e.key.keysym.sym]=0;
            }
        }

        //updates
        for (int i=0;i<plats.size();i++) {
            plats[i].update();
        }
        for (int i=0;i<boxes.size();i++) {
            boxes[i].update();
        }
        p.handleInput(keymap);
        p.update();
        p.setOffsets();


        //render
        window.drawBg(bg);
        window.drawScrollingTexture(mountains,40);
        p.render(idleslime,rrunningslime,lrunningslime,jumpingslime);
        for (int i=0;i<plats.size();i++) {
            plats[i].render(grasstiles);
        }
        for (int i=0;i<boxes.size();i++) {
            boxes[i].render(boxtile);
        }
        window.present();

        //fps
        setFrameRate(100);
    }

}
