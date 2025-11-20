#include "Player.h"

void Player::Initialize()
{
	// --- 3Dオブジェクト ---
	object_ = std::make_unique<Object3d>();
	object_->Initialize("player.obj");


	position_ = { 0.2f,0.7f,-1.2f };
	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	scale_ = { 1.0f,1.5f,1.0f };
	object_->SetScale(scale_);


	// ライト設定
	object_->SetDirectionalLightEnable(true);

	// 衝突判定
	colliderManager_ = ColliderManager::GetInstance();

	objectName_ = "Player";

	desc =
	{
		//ここに設定
		.owner = this,
		.colliderID = objectName_,
		.shape = Shape::AABB,
		.shapeData = &aabb_,
		.attribute = colliderManager_->GetNewAttribute(objectName_),
		.onCollision = std::bind(&Player::OnCollision, this, std::placeholders::_1),
		.onCollisionTrigger = std::bind(&Player::OnCollisionTrigger, this, std::placeholders::_1),
	};
	collider_.MakeAABBDesc(desc);
	colliderManager_->RegisterCollider(&collider_);

	// 画面が更新されたらビネットを0にする
	PostEffectManager::GetInstance()->GetPassAs<VignettePass>("Vignette")->SetStrength(0.0f);

	// ステータス
	hp_ = 8;
	isDead_ = false;
	isAutoControl_= false;

	// パーティクル

	// 環境マップ
	cubeMapPath_ = "resources/images/studio.dds";
	TextureManager::GetInstance()->LoadTexture(cubeMapPath_);
	cubeSrvIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(cubeMapPath_);
	cubeHandle_ = TextureManager::GetInstance()->GetSrvManager()->GetGPUDescriptorHandle(cubeSrvIndex_);

	// 移動可能フラグ
	isCanMove_ = true;

	// 死亡モーション初期値
	deathMotion_.isActive = false;
	deathMotion_.isComplete = false;
	deathMotion_.count = 0;
	deathMotion_.shakeFrames = 40;     // ぷるぷる時間（フレーム）
	deathMotion_.wobbleAmplitude = 0.10f;
	deathMotion_.wobbleFreq = 10.0f;
	deathMotion_.popScale = 2.0f;

}

void Player::Finalize()
{
	for (auto& bullet : pBullets_)
	{
		bullet->SetIsDead(true);
		bullet->Finalize();
	}

	pBullets_.remove_if([](PlayerBullet* bullet)
		{
			if (bullet->IsDead())
			{
				bullet->Finalize();
				delete bullet;
				return true;
			}
			return false;
		});

	colliderManager_->DeleteCollider(&collider_);
}

void Player::Update()
{
	// 死亡モーションがアクティブなら移動入力等を無視して DeadEffect を更新
	if (deathMotion_.isActive)
	{
		DeadEffect();
		return;
	}

	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	object_->SetScale(scale_);
	object_->Update();

	if (isAutoControl_)
	{
		// オート移動
		AutoMove();
		// オート攻撃
		AutoAttack();

		if (isDead_)
		{
			isDead_ = false;

			hp_ = 8;
		}

	}
	else if(isCanMove_)
 	{
		// 回避処理
		Evade();
		// アクティブフラグに回避フラグを入れる
		isActive_ = isEvading_;

		// 回避中は移動・攻撃を無効化
		if (!isEvading_ && !isDead_)
		{
			Move();
			Attack();
		}
	}
	else
	{
		if (isDead_)
		{
			isDead_ = false;

			hp_ = 8;
		}
	}

	// 死亡したらy軸回転を徐々に0にする
	if (isDead_)
	{
		rotation_.y = Lerp(rotation_.y, 0.0f, 0.05f);
	}

	// 弾の削除
	pBullets_.remove_if([](PlayerBullet* bullet)
		{
		if (bullet->IsDead()) {
			bullet->Finalize();
			delete bullet;
			return true;
		}
		return false;
		});

	// 弾更新
	for (auto& bullet : pBullets_)
	{
		bullet->Update();
	}

	// AABBの更新
	aabb_.min = position_ - object_->GetScale();
	aabb_.max = position_ + object_->GetScale();
	aabb_.max.y += 1.0f;
	collider_.SetPosition(position_);

	// 暗闇処理
	HitVignetteTrap();
}

