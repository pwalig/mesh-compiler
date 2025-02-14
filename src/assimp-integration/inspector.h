#pragma once
#include <assimp/scene.h>

namespace assimp {
	void printMesh(const aiMesh* m, bool detailed = false);
	void printMeshes(const aiScene* scene, bool detailed = false);

	void printAnimation(const aiAnimation* a, bool detailed = false);
	void printAnimations(const aiScene* scene, bool detailed = false);

	void printSkeleton(const aiSkeleton* s, bool detailed = false);
	void printSkeletons(const aiScene* scene, bool detailed = false);

	void printScene(const aiScene* scene, bool detailed = false);
}