#include "Player.h"

#include <cmath>

#include "TimeManager.h"

using namespace IIEngine;

void Player::Initialize()
{
	// 基底初期化
	Character::Initialize("player.obj", "Player", 12.0f);

	position_ = defaultPosition_;
	SyncObjectTransform();

	// ライト設定
	object_->SetLighting(true);

	// 使用枚数
	spriteNum_ = 3;

	// スプライト
	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		auto sprite = std::make_unique<Sprite>();

		if (i == 0)
		{
			sprite->Initialize("hp.png", { 0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.0f,0.5f });
			sprite->SetSize(hpBarSize_);

		}
		else if(i == 1)
		{
			
			sprite->Initialize("nearEnemy.png", { 0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,1.0f });

		}
		else if(i == 2)
		{
			sprite->Initialize("lockON.png", lockOnIndicatorPos_, lockOnIndicatorColor_, { 0.5f,0.5f });
			sprite->SetSize(lockOnIndicatorSize_);
	
		}

		sprites_.push_back(std::move(sprite));
	}

	// 画面が更新されたらビネットを0にする
	PostEffectManager::GetInstance()->GetPassAs<VignettePass>("Vignette")->SetStrength(0.0f);

	// 環境マップ
	cubeMapPath_ = "resources/images/studio.dds";
	TextureManager::GetInstance()->LoadTexture(cubeMapPath_);
	cubeSrvIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(cubeMapPath_);
	cubeHandle_ = TextureManager::GetInstance()->GetSrvManager()->GetGPUDescriptorHandle(cubeSrvIndex_);

	// 移動可能フラグ
	isCanMove_ = true;

	// 暗闇タイマー
	vignetteRemainingSec_ = kMaxVignetteSec_;

	// クールダウン
	shootCooldownSec_ = 0.0f;
	evadeRemainingSec_ = 0.0f;
	evadeCooldownSec_ = 0.0f;

	// フレームレート補正
	moveSpeed_.x *= kDefaultFrameRate;
	moveSpeed_.z *= kDefaultFrameRate;
	evadeSpeed_.x *= kDefaultFrameRate;
	evadeSpeed_.z *= kDefaultFrameRate;

	// RGBShit
	PostEffectManager::GetInstance()->SetActiveEffect("RGBShift", true);

	// ノイズ
	isNoiseActive_ = true;

	// バリア破壊フラグ
	isBarrierBroken_ = false;

	// Motion 初期化
	deathMotion_.Reset();
	clearMotion_.Reset();
	autoMotion_.Reset();
}

void Player::Finalize()
{
	for (auto& sprite : sprites_)
	{
		sprite.reset();
	}

	for (auto& bullet : pBullets_)
	{
		bullet->SetIsDead(true);
		bullet->Finalize();
	}

	RemoveDeadBullets<PlayerBullet>(pBullets_, [](const PlayerBullet& b) {
		IIEngine::ParticleEmitter::Emit("BltReaction", b.GetPosition(), 1);
		});

	colliderManager_->DeleteCollider(&collider_);
}

void Player::Update()
{
	if (deathMotion_.IsActive())
	{
		deathMotion_.Update(*this);

		Character::Update();
		return;
	}

	// 基底更新
	Character::Update();

	// 操作更新
	if (autoMotion_.IsActive())
	{
		autoMotion_.Update(*this);
		ReviveIfDeadInTitleDemo();
	}
	else
	{
		UpdateControl();
	}

	// 死亡したらy軸回転を徐々に0に&ノイズ解除
	if (isDead_)
	{
		rotation_.y = Lerp(rotation_.y, 0.0f, 0.05f);
		// ノイズを徐々に0にする
		finalStrength_ = Lerp(finalStrength_, 0.0f, 0.02f);
		PostEffectManager::GetInstance()->GetPassAs<NoisePass>("Noise")->SetIntensity(finalStrength_);

	}

	// 弾の削除
	RemoveDeadBullets<PlayerBullet>(pBullets_, [](const PlayerBullet& b)
		{
			IIEngine::ParticleEmitter::Emit("BltReaction", b.GetPosition(), 1);
		});

	// 弾更新
	for (auto& bullet : pBullets_)
	{
		bullet->Update();
	}


	// 暗闇処理
	HitVignetteTrap();

	// ステータス更新
	UpdateStatus();

	// 最も近い敵インジケーター更新
	UpdateNearEnemyIndicator();

	// スロー
	EvadeSlow();

	// RGBシフト
	DamageRGBShift();

	// 回避コンボ
	EvadeComboInversion();

	// HP減少ノイズ
	HPDecreaseNoise();
}

