//
//  KRTextureDebug.h
//  kingdomrush-x
//
//  Created by Guille on 7/30/15.
//
//

#ifndef __kingdomrush_x__KRTextureDebug__
#define __kingdomrush_x__KRTextureDebug__

#include <cocos2d.h>

USING_NS_CC;

using namespace std;

struct TextureDebugData {
    GLuint textureId;
    int width;
    int height;
    int format;
    int bitsPerPixel;
    string path;
    
    float size ()
    {
        return (width * height * bitsPerPixel) / (1024.0f * 1024.0f * 8);
    }
    
    bool equal(const TextureDebugData& other)
    {
        if (path.empty())
            return false;
        
        if(width!=other.width)
            return false;
        
        if(height != other.height)
            return false;
        
        if(bitsPerPixel != other.bitsPerPixel)
            return false;
        
        if(format != other.format)
            return false;
        
        return true;
    }
};

class KRTextureDebug : public Ref
{
public:
    // constructor
    static KRTextureDebug& getInstance();
    virtual ~KRTextureDebug();
    
    void registerTexture(Texture2D *texture);
    void unregisterTexture(Texture2D *texture);
    void mapTexture(Texture2D *texture, string path);
    void dumpMemoryInfo();
    static bool sortTextureDebugData(const TextureDebugData &lhs, const TextureDebugData &rhs);
    static string getPixelFormatName(int format);
    
    map<int, TextureDebugData>  textureInfo;
private:
    KRTextureDebug();
    KRTextureDebug(KRTextureDebug const&) = delete;
    void operator=(KRTextureDebug const&) = delete;
    
    void printMemoryInfo(TextureDebugData texData, std::vector<TextureDebugData> bucket);
    void printExtendedMemoryInfo(TextureDebugData texData, std::vector<TextureDebugData> bucket, std::vector<GLuint> ids, std::vector<std::string> paths, std::vector<std::string> duplicates);
};

#endif /* defined(__kingdomrush_x__KRTextureDebug__) */