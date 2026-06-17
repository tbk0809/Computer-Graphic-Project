/*
 TCG6223 Computer Graphics
 Combined Header: Iron Man + Captain America Multi-Texture
 (C++03 compatible - no lambdas, no range-for, no auto, no nullptr)
*/

#ifndef YP_CGLAB03_HPP
#define YP_CGLAB03_HPP

#include "CGLabmain.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <map>

#include "stb_image.h"

using namespace std;

namespace CGLab03 {

// ===========================================================================
//  Iron Man Loader
// ===========================================================================
class IronManLoader {
public:
    IronManLoader() : displayList(0), textureID(0), loaded(false) {}
    ~IronManLoader() {
        if (textureID) glDeleteTextures(1, &textureID);
        if (displayList) glDeleteLists(displayList, 1);
    }

    void load(const string& objFile, const string& textureFile, float scale = 1.0f) {
        cout << "[IronMan] Loading texture: " << textureFile << endl;
        if (!loadTexture(textureFile))
            cerr << "[IronMan] WARNING: texture failed to load.\n";
        cout << "[IronMan] Loading OBJ: " << objFile << endl;
        if (!loadOBJ(objFile, scale)) {
            cerr << "[IronMan] ERROR: OBJ file failed to load.\n";
            return;
        }
        buildDisplayList();
        loaded = true;
    }

    void draw() {
        if (!loaded) return;
        glCallList(displayList);
    }

private:
    struct Vertex { float px, py, pz; float nx, ny, nz; float u, v; };
    vector<float>        positions, texcoords, normals;
    vector<Vertex>       mesh;
    vector<unsigned int> indices;
    GLuint               displayList, textureID;
    bool                 loaded;

    static int resolveIdx(int idx, int total) {
        if (idx > 0) return idx - 1;
        if (idx < 0) return total + idx;
        return -1;
    }

    bool loadTexture(const string& filename) {
        int w, h, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filename.c_str(), &w, &h, &channels, 0);
        if (!data) return false;
        GLenum fmt = (channels == 4) ? GL_RGBA : GL_RGB;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gluBuild2DMipmaps(GL_TEXTURE_2D, fmt, w, h, fmt, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        return true;
    }

    bool loadOBJ(const string& filename, float scale) {
        ifstream fin(filename.c_str());
        if (!fin.is_open()) return false;
        string line;
        while (getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            istringstream ss(line);
            string token; ss >> token;
            if (token == "v") {
                float x, y, z; ss >> x >> y >> z;
                positions.push_back(x * scale);
                positions.push_back(y * scale);
                positions.push_back(z * scale);
            } else if (token == "vt") {
                float u, v; ss >> u >> v;
                texcoords.push_back(u); texcoords.push_back(v);
            } else if (token == "vn") {
                float nx, ny, nz; ss >> nx >> ny >> nz;
                normals.push_back(nx); normals.push_back(ny); normals.push_back(nz);
            } else if (token == "f") {
                vector<Vertex> faceVerts;
                string corner;
                while (ss >> corner) {
                    Vertex vtx; vtx.px=0; vtx.py=0; vtx.pz=0;
                                vtx.nx=0; vtx.ny=0; vtx.nz=1;
                                vtx.u=0;  vtx.v=0;
                    parseCorner(corner, vtx);
                    faceVerts.push_back(vtx);
                }
                for (size_t i = 1; i+1 < faceVerts.size(); ++i) {
                    unsigned int base = (unsigned int)mesh.size();
                    mesh.push_back(faceVerts[0]);
                    mesh.push_back(faceVerts[i]);
                    mesh.push_back(faceVerts[i+1]);
                    indices.push_back(base);
                    indices.push_back(base+1);
                    indices.push_back(base+2);
                }
            }
        }
        return !mesh.empty();
    }

