#include "CircleCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Script.h"
#include "RenderSystem.h"
#include "BoxCollider.h"
#include "Rigidbody.h"

void CircleCollider::OnEnable_Inner()
{
    ColliderSystem::Get().Regist(this);
	transform = gameObject->transform;
}

void CircleCollider::OnDisable_Inner()
{
	ColliderSystem::Get().Unregist(this);
}

void CircleCollider::OnDestroy_Inner()
{
    ColliderSystem::Get().Unregist(this);
}

// UpdateBounds()
// circle collider�� aabb bound�� update�Ѵ�
// colldierSystem�� sap �˰����� ���ȴ�. (����ȭ)
void CircleCollider::UpdateBounds()
{
    Vector2 center = GetCenter();
    float scaledRadius = radius * transform->GetWorldScale().x;

    minX = center.x - scaledRadius;
    maxX = center.x + scaledRadius;
    minY = center.y - scaledRadius;
    maxY = center.y + scaledRadius;
}

// isCollision()
// �Ѱܹ��� �ݶ��̴��� Ÿ�Կ� ���� �浹 ����� return�Ѵ�
// �浹�� ��� contact info�� ����Ѵ�
bool CircleCollider::isCollision(ICollider* other, ContactInfo& contact)
{
    if (!transform) return false;

    if (other->colliderType == ColliderType::Circle)
    {
        return CheckCircleCollision(static_cast<CircleCollider*>(other), contact);
    }
    else if (other->colliderType == ColliderType::Box)
    {
        return CheckBoxCollision(static_cast<BoxCollider*>(other), contact);
    }

    return false;
}

// FinalizeCollision()
// ���� ������ �浹 ������ ���� ������ �浹 ������ ���Ͽ�
// isTrigger ������ ���� Enter, Stay, Exit �浹 �̺�Ʈ �Լ��� ȣ���Ѵ�.
void CircleCollider::FinalizeCollision()
{
    // Enter & Stay
    for (auto& pair : currentFrameCollisions)
    {
        ICollider* other = pair.first;
        ContactInfo& contact = pair.second;

        if (lastFrameCollisions.find(other) == lastFrameCollisions.end())
        {
            if (isTrigger || other->isTrigger)
                OnTriggerEnter(other, contact);
            else
                OnCollisionEnter(other, contact);
        }
        else
        {
            if (isTrigger || other->isTrigger)
                OnTriggerStay(other, contact);
            else
                OnCollisionStay(other, contact);
        }
    }

    // Exit
    for (auto& pair : lastFrameCollisions)
    {
        ICollider* other = pair.first;
        ContactInfo& contact = pair.second;

        if (currentFrameCollisions.find(other) == currentFrameCollisions.end())
        {
            if (isTrigger || other->isTrigger)
                OnTriggerExit(other, contact);
            else
                OnCollisionExit(other, contact);
        }
    }

    lastFrameCollisions = currentFrameCollisions;
    currentFrameCollisions.clear();
}

// CheckCircleCollision()
// this circle�� other circle�� �浹 üũ
bool CircleCollider::CheckCircleCollision(CircleCollider* other, ContactInfo& contact)
{
    Vector2 center = GetCenter();
    Vector2 otherCenter = other->GetCenter();

    Vector2 scale = transform->GetWorldScale();
    Vector2 otherScale = other->transform->GetWorldScale();
    float scaledRadius = radius * scale.x;
    float otherScaledRadius = other->radius * otherScale.x;

    Vector2 diff = center - otherCenter;
    float distSq = diff.SqrMagnitude();
    float radiusSum = scaledRadius + otherScaledRadius;

    if (distSq > radiusSum * radiusSum)
        return false;

    // Contact Info
    Vector2 dir;
    float distance = sqrtf(distSq);

    if (distSq == 0.0f)
    {
        // ���� : �߽��� ��Ȯ�� ��ģ ���
        dir = Vector2(1, 0);
        contact.depth = radiusSum; // ��ģ ������ ��ü ������ ������ ����
    }
    else
    {
        dir = diff / distance;
        contact.depth = radiusSum - distance;
    }

    contact.normal = dir;
    contact.point = otherCenter + dir * otherScaledRadius;

    return true;
}