void Player::Draw()
{
	Character::Draw();

	// 弾描画
	for (auto& bullet : pBullets_)
	{
		bullet->Draw();
	}
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

	// ロックオン系
	if (ImGui::CollapsingHeader("LockOn System"))
	{
		ImGui::SliderFloat("chargeRange", &lockOnChargeRange_, 10.0f, 50.0f);
		ImGui::SliderFloat("chargeTimer", &lockOnChargeTimerSec_, 0.0f, 5.0f);
	}

	if (ImGui::CollapsingHeader("LockOn Indicator (sprites_[2])"))
	{
		ImGui::DragFloat2("Pos", &lockOnIndicatorPos_.x, 1.0f);
		ImGui::DragFloat2("Size base", &lockOnIndicatorSize_.x, 1.0f);
		ImGui::SliderFloat("Angle", &lockOnIndicatorAngle_, -3.14f * 2.0f, 3.14f * 2.0f);
		ImGui::SliderFloat("Alpha", &lockOnIndicatorAlpha_, 0.0f, 1.0f);
		ImGui::ColorEdit4("Color", &lockOnIndicatorColor_.x);

		if (sprites_.size() > 2 && sprites_[2])
		{
			Vector2 currentPos = sprites_[2]->GetPosition();
			Vector2 currentSize = sprites_[2]->GetSize();
			Vector4 currentColor = sprites_[2]->GetColor();

			ImGui::Text("Actual Sprite Pos:   %.1f, %.1f", currentPos.x, currentPos.y);
			ImGui::Text("Actual Sprite Size:  %.1f, %.1f", currentSize.x, currentSize.y);
			ImGui::Text("Actual Sprite Color: %.2f, %.2f, %.2f, %.2f", currentColor.x, currentColor.y, currentColor.z, currentColor.w);
		
		}
	}

	ImGui::End();

	for (auto& bullet : pBullets_)
	{
		bullet->ImGuiDraw();
	}

#endif // USE_IMGUI
}

void Player::ClearTargetEnemy()
{
	hasTarget_ = false;
	targetEnemyPosition_ = { 0.0f,0.0f,0.0f };
}

void Player::Move()
{
	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();
	// 入力時の省略
	auto* input = IIEngine::Input::GetInstance();

	moveVelocity_ = {};

	// コントローラー接続されているときはスティック入力、そうでないときはキーボード入力
	if (input->IsPadConnected())
	{
		StickState ls = input->GetLeftStick();

		// デッドゾーンはController側にもあるが、歩き出しの安定のためにここでも軽く
		const float th = 0.15f;
		if (std::abs(ls.x) < th) ls.x = 0.0f;
		if (std::abs(ls.y) < th) ls.y = 0.0f;

		// 画面上の見た目通りに移動
		moveVelocity_.x = ls.x * moveSpeed_.x;
		moveVelocity_.z = ls.y * moveSpeed_.z;
	}
	else
	{
		// 画面上の見た目通りに移動
		if (input->PushKey(DIK_W) or input->PushKey(DIK_UP))
		{
			moveVelocity_.z += moveSpeed_.z;
		}
		if (input->PushKey(DIK_S) or input->PushKey(DIK_DOWN))
		{
			moveVelocity_.z -= moveSpeed_.z;
		}
		if (input->PushKey(DIK_A) or input->PushKey(DIK_LEFT))
		{
			moveVelocity_.x -= moveSpeed_.x;
		}
		if (input->PushKey(DIK_D) or input->PushKey(DIK_RIGHT))
		{
			moveVelocity_.x += moveSpeed_.x;
		}
	}

	// 移動ベクトルがゼロでない場合にプレイヤーの向きを補間で更新
	if (moveVelocity_.x != 0.0f || moveVelocity_.z != 0.0f)
	{
		// ロックオンしているときは向き計算しない
		if (!isLockOn_)
		{
			// 正規化された方向ベクトル
			Vector3 normalizedDir = moveVelocity_;
			normalizedDir = normalizedDir.Normalize();

			// 回転
			RotationUpdate(normalizedDir);
		}

		// パーティクル
		IIEngine::ParticleEmitter::Emit("walk", position_, 1);
	}

	// 位置更新
	position_ += moveVelocity_ * dt;
}