    void parseCorner(const string& s, Vertex& out) {
        string tmp = s;
        for (size_t i = 0; i < tmp.size(); ++i)
            if (tmp[i] == '/') tmp[i] = ' ';
        istringstream ss(tmp);
        int pi=0, ti=0, ni=0;
        ss >> pi; ss >> ti; ss >> ni;

        int pIdx = resolveIdx(pi, (int)positions.size()/3);
        int tIdx = resolveIdx(ti, (int)texcoords.size()/2);
        int nIdx = resolveIdx(ni, (int)normals.size()/3);

        if (pIdx>=0 && pIdx*3+2<(int)positions.size()) {
            out.px=positions[pIdx*3]; out.py=positions[pIdx*3+1]; out.pz=positions[pIdx*3+2];
        }
        if (tIdx>=0 && tIdx*2+1<(int)texcoords.size()) {
            out.u=texcoords[tIdx*2]; out.v=texcoords[tIdx*2+1];
        }
        if (nIdx>=0 && nIdx*3+2<(int)normals.size()) {
            out.nx=normals[nIdx*3]; out.ny=normals[nIdx*3+1]; out.nz=normals[nIdx*3+2];
        }
    }

    void buildDisplayList() {
        displayList = glGenLists(1);
        glNewList(displayList, GL_COMPILE);
        if (textureID != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < indices.size(); ++i) {
            const Vertex& v = mesh[ indices[i] ];
            glNormal3f(v.nx, v.ny, v.nz);
            if (textureID != 0) glTexCoord2f(v.u, v.v);
            glVertex3f(v.px, v.py, v.pz);
        }
        glEnd();
        if (textureID != 0) glDisable(GL_TEXTURE_2D);
        glEndList();
    }
};

// ===========================================================================
//  Captain America Loader  (supports split body / hands for arm animation)
// ===========================================================================
class CaptainAmericaLoader {
public:
    CaptainAmericaLoader() : loaded(false) {}
    ~CaptainAmericaLoader() {
        for (map<string,GLuint>::iterator it = textureIDs.begin(); it != textureIDs.end(); ++it)
            if (it->second) glDeleteTextures(1, &it->second);
        for (size_t i = 0; i < displayLists.size(); ++i)
            if (displayLists[i]) glDeleteLists(displayLists[i], 1);
    }

    void load(const string& objFile, const map<string,string>& matToFile, float scale=1.0f) {
        cout << "[CaptainAmerica] Loading textures...\n";
        for (map<string,string>::const_iterator it = matToFile.begin(); it != matToFile.end(); ++it)
            loadTexture(it->first, it->second);
        cout << "[CaptainAmerica] Loading OBJ: " << objFile << "\n";
        if (!loadOBJ(objFile, scale)) {
            cerr << "[CaptainAmerica] ERROR: OBJ failed to load.\n";
            return;
        }
        buildDisplayLists();
        loaded = true;
    }

    void draw() {
        if (!loaded) return;
        glColor3f(1.0f, 1.0f, 1.0f);
        for (size_t i = 0; i < groups.size(); i++) {
            GLuint tid = groups[i].textureID;
            GLuint dl  = displayLists[i];
            if (tid) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, tid);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            }
            glCallList(dl);
            if (tid) glDisable(GL_TEXTURE_2D);
        }
    }

