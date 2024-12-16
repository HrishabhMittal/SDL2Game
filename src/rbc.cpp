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
bool operator==(const SDL_Rect& one,const SDL_Rect& other) {
    return one.x == other.x && one.y == other.y && one.w==other.w && one.h==other.h;
}
bool operator!=(const SDL_Rect& one,const SDL_Rect& other) {
    return one.x != other.x || one.y != other.y || one.w!=other.w || one.h!=other.h;
}
class UnifiedRbcs {
public:
    std::vector<RigidBoxCollider*> rbcs;
    UnifiedRbcs(int x,int y,int w,int h,int tex,int tex2,int tex3,int tex4,std::vector<std::string> plan,Player& p,char platchar='#',char pillarchar='|',char boxchar='O',char doorchar='%') {
        std::vector<SDL_Rect> rects,r2,r3,r4;
        char prev=0;
        for (int i=0;i<plan.size();i++) {
            prev=0;
            for (int j=0;j<plan[i].size();j++) {
                if (plan[i][j]==platchar) {
                    if (prev==platchar) rects.back().w++;
                    else rects.push_back({j,i,1,1});
                }
                if (plan[i][j]==pillarchar) {
                    if (prev==pillarchar) r2.back().w++;
                    else r2.push_back({j,i,1,1});
                }
                if (plan[i][j]==boxchar) {
                    if (prev==boxchar) r3.back().w++;
                    else r3.push_back({j,i,1,1});
                }
                if (plan[i][j]==doorchar) {
                    if (prev==doorchar) r4.back().w++;
                    else r4.push_back({j,i,1,1});
                }
                prev=plan[i][j];
            }
        }
        SDL_Rect done={-1,-1,-1,-1};
        for (int i=0;i<rects.size();i++) {
            for (int j=i+1;j<rects.size();j++) {
                if (rects[i]==done || rects[j]==done) continue;
                if (rects[i].x==rects[j].x && rects[i].w==rects[j].w && rects[i].y+rects[i].h==rects[j].y) {
                    rects[i].h+=rects[j].h;
                    rects[j]=done;
                }
            }
        }
        for (int i=0;i<r2.size();i++) {
            for (int j=i+1;j<r2.size();j++) {
                if (r2[i]==done || r2[j]==done) continue;
                if (r2[i].x==r2[j].x && r2[i].w==r2[j].w && r2[i].y+r2[i].h==r2[j].y) {
                    r2[i].h+=r2[j].h;
                    r2[j]=done;
                }
            }
        }
        for (int i=0;i<r3.size();i++) {
            for (int j=i+1;j<r3.size();j++) {
                if (r3[i]==done || r3[j]==done) continue;
                if (r3[i].x==r3[j].x && r3[i].w==r3[j].w && r3[i].y+r3[i].h==r3[j].y) {
                    r3[i].h+=r3[j].h;
                    r3[j]=done;
                }
            }
        }
        for (int i=0;i<r4.size();i++) {
            for (int j=i+1;j<r4.size();j++) {
                if (r4[i]==done || r4[j]==done) continue;
                if (r4[i].x==r4[j].x && r4[i].w==r4[j].w && r4[i].y+r4[i].h==r4[j].y) {
                    r4[i].h+=r4[j].h;
                    r4[j]=done;
                }
            }
        }
        int scalex=w/plan[0].size();
        int scaley=h/plan.size();
        for (int i=0;i<rects.size();i++) {
            if (rects[i]!=done) {
                rbcs.push_back(new platform(x+rects[i].x*scalex,y+rects[i].y*scaley,rects[i].w*scalex,rects[i].h*scaley,p,tex));
            }
        }
        for (int i=0;i<r2.size();i++) {
            if (r2[i]!=done) {
                rbcs.push_back(new pillar(x+r2[i].x*scalex,y+r2[i].y*scaley,r2[i].w*scalex,p,tex2));
            }
        }
        for (int i=0;i<r3.size();i++) {
            if (r3[i]!=done) {
                rbcs.push_back(new box(x+r3[i].x*scalex,y+r3[i].y*scaley,r3[i].w*scalex,p,tex3));
            }
        }
        for (int i=0;i<r3.size();i++) {
            if (r4[i]!=done) {
                rbcs.push_back(new door(x+r4[i].x*scalex,y+r4[i].y*scaley,r4[i].w*scalex,p,tex4));
            }
        }
    }
};
#endif
