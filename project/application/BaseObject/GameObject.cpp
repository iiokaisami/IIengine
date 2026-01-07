#include "GameObject.h"

#include <functional>
#include <sstream>

GameObject::GameObject()
{
    objectName_ = "unnamed-object";
    scale_ = { 1.0f, 1.0f, 1.0f };
    rotation_ = { 0.0f, 0.0f, 0.0f };
    position_ = { 0.0f, 0.0f, 0.0f };
}