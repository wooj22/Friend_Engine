#include "BoxCollider.h"
#include "GameObject.h"
#include "Transform.h"
#include "Script.h"
#include "RenderSystem.h"
#include "CircleCollider.h"
#include "Rigidbody.h"
#include <string>

void BoxCollider::OnEnable_Inner()
{
    ColliderSystem::Get().Regist(this);
	transform = this->gameObject->transform;
}

void BoxCollider::OnDisable_Inner()
{
	ColliderSystem::Get().Unregist(this);
}

void BoxCollider::OnDestroy_Inner()
{
    ColliderSystem::Get().Unregist(this);
}

// UpdateBounds()
// box collider�� aabb bound�� update�Ѵ�
void BoxCollider::UpdateBounds()
{
    Vector2 center = GetCenter();
    Vector2 scale = transform->GetWorldScale();
    Vector2 halfSize = Vector2(size.x * 0.5f * scale.x, size.y * 0.5f * scale.y);

    minX = center.x - halfSize.x;
    maxX = center.x + halfSize.x;
    minY = center.y - halfSize.y;
    maxY = center.y + halfSize.y;
}

// isCollision()
// �Ѱܹ��� �ݶ��̴��� Ÿ�Կ� ���� �浹 ����� return�Ѵ�
// �浹�� ��� contact info�� ����Ѵ�
bool BoxCollider::isCollision(ICollider* other, ContactInfo& contact)
{
    if (!transform) return false;

    if (other->colliderType == ColliderType::Box)
    {
        return CheckAABBCollision(static_cast<BoxCollider*>(other), contact);
    }
    else if (other->colliderType == ColliderType::Circle)
    {
        return CheckCircleCollision(static_cast<CircleCollider*>(other), contact);
    }

    return false;
}

// FinalizeCollision()
// ���� ������ �浹 ������ ���� ������ �浹 ������ ���Ͽ�
// isTrigger ������ ���� Enter, Stay, Exit �浹 �̺�Ʈ �Լ��� ȣ���Ѵ�.
void BoxCollider::FinalizeCollision()
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

// CheckAABBCollision()
// this box�� other box�� aabb �浹 ��� ��ȯ
bool BoxCollider::CheckAABBCollision(BoxCollider* other, ContactInfo& contact)
{
    // 1. AABB
    if (maxX < other->minX || minX > other->maxX || maxY < other->minY || minY > other->maxY)
        return false;

    // 2. �浹 ����: �� AABB �߽� �߰���
    Vector2 thisCenter = GetCenter();
    Vector2 otherCenter = other->GetCenter();
    contact.point = (thisCenter + otherCenter) * 0.5f;

    // 3. �ະ ħ�� ���� ���
    float overlapX = min(maxX, other->maxX) -max(minX, other->minX);
    float overlapY = min(maxY, other->maxY) - max(minY, other->minY);

    // 4. ħ���� ���� ���� ���� ���� ����
    if (overlapX < overlapY)
    {
        // X�� ���� ����
        contact.normal = (thisCenter.x < otherCenter.x) ? Vector2(-1, 0) : Vector2(1, 0);
        contact.depth = overlapX;
    }
    else
    {
        // Y�� ���� ����
        contact.normal = (thisCenter.y > otherCenter.y) ? Vector2(0, 1) : Vector2(0, -1);
        contact.depth = overlapY;
    }

    // platform (one-way) handling
    if (isFlatform || other->isFlatform)
    {
        BoxCollider* flatform = isFlatform ? this : other;
        BoxCollider* mover = (flatform == this) ? other : this;

        // Comparing CURRENT centers can't tell "landing from above" apart from
        // "passing through from below" once the two are already overlapping -
        // by definition of overlap, the mover's bottom is below the platform's
        // top either way, so the old center check flipped mid-overlap (right
        // as the mover's center crossed the platform's) and snapped it to
        // grounded while still jumping up through the platform. Use the
        // mover's bottom edge from BEFORE this frame's movement (its
        // Rigidbody's previousPosition) against the platform's current top
        // edge instead, anchoring the decision to the platform's real surface.
        Rigidbody* moverRb = mover->gameObject->GetComponent<Rigidbody>();
        float moverHalfHeight = mover->size.y * 0.5f * mover->transform->GetWorldScale().y;
        float moverPrevY = moverRb ? moverRb->previousPosition.y : mover->GetCenter().y;

        if (moverPrevY - moverHalfHeight < flatform->maxY)
            return false;

        // flatformDepthThreshold
        if (overlapY > flatformDepthThreshold)
            return false;
    }

    return true;
}

