#ifndef WALL
#define WALL
#include "drawing.cpp"
#include "player.cpp"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <string>
#include <vector>
class RigidBoxCollider {
public:
    int x,y,w,h,t;
    int flagx=0,flagy=0;
    bool iset=0;
    Window& window;
    Player& p;
    RigidBoxCollider(int x,int y,int w,int h,Player& p,int t): x(x),w(w),y(y),h(h),window(p.window),p(p),t(t) {}
    void update() {
        int nflagx=0,nflagy=0;
        if (p.x+p.w<=x) nflagx=-1;
        else if (p.x>=x+w) nflagx=1;
        if (p.y+p.h<=y) nflagy=-1;
        else if (p.y>=y+h) nflagy=1;
        if (nflagx==0 && nflagy==0) {
            if (flagx==-1) p.x=x-p.w;
            else if (flagx==1) p.x=x+w;
            if (flagy==-1) {
                p.y=y-p.h;
            }
            else if (flagy==1) {
                p.y=y+h;
                if (p.yspeed<0) p.yspeed=0;
            }
        } else {
            flagx=nflagx;
            flagy=nflagy;
        }
        if (nflagx!=0 && iset) {
            p.onplat=0;
            iset=0;
        }
        if (p.y+p.h==y && p.x+p.w>x && p.x<x+w) {
            iset=1;
            p.onplat=1;
        } else if (iset) {
            iset=0;
            p.onplat=0;
        }
    }
    virtual void render() {};
    virtual ~RigidBoxCollider() = default;
};
class platform: public RigidBoxCollider {
public:
    platform(int x,int y,int w,int h,Player& p,int t): RigidBoxCollider(x,y,w,h,p,t) {}
    void render() override {
        int tw,th;
        window.QueryTexture(t,tw,th);
        tw/=3;
        th/=3;
        int ww=w/tw;
        int hh=h/th;
        int www=w/ww,hhh=h/hh;
        for (int i=0;i<ww;i++) for (int j=0;j<hh;j++) {
            int ii,jj;
            if (i==0) ii=0;
            else if (i==ww-1) ii=2;
            else ii=1;
            if (j==0) jj=0;
            else if (j==hh-1) jj=2;
            else jj=1;
            window.drawTexture(t,{ii*tw,jj*th,tw,th},{x+i*www,y+j*hhh,www,hhh});
        }
    }
};
class box: public RigidBoxCollider {
public:
    box(int x,int y,int size,Player& p,int t): RigidBoxCollider(x,y,size,size,p,t) {}
    void render() override {
        window.drawTexture(t,{x,y,w,h});
    }
};
class pillar: public RigidBoxCollider {
public:
    pillar(int x,int y,int size,Player& p,int t): RigidBoxCollider(x,y,size,4*size,p,t) {}
    void render() override {
        window.drawTexture(t,{x,y,w,h});
    }
};
class door: public RigidBoxCollider {
public:
    int closed=1;
    door(int x,int y,int size,Player& p,int t): RigidBoxCollider(x,y,size,size,p,t) {}
    void render() override {
        if (p.inventory[1]>0 && collide({x,y,w,h},{p.x-5,p.y-5,p.w+10,p.h+10})) {
            p.inventory[1]--;
            closed=0;
        }
        if (closed && h<2*w) h++,p.shake=1;
        else if (!closed && h>w) h--,p.shake=1;
        else {
            p.shake=0;
        }
        int wi,hi;
        window.QueryTexture(t,wi,hi);
        window.drawTexture(t,{0,wi,wi,wi},{x,y+h-w,w,w});
        window.drawTexture(t,{0,0,wi,wi},{x,y,w,w});
    }
};

class UnifiedRbcs {
public:
    std::vector<RigidBoxCollider*> rbcs;
    UnifiedRbcs(int x,int y,int w,int h,int tex,int tex2,int tex3,int tex4,std::vector<std::string> plan,Player& p) {
        std::string chars="#|O%";
        std::vector<std::vector<SDL_Rect>> vrects(4);
        std::vector<int> texs{tex,tex2,tex3,tex4};
        char prev=0;
        int scalex=w/plan[0].size();
        int scaley=h/plan.size();
        SDL_Rect done={-1,-1,-1,-1};
        for (int i=0;i<plan.size();i++) {
            prev=0;
            for (int j=0;j<plan[i].size();j++) {
                for (int k=0;k<vrects.size();k++) {
                     if (plan[i][j]==chars[k]) {
                        if (prev==chars[k]) vrects[k].back().w++;
                        else vrects[k].push_back({j,i,1,1});
                    }
                }
                prev=plan[i][j];
            }
        }
        for (int k=0;k<vrects.size();k++) {
            for (int i=0;i<vrects[k].size();i++) {
                for (int j=i+1;j<vrects[k].size();j++) {
                    if (vrects[k][i]==done || vrects[k][j]==done) continue;
                    if (vrects[k][i].x==vrects[k][j].x && vrects[k][i].w==vrects[k][j].w && vrects[k][i].y+vrects[k][i].h==vrects[k][j].y) {
                        vrects[k][i].h+=vrects[k][j].h;
                        vrects[k][j]=done;
                    }
                }
            }
            for (int i=0;i<vrects[k].size();i++) {
                if (vrects[k][i]!=done) {
                    if (chars[k]=='#') rbcs.push_back(new platform(x+vrects[k][i].x*scalex,y+vrects[k][i].y*scaley,vrects[k][i].w*scalex,vrects[k][i].h*scaley,p,texs[k]));
                    else if (chars[k]=='|') rbcs.push_back(new pillar(x+vrects[k][i].x*scalex,y+vrects[k][i].y*scaley,vrects[k][i].w*scalex,p,texs[k]));
                    else if (chars[k]=='O') rbcs.push_back(new box(x+vrects[k][i].x*scalex,y+vrects[k][i].y*scaley,vrects[k][i].w*scalex,p,texs[k]));
                    else if (chars[k]=='%') rbcs.push_back(new door(x+vrects[k][i].x*scalex,y+vrects[k][i].y*scaley,vrects[k][i].w*scalex,p,texs[k]));

                }
            }
        }
    }
};
#endif
