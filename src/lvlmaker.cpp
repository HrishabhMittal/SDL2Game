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
//void levelMaker(std::string filename,Window& window,Player& p,std::vector<RigidBoxCollider*>& rbcs,std::vector<PassThroughDetectors*>& ptds,std::vector<OneWayPlat*>& owps) {
//    int mountains=window.loadImageOntoTexture(PICTUREDIR "mountains.png");
//    int bg=window.loadImageOntoTexture(PICTUREDIR "background.png");
//    int idleslime=window.loadImageOntoTexture(PICTUREDIR "slime.png");
//    int rrunningslime=window.loadImageOntoTexture(PICTUREDIR "running_slime.png");
//    int lrunningslime=window.loadImageOntoTexture(PICTUREDIR "left_running_slime.png");
//    int jumpingslime=window.loadImageOntoTexture(PICTUREDIR "jumping_slime.png");
//    int grasstiles=window.loadImageOntoTexture(PICTUREDIR "grass.png");
//    int boxtile=window.loadImageOntoTexture(PICTUREDIR "box.png");
//    int manastore=window.loadImageOntoTexture(PICTUREDIR "mana.png");
//    int manapickup=window.loadImageOntoTexture(PICTUREDIR "manapickup.png");
//    int spike=window.loadImageOntoTexture(PICTUREDIR "spike.png");
//    int hpbar=window.loadImageOntoTexture(PICTUREDIR "hp.png");
//    int beam=window.loadImageOntoTexture(PICTUREDIR "beam.png");
//    int ppillar=window.loadImageOntoTexture(PICTUREDIR "pillar.png");
//    int vines=window.loadImageOntoTexture(PICTUREDIR "vines.png");
//    int stonewall=window.loadImageOntoTexture(PICTUREDIR "stonewall.png");
//    int stonewallfull=window.loadImageOntoTexture(PICTUREDIR "stonewallfull.png");
//    int stonebrick=window.loadImageOntoTexture(PICTUREDIR "stonebrick.png");
//    int brick=window.loadImageOntoTexture(PICTUREDIR "bricks.png");
//    int scaffholding=window.loadImageOntoTexture(PICTUREDIR "scaffholding.png");
//    std::ifstream file(filename);
//    file.close();
//}
bool isAlpha(int i) {
    return (i>='a'&&i<='z')||(i>='A'&&i<='Z');
}
bool isAlpha(std::string s) {
    for (char i:s) {
        if (!isAlpha(i)) return false;
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
                } else if (classstack.back()=="box" || classstack.back()=="pillar" || classstack.back()=="onewayplat") {
                    std::stringstream ss;
                    std::string str;
                    ss<<line;
                    ss>>x>>y>>w>>str;
                    if (classstack.back()=="box") rbcs.push_back(new box(x,y,w,p,tm.getTexture(str)));
                    if (classstack.back()=="pillar") rbcs.push_back(new pillar(x,y,w,p,tm.getTexture(str)));
                    if (classstack.back()=="onewayplat") owps.push_back(new OneWayPlat(x,y,w,p,tm.getTexture(str)));
                } else if (classstack.back()=="pickup") {
                    std::stringstream ss;
                    std::string str;
                    ss<<line;
                    ss>>x>>y>>w>>h>>a>>str;
                    ptds.push_back(new Pickups(x,y,w,h,a,tm.getTexture(str),window,p));
                }
            }
        }
        file.close();
    }
};
