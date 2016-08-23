//
//  KRTextureDebug.cpp
//  kingdomrush-x
//
//  Created by Guille on 7/30/15.
//
//


#include "../../Classes/Support/KRUtils.h"
#include "../../Classes/Support/KRMacros.h"

#include "KRTextureDebug.h"


KRTextureDebug::KRTextureDebug() {
    
}

KRTextureDebug& KRTextureDebug::getInstance()
{
    // The only instance
    // Guaranteed to be lazy initialized
    // Guaranteed that it will be destroyed correctly
    static KRTextureDebug instance;
    return instance;
}

KRTextureDebug::~KRTextureDebug()
{
    
}

bool sortTextureData(const TextureDebugData &lhs, const TextureDebugData &rhs)
{
    return KRTextureDebug::sortTextureDebugData(lhs, rhs);
}

void KRTextureDebug::registerTexture(Texture2D *texture)
{
    GLuint textureId = texture->getName();
    auto it = this->textureInfo.find(textureId);
    if(it != this->textureInfo.end()){
        CCLOG("ERROR: There was already a texture with id %i and name %s",textureId, it->second.path.c_str());
        return;
    }
    
    TextureDebugData tData;
    TextureDebugData * textureData;
    textureData = &tData;
    textureData->textureId = textureId;
    textureData->width = texture->getPixelsWide();
    textureData->height = texture->getPixelsHigh();
    textureData->format = (int)texture->getPixelFormat();
    textureData->bitsPerPixel = texture->getBitsPerPixelForFormat();
    
    this->textureInfo[textureId] = *textureData;
}

void KRTextureDebug::unregisterTexture(Texture2D *texture)
{
    GLuint textureId = texture->getName();
    if(this->textureInfo.find(textureId) == this->textureInfo.end())
    {
        CCLOG("ERROR: There was no texture found with id %i", textureId);
        return;
    }
    
    this->textureInfo.erase(textureId);
}

void KRTextureDebug::mapTexture(Texture2D *texture, string path)
{
    GLuint textureId = texture->getName();
    if(this->textureInfo.find(textureId) == this->textureInfo.end()){
        CCLOG("ERROR: There was no texture found with id %i when mapping path %s",textureId,path.c_str());
        return;
    }
    this->textureInfo[textureId].path = path;
}

void KRTextureDebug::dumpMemoryInfo()
{
    //CCLOG("%s",Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
    
    bool foundDuplicates = false;
    CCLOG("TEXTUREDEBUG: Dumping Texture Memory Info");
    vector<TextureDebugData> sortedDatas;
    for(auto iterator = this->textureInfo.begin(); iterator != this->textureInfo.end(); iterator++){
        TextureDebugData data = iterator->second;
        sortedDatas.push_back(data);
    }
    
    sort(sortedDatas.begin(), sortedDatas.end(), sortTextureData);
    
    vector<vector<TextureDebugData>> buckets;
    TextureDebugData lastCompared;
    
    for (unsigned i=0; i<sortedDatas.size(); i++)
    {
        TextureDebugData data = sortedDatas[i];
        
        if(!(data.equal(lastCompared)))
        {
            vector<TextureDebugData> currentBucket;
            buckets.push_back(currentBucket);
        }
        buckets[buckets.size()-1].push_back(data);
        lastCompared = data;
    }
    
    float total = 0;
    
    vector<string> allDuplicates;
    vector<TextureDebugData> bucket;
    
    for (unsigned i=0; i<buckets.size(); i++)
    {
        bucket = buckets[i];
        
        TextureDebugData fistData = bucket[0];
        
        float size = fistData.size();
        
        vector<GLuint> ids;
        vector<string> paths;
        vector<string> duplicates;

        for (unsigned z=0; z<bucket.size(); z++)
        {
            TextureDebugData data = bucket[z];
            ids.push_back(data.textureId);
            
            string path = data.path;
            if (!path.empty() && (find(paths.begin(), paths.end(), path) != paths.end())) {
                foundDuplicates = true;
                duplicates.push_back(path);
            }
            if (!path.empty()) {
                paths.push_back(path);
            }else
            {
                paths.push_back("NOPATH");
            }
        }
        
        float bucketSize = size * bucket.size();
        total += bucketSize;
        allDuplicates.insert( allDuplicates.end(), duplicates.begin(), duplicates.end());
        
        this->printMemoryInfo(fistData, bucket);
        //this->printExtendedMemoryInfo(fistData, bucket, ids, paths, duplicates);
    }
    
    CCLOG("TEXTUREDEBUG: Total TextureMemory %i MB", static_cast<int>(total));
    
    if(foundDuplicates)
    {
                string allDuplicatesString = KRUtils::join(allDuplicates, "\n");
                CCLOG("TEXTUREDEBUG: ERROR - Found duplicate textures [%s]", allDuplicatesString.c_str());
        
        #ifdef KRDEBUG
                Director::getInstance()->getNotificationNode()->addChild(KRUtils::createBasicTextToast(allDuplicatesString, 5));
        #endif
        
    }
}