// CheckBoxCollision()
// this circle�� other box�� �浹 üũ
bool CircleCollider::CheckBoxCollision(BoxCollider* other, ContactInfo& contact)
{
    Vector2 circleCenter = GetCenter();
    float scaledRadius = radius * transform->GetWorldScale().x;

    Vector2 boxCenter = other->GetCenter();
    Vector2 boxHalfSize = other->size * other->transform->GetWorldScale() * 0.5f;

    // AABB ���ο��� ���� ����� �� ã��
    float closestX = clamp(circleCenter.x, boxCenter.x - boxHalfSize.x, boxCenter.x + boxHalfSize.x);
    float closestY = clamp(circleCenter.y, boxCenter.y - boxHalfSize.y, boxCenter.y + boxHalfSize.y);
    Vector2 closestPoint(closestX, closestY);

    // �� �߽ɰ� ���� ����� �� ���� �Ÿ�
    Vector2 diff = circleCenter - closestPoint;
    float distSq = diff.SqrMagnitude();

    if (distSq > scaledRadius * scaledRadius)
        return false;

    // Contact Info
    contact.point = closestPoint;

    if (distSq == 0.0f)
    {
        contact.normal = Vector2(0, 1); // ���� ó��
        contact.depth = scaledRadius;
    }
    else
    {
        float distance = sqrtf(distSq);
        contact.normal = diff.Normalized();
        contact.depth = scaledRadius - distance;
    }

    // platform (one-way) handling - this circle is always the mover here,
    // other (the box) is always the platform; see
    // BoxCollider::CheckAABBCollision for why this needs the mover's
    // pre-move position rather than current centers.
    if (other->isFlatform)
    {
        Rigidbody* moverRb = gameObject->GetComponent<Rigidbody>();
        float moverPrevY = moverRb ? moverRb->previousPosition.y : circleCenter.y;

        if (moverPrevY - scaledRadius < other->maxY)
            return false;

        // flatformDepthThreshold
        if (contact.depth > other->flatformDepthThreshold)
            return false;
    }

    return true;
}

// Raycast()
// ������ this collider�� �浹 ��� ��ȯ
bool CircleCollider::Raycast(const Ray& ray, float maxDistance, RaycastHit& hitInfo)
{
    Vector2 center = GetCenter();
    Vector2 oc = ray.origin - center;

    // radius must be scaled by the transform's world scale, same as
    // CheckCircleCollision/CheckBoxCollision/UpdateBounds do - otherwise the
    // raycast hit-sphere doesn't match the collider's actual (scaled) size.
    float scaledRadius = radius * transform->GetWorldScale().x;

    float a = ray.direction.Dot(ray.direction);         // == ray.direction.SqrMagnitude()
    float b = 2.0f * oc.Dot(ray.direction);
    float c = oc.Dot(oc) - scaledRadius * scaledRadius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
        return false;

    float sqrtD = std::sqrt(discriminant);
    float t1 = (-b - sqrtD) / (2.0f * a);
    float t2 = (-b + sqrtD) / (2.0f * a);

    // t1�� �� ª�� �Ÿ�. �̰� 0 �̻��̰� maxDistance���� �۾ƾ� �浹 ����
    float t = (t1 >= 0) ? t1 : t2;
    if (t < 0 || t > maxDistance)
        return false;

    // �浹 ���� ���
    hitInfo.distance = t;
    hitInfo.point = ray.origin + ray.direction * t;
    hitInfo.collider = this;

    return true;
}

