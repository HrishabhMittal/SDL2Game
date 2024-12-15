#ifndef ONEWAYPLAT 
#define ONEWAYPLAT
#include "drawing.cpp"
#include "player.cpp"
class OneWayPlat {
public:
    int x,y,w,t;
    int flagx=0,flagy=0;
    bool iset=0;
    Window& window;
    Player& p;
    OneWayPlat(int x,int y,int w,Player& p,int t): x(x),w(w),y(y),window(p.window),p(p),t(t) {}
    void update() {
        if (p.down) {
            flagx=0,flagy=0;
            if (iset) {
                p.onplat=0;
                iset=0;
            }
            return;
        }
        int nflagx=0,nflagy=0;
        if (p.x+p.w<=x) nflagx=-1;
        else if (p.x>=x+w) nflagx=1;
        if (p.y+p.h<=y) nflagy=-1;
        if (nflagx==0 && nflagy==0) {
            if (flagy==-1) {
                p.y=y-p.h;
            }
        } else {
            flagx=nflagx;
            flagy=nflagy;
        }
        if (nflagx!=0 && iset) {
            p.onplat=0;
            iset=0;
        }
        if (p.y+p.h==y && p.x+p.w>=x && p.x<=x+w) {
            iset=1;
            p.onplat=1;
        }
    }
    void render() {
        window.drawTexture(t,{x,y,w,w});
    };
};
#endif