private:
    struct Vertex { float px,py,pz; float nx,ny,nz; float u,v; };
    struct Group  { string matName; GLuint textureID; vector<Vertex> mesh; };

    vector<Group>      groups;
    vector<GLuint>     displayLists;
    vector<float>      positions, texcoords, normals;
    map<string,GLuint> textureIDs;
    bool               loaded;

    static int resolveIdx(int idx, int total) {
        if (idx > 0) return idx - 1;
        if (idx < 0) return total + idx;
        return -1;
    }

    void loadTexture(const string& matKey, const string& filename) {
        int w, h, ch;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filename.c_str(), &w, &h, &ch, 0);
        if (!data) { textureIDs[matKey] = 0; return; }
        GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
        GLuint tid = 0;
        glGenTextures(1, &tid);
        glBindTexture(GL_TEXTURE_2D, tid);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gluBuild2DMipmaps(GL_TEXTURE_2D, fmt, w, h, fmt, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        textureIDs[matKey] = tid;
    }

    GLuint findTexID(const string& matName) {
        for (map<string,GLuint>::const_iterator it = textureIDs.begin(); it != textureIDs.end(); ++it)
            if (matName.find(it->first) != string::npos) return it->second;
        return 0;
    }

    bool loadOBJ(const string& filename, float scale) {
        ifstream fin(filename.c_str());
        if (!fin.is_open()) return false;
        Group* cur = NULL;
        string line;
        while (getline(fin, line)) {
            if (line.empty() || line[0] == '#') continue;
            if (!line.empty() && line[line.size()-1] == '\r')
                line.erase(line.size()-1, 1);
            istringstream ss(line);
            string tok; ss >> tok;
            if (tok == "v") {
                float x,y,z,r,g,b; ss>>x>>y>>z; ss>>r>>g>>b;
                positions.push_back(x*scale);
                positions.push_back(y*scale);
                positions.push_back(z*scale);
            } else if (tok == "vt") {
                float u,v; ss>>u>>v;
                texcoords.push_back(u); texcoords.push_back(v);
            } else if (tok == "vn") {
                float nx,ny,nz; ss>>nx>>ny>>nz;
                normals.push_back(nx); normals.push_back(ny); normals.push_back(nz);
            } else if (tok == "usemtl") {
                string matName; getline(ss, matName);
                if (!matName.empty() && matName[0] == ' ') matName.erase(0, 1);
                groups.push_back(Group());
                cur = &groups.back();
                cur->matName = matName;
                cur->textureID = findTexID(matName);
            } else if (tok == "f" && cur != NULL) {
                vector<Vertex> fv;
                string corner;
                while (ss >> corner) {
                    Vertex vtx; vtx.px=0; vtx.py=0; vtx.pz=0;
                                vtx.nx=0; vtx.ny=0; vtx.nz=1;
                                vtx.u=0;  vtx.v=0;
                    parseCorner(corner, vtx);
                    fv.push_back(vtx);
                }
                for (size_t i = 1; i+1 < fv.size(); ++i) {
                    cur->mesh.push_back(fv[0]);
                    cur->mesh.push_back(fv[i]);
                    cur->mesh.push_back(fv[i+1]);
                }
            }
        }
        return !groups.empty();
    }

    void parseCorner(const string& s, Vertex& out) {
        string tmp = s;
        for (size_t i = 0; i < tmp.size(); ++i)
            if (tmp[i] == '/') tmp[i] = ' ';
        istringstream ss(tmp);
        int pi=0, ti=0, ni=0;
        ss >> pi; ss >> ti; ss >> ni;

        int pIdx = resolveIdx(pi, (int)positions.size()/3);
        int tIdx = resolveIdx(ti, (int)texcoords.size()/2);
        int nIdx = resolveIdx(ni, (int)normals.size()/3);

        if (pIdx>=0 && pIdx*3+2<(int)positions.size()) {
            out.px=positions[pIdx*3]; out.py=positions[pIdx*3+1]; out.pz=positions[pIdx*3+2];
        }
        if (tIdx>=0 && tIdx*2+1<(int)texcoords.size()) {
            out.u=texcoords[tIdx*2]; out.v=texcoords[tIdx*2+1];
        }
        if (nIdx>=0 && nIdx*3+2<(int)normals.size()) {
            out.nx=normals[nIdx*3]; out.ny=normals[nIdx*3+1]; out.nz=normals[nIdx*3+2];
        }
    }

    void buildDisplayLists() {
        displayLists.resize(groups.size(), 0);
        for (size_t g = 0; g < groups.size(); g++) {
            const Group& grp = groups[g];
            GLuint dl = glGenLists(1);
            displayLists[g] = dl;
            glNewList(dl, GL_COMPILE);
            glBegin(GL_TRIANGLES);
            for (size_t j = 0; j < grp.mesh.size(); ++j) {
                const Vertex& v = grp.mesh[j];
                glNormal3f(v.nx, v.ny, v.nz);
                if (grp.textureID) glTexCoord2f(v.u, v.v);
                glVertex3f(v.px, v.py, v.pz);
            }
            glEnd();
            glEndList();
        }
    }
};

// ===========================================================================
//  Virtual World
// ===========================================================================
class MyVirtualWorld {
public:
    // --- Model loaders ---
    IronManLoader        ironman;

