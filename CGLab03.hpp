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
//  Iron Man Loader
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
