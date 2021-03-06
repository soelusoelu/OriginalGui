#pragma once

#include "../../Math/Math.h"

#define ALIGN16 _declspec(align(16))

//テクスチャ
struct TextureConstantBuffer {
    Matrix4 wp;
    Vector4 color;
    Vector4 uv;
};

//シンプルメッシュ
struct MeshConstantBuffer {
    Matrix4 world;
    Matrix4 wvp;
};

//メッシュ共通バッファ
struct MeshCommonConstantBuffer {
    ALIGN16 Matrix4 world; //ワールド行列
    ALIGN16 Matrix4 view; //ビュー行列
    ALIGN16 Matrix4 projection; //プロジェクション行列
    ALIGN16 Matrix4 wvp; //ワールドから射影までの変換行列
    ALIGN16 Vector3 lightDir; //ワールド行列
    ALIGN16 Vector3 lightColor; //ライトの色
    ALIGN16 Vector3 cameraPos; //カメラ位置
};

//マテリアル
struct MaterialConstantBuffer {
    ALIGN16 Vector3 ambient;
    ALIGN16 Vector4 diffuse;
    ALIGN16 Vector3 specular;
    ALIGN16 float shininess;
};

//スキンメッシュ
struct SkinMeshConstantBuffer {
    Matrix4 bones[256];
};

//ポイント
struct PointConstantBuffer {
    Matrix4 wvp;
    Vector4 color;
};

//ライン
struct LineConstantBuffer {
    Matrix4 wp;
    Vector4 color;
};

//Deferred
struct GBufferShaderConstantBuffer {
    ALIGN16 Vector3 dirLightDir; //ライト方向
    ALIGN16 Vector3 dirLightColor; //ライト色
    ALIGN16 Vector3 cameraPos; //カメラ位置
    ALIGN16 Vector3 ambientLight; //環境光
};

//ポイントライト
struct PointLightConstantBuffer {
    ALIGN16 Matrix4 wvp;
    ALIGN16 Vector3 worldPos;
    ALIGN16 Vector3 cameraPos;
    ALIGN16 Vector2 windowSize;
    ALIGN16 Vector3 diffuseColor;
    ALIGN16 float innerRadius;
    ALIGN16 float outerRadius;
    ALIGN16 float intensity;
};

//シンプルメッシュ
struct SimpleMeshConstantBuffer {
    Matrix4 wvp;
};

//アウトライン
struct OutLineConstantBuffer {
    Matrix4 wvp;
    Vector4 outlineColor;
};

//影
struct ShadowConstantBuffer {
    Matrix4 lightView; //ライトの視点
    Matrix4 lightProj; //ライトのプロジェクション行列
};

//GUI
struct GuiConstantBuffer {
    Matrix4 wp;
};