void CircleCollider::OnCollisionEnter(ICollider* other, ContactInfo& contact)
{
    Rigidbody* rb = gameObject->GetComponent<Rigidbody>();
    if (rb)
    {
        // �浹 ����
        // position correction: if the other side also has a non-kinematic
        // Rigidbody, both sides independently correct, so only apply half
        // the depth each - otherwise the pair separates by 2x the overlap.
        Rigidbody* otherRb = other->gameObject->GetComponent<Rigidbody>();
        float correctionFactor = (otherRb && !otherRb->isKinematic) ? 0.5f : 1.0f;
        rb->CorrectPosition(contact, correctionFactor);

        // ground
        if (contact.normal.y > 0)
        {
            rb->groundContactCount++;
            rb->isGrounded = true;
        }

        // script
        auto scripts = gameObject->GetComponents<Script>();
        for (auto s : scripts)
            s->OnCollisionEnter(other, contact);
    }
}

void CircleCollider::OnCollisionStay(ICollider* other, ContactInfo& contact)
{
    Rigidbody* rb = gameObject->GetComponent<Rigidbody>();
    if (rb)
    {
        // ground
        if (contact.normal.y > 0)
        {
            rb->isGrounded = true;
        }

        // script
        auto scripts = gameObject->GetComponents<Script>();
        for (auto s : scripts)
            s->OnCollisionStay(other, contact);
    }
}

void CircleCollider::OnCollisionExit(ICollider* other, ContactInfo& contact)
{
    // isGrounded
    Rigidbody* rb = gameObject->GetComponent<Rigidbody>();
    if (rb)
    {
        // ground
        if (contact.normal.y > 0)
        {
            rb->groundContactCount--;

            if (rb->groundContactCount <= 0)
            {
                rb->groundContactCount = 0;
                rb->isGrounded = false;
            }
        }

        // block free
        if (contact.normal.x > 0)      rb->isBlockedLeft = false;
        else if (contact.normal.x < 0) rb->isBlockedRight = false;
        if (contact.normal.y > 0)      rb->isBlockedDown = false;
        else if (contact.normal.y < 0) rb->isBlockedUp = false;

        // script
        auto scripts = gameObject->GetComponents<Script>();
        for (auto s : scripts)
            s->OnCollisionExit(other, contact);
    }
}

void CircleCollider::OnTriggerEnter(ICollider* other, ContactInfo& contact)
{
    // script
    auto scripts = gameObject->GetComponents<Script>();
    for (auto s : scripts)
        s->OnTriggerEnter(other, contact);
}

void CircleCollider::OnTriggerStay(ICollider* other, ContactInfo& contact)
{
    // script
    auto scripts = gameObject->GetComponents<Script>();
    for (auto s : scripts)
        s->OnTriggerStay(other, contact);
}

void CircleCollider::OnTriggerExit(ICollider* other, ContactInfo& contact)
{
    // script
    auto scripts = gameObject->GetComponents<Script>();
    for (auto s : scripts)
        s->OnTriggerExit(other, contact);
}

Vector2 CircleCollider::GetCenter() const
{
    // transform ��ġ + offset + size�� ���� (�߽�)
    Vector2 scale = transform->GetScale();
    Vector2 worldPos = transform->GetWorldPosition();
    return worldPos + Vector2(offset.x * scale.x, offset.y * scale.y);
}

void CircleCollider::DebugColliderDraw()
{
    // circle
    Vector2 localPos = Vector2(offset.x, -offset.y);
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        D2D1::Point2F(localPos.x, localPos.y),
        radius,
        radius
    );

    RenderSystem::Get().DebugDrawCircle(ellipse, transform->GetScreenMatrix(), 2.0f);

    // center
    Vector2 localPos2 = Vector2(offset.x, -offset.y);
    D2D1_ELLIPSE ellipse2 = D2D1::Ellipse(
        D2D1::Point2F(localPos.x, localPos.y),
        2,
        2
    );
    RenderSystem::Get().DebugDrawCircle(ellipse2, transform->GetScreenMatrix());
}