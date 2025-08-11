#include <DX3D/Graphics/ResourceManager.h>
#include <filesystem>
#include <algorithm>

using namespace dx3d;

const std::vector<std::string> ResourceManager::s_texturePaths = {
    "DX3D/Assets/Textures/",
    "GDENG03-Engine/DX3D/Assets/Textures/",
    "../DX3D/Assets/Textures/",
    "../../DX3D/Assets/Textures/",
    "Assets/Textures/",
    "Textures/"
};

void ResourceManager::initialize(const GraphicsResourceDesc& resourceDesc) {
    std::lock_guard<std::mutex> lock(m_textureMutex);
    if (m_initialized) {
        return;
    }

    m_resourceDesc = std::make_unique<GraphicsResourceDesc>(resourceDesc); 
    m_initialized = true;
}

void ResourceManager::shutdown() {
    std::lock_guard<std::mutex> lock(m_textureMutex);
    m_textureCache.clear();
    m_initialized = false;
}



std::shared_ptr<Texture2D> ResourceManager::loadTexture(const std::string& fileName) {
    if (!m_initialized || !m_resourceDesc) return nullptr;
    if (!m_initialized) return nullptr;
    if (fileName.empty()) return nullptr;

    std::lock_guard<std::mutex> lock(m_textureMutex);

    // Check if texture is already cached
    auto it = m_textureCache.find(fileName);
    if (it != m_textureCache.end()) {
        return it->second;
    }

    // Find the texture file
    std::string fullPath = findTexturePath(fileName);
    if (fullPath.empty()) {
        return nullptr;
    }
    try {
        auto texture = std::make_shared<Texture2D>(fullPath, *m_resourceDesc); 
        m_textureCache[fileName] = texture;
        return texture;
    }
    catch (const std::exception& e) {
        return nullptr;
    }
}

std::shared_ptr<Material> ResourceManager::createMaterial(const std::string& name) {
    std::string materialName = name.empty() ? "Material_" + std::to_string(rand()) : name;
    return std::make_shared<Material>(materialName);
}

void ResourceManager::clearTextureCache() {
    std::lock_guard<std::mutex> lock(m_textureMutex);
    m_textureCache.clear();
}

void ResourceManager::removeTexture(const std::string& fileName) {
    std::lock_guard<std::mutex> lock(m_textureMutex);
    auto it = m_textureCache.find(fileName);
    if (it != m_textureCache.end())
    {
        m_textureCache.erase(it);
    }
}

std::vector<std::string> ResourceManager::getLoadedTextureNames() const
{
    std::lock_guard<std::mutex> lock(m_textureMutex);
    std::vector<std::string> names;
    names.reserve(m_textureCache.size());

    for (const auto& pair : m_textureCache) {
        names.push_back(pair.first);
    }

    return names;
}

std::string ResourceManager::findTexturePath(const std::string& fileName) const
{
    for (const auto& basePath : s_texturePaths) {
        std::string fullPath = basePath + fileName;
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }
    }

    // try the filename as-is (in case full path)
    if (std::filesystem::exists(fileName)) {
        return fileName;
    }

    // all extensions
    if (fileName.find('.') == std::string::npos) {
        std::vector<std::string> extensions = { ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".dds" };
        for (const auto& basePath : s_texturePaths) {
            for (const auto& ext : extensions) {
                std::string fullPath = basePath + fileName + ext;
                if (std::filesystem::exists(fullPath)) {
                    return fullPath;
                }
            }
        }
    }
    return "";
}

std::shared_ptr<Texture2D> ResourceManager::getTexture(const std::string& fileName) const
{
    if (!m_initialized)
        return nullptr;

    std::lock_guard<std::mutex> lock(m_textureMutex);

    auto it = m_textureCache.find(fileName);
    return (it != m_textureCache.end()) ? it->second : nullptr;
}

bool ResourceManager::isTextureLoaded(const std::string& fileName) const
{
    if (!m_initialized)
        return false;

    std::lock_guard<std::mutex> lock(m_textureMutex);
    return m_textureCache.find(fileName) != m_textureCache.end();
}