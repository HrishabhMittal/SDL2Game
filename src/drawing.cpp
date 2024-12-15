#ifndef DRAWING
#define DRAWING
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
int integralSquareRoot(int num) {
    if (num < 0) return -1; 
    if (num == 0 || num == 1) return num;
    int low = 1, high = num, result = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (mid == num / mid) {
            return mid; 
        } else if (mid < num / mid) {
            result = mid; 
            low = mid + 1; 
        } else {
            high = mid - 1; 
        }
    }
    return result; 
}
double calcAngle(int x1, int y1, int x2, int y2) {
    return fmod(atan2(y2 - y1, x2 - x1) * (180.0 / M_PI) + 360.0, 360.0);
}
bool collide(SDL_Rect r1,SDL_Rect r2) {
    if (r1.x+r1.w>r2.x && r2.x+r2.w>r1.x && r1.y+r1.h>r2.y && r2.y+r2.h>r1.y) return true;
    return false;
}
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
    int frames=0;
    int beamstartframe=-1;
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
    void QueryTexture(int i,int& w,int& h) {
            SDL_QueryTexture(textures[i],NULL,NULL,&w,&h);
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
    SDL_Texture* makeBeam(int i,int beamwidth,int length) {
        if (beamstartframe==-1) beamstartframe=frames;
        int cox=offsetx,coy=offsety;
        offsety=0,offsetx=0;
        int w,h;
        SDL_QueryTexture(textures[i],NULL,NULL,&w,&h);
        int cutoff=w/3;
        int totalframes=h/cutoff;
        int curframe=((frames-beamstartframe)/10);
        if (curframe>=20) {
            curframe=curframe%(totalframes-20)+20;
        }
        SDL_Texture* out=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,length,beamwidth);
        SDL_SetTextureBlendMode(out, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer,out);
        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_RenderClear(renderer);
        drawTexture(i,{0,curframe*cutoff,cutoff,cutoff},{0,0,beamwidth,beamwidth});
        drawTexture(i,{cutoff,curframe*cutoff,cutoff,cutoff},{beamwidth,0,length-2*beamwidth,beamwidth});
        drawTexture(i,{2*cutoff,curframe*cutoff,cutoff,cutoff},{length-beamwidth,0,beamwidth,beamwidth});
        SDL_SetRenderTarget(renderer,NULL);
        offsety=coy;
        offsetx=cox;
        return out;
    }
    void drawBeam(int i,int w,int x1,int y1,int x2,int y2) {
        int d=integralSquareRoot((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
        SDL_Texture* tex=makeBeam(i,w,d);
        SDL_Rect img{0,0,0,0};
        SDL_QueryTexture(tex,NULL,NULL,&img.w,&img.h);
        SDL_Rect win{-img.w/2+(x2+x1)/2,-img.h/2+(y2+y1)/2,img.w,img.h};
        SDL_Point p{img.w/2,img.h/2};
        SDL_RenderCopyEx(renderer,tex,&img,&win,calcAngle(x1,y1,x2,y2),&p,SDL_FLIP_NONE);
    }
    void drawHp(int i,int now,int total) {
        SDL_Texture* texture = textures[i];
        SDL_Rect img{0,0,0,0};
        SDL_Rect win{50,50,256*now/total,32};
        SDL_QueryTexture(texture,NULL,NULL,&img.w,&img.h);
        img.w=img.w*now/total;
        SDL_RenderCopy(renderer,texture,&img,&win);
    }
    void drawScrollingTexture(int i,int scroll) {
        SDL_Texture* texture=textures[i];
        scroll=-offsetx/scroll;
        int w,h;
        SDL_QueryTexture(texture,NULL,NULL,&w,&h);
        scroll=(scroll%w+w)%w;
        SDL_Rect win;
        SDL_Rect img;
        if (w-scroll>h*width/height) {
            win={0,0,width,height};
            img={scroll,0,h*width/height,h};
        } else {
            int a=(w-scroll)*height/h;
            int b=h*width/height;
            win={a,0,width-a,height};
            img={0,0,b-w+scroll,h};
            SDL_RenderCopy(renderer,texture,&img,&win);
            win={0,0,a,height};
            img={scroll,0,w-scroll,h};
        }
        SDL_RenderCopy(renderer,texture,&img,&win);
    }
    void animate(int i,SDL_Rect loc,int fr) {
        int f=(frames/20)%fr;
        SDL_Texture* texture=textures[i];
        SDL_Rect img{0,0,0,0};
        SDL_QueryTexture(texture,NULL,NULL,&img.w,&img.h);
        drawTexture(i,{f*img.w/fr,0,img.w/fr,img.h},loc);
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
        frames++;
    }
    void clear() {
        SDL_RenderClear(renderer);
    }
};
#endif
