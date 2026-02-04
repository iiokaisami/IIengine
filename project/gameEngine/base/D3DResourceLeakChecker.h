#pragma once

namespace IIEngine
{

	/// <summary>
	/// Direct3Dリソースリークチェッカー
	/// Direct3Dのリソースが正しく解放されているかをチェックするクラス
	/// </summary>
	class D3DResourceLeakChecker
	{
	public:

		/// <summary>
		/// デストラクタ
		/// </summary>
		~D3DResourceLeakChecker();

	};

}