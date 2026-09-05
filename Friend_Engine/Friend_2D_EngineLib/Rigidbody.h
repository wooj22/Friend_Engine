#pragma once
#include "Component.h"
#include "PhysicsSystem.h"
#include "Vector2.h"
#include "BoxCollider.h"
#include "CircleCollider.h"

/* [Rigidbody Component]
* ���� �Ի��� ����ϴ� ������Ʈ��, Transform�� �����Ͽ� �̵���Ų��.
* �ӵ�, ���ӵ�, ����, �߷�, ���� ���� ���� ���� �Ӽ��� �����ϸ�
* PhysicsSystem�� ��ϵǾ� FixedUpdate()�ֱ�� ������ ó���Ѵ�.
*/

// Collision Detection
// Continuous�� ������ �̵��ϴ� ��ü�� �ͳθ��� ������ �� �ִ�.
enum class CollisionDetection
{
    Discrete, Continuous
};

class Transform;
class Rigidbody : public Component
{
private:
    Transform* transform = nullptr;
    Vector2 previousPosition = Vector2::zero;

private:
    // ground gravity controll
    bool isGrounded;
    int groundContactCount = 0;

    // collision block controll
    bool isBlockedLeft = false;
    bool isBlockedRight = false;
    bool isBlockedUp = false;
    bool isBlockedDown = false;

public:
    Vector2 velocity = Vector2::zero;           // �ӵ�
    Vector2 acceleration = Vector2::zero;       // ���ӵ�
    float mass = 1.0f;                          // ����
    bool useGravity = true;                     // �߷� ��� ����
    float gravityScale = 1.0f;                  // �߷� ������
    float drag = 0.0f;                          // ���� ����, ������ (Ŭ���� �ӵ� ����)
    bool isKinematic = false;                   // kinematic ������ ���� ����� �浹 ���� x
    CollisionDetection collisionDetection = CollisionDetection::Discrete;

private:
    Vector2 impulse = Vector2::zero;            // ������ (�������� ��)

public:
    // component cycle
    Rigidbody() = default;
    ~Rigidbody() override = default;

    void OnEnable_Inner() override final;
	void OnDisable_Inner() override final;
    void FixedUpdate();
    void OnDestroy_Inner() override final;

private:
    // colliison position correct
    // factor: 1.0 corrects the full overlap depth (default - matches a static/kinematic
    // opponent). When the OTHER collider also has a non-kinematic Rigidbody, both sides
    // independently call this, so pass 0.5 on each side - otherwise the pair separates
    // by 2x the actual overlap (Enter -> pops apart -> Exit -> drifts back -> Enter...).
    void CorrectPosition(const ContactInfo& contact, float factor = 1.0f);

    // CCD (turnneling continuous collision detection)
    void CollisionContinuousDetection();

public:
    // func
    void AddForce(const Vector2& force);
    void AddImpulse(const Vector2& impulse);

    // friend
    friend class BoxCollider;
    friend class CircleCollider;
};