void Player::Attack()
{
	if (IIEngine::Input::GetInstance()->PushKey(DIK_SPACE) or
		(IIEngine::Input::GetInstance()->IsPadConnected() &&
		IIEngine::Input::GetInstance()->PushPadButton(ControllerButtonType::RT)))
	{
		if (shootCooldownSec_ <= 0.0f)
		{
			FireBulletForward();

			// クールタイムをリセット
			shootCooldownSec_ = kShootCoolDownSec_;
		}
	}
}

void Player::FireBulletForward()
{
	Vector3 bulletVelocity =
	{
		std::cosf(rotation_.x) * std::sinf(rotation_.y),
		std::sinf(-rotation_.x),
		std::cosf(rotation_.x) * std::cosf(rotation_.y)
	};

	auto newBullet = std::make_unique<PlayerBullet>();
	newBullet->SetPosition(position_);
	newBullet->Initialize();
	newBullet->SetVelocity(bulletVelocity);
	pBullets_.push_back(std::move(newBullet));
}

void Player::Evade()
{
	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// 回避入力(クールタイムが0のときのみ許可)
	if (!isEvading_ && evadeCooldownSec_ <= 0.0f &&
		(IIEngine::Input::GetInstance()->PushKey(DIK_LSHIFT) or
		(IIEngine::Input::GetInstance()->IsPadConnected() &&
		IIEngine::Input::GetInstance()->PushPadButton(ControllerButtonType::A))))
	{
		// 移動方向がある場合のみ回避
		if (moveVelocity_.x != 0.0f or moveVelocity_.z != 0.0f)
		{
			isEvading_ = true;

			evadeRemainingSec_ = kEvadeDurationSec_;
			// 現在の移動方向を回避方向として保存
			evadeDirection_ = moveVelocity_;
			// 正規化
			float len = std::sqrt(evadeDirection_.x * evadeDirection_.x + evadeDirection_.z * evadeDirection_.z);
			if (len > 0.0f)
			{
				evadeDirection_.x /= len;
				evadeDirection_.z /= len;
			}
			// 回避開始時のy軸角度を保存
			evadeStartRotationY_ = rotation_.y;
			// 目標角度を設定
			float sign = (evadeDirection_.x >= 0.0f) ? 1.0f : -1.0f;

			// 目標Y回転
			evadeTargetRotationY_ = evadeStartRotationY_ + kEvadeRotateAngle_ * sign;

			// 回避開始時にクールタイムを設定
			evadeCooldownSec_ = kEvadeCoolDownSec_;
		}
	}
	// 回避中の処理
	if (isEvading_)
	{
		// 回避移動
		position_ += evadeDirection_ * Vector3{ evadeSpeed_.x, 0.0f, evadeSpeed_.z } *dt;

		// 回避中の回転
		float t = 1.0f - (evadeRemainingSec_ / kEvadeDurationSec_);
		t = std::clamp(t, 0.0f, 1.0f); 
		rotation_.y = evadeStartRotationY_ + (evadeTargetRotationY_ - evadeStartRotationY_) * t * dt * kDefaultFrameRate;

		// 回避時間の減少
		evadeRemainingSec_ -= dt;

		// 回避終了処理
		if (evadeRemainingSec_ <= 0.0f)
		{
			isEvading_ = false;
			evadeRemainingSec_ = 0.0f;
			rotation_.y = evadeStartRotationY_;
			// 回避終了時にコンボ判定をリセットしておく
			isEvadeComboActive_ = false;
			hasEvadeSlowHit_ = false;
			evadeComboWindowSec_ = 0.0f;
		}
	}
}

