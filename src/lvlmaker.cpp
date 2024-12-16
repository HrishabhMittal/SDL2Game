#include "drawing.cpp"
#include "player.cpp"
#include "rbc.cpp"
#include "onewayplat.cpp"
#include "ptd.cpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#define PICTUREDIR "assets/"
class TextureManager {
public:
    Window& window;
    std::unordered_map<std::string,int> textures;
    TextureManager(Window& window): window(window) {}
    int getTexture(const std::string& textureName) {
        if (!textures.count(textureName)) {
            textures[textureName] =  window.loadImageOntoTexture(PICTUREDIR+textureName+".png");
        }
        return textures[textureName];
    }
};
bool isAlpha(int i) {
    return (i>='a'&&i<='z')||(i>='A'&&i<='Z');
}
bool isAlpha(std::string s) {
    for (char i:s) {
        if (!isAlpha(i) && i!=' ') return false;
    }
    return true;
}
void panic(std::string err) {
    std::cout << err << std::endl;
    exit(1);
}
class LevelMaker {
public:
    TextureManager& tm;
    Window& window;
    Player& p;
    std::vector<std::string> classstack;
    std::vector<RigidBoxCollider*>& rbcs;
    std::vector<PassThroughDetectors*>& ptds;
    std::vector<OneWayPlat*>& owps;
    LevelMaker(std::string filename,Window& window,Player& p,std::vector<RigidBoxCollider*>& rbcs,std::vector<PassThroughDetectors*>& ptds,std::vector<OneWayPlat*>& owps,TextureManager& tm)
    : window(window),p(p),rbcs(rbcs),ptds(ptds),owps(owps),tm(tm) {
        std::ifstream file(filename);
        std::string line;
        int x,y,w,h,a; //some ints
        while (std::getline(file,line)) {
            if (!line.size() || line[0]=='#') continue;
            if (isAlpha(line)) {
                std::stringstream ss;
                ss<<line;
                ss>>line;
                if (line=="end") {
                    if (classstack.size()==0) panic("extra 'end' statements found");
                    classstack.pop_back();
                } else classstack.push_back(line);
            } else {
                if (classstack.size()==0) panic("weird statements found");
                if (classstack.back()=="player") {
                    std::stringstream ss;
                    std::string str;
                    ss<<line;
                    ss>>x>>y>>w>>h;
                    p.x=x;p.y=y;p.w=w;p.h=h;
                    ss>>str;
                    p.mt=tm.getTexture(str);
                    ss>>str;
                    p.mtlow=tm.getTexture(str);
                    ss>>str;
                    p.mthigh=tm.getTexture(str);
                    ss>>str;
                    p.idle=tm.getTexture(str);
                    ss>>str;
                    p.rightrun=tm.getTexture(str);
                    ss>>str;
                    p.leftrun=tm.getTexture(str);
                    ss>>str;
                    p.jump=tm.getTexture(str);
                    ss>>str;
                    p.hpbar=tm.getTexture(str);
                    ss>>str;
                    p.beam=tm.getTexture(str);
                    p.manax=p.x-50;
                    p.manay=p.y-50;
                } else if (classstack.back()=="rbc" || classstack.back()=="platform" || classstack.back()=="ptd" || classstack.back()=="spike" || classstack.back()=="vines") {
                    std::stringstream ss;
                    std::string str;
                    ss<<line;
                    ss>>x>>y>>w>>h>>str;
                    if (classstack.back()=="rbc") rbcs.push_back(new RigidBoxCollider(x,y,w,h,p,tm.getTexture(str)));
                    if (classstack.back()=="vines") ptds.push_back(new Vines(x,y,w,h,tm.getTexture(str),window,p));
                    if (classstack.back()=="ptd") ptds.push_back(new PassThroughDetectors(x,y,w,h,tm.getTexture(str),window,p));
                    if (classstack.back()=="spike") ptds.push_back(new Spike(x,y,w,h,tm.getTexture(str),window,p));
                    if (classstack.back()=="platform") rbcs.push_back(new platform(x,y,w,h,p,tm.getTexture(str)));
                } else if (classstack.back()=="box" || classstack.back()=="pillar" || classstack.back()=="onewayplat" || classstack.back()=="door") {
                    std::stringstream ss;
                    std::string str;
                    ss<<line;
                    ss>>x>>y>>w>>str;
                    //if (classstack.back()=="button") rbcs.push_back(new button(x,y,w,p,tm.getTexture(str)));
                    if (classstack.back()=="door") rbcs.push_back(new door(x,y,w,p,tm.getTexture(str)));
                    if (classstack.back()=="box") rbcs.push_back(new box(x,y,w,p,tm.getTexture(str)));
                    if (classstack.back()=="pillar") rbcs.push_back(new pillar(x,y,w,p,tm.getTexture(str)));
                    if (classstack.back()=="onewayplat") owps.push_back(new OneWayPlat(x,y,w,p,tm.getTexture(str)));
                } else if (classstack.back()=="pickup") {
                    std::stringstream ss;
                    std::string str;
                    ss<<line;
                    ss>>x>>y>>w>>h>>a>>str;
                    ptds.push_back(new Pickups(x,y,w,h,a,tm.getTexture(str),window,p));
                } else if (classstack.back()=="unifiedrbcs") {
                    std::stringstream ss;
                    std::string str,str2,str3,str4;
                    ss<<line;
                    ss>>x>>y>>w>>h;
                    str="";
                    std::vector<std::string> plan;
                    while (true) {
                        std::getline(file,str);
                        if (str!="done") plan.push_back(str);
                        else break;
                    }
                    ss>>str>>str2>>str3>>str4;
                    UnifiedRbcs unp(x,y,w,h,tm.getTexture(str),tm.getTexture(str2),tm.getTexture(str3),tm.getTexture(str4),plan,p);
                    for (int i=0;i<unp.rbcs.size();i++) {
                        rbcs.push_back(unp.rbcs[i]);
                    }
                }
            }
        }
        file.close();
    }
};
