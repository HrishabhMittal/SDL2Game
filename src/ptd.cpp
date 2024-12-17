#ifndef PICKUP 
#define PICKUP
#include "drawing.cpp"
#include "player.cpp"
class PassThroughDetectors {
public:
    int x,y,w,h,tex;
    Window& window;
    Player& p;
    PassThroughDetectors(int x,int y,int w,int h,int tex,Player& p): x(x),y(y),w(w),h(h),tex(tex),window(p.window),p(p) {}
    virtual void update() {}
    virtual void render() {}
    virtual ~PassThroughDetectors() = default;
};
class Pickups: public PassThroughDetectors {
public:
    int frame=0;
    int ind;
    bool pickedup=0;
    Pickups(int x,int y,int w,int h,int index,Player& p,int tex): PassThroughDetectors(x,y,w,h,tex,p),ind(index) {}
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
    Spike(int x,int y,int w,int h,Player& p,int tex): PassThroughDetectors(x,y,w,h,tex,p) {}
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
    Vines(int x,int y,int w,int h,Player& p,int tex): PassThroughDetectors(x,y,w,h,tex,p) {}
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
class UnifiedPtds {
public:
    std::vector<PassThroughDetectors*> ptds;
    UnifiedPtds(int x,int y,int w,int h,int tex,int tex2,int tex3,int index,std::vector<std::string> plan,Player& p) {
        std::string chars="~A*";
        std::vector<std::vector<SDL_Rect>> vrects(3);
        std::vector<int> texs{tex,tex2,tex3};
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
                    if (chars[k]=='A') {
                        ptds.push_back(new Spike(x+(vrects[k][i].x*2+vrects[k][i].w)*scalex/2,y+(vrects[k][i].y*2+vrects[k][i].h)*scaley/2,vrects[k][i].w*scalex/2,vrects[k][i].h*scaley/2,p,texs[k]));
                        ptds.push_back(new Spike(x+vrects[k][i].x*scalex,y+(vrects[k][i].y*2+vrects[k][i].h)*scaley/2,vrects[k][i].w*scalex/2,vrects[k][i].h*scaley/2,p,texs[k]));
                    }
                    else if (chars[k]=='~') ptds.push_back(new Vines(x+vrects[k][i].x*scalex,y+vrects[k][i].y*scaley,vrects[k][i].w*scalex,vrects[k][i].h*scaley,p,texs[k]));
                    else if (chars[k]=='*') ptds.push_back(new Pickups(x+vrects[k][i].x*scalex+vrects[k][i].w*scalex*3/8,y+vrects[k][i].y*scaley+vrects[k][i].h*scaley/2,vrects[k][i].w*scalex/4,vrects[k][i].h*scaley/2,index,p,texs[k]));

                }
            }
        }
    }
};
#endif