void Player::UpdateNearEnemyIndicator()
{
	auto camera = CameraManager::GetInstance().GetActiveCamera();
	if (!camera)
	{
		return;
	}

	// ターゲットがセットされていない場合は画面外
	if (!hasTarget_)
	{
		sprites_[1]->SetPosition({ -10000.0f, -10000.0f });
		sprites_[1]->Update();
		return;
	}

	// スクリーン上の中心
	Vector3 worldCenter = position_;
	Vector2 screenCenter = camera->WorldToScreen(worldCenter);

	// ターゲットをスクリーンに投影
	Vector2 enemyScreen = camera->WorldToScreen(targetEnemyPosition_);

	// スクリーン差分
	float dx = enemyScreen.x - screenCenter.x;
	float dy = enemyScreen.y - screenCenter.y;

	// targetAngleの計算
	float targetAngle = std::atan2f(dx, -dy);

	// インジケーターの回転角度を徐々に変化させる
	const float TWO_PI = 6.28318530717958647692f;
	if (indicatorSpinAngle_ < 0.0f)
	{
		indicatorSpinAngle_ += TWO_PI;
	}

	// 位置角
	float posAngle = targetAngle;
	Vector2 size = sprites_[1]->GetSize();
	float sx = screenCenter.x + std::sinf(posAngle) * indicatorRadius_;
	float sy = screenCenter.y - std::cosf(posAngle) * indicatorRadius_;

	// スプライトの回転
	float arrowOffset = 0.0f;
	float spriteRotation = targetAngle + arrowOffset;

	// 距離に基づくサイズと透明度
	Vector3 diffWorld = targetEnemyPosition_ - position_;
	float worldDist = std::sqrt(diffWorld.x * diffWorld.x + diffWorld.y * diffWorld.y + diffWorld.z * diffWorld.z);
	float norm = std::clamp(worldDist / indicatorMaxDetectRange_, 0.0f, 1.0f);
	float sizeScale = std::lerp(1.2f, 0.6f, norm);
	float alpha = std::lerp(1.0f, 0.4f, norm);

	// 反映
	sprites_[1]->SetPosition({ sx, sy });
	sprites_[1]->SetRotation(spriteRotation);
	sprites_[1]->SetSize(Vector2{ 62.0f * sizeScale, 62.0f * sizeScale });

	// 色のアルファを反映
	Vector4 col = sprites_[1]->GetColor();
	col.w = alpha;
	sprites_[1]->SetColorChange(col);

	sprites_[1]->Update();

}

void Player::StartDeathMotion()
{
	deathMotion_.Start(*this);
}

void Player::ClearSceneUpdate()
{
	if (!clearMotion_.IsActive()) 
	{
		clearMotion_.Start(*this);
	}
	clearMotion_.Update(*this);
}

void Player::UpdateControl()
{
	if (!isCanMove_)
	{
		return;
	}

	// 回避処理
	Evade();

	// アクティブフラグに回避フラグを入れる
	isActive_ = isEvading_;

	// ロックオン処理
	LockOn();

	// 回避中は移動・攻撃を無効化
	if (!isEvading_ && !isDead_)
	{
		Move();
		Attack();
	}
}

void Player::UpdateStatus()
{
	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// 攻撃クールダウン
	if (shootCooldownSec_ > 0.0f)
	{
		shootCooldownSec_ -= dt;
		if (shootCooldownSec_ < 0.0f) shootCooldownSec_ = 0.0f;
	}

	// 回避クールダウン
	if (evadeCooldownSec_ > 0.0f)
	{
		evadeCooldownSec_ -= dt;
		if (evadeCooldownSec_ < 0.0f) evadeCooldownSec_ = 0.0f;
	}
}

void Player::ClampPosition()
{
	position_.x = std::clamp(position_.x, limitMin_.x, limitMax_.x);
	position_.z = std::clamp(position_.z, limitMin_.y, limitMax_.y);
}

void Player::EvadeSlow()
{
	// 回避中のスローモーション
	if (isSlowMotion_)
	{
		slowTimerSec_ -= TimeManager::Instance().GetUnscaledDeltaTime();

		if (slowTimerSec_ > kSlowRecoverTime_)
		{
			// 固定スロー区間
			TimeManager::Instance().SetTimeScale(0.1f);
			radialStrength_ = 0.4f;
		} 
		else
		{
			// 復帰区間
			float t = 1.0f - (slowTimerSec_ / kSlowRecoverTime_);
			t = std::clamp(t, 0.0f, 1.0f);

			float timeScale = std::lerp(0.1f, 1.0f, t);
			TimeManager::Instance().SetTimeScale(timeScale);

			radialStrength_ = std::lerp(0.8f, 0.0f, t);
		}

		// RadialBlurPassを取得
		auto* radial = PostEffectManager::GetInstance()->GetPassAs<RadialBlurPass>("RadialBlur");
		radial->SetStrength(radialStrength_);

		if (slowTimerSec_ <= 0.0f)
		{
			isSlowMotion_ = false;
			TimeManager::Instance().SetTimeScale(1.0f);
			// ブラー解除
			radial->SetStrength(0.0f);
			PostEffectManager::GetInstance()->SetActiveEffect("RadialBlur", false);

		}
	}

}

