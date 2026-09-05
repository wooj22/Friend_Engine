#include "Rigidbody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Time.h"

void Rigidbody::OnEnable_Inner()
{
    PhysicsSystem::Get().Regist(this);
	transform = this->gameObject->transform;
}

void Rigidbody::OnDisable_Inner()
{
	PhysicsSystem::Get().Unregist(this);
	transform = nullptr;
}

void Rigidbody::OnDestroy_Inner()
{
    PhysicsSystem::Get().Unregist(this);
}

void Rigidbody::FixedUpdate()
{
    if (!transform) return;
    previousPosition = transform->GetWorldPosition();

    if (!isKinematic)
    {
        // impulse update - AddImpulse() already divides by mass before storing it,
        // so dividing again here would turn Δv = J / mass into J / mass^2.
        velocity += impulse;

        // gravity update
        if (useGravity && !isGrounded) acceleration += Vector2(0, -9.8f) * gravityScale;

        // acceleration update
        velocity += acceleration * Time::GetFixedDeltaTime();
        velocity *= (1.0f - drag);

        // block update
        if (isBlockedLeft && velocity.x < 0) velocity.x = 0;
        if (isBlockedRight && velocity.x > 0) velocity.x = 0;
        if (isBlockedDown && velocity.y < 0) velocity.y = 0;
        if (isBlockedUp && velocity.y > 0) velocity.y = 0;

        // position update
        switch (collisionDetection)
        {
        case CollisionDetection::Discrete:
            transform->SetPosition(transform->GetWorldPosition() + velocity * Time::GetFixedDeltaTime());
            break;

        case CollisionDetection::Continuous:
            CollisionContinuousDetection();
            break;

        default:
            break;
        }

        // reset
        impulse = Vector2::zero;
        acceleration = Vector2::zero;
    }
}

// colliison position correct
void Rigidbody::CorrectPosition(const ContactInfo& contact, float factor)
{
    if (!isKinematic)
    {
        // position
        transform->Translate(contact.normal * contact.depth * factor);

        // block
        if (contact.normal.x > 0)      isBlockedLeft = true;
        else if (contact.normal.x < 0) isBlockedRight = true;
        if (contact.normal.y > 0)      isBlockedDown = true;
        else if (contact.normal.y < 0) isBlockedUp = true;
    }
}

// CCD (continuous collision detection)
void Rigidbody::CollisionContinuousDetection()
{
    // Sweep Test
    // 1. 이전 프레임 위치와 현재 프레임에 이동할 위치 사이를 가상으로 쓸고 지나간다. (+= Step)
    // 2. ColliderSystem에서 모든 Colldier를 Get해와 해당 오브젝트가 충돌하는 콜라이더가 있는지 체크한다.
    // 3. 만약 충돌하는 콜라이더가 있다면, 그 충돌한 지점 이전 지점으로 포지션을 지정한다.

    // return
    if (velocity.SqrMagnitude() < 0.0001f) return;

    Vector2 startPos = previousPosition;
    Vector2 endPos = startPos + velocity * Time::GetFixedDeltaTime();

    ICollider* myCollider = this->gameObject->GetComponent<ICollider>();
    if (!myCollider) return;

    // step
    const int maxSteps = 10;
    Vector2 step = (endPos - startPos) / maxSteps;

    // sweeeeeeeeep!
    for (int i = 1; i <= maxSteps; ++i)
    {
        Vector2 testPos = startPos + step * i;
        myCollider->gameObject->transform->SetPosition(testPos);
        myCollider->UpdateBounds();

        for (ICollider* other : ColliderSystem::Get().GetColliders())
        {
            if (other == myCollider || other->isTrigger) continue;

            ContactInfo contact;
            if (myCollider->isCollision(other, contact))
            {
                // 이동경로중에 충돌이 있을 때, 
                // velocity와 반대 방향의 충돌일 때만 보정
                if (Vector2::Dot(velocity, contact.normal) < 0)
                {
                    Rigidbody* otherRb = other->gameObject->GetComponent<Rigidbody>();
                    float factor = (otherRb && !otherRb->isKinematic) ? 0.5f : 1.0f;
                    CorrectPosition(contact, factor);
                    return;
                }
            }
        }
    }

    // 충돌이 없다면 이동하려면 endPos로 이동
    myCollider->gameObject->transform->SetPosition(endPos);
}

void Rigidbody::AddForce(const Vector2& force)
{
    acceleration += force / mass;
}

void Rigidbody::AddImpulse(const Vector2& impulse) 
{
    this->impulse = impulse / mass;
}
