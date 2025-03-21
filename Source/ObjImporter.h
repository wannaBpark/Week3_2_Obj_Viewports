#pragma once
#include "Core/Container/Array.h"
#include "Core/Container/String.h"

#include <fstream>

class FObjImporter
{
public:
    FObjImporter();
    
    FObjImporter(const FString& InFilePath);
    
    ~FObjImporter();

    TArray<float> GetVertex(int32 Idx);
    
    TArray<float> GetNormal(int32 Idx);
    
    TArray<float> GetUV(int32 Idx);
    
    TArray<TArray<TArray<uint32>>> GetFaces() { return Faces; }

    TArray<uint32> GetVertexIndices();

    uint32 GetVertexNum() const { return Vertices.Num(); }

    uint32 GetNormalNum() const { return Normals.Num(); }

    uint32 GetUVNum() const { return UVs.Num(); }

    uint32 GetFaceNum() const { return Faces.Num(); }

protected:
    FString FilePath;
    
    std::ifstream File;

    void SetFilePath(const FString& InFilePath);

    bool CheckFile(const FString& InFilePath) const;
    
    void ReadFile();

    void Clear();

    TArray<TArray<float>> Vertices;
    TArray<TArray<float>> Normals;
    TArray<TArray<float>> UVs;
    TArray<TArray<TArray<uint32>>> Faces;
};