void Player::DamageRGBShift()
{
	// delta
	const float dt = TimeManager::Instance().GetDeltaTime();
	
	// RGBシフトエフェクトの更新
	if (isRGBShiftActive_)
	{
		rgbShiftTimer_ += dt;

		// 一定時間が経過したらエフェクトを無効化
		if (rgbShiftTimer_ >= rgbShiftDuration_)
		{
			isRGBShiftActive_ = false;
			rgbShiftTimer_ = 0.0f;
		}

		float t = rgbShiftTimer_;
		float power = 0.0f;

		// 大きければ強度も大きくなる
		float amplitude = rgbShiftAmplitude_;
		power = amplitude * abs(sin(t * rgbShiftOscillation_)) * exp(-t * rgbShiftDamping_);


		// Powerをエフェクトに反映
		PostEffectManager::GetInstance()->GetPassAs<RGBShiftPass>("RGBShift")->SetIntensity(power);
	}

}

void Player::EvadeComboInversion()
{
	if (isEvading_)
	{
		evadeComboWindowSec_ += IIEngine::TimeManager::Instance().GetUnscaledDeltaTime();
	}

	// Inversion の残り時間更新
	if (inversionRemainingSec_ > 0.0f)
	{
		inversionRemainingSec_ -= TimeManager::Instance().GetUnscaledDeltaTime();
		if (inversionRemainingSec_ <= 0.0f)
		{
			inversionRemainingSec_ = 0.0f;
			// カウント完了で Inversion を無効化
			PostEffectManager::GetInstance()->SetActiveEffect("Inversion", false);
		}
	}

	// 一度発動したらリセット
	isEvadeComboActive_ = false;
}

void Player::HPDecreaseNoise()
{
	if (!autoMotion_.IsActive() && !isDead_ && isNoiseActive_)
	{
		float hpRate = hp_ / maxHP_;
		const float dangerThreshold = hpDangerThreshold_;

		auto* noise = PostEffectManager::GetInstance()->GetPassAs<NoisePass>("Noise");

		// ゴールに触れたらノイズ下げる
		if (isTouchGoal_)
		{
			// 強度を徐々に下げる
			finalStrength_ = Lerp(finalStrength_, 0.0f, 0.02f);
			noise->SetIntensity(finalStrength_);

		}
		else if (hpRate < dangerThreshold)
		{
			float t = 1.0f - (hpRate / dangerThreshold);
			t = std::clamp(t, 0.0f, 1.0f);

			// 強めイージング
			float eased = powf(t, hpNoisePow_);

			// 不安定揺らぎ
			float time = TimeManager::Instance().GetTotalTime();
			float pulse = sinf(time * hpNoisePulseFreq_) * hpNoisePulseAmp_;

			// ベース強度アップ
		    finalStrength_ = eased * hpNoiseBaseGain_ + pulse;
			finalStrength_ = std::clamp(finalStrength_, 0.0f, 1.0f);

			PostEffectManager::GetInstance()->SetActiveEffect("Noise", true);
			noise->SetIntensity(finalStrength_);
		} 
		else
		{
			PostEffectManager::GetInstance()->SetActiveEffect("Noise", false);
		}
	}
}

