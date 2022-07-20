#pragma once
#ifndef __ModelLoad_h__
#define __ModelLoad_h__


#include "Core\Common.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


#include <memory>
#include <string>
#include <vector>

namespace Rattler {


class ModelLoad {

public:
	//  º¯Êý  
	void loadModel(std::string path, std::string outDir);
	void processNode(aiNode *node, const aiScene *scene);
	void processMesh(aiMesh *mesh, const aiScene *scene);
private:
	int MeshNum = 0;
	float ModelMinPos[3] = { 1000000, 1000000, 1000000 }, ModelMaxPos[3] = { -1000000, -1000000, -1000000 };
	std::string intputDir;
	std::string outputDir;

};



}














#endif


