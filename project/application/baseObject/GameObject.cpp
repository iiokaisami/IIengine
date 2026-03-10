#include "GameObject.h"

#include <functional>
#include <sstream>

GameObject::GameObject()
{
    objectName_ = "unnamed-object";

}

void GameObject::SyncObjectTransform()
{
	if (!object_)
	{
		return;
	}

	// GameObjectの transformを object_に反映
	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	object_->SetScale(scale_);
	// Update
	object_->Update();
}