void Player::LockOn()
{	
	// ターゲットがいなかったらスキップ
	if (!hasTarget_)
	{
		// ターゲットがいない場合はロックオフ
		isLockOn_ = false;
		lockOnChargeTimerSec_ = 0.0f;

		// ターゲットがいない場合は透明にして更新する
		lockOnIndicatorAlpha_ = 0.0f;
		lockOnIndicatorColor_ = Vector4{ 1.0f, 0.2f, 0.2f, lockOnIndicatorAlpha_ };
		sprites_[2]->SetColorChange(lockOnIndicatorColor_);
		sprites_[2]->Update();

		return;
	}

	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();
	
	// 初回は比較しない
	if (preTargetPos_ != Vector3{ 0.0f, 0.0f, 0.0f })
	{
		const float kTargetSwitchEpsilon = 1.0f; // 調整用
		Vector3 diff = targetEnemyPosition_ - preTargetPos_;

		if (diff.Length() > kTargetSwitchEpsilon)
		{
			isLockOn_ = false;
			lockOnChargeTimerSec_ = 0.0f;
		}
	}

	// ターゲットの位置を保存
	preTargetPos_ = targetEnemyPosition_;

	// ターゲットとのベクトルと距離を計算
	Vector3 toTarget = targetEnemyPosition_ - position_;
	float distance = toTarget.Length();

	// ロックオン可能範囲内ならチャージ開始
	if ((distance <= lockOnChargeRange_) && !isLockOn_)
	{
		// チャージタイマーを増加
		lockOnChargeTimerSec_ += dt;

		// チャージが完了したらロックオン状態に
		if (lockOnChargeTimerSec_ >= kLockOnChargeMaxTimeSec_)
		{
			isLockOn_ = true;

		}
	}
	else if(distance > lockOnChargeRange_)
	{
		// 範囲外に出たらチャージリセット
		isLockOn_ = false;
		lockOnChargeTimerSec_ = 0.0f;
					
	}

	// ロックオン状態のときの処理
	if (isLockOn_)
	{
		// ターゲットの向きを取得
		toTarget = targetEnemyPosition_ - position_;
		// ターゲットの方向に向ける
		RotationUpdate(toTarget);
	}

	// ===== UI更新 =====
	auto camera = CameraManager::GetInstance().GetActiveCamera();

	float progress = lockOnChargeTimerSec_ / kLockOnChargeMaxTimeSec_;
	progress = std::clamp(progress, 0.0f, 1.0f);

	// スクリーン位置計算
	lockOnIndicatorPos_ = camera->WorldToScreen(targetEnemyPosition_);
	sprites_[2]->SetPosition({lockOnIndicatorPos_.x, lockOnIndicatorPos_.y + lockOnIndicatorOffsetY_});

	// スケール
	float scale = 0.6f + progress * 0.6f;
	sprites_[2]->SetSize(lockOnIndicatorSize_ * scale);

	// 回転
	lockOnIndicatorAngle_ += dt * (1.0f + progress * 3.0f);
	sprites_[2]->SetRotation(lockOnIndicatorAngle_);

	// 透明度
	lockOnIndicatorAlpha_ = progress;

	// 点滅
	if (progress < 0.8f)
	{
		float blink = sin(IIEngine::TimeManager::Instance().GetTotalTime() * 30.0f) * 0.5f + 0.5f;
		lockOnIndicatorAlpha_ *= blink;
	}

	lockOnIndicatorColor_ = Vector4{ 1.0f, 0.0f, 0.0f, lockOnIndicatorAlpha_ };
	sprites_[2]->SetColorChange(lockOnIndicatorColor_);

	sprites_[2]->Update();

}

void Player::ReviveIfDeadInTitleDemo()
{
	// タイトルデモ中のみ呼ばれる想定
	if (!isDead_)
	{
		return;
	}

	isDead_ = false;
	hp_ = 8.0f;

	// 死亡演出が走っていたら止める
	deathMotion_.Reset();

	// ロックオン状態が残ると向きがズレるので解除
	isLockOn_ = false;
	lockOnChargeTimerSec_ = 0.0f;
	preTargetPos_ = { 0.0f,0.0f,0.0f };
}

void Player::RotationUpdate(const Vector3& _toAngle)
{
	// ターゲットの方向に向ける
	float targetRotationY = std::atan2(_toAngle.x, _toAngle.z);
	Vector3 currentRotation = rotation_;

	// Y軸の回転のみ、最短経路で補間
	float easedRotationY = LerpAngle(currentRotation.y, targetRotationY, turnLerpRate_);

	// 回転を更新
	rotation_ = { currentRotation.x, easedRotationY, currentRotation.z };
}

