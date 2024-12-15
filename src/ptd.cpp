#ifndef PICKUP 
#define PICKUP
#include "drawing.cpp"
#include "player.cpp"
class PassThroughDetectors {
public:
    int x,y,w,h,tex;
    Window& window;
    Player& p;
    PassThroughDetectors(int x,int y,int w,int h,int tex,Window& ww,Player& p): x(x),y(y),w(w),h(h),tex(tex),window(ww),p(p) {}
    virtual void update() {}
    virtual void render() {}
    virtual ~PassThroughDetectors() = default;
};
class Pickups: public PassThroughDetectors {
public:
    int frame=0;
    int ind;
    bool pickedup=0;
    Pickups(int x,int y,int w,int h,int index,int tex,Window& ww,Player& p): PassThroughDetectors(x,y,w,h,tex,ww,p),ind(index) {}
    void update() override {
        if (!pickedup) {
            if (collide({x,y,w,h},{p.x,p.y,p.w,p.h})) {
                pickedup=true;
                p.inventory[ind]++;
            }
        }
    }
    int oscillator() {
        int x=(frame/20)%20-10;
        return x>0?x:-x;
    }
    void render() override {
        if (!pickedup) {
            window.animate(tex,{x,y-oscillator(),w,h},3);
        }
        frame++;
    }
};
class Spike: public PassThroughDetectors {
public:
    int pc=0;
    int lastspike=0;
    Spike(int x,int y,int w,int h,int tex,Window& ww,Player& p): PassThroughDetectors(x,y,w,h,tex,ww,p) {}
    void update() override {
        int c=collide({x,y,w,h},{p.x,p.y,p.w,p.h});
        if (c) {
            if (!pc || lastspike==100) p.hp--,lastspike=0;
            else lastspike++;
        }
        pc=c;
    }
    void render() override {
        window.drawTexture(tex,{x,y,w,h+20});
    }
};
class Vines: public PassThroughDetectors {
public:
    Vines(int x,int y,int w,int h,int tex,Window& ww,Player& p): PassThroughDetectors(x,y,w,h,tex,ww,p) {}
    void update() override {
        int c=collide({x,y,w,h},{p.x,p.y,p.w,p.h});
        if (c) {
            p.climbing=1;
        }
    }
    void render() override {
        int i;
        for (i=0;(i+1)*w*2<h;i++) {
            window.drawTexture(tex,{x,y+i*w*2,w,2*w});
        }
        window.drawTexture(tex,{0,0,64,(h-i*2*w)*64/w},{x,y+i*2*w,w,h-i*2*w});
    }
};
#endif
