#ifndef WALL
#define WALL
#include "drawing.cpp"
#include "player.cpp"
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
#endif