// CheckCircleCollision()
// this box�� other circle�� �浹 ��� ��ȯ
bool BoxCollider::CheckCircleCollision(CircleCollider* other, ContactInfo& contact)
{
    Vector2 boxCenter = GetCenter();
    Vector2 boxSize = transform->GetWorldScale() * size * 0.5f;

    Vector2 circleCenter = other->GetCenter();
    float circleRadius = other->radius * other->transform->GetWorldScale().x;

    // AABB ���ο��� ���� ����� �� ã��
    float closestX = clamp(circleCenter.x, boxCenter.x - boxSize.x, boxCenter.x + boxSize.x);
    float closestY = clamp(circleCenter.y, boxCenter.y - boxSize.y, boxCenter.y + boxSize.y);
    Vector2 closestPoint(closestX, closestY);

    // �� �߽ɰ� ���� ����� �� ���� �Ÿ�
    Vector2 diff = circleCenter - closestPoint;
    float distSq = diff.SqrMagnitude();

    if (distSq > circleRadius * circleRadius)
        return false;

    // Contact Info
    contact.point = closestPoint;

    if (distSq == 0.0f)
    {
        contact.normal = Vector2(0, 1); // ���� ó�� : ���� ������ ����
        contact.depth = circleRadius;
    }
    else
    {
        float distance = sqrtf(distSq);
        contact.normal = -diff.Normalized();
        contact.depth = circleRadius - distance;
    }

    // platform (one-way) handling - this box is always the platform here,
    // other (the circle) is always the mover; see CheckAABBCollision for why
    // this needs the mover's pre-move position rather than current centers.
    if (isFlatform)
    {
        Rigidbody* moverRb = other->gameObject->GetComponent<Rigidbody>();
        float moverPrevY = moverRb ? moverRb->previousPosition.y : circleCenter.y;

        if (moverPrevY - circleRadius < maxY)
            return false;

        // flatformDepthThreshold
        if (contact.depth > flatformDepthThreshold)
            return false;
    }

    return true;
}

// Raycast()
// ������ this collider�� �浹 ��� ��ȯ
bool BoxCollider::Raycast(const Ray& ray, float maxDistance, RaycastHit& hitInfo)
{
    // ���� ������ 0�� �� �����Ƿ� ���� ó��
    float dirX = ray.direction.x;
    float dirY = ray.direction.y;

    float tMinX, tMaxX;
    if (dirX != 0.0f)
    {
        tMinX = (minX - ray.origin.x) / dirX;
        tMaxX = (maxX - ray.origin.x) / dirX;
        if (tMinX > tMaxX) std::swap(tMinX, tMaxX);
    }
    else
    {
        // ���̰� X������ ������ ���, �ٿ�带 ����� �浹 ����
        if (ray.origin.x < minX || ray.origin.x > maxX)
            return false;
        tMinX = -INFINITY;
        tMaxX = INFINITY;
    }

    float tMinY, tMaxY;
    if (dirY != 0.0f)
    {
        tMinY = (minY - ray.origin.y) / dirY;
        tMaxY = (maxY - ray.origin.y) / dirY;
        if (tMinY > tMaxY) std::swap(tMinY, tMaxY);
    }
    else
    {
        if (ray.origin.y < minY || ray.origin.y > maxY)
            return false;
        tMinY = -INFINITY;
        tMaxY = INFINITY;
    }

    float tEnter = max(tMinX, tMinY);
    float tExit = min(tMaxX, tMaxY);

    if (tExit < 0 || tEnter > tExit || tEnter > maxDistance)
        return false;

    // �浹 ���� ���
    hitInfo.distance = tEnter;
    hitInfo.point = ray.origin + ray.direction * tEnter;
    hitInfo.collider = this;

    return true;
}

void BoxCollider::OnCollisionEnter(ICollider* other, ContactInfo& contact)
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

void BoxCollider::OnCollisionStay(ICollider* other, ContactInfo& contact)
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

void BoxCollider::OnCollisionExit(ICollider* other, ContactInfo& contact)
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

void BoxCollider::OnTriggerEnter(ICollider* other, ContactInfo& contact)
{
    // script
    auto scripts = gameObject->GetComponents<Script>();
    for (auto s : scripts)
        s->OnTriggerEnter(other, contact);
}

void BoxCollider::OnTriggerStay(ICollider* other, ContactInfo& contact)
{
    // script
    auto scripts = gameObject->GetComponents<Script>();
    for (auto s : scripts)
        s->OnTriggerStay(other, contact);
}

void BoxCollider::OnTriggerExit(ICollider* other, ContactInfo& contact)
{
    // script
    auto scripts = gameObject->GetComponents<Script>();
    for (auto s : scripts)
        s->OnTriggerExit(other, contact);
}

Vector2 BoxCollider::GetCenter() const
{
    // transform ��ġ + offset + size�� ���� (�߽�)
    Vector2 scale = transform->GetWorldScale();
    Vector2 worldPos = transform->GetWorldPosition();
    return worldPos + Vector2(offset.x * scale.x, offset.y * scale.y);
}

void BoxCollider::DebugColliderDraw()
{
    // rect
    float left = -size.x * 0.5f + offset.x;
    float right = size.x * 0.5f + offset.x;
    float top = -size.y * 0.5f - offset.y;
    float bottom = size.y * 0.5f - offset.y;

    D2D1_RECT_F localRect = D2D1::RectF(left, top, right, bottom);
    RenderSystem::Get().DebugDrawRect(localRect, transform->GetScreenMatrix());

    // center
    D2D1_ELLIPSE ellipse2 = D2D1::Ellipse(
        D2D1::Point2F(offset.x, -offset.y),
        2,
        2
    );
    RenderSystem::Get().DebugDrawCircle(ellipse2, transform->GetScreenMatrix());
}