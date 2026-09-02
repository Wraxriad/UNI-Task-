#include "mesh.h"

#include <fstream>
#include <sstream>
#include "Utilities.h"
#include "Logger.h"

Mesh::Mesh(std::string fileName) : mFileName(fileName)
{
    qDebug() << "=== Mesh constructor ===";
    qDebug() << "Looking for file:" << QString::fromStdString(fileName);
    qDebug() << "Full path:" << QString::fromStdString(PATH + "Assets/" + fileName);
    makeObj();
}

void Mesh::makeObj()
{
    std::string fullPath = PATH + "Assets/" + mFileName;
    qDebug() << "Attempting to open:" << QString::fromStdString(fullPath);

    std::ifstream fileIn;
    fileIn.open(fullPath, std::ifstream::in);

    if (!fileIn)
    {
        Logger::getInstance().logText("ERROR: could not find OBJ: " + mFileName, Logger::LogType::ERR);
        qDebug() << "ERROR: File not found!" << QString::fromStdString(fullPath);
        return;
    }

    std::string oneLine;
    std::string oneWord;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempUVs;

    // Store materials from MTL file
    std::map<std::string, MaterialInfo> materials;
    std::string currentMaterial;

    // Store temporary vertices with material info
    std::vector<TempVertexData> tempData;

    // Add a dummy at index 0 (OBJ indices start at 1)
    tempVertices.push_back(glm::vec3(0.0f));
    tempNormals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    tempUVs.push_back(glm::vec2(0.0f));

    unsigned int temp_index = 0;

    while (std::getline(fileIn, oneLine))
    {
        if (oneLine.empty()) continue;

        std::stringstream sStream(oneLine);
        oneWord = "";
        sStream >> oneWord;

        if (oneWord == "#" || oneWord == "") continue;

        if (oneWord == "v")  // Vertex position
        {
            glm::vec3 v;
            sStream >> v.x >> v.y >> v.z;
            tempVertices.push_back(v);
        }
        else if (oneWord == "vt")  // Vertex texture coordinate
        {
            glm::vec2 vt;
            sStream >> vt.x >> vt.y;
            vt.y = 1.0f - vt.y; // Flip V for OpenGL/Vulkan
            tempUVs.push_back(vt);
        }
        else if (oneWord == "vn")  // Vertex normal
        {
            glm::vec3 vn;
            sStream >> vn.x >> vn.y >> vn.z;
            tempNormals.push_back(vn);
        }
        else if (oneWord == "mtllib")
        {
            std::string mtlFile;
            sStream >> mtlFile;
            loadMaterialFile(mtlFile, materials);
        }
        else if (oneWord == "usemtl")
        {
            sStream >> currentMaterial;
        }
        else if (oneWord == "f")  // Face
        {
            std::vector<std::string> faceVertices;
            std::string vertexStr;

            while (sStream >> vertexStr)
            {
                faceVertices.push_back(vertexStr);
            }

            // Triangulate the face
            for (size_t i = 1; i < faceVertices.size() - 1; i++)
            {
                int idx[3] = {0, (int)i + 1, (int)i};

                for (int j = 0; j < 3; j++)
                {
                    std::string vData = faceVertices[idx[j]];
                    std::stringstream vStream(vData);
                    std::string segment;
                    std::vector<std::string> segments;

                    while (std::getline(vStream, segment, '/'))
                    {
                        segments.push_back(segment);
                    }

                    TempVertexData tempDataItem;
                    Vertex& v = tempDataItem.vertex;

                    // Parse vertex index
                    if (!segments[0].empty())
                    {
                        int vIdx = std::stoi(segments[0]);
                        if (vIdx > 0 && vIdx < (int)tempVertices.size())
                            v.position = tempVertices[vIdx];
                    }

                    // Parse texture index
                    if (segments.size() > 1 && !segments[1].empty())
                    {
                        int vtIdx = std::stoi(segments[1]);
                        if (vtIdx > 0 && vtIdx < (int)tempUVs.size())
                            v.textureCoordinate = tempUVs[vtIdx];
                    }

                    // Parse normal index
                    if (segments.size() > 2 && !segments[2].empty())
                    {
                        int vnIdx = std::stoi(segments[2]);
                        if (vnIdx > 0 && vnIdx < (int)tempNormals.size())
                        {
                            v.color = (tempNormals[vnIdx] + glm::vec3(1.0f)) * 0.5f;
                        }
                        else
                        {
                            v.color = glm::vec3(0.8f, 0.8f, 0.8f);
                        }
                    }
                    else
                    {
                        v.color = glm::vec3(0.8f, 0.8f, 0.8f);
                    }

                    tempDataItem.materialName = currentMaterial;
                    tempData.push_back(tempDataItem);
                    mIndices.push_back(temp_index++);
                }
            }
        }
    }

    fileIn.close();

    for (const auto& item : tempData) {
        mVertices.push_back(item.vertex);
    }

    // Log results
    qDebug() << "OBJ Loaded:" << QString::fromStdString(mFileName);
    qDebug() << "  Vertices:" << mVertices.size();
    qDebug() << "  Indices:" << mIndices.size();
    qDebug() << "  Materials found:" << materials.size();

    if (mVertices.empty())
    {
        Logger::getInstance().logText("ERROR: No vertices loaded from " + mFileName, Logger::LogType::ERR);
    }
    else
    {
        std::string logMsg = "Loaded OBJ: " + mFileName +
                             " - Vertices: " + std::to_string(mVertices.size()) +
                             ", Indices: " + std::to_string(mIndices.size());
        Logger::getInstance().logText(logMsg, Logger::LogType::HIGHLIGHT);
    }
}