void KRTextureDebug::printMemoryInfo(TextureDebugData texData, std::vector<TextureDebugData> bucket)
{
    CCLOG("TEXTUREDEBUG: [%lu] %i x %i format: %s total: %iMB", bucket.size(), texData.width,
                                                                texData.height, KRTextureDebug::getPixelFormatName(texData.format).c_str(),
                                                                static_cast<int>(texData.size() * bucket.size()));
}

void KRTextureDebug::printExtendedMemoryInfo(TextureDebugData texData, std::vector<TextureDebugData> bucket, std::vector<GLuint> ids, std::vector<std::string> paths, std::vector<std::string> duplicates)
{
    
    CCLOG("TEXTUREDEBUG: Texture - cant:%lu - width:%i - height:%i - format:%s - individualSize:%f - bucketSize:%f - ids:[%s] - paths:[%s] - duplicates:[%s]", bucket.size(), texData.width, texData.height, KRTextureDebug::getPixelFormatName(texData.format).c_str(), texData.size(), texData.size() * bucket.size(),KRUtils::join(ids, ",").c_str(), KRUtils::join(paths, ",").c_str(), KRUtils::join(duplicates, ",").c_str());
}

bool KRTextureDebug::sortTextureDebugData(const TextureDebugData &lhs, const TextureDebugData &rhs)
{
    if (rhs.path.empty())
        return false;
    
    if(lhs.width<rhs.width)
        return true;
    
    if(lhs.width>rhs.width)
        return false;
    
    if(lhs.height < rhs.height)
        return true;
    
    if(lhs.height > rhs.height)
        return false;
    
    if(lhs.bitsPerPixel < rhs.bitsPerPixel)
        return true;
    
    if(lhs.bitsPerPixel > rhs.bitsPerPixel)
        return false;
    
    if(lhs.format < rhs.format)
        return true;
    
    if(lhs.format < rhs.format)
        return false;
    
    return false;
}

string KRTextureDebug::getPixelFormatName(int format)
{
    
    switch (format) {
        case 0:
            return "AUTO";
        case 1:
            return "RGBA8888";
        case 2:
            return "RGBA8888";
        case 3:
            return "RGB888";
        case 4:
            return "RGB565";
        case 5:
            return "A8";
        case 6:
            return "I8";
        case 7:
            return "AI88";
        case 8:
            return "RGBA4444";
        case 9:
            return "RGB5A1";
        case 10:
            return "PVRTC4";
        case 11:
            return "PVRTC4A";
        case 12:
            return "PVRTC2";
        case 13:
            return "PVRTC2A";
        case 14:
            return "ETC";
        case 15:
            return "S3TC_DXT1";
        case 16:
            return "S3TC_DXT3";
        case 17:
            return "S3TC_DXT5";
        case 18:
            return "ATC_RGB";
        case 19:
            return "ATC_EXPLICIT_ALPHA";
        case 20:
            return "ATC_INTERPOLATED_ALPHA";
        case 21:
            return "AUTO";
        default:
            return "UNKNOWN";
    }
};