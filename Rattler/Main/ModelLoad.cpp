#include "Main\ModelLoad.h"
#include "Core\Geometry.h"
#include "Core\Transform.h"
#include "ImageProcess.h"

#include <string>
#include <iostream>
#include <fstream>
#include<windows.h>

namespace Rattler {

void ModelLoad::processMesh(aiMesh *mesh, const aiScene *scene) {

	// 注意，对于.obj模型来说，它的面片对顶点的索引是从1开始的，但Assimp会自动帮我们矫正到从0开始索引。
	// 1 2 3 -> 0 1 2

	long nVertices = mesh->mNumVertices;
	long nTriangles = mesh->mNumFaces;

	// 每个三角面片的顶点索引
	int *vertexIndices = new int[nTriangles * 3];
	// 顶点坐标
	Point3f *P = new Point3f[nVertices];
	// 法向量
	int nNormal = 0;
	Normal3f *N = new Normal3f[nVertices];
	// 纹理坐标
	int nUV = 0;
	Point2f *uv = new Point2f[nVertices];
	// 切线向量
	int nTan = 0;
	Vector3f *Tan = new Vector3f[nVertices];
	// 副切线向量
	int nBiTan = 0;
	Vector3f *BiTan = new Vector3f[nVertices];
	int *faceIndices = nullptr; //第几个面的编号，暂时没有用处
	
	// 填装每个顶点的信息
	float minPos[3] = { 10000000, 10000000, 10000000 }, maxPos[3] = { -10000000, -10000000, -10000000 };
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		//顶点
		P[i].x = mesh->mVertices[i].x;
		P[i].y = mesh->mVertices[i].y;
		P[i].z = mesh->mVertices[i].z;
		if (P[i].x > maxPos[0]) maxPos[0] = P[i].x;
		if (P[i].y > maxPos[1]) maxPos[1] = P[i].y;
		if (P[i].z > maxPos[2]) maxPos[2] = P[i].z;
		if (P[i].x < minPos[0]) minPos[0] = P[i].x;
		if (P[i].y < minPos[1]) minPos[1] = P[i].y;
		if (P[i].z < minPos[2]) minPos[2] = P[i].z;

		//法向量
		if (mesh->HasNormals()){
			nNormal = nVertices;
			N[i].x = mesh->mNormals[i].x;
			N[i].y = mesh->mNormals[i].y;
			N[i].z = mesh->mNormals[i].z;
		}
		//纹理 
		if (mesh->mTextureCoords[0]) {
			nUV = nVertices;
			uv[i].x = mesh->mTextureCoords[0][i].x;
			uv[i].y = mesh->mTextureCoords[0][i].y;
		}
		//切线向量
		if (mesh->HasTangentsAndBitangents()) {
			nTan = nVertices; nBiTan = nVertices;
			Tan[i].x = mesh->mTangents[i].x;
			Tan[i].y = mesh->mTangents[i].y;
			Tan[i].z = mesh->mTangents[i].z;
			BiTan[i].x = mesh->mBitangents[i].x;
			BiTan[i].y = mesh->mBitangents[i].y;
			BiTan[i].z = mesh->mBitangents[i].z;
		}
	}
	// 填装每个面的顶点索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i]; 
		if (face.mNumIndices != 3) {
			delete[] vertexIndices;
			delete[] P;
			delete[] Tan;
			delete[] BiTan;
			delete[] N;
			delete[] uv;
			std::cout << "Error: 存在超过三个顶点的表面！" << std::endl;
			return;
		}
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			vertexIndices[3 * i + j] = face.mIndices[j];
	}
	
	// 获取本Mesh的纹理
	std::vector<std::string> diffTexName, specTexName; //纹理序列
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		int count = material->GetTextureCount(aiTextureType_DIFFUSE);
		if (count == 0) {
			diffTexName.push_back("");
		}
		else {
			for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++) {
				aiString str;
				material->GetTexture(aiTextureType_DIFFUSE, i, &str);
				std::string filename = str.C_Str();
				diffTexName.push_back(filename);
				break;
			}
		}
		count = material->GetTextureCount(aiTextureType_SPECULAR);
		if (count == 0) {
			specTexName.push_back("");
		}
		else {
			for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++) {
				aiString str;
				material->GetTexture(aiTextureType_SPECULAR, i, &str);
				std::string filename = str.C_Str();
				specTexName.push_back(filename);
				break;
			}
		}
	}

	

	// 写入文件
	{
		// (0) 写入编号MeshNum的文件 .ratr
		// (1) 先写 VertexNum(顶点数) FaceNum(三角形数) VertIndexNum(顶点索引数=FaceNum*3) NormNum(法向量数) UVNum(纹理坐标数) TanNum(切线坐标数) BiTanNum(副切线坐标数) 
		// (2) 模型最大坐标 maxPos 和最小坐标 minPos
		// (3) 纹理的名字，根据不透明度来制作透明纹理，将纹理根据Mesh进行编号
		// (4) 字符输出 data，表示开始写具体的数据
		// (5) 输出模型的数据

		std::string aimFile = outputDir + "/MeshInfo_" + std::to_string(MeshNum) + ".ratr";
		std::ofstream file(aimFile);
		// (1) 先写 VertexNum(顶点数) FaceNum(三角形数) VertIndexNum(顶点索引数 = FaceNum * 3) NormNum(法向量数) UVNum(纹理坐标数) TanNum(切线坐标数) BiTanNum(副切线坐标数)
		{
			file << "VertexNum " << nVertices << std::endl;
			file << "FaceNum " << nTriangles << std::endl;
			file << "VertIndexNum " << nTriangles * 3 << std::endl;
			file << "NormNum " << nNormal << std::endl;
			file << "UVNum " << nUV << std::endl;
			file << "TanNum " << nTan << std::endl;
			file << "BiTanNum " << nBiTan << std::endl;
		}

		// (2) 模型最大坐标 maxPos 和最小坐标 minPos
		{
			file << "minPos " << minPos[0] << " " << minPos[1] << " " << minPos[2] << std::endl;
			file << "maxPos " << maxPos[0] << " " << maxPos[1] << " " << maxPos[2] << std::endl;
		}

		// (3) 纹理的名字，根据透明度来制作透明纹理，将纹理根据Mesh进行编号
		{
			// 如果有Diffuse纹理，就用Diffuse制作；如果没有，就不生成透明纹理。
			// 按理来说，每个Mesh的每个纹理种类只能最多对应一张图。
			// 读取时，没有纹理就统一初始化为灰白色。
			if (diffTexName.size() > 0) {
				std::string diffFilePath = intputDir + "/" + diffTexName[0];
				// 替换字符，'\' 无法用于文件目录，需要替换为 '/'
				std::replace(diffFilePath.begin(), diffFilePath.end(), '\\', '/');

				std::string diffOutName = "Mesh_" + std::to_string(MeshNum) + "_diffuse.png";
				std::string alphaOutName = "Mesh_" + std::to_string(MeshNum) + "_alpha.png";
				
				bool isGetDiffuse = imageCopy(diffFilePath, outputDir + "/" + diffOutName);
				std::cout << diffFilePath << " " << std::endl;
				bool isGetAlpha = abstractAlpha(diffFilePath, outputDir + "/" + alphaOutName);
				
				if (isGetDiffuse) file << "Diffuse " << diffOutName << std::endl;
				else file << "Diffuse " << "NoFile_Rattler" << std::endl;

				if (isGetAlpha) file << "Alpha " << alphaOutName << std::endl;
				else file << "Alpha " << "NoFile_Rattler" << std::endl;

				if (specTexName.size() > 0) {
					std::string specFilePath = intputDir + "/" + specTexName[0];
					std::replace(specFilePath.begin(), specFilePath.end(), '\\', '/');

					std::string specOutName = "Mesh_" + std::to_string(MeshNum) + "_specular.png";
					bool isGetSpecular = imageCopy(specFilePath, outputDir + "/" + specOutName);
					if(isGetSpecular) file << "Specular " << specOutName << std::endl;
					else file << "Specular " << "NoFile_Rattler" << std::endl;
				}
				else {
					file << "Specular " << "NoFile_Rattler" << std::endl;
				}
				
			}
			else {
				file << "Diffuse " << "NoFile_Rattler" << std::endl;
				file << "Alpha " << "NoFile_Rattler" << std::endl;
				file << "Specular " << "NoFile_Rattler" << std::endl;
			}
			

		}

		// (4) 字符输出 data，表示开始写具体的数据
		{
			file << "data" << std::endl;
		}

		// (5) 输出模型的数据
		{
			for (int i = 0; i < nVertices; i++) {
				file << "Vetx " << P[i].x << " " << P[i].y << " " << P[i].z << std::endl;
			}
			for (int i = 0; i < nTriangles; i++) {
				file << "FaceVetxIdx " << vertexIndices[3 * i + 0] << " " << vertexIndices[3 * i + 1] << " " << vertexIndices[3 * i + 2] << std::endl;
			}
			for (int i = 0; i < nNormal; i++) {
				file << "Norm " << N[i].x << " " << N[i].y << " " << N[i].z << std::endl;
			}
			for (int i = 0; i < nUV; i++) {
				file << "UV " << uv[i].x << " " << uv[i].y << std::endl;
			}
			for (int i = 0; i < nTan; i++) {
				file << "Tan " << Tan[i].x << " " << Tan[i].y << " " << Tan[i].z << std::endl;
			}
			for (int i = 0; i < nBiTan; i++) {
				file << "BiTan " << BiTan[i].x << " " << BiTan[i].y << " " << BiTan[i].z << std::endl;
			}
		}

		file.close();
	}
	

	delete[] vertexIndices;
	delete[] P;
	delete[] Tan;
	delete[] BiTan;
	delete[] N;
	delete[] uv;

	if (ModelMinPos[0] > minPos[0]) ModelMinPos[0] = minPos[0];
	if (ModelMinPos[1] > minPos[1]) ModelMinPos[1] = minPos[1];
	if (ModelMinPos[2] > minPos[2]) ModelMinPos[2] = minPos[2];

	if (ModelMaxPos[0] < maxPos[0]) ModelMaxPos[0] = maxPos[0];
	if (ModelMaxPos[1] < maxPos[1]) ModelMaxPos[1] = maxPos[1];
	if (ModelMaxPos[2] < maxPos[2]) ModelMaxPos[2] = maxPos[2];

	MeshNum++;
}
void ModelLoad::processNode(aiNode *node, const aiScene *scene){
	// 处理节点所有的网格（如果有的话）
	for (unsigned int i = 0; i < node->mNumMeshes; i++){
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		// 处理节点内的Mesh
		processMesh(mesh, scene);
	}
	// 接下来对它的子节点重复这一过程
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		processNode(node->mChildren[i], scene);
}
void ModelLoad::loadModel(std::string path, std::string outDir){

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        return;

	intputDir = path.substr(0, path.find_last_of('/'));

	outputDir = outDir;
	CreateDirectory(outputDir.c_str(), NULL);

    processNode(scene->mRootNode, scene);

	// 写一个总的文件
	std::string aimFile = outputDir + "/ModelInfo.rattler";
	std::ofstream file(aimFile);

	file << "minPos " << ModelMinPos[0] << " " << ModelMinPos[1] << " " << ModelMinPos[2] << std::endl;
	file << "maxPos " << ModelMaxPos[0] << " " << ModelMaxPos[1] << " " << ModelMaxPos[2] << std::endl;

	for (int i = 0; i < MeshNum; i++) {
		file << "MeshFile " << "/MeshInfo_" + std::to_string(i) + ".ratr" << std::endl;
	}

	file.close();

}






}