    CaptainAmericaLoader captainamerica;   // full / fallback
    CaptainAmericaLoader captainBody;      // body without arms
    CaptainAmericaLoader captainLeftHand;  // left arm / hand
    CaptainAmericaLoader captainRightHand; // right arm / hand

    // Custom Thrown Weapons
    CaptainAmericaLoader customShield;
    CaptainAmericaLoader customHammer;

    bool splitCaptainLoaded;
    bool customWeaponsLoaded;

    // --- Position coordinates ---
    float imX, imY, imZ;
    float caX, caY, caZ;

    // --- Battle variables ---
    int  imHP,  caHP;
    bool imAttacking, caAttackingShield, caAttackingHammer, caAttackingSuper;

    // 【修改】将计时器类型从 int 改为 float，以支持小数递减延长动画
    float  imAttackTimer, caAttackTimer;

    // --- Hand animation ---
    float captainHandAngle;     // current rotation angle of idle arms
    bool  captainHandAutoMode;

    long int timeold, timenew, elapseTime;

    // -----------------------------------------------------------------------
    //  Public API
    // -----------------------------------------------------------------------
    void draw();

    void tickTime() {
        timenew    = glutGet(GLUT_ELAPSED_TIME);
        elapseTime = timenew - timeold;
        timeold    = timenew;

        if (captainHandAutoMode && (caAttackingShield || caAttackingHammer || caAttackingSuper)) {
            // Allows the idle arm to sway slightly while attacking
            // 【修改】注释掉这行代码，防止释放技能时闲置手臂异常抽搐
            // captainHandAngle = 10.0f * sin(caAttackTimer * 0.08f);
        }
    }

    void toggleCaptainHandAuto() {
        captainHandAutoMode = !captainHandAutoMode;
        cout << "[Captain America] Hand auto-mode: "
             << (captainHandAutoMode ? "ON" : "OFF") << endl;
    }

    void moveCaptainHands(float delta) {
        if (!caAttackingShield && !caAttackingHammer && !caAttackingSuper) {
            captainHandAngle += delta;
            cout << "[Captain America] Hand angle: " << captainHandAngle << endl;
        }
    }

    static string findFile(const string candidates[], int count) {
        for (int i = 0; i < count; i++) {
            ifstream f(candidates[i].c_str());
            if (f.good()) return candidates[i];
        }
        return "";
    }

private:
    void drawShadowText(float x, float y, const string& text, float r, float g, float b) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glRasterPos2f(x + 2, y - 2);
        for (size_t i = 0; i < text.length(); ++i)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