// Add this function to mesh.cpp
void Mesh::loadMaterialFile(const std::string& filename, std::map<std::string, MaterialInfo>& materials)
{
    std::string fullPath = PATH + "Assets/" + filename;
    std::ifstream fileIn(fullPath);

    if (!fileIn.is_open())
    {
        Logger::getInstance().logText("Could not open MTL file: " + filename, Logger::LogType::WARNING);
        return;
    }

    std::string line;
    std::string currentMaterial;
    MaterialInfo currentMat;

    while (std::getline(fileIn, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "newmtl")
        {
            // Save previous material
            if (!currentMaterial.empty())
            {
                currentMat.name = currentMaterial;
                materials[currentMaterial] = currentMat;
            }

            // Start new material
            ss >> currentMaterial;
            currentMat = MaterialInfo();
            currentMat.name = currentMaterial;
        }
        else if (token == "map_Kd")  // Diffuse texture map
        {
            ss >> currentMat.diffuseTexture;
            // Log the texture name to verify
            Logger::getInstance().logText("Material " + currentMaterial +
                                              " uses texture: " + currentMat.diffuseTexture,
                                          Logger::LogType::HIGHLIGHT);
        }
        else if (token == "Kd")  // Diffuse color
        {
            ss >> currentMat.diffuseColor.r >> currentMat.diffuseColor.g >> currentMat.diffuseColor.b;
        }
        else if (token == "Ka")  // Ambient color
        {
            ss >> currentMat.ambientColor.r >> currentMat.ambientColor.g >> currentMat.ambientColor.b;
        }
        else if (token == "Ks")  // Specular color
        {
            ss >> currentMat.specularColor.r >> currentMat.specularColor.g >> currentMat.specularColor.b;
        }
        else if (token == "Ns")  // Shininess
        {
            ss >> currentMat.shininess;
        }
    }

    // Save last material
    if (!currentMaterial.empty())
    {
        currentMat.name = currentMaterial;
        materials[currentMaterial] = currentMat;
    }

    fileIn.close();

    // Store materials in the class member
    mMaterials = materials;

    Logger::getInstance().logText("Loaded MTL file: " + filename +
                                      " with " + std::to_string(materials.size()) + " materials",
                                  Logger::LogType::HIGHLIGHT);
}

//new
void Mesh::loadRawPoints(std::string fileName) {
    std::string fullPath = PATH + "PointsFromLaz/" + fileName;
    std::ifstream file(fullPath);

    if (!file.is_open()) {
        Logger::getInstance().logText("Could not find the Laz Pints " + fileName, Logger::LogType::ERR);
        return;
    }

    std::string line;
    // this will skip the first two lines
    std::getline(file, line);
    std::getline(file, line);

    bool firstPoint = true;
    glm::dvec3 offset(0.0);

    float scale = 0.001f; // resuse the size if the index's

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        double rawX, rawY, rawZ;
        ss >> rawX >> rawY >> rawZ;

        // Set it in the center
        if (firstPoint) {
            offset = glm::dvec3(rawX, rawY, rawZ);
            firstPoint = false;
        }

        Vertex v;
        v.position.x = static_cast<float>((rawX - offset.x) * scale);
        v.position.y = static_cast<float>((rawY - offset.y) * scale);
        v.position.z = static_cast<float>((rawZ - offset.z) * scale);

        // Gives it a diffrent color
        v.color = glm::vec3(1.0f, 1.0f, 1.0f);
        v.textureCoordinate = glm::vec2(0.0f);

        mVertices.push_back(v);

        mIndices.push_back(static_cast<uint16_t>(mIndices.size()));
    }
    file.close();
    Logger::getInstance().logText("Loaded " + std::to_string(mVertices.size()) + " points.", Logger::LogType::HIGHLIGHT);
}