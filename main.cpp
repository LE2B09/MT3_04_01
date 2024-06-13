#include <Novice.h>
#include <imgui.h>
#include "Math/MathFunction.h"

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;

const char kWindowTitle[] = "提出用課題";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	int prevMouseX = 0;
	int prevMouseY = 0;
	bool isDragging = false;

	// 動いているかどうかのフラグ
	bool isActive = false;

	float angularVelocity = 3.14f;
	float angle = 0.0f;
	//デルタタイム
	float deltaTime = 1.0f / 60.0f;

	float r = 0.8f;

	MathFunction mathFunc;

	Vector3 translate{};
	Vector3 rotate{};

	Vector3 cameraTranslate = { 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate = { 0.26f, 0.0f, 0.0f };

	Vector3 c{};

	//球体の情報
	Sphere p{ { c.x + std::cos(angle) * r, c.y + std::sinf(angle) * r, c.z },{0.08f} };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// マウス入力を取得
		POINT mousePosition;
		GetCursorPos(&mousePosition);

		///
		/// ↓更新処理ここから
		///

		// マウスドラッグによる回転制御
		if (Novice::IsPressMouse(1))
		{
			if (!isDragging)
			{
				isDragging = true;
				prevMouseX = mousePosition.x;
				prevMouseY = mousePosition.y;
			}
			else
			{
				int deltaX = mousePosition.x - prevMouseX;
				int deltaY = mousePosition.y - prevMouseY;
				rotate.y += deltaX * 0.01f; // 水平方向の回転
				rotate.x += deltaY * 0.01f; // 垂直方向の回転
				prevMouseX = mousePosition.x;
				prevMouseY = mousePosition.y;
			}
		}
		else
		{
			isDragging = false;
		}

		// マウスホイールで前後移動
		int wheel = Novice::GetWheel();
		if (wheel != 0)
		{
			cameraTranslate.z += wheel * 0.01f; // ホイールの回転方向に応じて前後移動
		}

		ImGui::Begin("Window");
		if (ImGui::Button("Start"))
		{
			isActive = true; // 動きを開始
		}
		if (ImGui::Button("Reset"))
		{
			isActive = false; // 動きを停止
			angle = 0.0f; // 角度をリセット
		}
		ImGui::End();

		//各種行列の計算
		Matrix4x4 worldMatrix = mathFunc.MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = mathFunc.MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewWorldMatrix = mathFunc.Inverse(worldMatrix);
		Matrix4x4 viewCameraMatrix = mathFunc.Inverse(cameraMatrix);
		// 透視投影行列を作成
		Matrix4x4 projectionMatrix = mathFunc.MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		//ビュー座標変換行列を作成
		Matrix4x4 viewProjectionMatrix = mathFunc.Multiply(viewWorldMatrix, mathFunc.Multiply(viewCameraMatrix, projectionMatrix));
		//ViewportMatrixビューポート変換行列を作成
		Matrix4x4 viewportMatrix = mathFunc.MakeViewportMatrix(0.0f, 0.0f, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		// 円運動の更新
		if (isActive)
		{
			angle += angularVelocity * deltaTime;
		}

		p = { {c.x + std::cos(angle) * r ,c.y + std::sin(angle) * r,c.z }, {0.08f} };


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// Gridを描画
		mathFunc.DrawGrid(viewProjectionMatrix, viewportMatrix);

		// Sphereを描画 (球体を描画)
		mathFunc.DrawSphere(p, viewProjectionMatrix, viewportMatrix, WHITE);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0)
		{
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
