#include "ParticleEmitter.h"

namespace IIEngine
{

	void ParticleEmitter::Emit(const std::string& groupName, const Vector3& position, uint32_t count)
	{
		if (auto manager = ParticleManager::GetInstance())
		{
			manager->Emit(groupName, position, count);
		}
	}

	void ParticleEmitter::StartLoop(const std::string& groupName, const std::string& motionName, const Vector3& position, uint32_t count)
	{
		if (auto manager = ParticleManager::GetInstance())
		{
			EmitSetting setting;
			setting.groupName = groupName;
			setting.motionName = motionName;
			setting.emitPosition = position;
			setting.emitCount = count;
			setting.isLooping = true;
			manager->AddEmitterSetting(setting);
		}
	}

	void ParticleEmitter::EmitOnce(const Vector3& position, uint32_t count)
	{
		if (manager_)
		{
			manager_->Emit(groupName_, position, count);
		}
	}

	void ParticleEmitter::StartLoopEmit(const Vector3& position, uint32_t count)
	{
		if (!manager_)
		{
			return;
		}

		EmitSetting setting;
		setting.groupName = groupName_;
		setting.motionName = motionName_;
		setting.emitPosition = position;
		setting.emitCount = count;
		setting.isLooping = true;

		manager_->AddEmitterSetting(setting);
	}

}