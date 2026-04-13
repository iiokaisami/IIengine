#pragma once

#include <Model.h>

namespace IIEngine
{

	/// <summary>
	/// メッシュ作成クラス
	/// メッシュを頂点とインデックスで作成する
	/// </summary>
	class MeshBuilder
	{
	public:

		/// <summary>
		/// Ring作成(円状)
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildRing(Model* model);

		/// <summary>
		/// Cylinder作成(円柱状上下ふた無)
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildCylinder(Model* model);

		/// <summary>
		/// Cone作成(円錐)
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildCone(Model* model);

		/// <summary>
		/// Spiral作成(ねじれ線)
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildSpiral(Model* model);

		/// <summary>
		/// Torus作成(ドーナツ)
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildTorus(Model* model);

		/// <summary>
		/// Helix作成(ばね状)
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildHelix(Model* model);

		/// <summary>
		/// 小さい球
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildSphere(Model* model);

		/// <summary>
		/// 花びら
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildPetal(Model* model);

		/// <summary>
		/// ペラペラな三角形
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildTriangle(Model* model);

		/// <summary>
		/// 小さい立方体
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildCube(Model* model);

		/// <summary>
		/// 線形状
		/// </summary>
		/// <param name="model">モデルポインタ</param>
		static void BuildLine(Model* model);

		/// <summary>
        /// Quad作成(板ポリ四角形)
        /// </summary>
        /// <param name="model">モデルポインタ</param>
		static void BuildQuad(Model* model);

	};

}