        glColor3f(r, g, b);
        glRasterPos2f(x, y);
        for (size_t i = 0; i < text.length(); ++i)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }

    void drawCircle2D(float cx, float cy, float radius) {
        glBegin(GL_POLYGON);
        for (int i = 0; i < 32; i++) {
            float theta = 2.0f * 3.1415926f * float(i) / 32.0f;
            glVertex2f(cx + radius * cos(theta), cy + radius * sin(theta));
        }
        glEnd();
    }

    void drawHUD() {
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, w, 0, h);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        int displayImHP = (imHP > 0) ? imHP : 0;
        int displayCaHP = (caHP > 0) ? caHP : 0;

        float hpScale = 3.0f;

        // Iron Man – top left
        drawShadowText(80, h - 30, "IRON MAN", 1.0f, 0.8f, 0.0f);

        glColor3f(0.4f, 0.4f, 0.4f); drawCircle2D(45, h - 45, 22.0f);
        glColor3f(0.0f, 0.8f, 1.0f); drawCircle2D(45, h - 45, 14.0f);
        glColor3f(1.0f, 1.0f, 1.0f); drawCircle2D(45, h - 45,  6.0f);

        glColor3f(0.2f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(80,                      h - 60);
            glVertex2f(80 + (100 * hpScale),    h - 60);
            glVertex2f(95 + (100 * hpScale),    h - 40);
            glVertex2f(95,                      h - 40);
        glEnd();

        glBegin(GL_QUADS);
            glColor3f(0.6f, 0.0f, 0.0f); glVertex2f(80,                         h - 60);
                                          glVertex2f(80 + (displayImHP*hpScale), h - 60);
            glColor3f(1.0f, 0.2f, 0.2f); glVertex2f(95 + (displayImHP*hpScale), h - 40);
                                          glVertex2f(95,                         h - 40);
        glEnd();

        glLineWidth(2.0f); glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(80,                   h - 60);
            glVertex2f(80 + (100*hpScale),   h - 60);
            glVertex2f(95 + (100*hpScale),   h - 40);
            glVertex2f(95,                   h - 40);
        glEnd();

        // Captain America – top right
        drawShadowText(w - 240, h - 30, "CAPTAIN AMERICA", 1.0f, 0.8f, 0.0f);

        glColor3f(0.8f, 0.1f, 0.1f); drawCircle2D(w - 45, h - 45, 22.0f);
        glColor3f(0.9f, 0.9f, 0.9f); drawCircle2D(w - 45, h - 45, 16.0f);
        glColor3f(0.8f, 0.1f, 0.1f); drawCircle2D(w - 45, h - 45, 11.0f);
        glColor3f(0.1f, 0.3f, 0.8f); drawCircle2D(w - 45, h - 45,  6.0f);

        float caBaseRight = w - 80;

        glColor3f(0.0f, 0.0f, 0.2f);
        glBegin(GL_QUADS);
            glVertex2f(caBaseRight - (100*hpScale),          h - 60);
            glVertex2f(caBaseRight,                          h - 60);
            glVertex2f(caBaseRight - 15,                     h - 40);
            glVertex2f(caBaseRight - 15 - (100*hpScale),     h - 40);
        glEnd();

        glBegin(GL_QUADS);
            glColor3f(0.0f, 0.3f, 0.6f);
            glVertex2f(caBaseRight - (displayCaHP*hpScale),  h - 60);
            glVertex2f(caBaseRight,                          h - 60);
            glColor3f(0.2f, 0.6f, 1.0f);
            glVertex2f(caBaseRight - 15,                     h - 40);
            glVertex2f(caBaseRight - 15 - (displayCaHP*hpScale), h - 40);
        glEnd();

        glLineWidth(2.0f); glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(caBaseRight - (100*hpScale),      h - 60);
            glVertex2f(caBaseRight,                      h - 60);
            glVertex2f(caBaseRight - 15,                 h - 40);
            glVertex2f(caBaseRight - 15 - (100*hpScale), h - 40);
        glEnd();
        glLineWidth(1.0f);

        if (imHP > 0 && caHP > 0)
            drawShadowText(w / 2 - 15, h - 45, "VS", 1.0f, 1.0f, 1.0f);

        if (imHP <= 0 || caHP <= 0) {
            string winText = (imHP <= 0) ? "CAPTAIN AMERICA WINS!" : "IRON MAN WINS!";
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
            glBegin(GL_QUADS);
                glVertex2f(0,   h/2 - 30);
                glVertex2f(w,   h/2 - 30);
                glVertex2f(w,   h/2 + 30);
                glVertex2f(0,   h/2 + 30);
            glEnd();
            glDisable(GL_BLEND);
            drawShadowText(w/2 - 100, h/2 - 5, winText, 1.0f, 0.8f, 0.0f);
        }

        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

public:
    void init() {
        glEnable(GL_LIGHTING);

        imX = 10.0f; imY = -5.0f; imZ = -15.0f;
        caX = -5.0f; caY = -5.0f; caZ =  15.0f;

        imHP = 100; caHP = 100;
        imAttacking  = false; caAttackingShield = false; caAttackingHammer = false; caAttackingSuper = false;
        imAttackTimer = 0.0f; caAttackTimer = 0.0f; // 【修改】初始化为 0.0f

        captainHandAngle    = 0.0f;
        captainHandAutoMode = true;
        splitCaptainLoaded  = false;
        customWeaponsLoaded = false;

        string imModelCandidates[] = { "data/iron.txt","../data/iron.txt","../../data/iron.txt" };
        string imTexCandidates[] = { "data/Image_0.jpg","../data/Image_0.jpg","../../data/Image_0.jpg" };
        string imModelPath = findFile(imModelCandidates, 3);
        string imTexPath   = findFile(imTexCandidates,   3);
        if (!imModelPath.empty()) {
            ironman.load(imModelPath, imTexPath, 3.0f);
        } else {
            cerr << "[IronMan] Cannot find iron.txt\n";
        }

        struct MatEntry { const char* key; const char* file; };
        MatEntry entries[] = {
            { "Captain_america-Civil_War",  "Captain_america_Civil_War_WP.jpg"  },
            { "Captain america-Civil War",  "Captain_america_Civil_War_WP.jpg"  },
            { "hero-thor01",                "hero_thor01_s06_wp02.jpg"          },
            { "captainAmerica-s07-1",       "hero_captainAmerica_S07_1.jpg"     },
            { "captainAmerica-s07-2",       "hero_captainAmerica_S07_2.jpg"     },
            { "captainAmerica-s07-3",       "hero_captainAmerica_S07_3.jpg"     }
        };
        const char* roots[]  = { "data/", "../data/", "../../data/", "" };
        int numEntries = 6, numRoots = 4;

        map<string,string> matToFile;
        for (int ei = 0; ei < numEntries; ++ei)
            for (int ri = 0; ri < numRoots; ++ri) {
                string path = string(roots[ri]) + entries[ei].file;
                ifstream f(path.c_str());
                if (f.good()) { matToFile[entries[ei].key] = path; break; }
            }

        // Search for specific new names the user uploaded
        string caBodyCandidates[] = { "data/captainamericabody.txt", "../data/captainamericabody.txt", "../../data/captainamericabody.txt", "captainamericabody.txt" };
        string caLHandCandidates[] = { "data/Captainlefthand.txt", "../data/Captainlefthand.txt", "../../data/Captainlefthand.txt", "Captainlefthand.txt" };
        string caRHandCandidates[] = { "data/Captainrighthand.txt", "../data/Captainrighthand.txt", "../../data/Captainrighthand.txt", "Captainrighthand.txt" };

        string caBodyPath  = findFile(caBodyCandidates,  4);
        string caLHandPath = findFile(caLHandCandidates, 4);
        string caRHandPath = findFile(caRHandCandidates, 4);

        if (!caBodyPath.empty() && !caLHandPath.empty() && !caRHandPath.empty()) {
            captainBody.load(caBodyPath,  matToFile, 12.5f);
            captainLeftHand.load(caLHandPath,  matToFile, 12.5f);
            captainRightHand.load(caRHandPath, matToFile, 12.5f);
            splitCaptainLoaded = true;
            cout << "[CaptainAmerica] Split models loaded.\n";
        } else {
            string caModelCandidates[] = { "data/Captain_america.txt", "../data/Captain_america.txt", "../../data/Captain_america.txt", "Captain_america.txt" };
            string caModelPath = findFile(caModelCandidates, 4);
            if (!caModelPath.empty()) {
                captainamerica.load(caModelPath, matToFile, 12.5f);
                cout << "[CaptainAmerica] Combined model loaded.\n";
            } else {
                cerr << "[CaptainAmerica] Cannot find any Captain America model.\n";
            }
        }

        // Custom Weapons loader
        string shieldCandidates[] = { "data/sheild.txt", "../data/sheild.txt", "../../data/sheild.txt", "sheild.txt" };
        string hammerCandidates[] = { "data/hammer.txt", "../data/hammer.txt", "../../data/hammer.txt", "hammer.txt" };
        string shieldPath = findFile(shieldCandidates, 4);
        string hammerPath = findFile(hammerCandidates, 4);

        if (!shieldPath.empty() && !hammerPath.empty()) {
            customShield.load(shieldPath, matToFile, 12.5f);
            customHammer.load(hammerPath, matToFile, 12.5f);
            customWeaponsLoaded = true;
            cout << "[Weapons] Custom shield and hammer loaded.\n";
        }

        timeold = glutGet(GLUT_ELAPSED_TIME);
    }
};

} // end namespace CGLab03

#endif // YP_CGLAB03_HPP