void Player::OnCollisionTrigger(const Collider* _other)
{

	// 回避中のスローモーション発動
	if (isEvading_)
	{
		// 回避中に当たったらスロモにしたいもの
		const bool isSlowMotionTarget =
			(_other->GetColliderID() == "VignetteTrap") or
			(_other->GetColliderID() == "EnemyBullet") or
			(_other->GetColliderID() == "SetTimeBomb") or
			(_other->GetColliderID() == "Corruptor");
		if (isSlowMotionTarget)
		{
			if (!hasEvadeSlowHit_)
			{
				// 1回目 マークだけ
				hasEvadeSlowHit_ = true;
				evadeComboWindowSec_ = 0.0f; // 1回目から計測開始
			}
			else
			{
				// 2回目 一定時間以上離れていれば「コンボ」として認める
				if (evadeComboWindowSec_ >= kEvadeComboMinIntervalSec_)
				{
					isEvadeComboActive_ = true;
					inversionRemainingSec_ = inversionDuration_;
					PostEffectManager::GetInstance()->SetActiveEffect("Inversion", true);

				}
			}

			// スローモーション開始
			isSlowMotion_ = true;
			slowTimerSec_ = kSlowHoldTime_ + kSlowRecoverTime_;

			PostEffectManager::GetInstance()->SetActiveEffect("RadialBlur", true);
		}

		// 回避中は通常の被弾処理をさせない
		return;
	}

	if (_other->GetColliderID() == "EnemyBullet" or
		_other->GetColliderID() == "NormalEnemy" or
		_other->GetColliderID() == "TrapEnemy" or
		_other->GetColliderID() == "Corruptor")
	{
		// プレイヤーのHPを減少
		if (hp_ > 0.3)
		{
			hp_--;

			isHitMoment_ = true;

			// RGBシフトエフェクトの有効化
			rgbShiftTimer_ = 0.0f;
			isRGBShiftActive_ = true;

			IIEngine::ParticleEmitter::Emit("HitReaction", position_, 5);
		}
		else
		{
			isDead_ = true;
		}


	}

	if (_other->GetColliderID() == "ExplosionTimeBomb" or
		_other->GetColliderID() == "Corruptor")
	{
		if (_other->GetOwner()->IsActive())
		{
			// プレイヤーのHPを減少
			if (hp_ > deathHpThreshold_)
			{
				hp_ -= explosionDamage_;
				IIEngine::ParticleEmitter::Emit("HitReaction", position_, hitParticleCountHeavy_);
			} 
			else
			{
				isDead_ = true;
			}

			// RGBシフトエフェクトの有効化
			rgbShiftTimer_ = 0.0f;
			isRGBShiftActive_ = true;

			isHitMoment_ = true;
		}
	}

	if (_other->GetColliderID() == "VignetteTrap")
	{
		if (_other->GetOwner()->IsActive())
		{
			// VignetteTrapに当たった場合
			isHitVignetteTrap_ = true;
		}
	}

	if (_other->GetColliderID() == "Goal")
	{
		// ゴールに触れたらフラグ trueに
		isTouchGoal_ = true;
	}

}

void Player::OnCollision(const Collider* _other)
{
	if ((_other->GetColliderID() == "Wall" or
		_other->GetColliderID() == "Barrie" or
		_other->GetColliderID() == "NormalEnemy")
		&& !isBarrierBroken_)
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
	// デルタタイム
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// フェードアウト中の処理
	if (isFadingOut_)
	{
		static const float fadeDurationSec = 30.0f / kDefaultFrameRate;
		static float fadeTimer = 0.0f;

		fadeTimer += dt;

		float t = fadeTimer / fadeDurationSec;
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
		// フェードインの開始タイミング
		const float elapsedSec = kMaxVignetteSec_ - vignetteRemainingSec_;

		const float fadeInDurationSec = 30.0f / kDefaultFrameRate;

		if (elapsedSec < fadeInDurationSec)
		{
			// フェードイン
			float t = elapsedSec / fadeInDurationSec;
			t = std::clamp(t, 0.0f, 1.0f);

			// 0→最大へ 
			vignetteStrength_ = std::lerp(0.0f, 1.8f, t);
		}
		else
		{
			vignetteStrength_ = 1.8f;
		}

		// vignetteの強さを設定
		PostEffectManager::GetInstance()->SetActiveEffect("Vignette", isHitVignetteTrap_);
		PostEffectManager::GetInstance()->GetPassAs<VignettePass>("Vignette")->SetStrength(vignetteStrength_);

		IIEngine::ParticleEmitter::Emit("debuff", position_, 2);

		environmentStrength_ = 1.0f;

		object_->SetEnvironmentMapHandle(cubeHandle_, true);
		object_->SetEnvironmentStrength(environmentStrength_);


		// タイマー更新
		vignetteRemainingSec_ -= dt;

		if (vignetteRemainingSec_ <= 0.0f)
		{
			isHitVignetteTrap_ = false;
			isFadingOut_ = true;

			vignetteRemainingSec_ = kMaxVignetteSec_;
		}
	}
}

void Player::SetAutoControl(bool _enable)
{
	if (_enable)
	{
		autoMotion_.Start(*this);
	} 
	else
	{
		autoMotion_.Reset();
		// 手動に戻すなら移動許可を戻す
		isCanMove_ = true;
	}
}