void Player::Draw()
{
	object_->Draw();

	// 弾描画
	for (auto& bullet : pBullets_)
	{
		bullet->Draw();
	}
}

void Player::Draw2D()
{
}

void Player::ImGuiDraw()
{
#ifdef USE_IMGUI

	ImGui::Begin("Player");

	ImGui::SliderFloat3("pos", &position_.x, -290.0f, 290.0f);
	ImGui::SliderFloat3("rot", &rotation_.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("scale", &scale_.x, 0.0f, 10.0f);

	// HitMomentオン
	if (ImGui::Button("HitMoment"))
	{
		isHitMoment_ = true;
	}

	// HP
	ImGui::Text("HP: %.0f", hp_);

	ImGui::End();

	for (auto& bullet : pBullets_)
	{
		bullet->ImGuiDraw();
	}

#endif // USE_IMGUI
}

void Player::Move()
{
	moveVelocity_ = {};

	// 画面上の見た目通りに移動（カメラ回転なし）
	if (Input::GetInstance()->PushKey(DIK_W))
	{
		moveVelocity_.z += moveSpeed_.z;
	}
	if (Input::GetInstance()->PushKey(DIK_S))
	{
		moveVelocity_.z -= moveSpeed_.z;
	}
	if (Input::GetInstance()->PushKey(DIK_A))
	{
		moveVelocity_.x -= moveSpeed_.x;
	}
	if (Input::GetInstance()->PushKey(DIK_D))
	{
		moveVelocity_.x += moveSpeed_.x;
	}

	// 移動ベクトルがゼロでない場合にプレイヤーの向きを補間で更新
	if (moveVelocity_.x != 0.0f || moveVelocity_.z != 0.0f)
	{
		// 正規化された方向ベクトル
		Vector3 normalizedDir = moveVelocity_;
		
		normalizedDir = normalizedDir.Normalize();

		// Y軸回りの目標回転角度を計算
		float targetRotationY = std::atan2(normalizedDir.x, normalizedDir.z);

		// 現在の回転を取得
		Vector3 currentRotation = rotation_;

		// Y軸の回転のみ、最短経路で補間
		float easedRotationY = LerpAngle(currentRotation.y, targetRotationY, 0.2f);

		// 回転を更新
		rotation_ = { currentRotation.x, easedRotationY, currentRotation.z };
	}

	// 位置更新
	position_ += moveVelocity_;

	// 移動制限
	ClampPosition();

	// パーティクル
	ParticleEmitter::Emit("walk", position_, 1);
}

void Player::Attack()
{
	if (Input::GetInstance()->PushKey(DIK_SPACE))
	{
		// プレイヤーの向きに合わせて弾の速度を変更
		Vector3 bulletVelocity =
		{
			std::cosf(rotation_.x) * std::sinf(rotation_.y),     // x
			std::sinf(-rotation_.x),                             // y
			std::cosf(rotation_.x) * std::cosf(rotation_.y)      // z
		};

		if (countCoolDownFrame_ <= 0)
		{
			// 弾を生成し、初期化
			PlayerBullet* newBullet = new PlayerBullet();

			newBullet->SetPosition(position_);
			newBullet->Initialize();
			newBullet->SetVelocity(bulletVelocity);

			newBullet->RunSetMask();
			collider_.SetMask(colliderManager_->GetNewMask(collider_.GetColliderID(), "PlayerBullet"));

			// 弾を登録する
			pBullets_.push_back(newBullet);

			countCoolDownFrame_ = kShootCoolDownFrame_;
		}
	}
	countCoolDownFrame_--;
}

void Player::Evade()
{
	// 回避入力（左Shiftキー）
	if (!isEvading_ && Input::GetInstance()->PushKey(DIK_LSHIFT))
	{
		// 移動方向がある場合のみ回避
		if (moveVelocity_.x != 0.0f or moveVelocity_.z != 0.0f) 
		{
			isEvading_ = true;
			evadeFrame_ = kEvadeDuration_;
			// 現在の移動方向を回避方向として保存
			evadeDirection_ = moveVelocity_;
			// 正規化
			float len = std::sqrt(evadeDirection_.x * evadeDirection_.x + evadeDirection_.z * evadeDirection_.z);
			if (len > 0.0f) 
			{
				evadeDirection_.x /= len;
				evadeDirection_.z /= len;
			}
			// 回避開始時のx軸角度を保存
			evadeStartRotationX_ = rotation_.x;
			// 目標角度を設定（1回転分加算）
			evadeTargetRotationX_ = evadeStartRotationX_ + kEvadeRotateAngle_;
		}
	}

	if (isEvading_)
	{
		// 回避移動
		position_ += evadeDirection_ * kEvadeSpeed_;

		// 移動制限
		ClampPosition();

		// 回避中のx軸回転（線形補間で速めに回す）
		float t = 1.0f - static_cast<float>(evadeFrame_) / static_cast<float>(kEvadeDuration_);
		rotation_.x = evadeStartRotationX_ + (evadeTargetRotationX_ - evadeStartRotationX_) * t;

		evadeFrame_--;
		if (evadeFrame_ <= 0)
		{
			isEvading_ = false;
			// 回避終了時に元の角度に戻す
			rotation_.x = evadeStartRotationX_;
		}
	}
}

void Player::DeadEffect()
{
	if (!deathMotion_.isActive)
	{
		return;
	}

	uint32_t frame = deathMotion_.count;

	// まずは「ぷるぷる」フェーズ：frame < shakeFrames
	if (frame < deathMotion_.shakeFrames)
	{
		// 正規化 t [0,1]
		float t = static_cast<float>(frame) / static_cast<float>(deathMotion_.shakeFrames);
		t = std::clamp(t, 0.0f, 1.0f);

		// 減衰付きサイン振動
		float angular = static_cast<float>(frame) * deathMotion_.wobbleFreq * (2.0f * 3.14159265f / 60.0f);
		float decay = 1.0f - t; // だんだん振幅を減らす
		float wobble = std::sin(angular) * deathMotion_.wobbleAmplitude * decay;

		// 基準スケールに wobble を加算（均等）
		float baseSx = deathMotion_.startScale.x;
		float s = baseSx + wobble;
		if (s < 0.001f) s = 0.001f; // 負スケール防止
		scale_.x = s;
		scale_.y = s;
		scale_.z = s;

		// ちょっと上下に揺らす（視覚効果）
		position_ = deathMotion_.startPosition;
		position_.y += std::sin(angular) * 0.01f * decay;

		// 軽く回す
		rotation_ = deathMotion_.startRotation;
		rotation_.y += 0.04f * decay;

		// 反映
		if (object_)
		{
			object_->SetPosition(position_);
			object_->SetRotate(rotation_);
			object_->SetScale(scale_);
			object_->Update();
		}

		deathMotion_.count++;
		return;
	}

	// 最後は大きさ０にする
	scale_.x = 0.0f;
	scale_.y = 0.0f;
	scale_.z = 0.0f;

	if (object_)
	{
		object_->SetScale(scale_);
		object_->Update();
	}

	// パーティクルを発生させる
	ParticleEmitter::Emit("rupture", position_, 20);

	// モーション終了扱いにする(isActive=false)
	deathMotion_.isActive = false;
	deathMotion_.isComplete = true;
}

void Player::StartDeathMotion()
{
	if (deathMotion_.isActive)
	{
		return;
	}

	deathMotion_.isActive = true;
	deathMotion_.count = 0;
	// 保存しておく現在のトランスフォーム
	deathMotion_.startPosition = position_;
	deathMotion_.startRotation = rotation_;
	deathMotion_.startScale = scale_;

	// 即時に少し大きく見せたい場合は startScale を調整しておく
	// deathMotion_.startScale = scale_;

	// 反映
	if (object_)
	{
		object_->SetPosition(position_);
		object_->SetRotate(rotation_);
		object_->SetScale(scale_);
		object_->Update();
	}
}

void Player::ClearSceneUpdate()
{
	isCanMove_ = false;

	using clock = std::chrono::steady_clock;
	constexpr float TWO_PI = 3.14159265358979323846f * 2.0f;

	// 調整用パラメータ
	static Vector3 center{ 0.0f, 0.5f, 0.0f }; // 軌道中心（高さ = center.y）
	static float radius = 8.0f;              // 軌道半径
	static float angularSpeed = 1.5f;        // 角速度 (rad/s)
	static float currentAngle = 0.2f;        // 現在角度（初期オフセット含む）
	static bool clockwise = false;           // 回転方向

	// ポヨポヨ（XZとYを別々に制御）
	static float poyoAmp = 0.2f;            // 横(X)振幅
	static float poyoAmpZ = 0.2f;           // 縦(Z)振幅（独立）
	static float poyoAmpY = 0.2f;           // 高さ(Y)振幅（独立）
	static float poyoFreq = 1.2f;            // 周波数 (Hz) （X/Z の基本周波数）
	static float poyoFreqY = 1.2f;           // Y の周波数 (Hz)
	static float poyoPhase = 0.0f;           // 位相（X/Z）
	static float poyoPhaseY = 0.0f;          // 位相（Y）
	static bool maintainArea = false;        // true -> sx * sz ≈ 1 に保つ（XZ面積維持）
	static bool maintainVolume = false;     // true -> sx * sy * sz ≈ 1 に保つ（3D体積維持）

	// 基本スケール
	static Vector3 baseScale{ 1.0f, 1.0f, 1.0f };


	// 時刻
	static clock::time_point lastTime = clock::now();
	static bool initialized = false;
	static float totalTime = 0.0f;

	auto now = clock::now();
	float dt = 1.0f / 60.0f;
	if (!initialized)
	{
		lastTime = now;
		initialized = true;
	} else
	{
		std::chrono::duration<float> elapsed = now - lastTime;
		dt = elapsed.count();
		if (dt > 0.1f) dt = 0.1f; // ウィンドウ復帰などで大きくなりすぎないようにする
		lastTime = now;
	}
	totalTime += dt;

	// 角度更新
	float dir = clockwise ? -1.0f : 1.0f;
	currentAngle += dir * angularSpeed * dt;
	// 角度を安定化
	if (currentAngle > TWO_PI) currentAngle = std::fmod(currentAngle, TWO_PI);
	else if (currentAngle < 0.0f) currentAngle = std::fmod(currentAngle + TWO_PI, TWO_PI);

	// 位置
	clearMotion_.position.x = center.x + radius * std::cos(currentAngle);
	clearMotion_.position.y = center.y;
	clearMotion_.position.z = center.z + radius * std::sin(currentAngle);

	// 進行方向に向ける円運動の速度ベクトル
	float velX = -radius * std::sin(currentAngle) * (dir * angularSpeed);
	float velZ = radius * std::cos(currentAngle) * (dir * angularSpeed);
	// 小さな速度では直前の向きを保つためのガード
	static float lastYaw = 0.0f;
	if (std::abs(velX) < 1e-6f && std::abs(velZ) < 1e-6f)
	{
		// 速度がほぼ0なら角度は更新しない
	} else
	{
		// atan2(x,z) を使って yaw を得る（プロジェクト内の回転軸と合わせてください）
		lastYaw = std::atan2(velX, velZ);
	}
	clearMotion_.rotation.x = 0.0f;
	clearMotion_.rotation.y = lastYaw;
	clearMotion_.rotation.z = 0.0f;

	// スケール (X, Z は既存ロジック。Y を追加して体積維持 or 独立制御を可能にする)
	float omegaXZ = 2.0f * 3.14159265358979323846f * poyoFreq;
	float sXZ = std::sin(omegaXZ * totalTime + poyoPhase);
	float sx = 1.0f + poyoAmp * sXZ;
	sx = std::max(0.05f, sx); // 極端に潰れないようにクランプ

	float sz;
	if (maintainArea)
	{
		// 面積維持（XZ面）
		sz = 1.0f / sx;
		sz = std::clamp(sz, 0.05f, 5.0f);
	} else
	{
		sz = 1.0f + poyoAmpZ * std::sin(omegaXZ * totalTime + poyoPhase + 3.14159265f / 2.0f);
		sz = std::clamp(sz, 0.05f, 5.0f);
	}

	// Y方向（高さ）のポヨポヨ
	float sy; // 実スケール（乗算前）
	if (maintainVolume)
	{
		// 3D体積維持：相対スケールの積を1に近づける -> sy_rel = 1 / (sx_rel * sz_rel)
		float sx_rel = std::max(0.01f, sx);
		float sz_rel = std::max(0.01f, sz);
		float sy_rel = 1.0f / (sx_rel * sz_rel);
		sy_rel = std::clamp(sy_rel, 0.05f, 5.0f);
		sy = baseScale.y * sy_rel;
	} else
	{
		// Yは独立して振動させる
		float omegaY = 2.0f * 3.14159265358979323846f * poyoFreqY;
		float sY = std::sin(omegaY * totalTime + poyoPhaseY);
		float sy_rel = 1.0f + poyoAmpY * sY;
		sy_rel = std::clamp(sy_rel, 0.05f, 5.0f);
		sy = baseScale.y * sy_rel;
	}

	// baseScale を乗算して最終スケールを出す
	clearMotion_.scale.x = baseScale.x * sx; // 横方向
	clearMotion_.scale.y = sy;               // 高さも変化させる
	clearMotion_.scale.z = baseScale.z * sz; // 縦方向（top-down だと視覚的に z を使う）

	object_->SetPosition(clearMotion_.position);
	object_->SetRotate(clearMotion_.rotation);
	object_->SetScale(clearMotion_.scale);
	object_->Update();

	// パーティクル
	ParticleEmitter::Emit("walk", clearMotion_.position, 1);

}

void Player::AutoMove()
{
	static int moveTimer = 0;
	static Vector3 autoDir = { 0.0f, 0.0f, 1.0f }; // 初期は前進

	// フィールド端の範囲
	const float minX = -15.0f, maxX = 15.0f;
	const float minZ = -15.0f, maxZ = 15.0f;

	// 一定フレームごとに新しいランダム方向を決める
	if (moveTimer <= 0)
	{
		// -1.0f～1.0fの範囲でランダムなx,zを生成
		float randX = (float(rand()) / RAND_MAX) * 2.0f - 1.0f;
		float randZ = (float(rand()) / RAND_MAX) * 2.0f - 1.0f;
		Vector3 dir = { randX, 0.0f, randZ };
		if (dir.Length() < 0.1f) dir.z = 1.0f; // ゼロベクトル対策
		autoDir = dir.Normalize();
		moveTimer = 60 + rand() % 90; // 1～2.5秒ごとに方向転換
	}
	moveTimer--;

	// 壁際判定：端に近づいたら(今の進行方向が外に向かっていたら)強制的に内向きにリダイレクト
	bool redirected = false;
	Vector3 nextPos = position_ + Vector3{ autoDir.x * moveSpeed_.x, 0.0f, autoDir.z * moveSpeed_.z };

	if (nextPos.x < minX or nextPos.x > maxX) 
	{
		autoDir.x = -autoDir.x;
		redirected = true;
	}
	if (nextPos.z < minZ or nextPos.z > maxZ) 
	{
		autoDir.z = -autoDir.z;
		redirected = true;
	}

	if (redirected) 
	{
		// 端で方向反転したら新たにmoveTimerを設定し、即座に再ランダム化しない
		moveTimer = 60 + rand() % 90;
	}

	// 移動速度を計算
	moveVelocity_ = { autoDir.x * moveSpeed_.x, 0.0f, autoDir.z * moveSpeed_.z };

	// プレイヤーの向き補間
	if (moveVelocity_.x != 0.0f or moveVelocity_.z != 0.0f)
	{
		Vector3 normalizedDir = moveVelocity_.Normalize();
		float targetRotationY = std::atan2(normalizedDir.x, normalizedDir.z);
		Vector3 currentRotation = rotation_;
		float easedRotationY = LerpAngle(currentRotation.y, targetRotationY, 0.2f);
		rotation_ = { currentRotation.x, easedRotationY, currentRotation.z };
	}

	// 位置更新
	position_ += moveVelocity_;

	// 移動制限
	ClampPosition();

	// パーティクル
	ParticleEmitter::Emit("walk", position_, 1);
}

void Player::AutoAttack()
{
	// 一定間隔で自動攻撃
	static int attackCooldown = 0;
	const int attackInterval = 50; // フレーム数
	if (attackCooldown <= 0)
	{
		// プレイヤーの向きに合わせて弾の速度を変更
		Vector3 bulletVelocity =
		{
			std::cosf(rotation_.x) * std::sinf(rotation_.y),     // x
			std::sinf(-rotation_.x),                             // y
			std::cosf(rotation_.x) * std::cosf(rotation_.y)      // z
		};
		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->SetPosition(position_);
		newBullet->Initialize();
		newBullet->SetVelocity(bulletVelocity);
		newBullet->RunSetMask();
		collider_.SetMask(colliderManager_->GetNewMask(collider_.GetColliderID(), "PlayerBullet"));
		// 弾を登録する
		pBullets_.push_back(newBullet);
		attackCooldown = attackInterval;
	}
	else
	{
		attackCooldown--;
	}
}

void Player::ClampPosition()
{
	position_.x = std::clamp(position_.x, limitMin_.x, limitMax_.x);
	position_.z = std::clamp(position_.z, limitMin_.y, limitMax_.y);
}

void Player::OnCollisionTrigger(const Collider* _other)
{

	if (!isEvading_ && (_other->GetColliderID() == "EnemyBullet" or
		_other->GetColliderID() == "NormalEnemy"))
	{
		// プレイヤーのHPを減少
		if (hp_ > 0.3)
		{
			hp_--;
		}
		else
		{
			isDead_ = true;
		}

		isHitMoment_ = true;
	} 

	if (!isEvading_ && (_other->GetColliderID() == "ExplosionTimeBomb" or
		_other->GetColliderID() == "Corruptor"))
	{
		if (_other->GetOwner()->IsActive())
		{
			// プレイヤーのHPを減少
			if (hp_ > 0.3)
			{
				hp_ -= 1.5f;
			}
			else
			{
				isDead_ = true;
			}

			isHitMoment_ = true;
		}
	}

	if (!isEvading_ && _other->GetColliderID() == "VignetteTrap")
	{
		if (_other->GetOwner()->IsActive())
		{
			// VignetteTrapに当たった場合
			isHitVignetteTrap_ = true;
		}
	}
}

void Player::OnCollision(const Collider* _other)
{
	if (_other->GetColliderID() == "Wall" or _other->GetColliderID() == "Barrie" or _other->GetColliderID() == "NormalEnemy")
	{
		// 相手のAABBを取得
		const AABB* otherAABB = _other->GetAABB();
		
		if (otherAABB) 
		{
			// 自分のAABBと位置を渡して補正
			CorrectOverlap(*otherAABB, aabb_, position_);
		}
	}
}

void Player::HitVignetteTrap()
{
	// フェードアウト中の処理
	if (isFadingOut_)
	{
		static const float fadeDuration = 30.0f;
		static float fadeTimer = 0.0f;

		fadeTimer++;
		float t = fadeTimer / fadeDuration;
		t = std::clamp(t, 0.0f, 1.0f);
		vignetteStrength_ = std::lerp(1.8f, 0.0f, t);

		PostEffectManager::GetInstance()->GetPassAs<VignettePass>("Vignette")->SetStrength(vignetteStrength_);

		if (t >= 1.0f)
		{
			// 完了：すべてリセット
			isFadingOut_ = false;
			fadeTimer = 0.0f;
			vignetteStrength_ = 0.0f;
			PostEffectManager::GetInstance()->SetActiveEffect("Vignette", isHitVignetteTrap_);

			// 環境マップを無効化
			environmentStrength_ = 0.0f;
			object_->SetEnvironmentStrength(environmentStrength_);

		}

		return;
	}

	// 通常の効果中
	if (isHitVignetteTrap_)
	{
		if (vignetteTime_ > 150)
		{
			// フェードイン
			float t = 1.0f - static_cast<float>(kMaxVignetteTime - vignetteTime_) / 30.0f;
			t = std::clamp(t, 0.0f, 1.0f);
			vignetteStrength_ = std::lerp(1.8f, 0.0f, t);
		}
		else
		{
			vignetteStrength_ = 1.8f;
		}

		// vignetteの強さを設定
		PostEffectManager::GetInstance()->SetActiveEffect("Vignette", isHitVignetteTrap_);
		PostEffectManager::GetInstance()->GetPassAs<VignettePass>("Vignette")->SetStrength(vignetteStrength_);

		ParticleEmitter::Emit("debuff", position_, 2);

		environmentStrength_ = 1.0f;

		object_->SetEnvironmentMapHandle(cubeHandle_, true);
		object_->SetEnvironmentStrength(environmentStrength_);


		// タイマー更新
		if (vignetteTime_ > 0)
		{
			vignetteTime_--;
		}
		else
		{
			// 明るくする準備
			isHitVignetteTrap_ = false;
			isFadingOut_ = true;
			// タイマーをリセット
			vignetteTime_ = kMaxVignetteTime;
		}
	}
}