#ifndef DRAWING
#define DRAWING
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <string>
#include <iostream>
#include <vector>
void setFrameRate(int targetFPS) {
    static Uint32 lastTime = 0;
    Uint32 frameTime = 1000 / targetFPS;
    Uint32 currentTime = SDL_GetTicks();
    Uint32 deltaTime = currentTime - lastTime;
    if (deltaTime < frameTime) {
        SDL_Delay(frameTime - deltaTime);
    }
    lastTime = SDL_GetTicks();
}
class Window {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Color curcolor;
    std::vector<SDL_Texture*> textures;
    int width,height;
public:
    int offsetx=0,offsety=0;
    Window(const std::string& title,int width,int height): width(width),height(height) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return;
        }
        if (TTF_Init() == -1) {
            std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
            return;
        }
        if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
            std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
            return;
        }
        window = SDL_CreateWindow(title.c_str(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width,height,SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            return;
        }
        renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            return;
        }
    }
    ~Window() {
        for (int i=0;i<textures.size();i++) {
            SDL_DestroyTexture(textures[i]);
        }
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
    }
    void drawBg(int i) {
        SDL_Texture* texture = textures[i];
        SDL_Rect img{0,0,0,0};
        SDL_Rect win{0,0,width,height};
        SDL_QueryTexture(texture,NULL,NULL,&img.w,&img.h);
        SDL_RenderCopy(renderer,texture,&img,&win);
    }
    int Width() {
        return width;
    }
    int Height() {
        return height;
    }
    void setOffsets(int x,int y) {
        offsetx=x;
        offsety=y;
    }
    void setColor(Uint8 r,Uint8 g,Uint8 b,Uint8 a) {
        SDL_SetRenderDrawColor(renderer,r,g,b,a);
        curcolor={r,g,b,a};
    }
    void drawPoint(int x,int y) {
        x+=offsetx;y+=offsety;
        SDL_RenderDrawPoint(renderer,x+offsetx,y+offsety);
    }
    void drawLine(int x1,int y1,int x2,int y2) {
        aalineRGBA(renderer,x1+offsetx,y1+offsety,x2+offsetx,y2+offsety,curcolor.r,curcolor.g,curcolor.b,curcolor.a);
    }
    void drawRect(int x,int y,int w,int h) {
        x+=offsetx;y+=offsety;
        SDL_Rect rect = {x,y,w,h};
        SDL_RenderDrawRect(renderer,&rect);
    }
    void fillRect(int x,int y,int w,int h) {
        x+=offsetx;y+=offsety;
        SDL_Rect rect = {x,y,w,h};
        SDL_RenderFillRect(renderer,&rect);
    }
    void fillRoundedRect(int x, int y, int w, int h, int r) {
        x+=offsetx;y+=offsety;
        SDL_Rect rect = {x + r, y, w - 2 * r, h};
        SDL_RenderFillRect(renderer, &rect);
        SDL_Rect rect2 = {x, y + r, r, h - 2 * r}; 
        SDL_RenderFillRect(renderer, &rect2);
        SDL_Rect rect3 = {x + w - r, y + r, r, h - 2 * r}; 
        SDL_RenderFillRect(renderer, &rect3);
        fillCircle(x + r, y + r, r); 
        fillCircle(x + w - r - 1, y + r, r); 
        fillCircle(x + r, y + h - r - 1, r); 
        fillCircle(x + w - r - 1, y + h - r - 1, r); 
    }
    void drawCircle(int centerX,int centerY,int radius) {
        circleRGBA(renderer,offsetx+centerX,offsety+centerY,radius,curcolor.r,curcolor.g,curcolor.b,curcolor.a);
    }
    void fillCircle(int centerX,int centerY,int radius) {
        filledCircleRGBA(renderer,offsetx+centerX,offsety+centerY,radius,curcolor.r,curcolor.g,curcolor.b,curcolor.a);
    }
    void drawArc(int x,int y,int radius,float startAngle,float endAngle) {
        x+=offsetx;y+=offsety;
        float startRad = startAngle * (M_PI / 180.0f);
        float endRad = endAngle * (M_PI / 180.0f);
        arcRGBA(renderer,x,y,radius,startAngle,endAngle,curcolor.r,curcolor.g,curcolor.b,curcolor.a);
    }
    int loadImageOntoTexture(const std::string& path) {
        SDL_Texture* texture = IMG_LoadTexture(renderer,path.c_str());
        if (!texture) {
            std::cerr << "IMG_LoadTexture Error: " << IMG_GetError() << std::endl;
            return -1;
        }
        textures.push_back(texture);
        return textures.size()-1;
    }
    void drawTexture(int i,SDL_Rect img,SDL_Rect win) {
        SDL_Texture* texture = textures[i];
        win.x+=offsetx;
        win.y+=offsety;
        SDL_RenderCopy(renderer,texture,&img,&win);
    }
    void drawTexture(int i,SDL_Rect win) {
        SDL_Texture* texture = textures[i];
        win.x+=offsetx;
        win.y+=offsety;
        SDL_Rect img{0,0,0,0};
        SDL_QueryTexture(texture,NULL,NULL,&img.w,&img.h);
        SDL_RenderCopy(renderer,texture,&img,&win);
    }
    void drawText(const std::string& text,int x,int y,const std::string& fontPath,int fontSize,SDL_Color color) {
        x+=offsetx;y+=offsety;
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(),fontSize);
        if (!font) {
            std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
            return;
        }
        SDL_Surface* surface = TTF_RenderText_Blended(font,text.c_str(),color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
        SDL_FreeSurface(surface);
        SDL_Rect destRect = {x,y,0,0};
        SDL_QueryTexture(texture,NULL,NULL,&destRect.w,&destRect.h);
        SDL_RenderCopy(renderer,texture,NULL,&destRect);
        SDL_DestroyTexture(texture);
        TTF_CloseFont(font);
    }
    void present() {
        SDL_RenderPresent(renderer);
    }
    void clear() {
        SDL_RenderClear(renderer);
    }
};
#endif
