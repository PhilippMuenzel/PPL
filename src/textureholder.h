#ifndef TEXTUREHOLDER_H
#define TEXTUREHOLDER_H

#include <vector>
#include <string>

class TextureHolder
{
public:
    TextureHolder();
    int loadTexture(const std::string& filename);
private:
    std::vector<int> m_textures;
};

#endif // TEXTUREHOLDER_H
