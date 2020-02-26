#pragma once

#include <string>
#include <vector>

enum XRESULT;
struct D3DXVECTOR2;
struct MeshInfo;

class GMesh {
public:
	GMesh();
	virtual ~GMesh();

	enum ELoadType {
		LT_DEFAULT,
		LT_SIMPLEOBJ
	};

	/** Load a mesh from file  */
	XRESULT LoadMesh(const std::string & file, float scale = 1.0f);

	/** Fills this mesh with a grid */
	XRESULT CreateGrid(int tesselation, const D3DXVECTOR2 & min, const D3DXVECTOR2 & max, float height);

	/** Draws all buffers this holds */
	void DrawMesh();

	/** Returns the meshes */
	std::vector<MeshInfo *> & GetMeshes() { return Meshes; }
	std::vector<std::string> & GetTextures() { return Textures; }

private:
	/** Loads the cache-file-format */
	XRESULT LoadCached(const std::string & file);

	std::vector<MeshInfo *> Meshes;
	std::vector<std::string> Textures;
};
