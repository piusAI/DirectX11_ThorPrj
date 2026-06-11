////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

#include <fstream>
#include <unordered_map>
using namespace std;

#include "TransformComponent.h"


class TextureClass;
////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	
	struct InstanceType
	{
		XMFLOAT3 position;
	};

public:
	ModelClass();
	ModelClass(float, float, float); //생성자 T
	ModelClass(float, float, float, float, float, float); //생성자 TR
	ModelClass(float, float, float, float, float, float, float, float, float); //생성자 TRS

	void ModelInstance();
	void SetInstanceNum(int num) { instanceNum = num; }
	int GetInstanceCount() { return instanceNum; }
	void SetInstancePosition(int, float, float, float);


	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, const WCHAR*, const WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	//void Tick(float, XMFLOAT3, float);
	void Tick(float, XMFLOAT3, float, float);


	void SetTranslate(float, float, float);



	int GetIndexCount();

	bool LoadModel(const WCHAR*);
	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseModel();

	bool ReadFileCounts(const WCHAR*);
	bool LoadDataStructures(const WCHAR*, int, int, int, int);

	//04 Object Texture Load
	ID3D11ShaderResourceView* GetTexture();

	const XMMATRIX& GetTransformMatrix();

	//void MoveQueueOrder(float);

	TransformComponent* GetTransformComponent() { return m_TransformComp; }

private:
	bool InitializeBuffers(ID3D11Device* device);

	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	ID3D11Buffer* m_instanceBuffer;  // ← 추가
	int m_vertexCount, m_indexCount, m_textureCount, m_normalCount, m_faceCount;

	ModelType* m_model;

	//04 ObjectTexture
	class TextureClass* m_Texture;

	// float world_x;
	// float world_y;
	// float world_z;


	TransformComponent* m_TransformComp = 0 ;
	int instanceNum = 0;
	vector<InstanceType> instances;

};

#endif