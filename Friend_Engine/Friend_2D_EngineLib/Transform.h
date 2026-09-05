#pragma once
#include <d2d1_1.h>
#include <vector>
#include "Vector2.h"
#include "ITransform.h"
#include "TransformSystem.h"

/* [Transform Component]
* D2D1::MATRIX_3X2_F : 2D 변환(이동, 회전, 스케일, 기울기)을 표현하는 3*2 행렬
* 위 행렬을 이용하여 오브젝트의 변환을 담당하는 컴포넌트이다.
* 컴포넌트 생성시 Transform System에 등록되어 dirty패턴에 따라 matrix를 계속 계산한다.

// position 계산 방법
* World = local * parent->world
* Screen = world * camera 역행렬

// Uniny Style Screen position
* 좌표계 - 중앙 (0,0), 왼쪽위(-, +), 오른쪽 아래(+, -)
* D2D에서 유니티 좌표계처럼 사용하려면 꼼수가 필요함
* Screen = renderMatrix * world * camera 역행렬 * unity matrix
* unity matrix는 중심 이동과 y 반전에 대항 행렬값을 가지고 있다.
* renderMatrix는 unity matrix에 대응하기 위해 y scale에 -1을 준 행렬
* transform 멤버에 저장하면 자식까지 누적 영향을 미치기 때문에 연산에만 사용해야한다.

// Dirty 패턴
* transform이 변경에 대한 flag를 활용하여
* 매 프레임마다 불필요하게 행렬을 다시 계산하지 않도록 하는 방식
*/

class Transform : public ITransform
{
private:
    // transform
    Vector2 position;
    float rotation;
    Vector2 scale;

public:
    Vector2 prePosition;    // 이전 프레임 position

private:
    // parent, children
    Transform* parent = nullptr;
    std::vector<Transform*> children;
    
    // this matrix
    D2D1::Matrix3x2F localMatrix;
    D2D1::Matrix3x2F worldMatrix;
    D2D1::Matrix3x2F screenMatrix;

    // dirty
    bool isLocalDirty = true;
    bool isWorldDirty = true;

public:
    // matrix
    static D2D1::Matrix3x2F unityMatrix;          // 고정, screen size 필요
    static D2D1::Matrix3x2F renderMatrix;         // 고정

public:
    // component cycle
    Transform() : position{ 0.0f, 0.0f }, rotation(0.0f), scale{ 1.0f, 1.0f }, parent(nullptr) 
    {
        localMatrix = D2D1::Matrix3x2F::Identity();
        
    }
    ~Transform() override = default;

    void OnEnable_Inner() override final;
	void OnDisable_Inner() override final;
    void Update() override final;
    void OnDestroy_Inner() override final;

public:
    // parent, children
    void SetParent(Transform* newParent);
	Transform* GetParent(Transform* newParent) { return parent; }
    const vector<Transform*>& GetChildrens() { return children; }
    void RemoveChild(Transform* child);
    void MarkWorldDirty();

    // set
    void SetPosition(const Vector2& position);
    void SetPosition(const float& x, const float& y);
    void SetRotation(const float& angle);
    void SetScale(const Vector2& scale);
    void SetScale(const float& scaleX, const float& scaleY);
    void ResetTransform();

    // add
    void Translate(const Vector2& translation);
    void Translate(const float& x, const float& y);
    void Rotate(const float& angle);
    void Scaleing(const Vector2& scale);
    void Scaleing(const float& scaleX, const float& scaleY);

    // get
    Vector2 GetPosition() const { return position; }
    float GetRotation() const { return rotation; }
    Vector2 GetScale() const { return scale; }

    Vector2 GetWorldPosition() const
    {
        if (parent)
        {
            // GetWorldMatrix() composes the full parent chain (rotation + scale +
            // translation), so a plain position sum here drifts from where the
            // object is actually rendered once any parent is rotated or scaled -
            // rotate/scale this local offset by the parent's world rotation/scale
            // before adding the parent's world position, mirroring that.
            Vector2 parentScale = parent->GetWorldScale();
            float parentRotation = parent->GetWorldRotation();

            D2D1::Matrix3x2F parentScaleRotation =
                D2D1::Matrix3x2F::Scale(parentScale.x, parentScale.y) *
                D2D1::Matrix3x2F::Rotation(parentRotation);
            D2D1_POINT_2F offset = parentScaleRotation.TransformPoint(D2D1::Point2F(position.x, position.y));

            return parent->GetWorldPosition() + Vector2(offset.x, offset.y);
        }
        return position;
    }

    float GetWorldRotation() const
    {
        if (parent)
            return parent->GetWorldRotation() + rotation;
        return rotation;
    }

    Vector2 GetWorldScale() const
    {
        if (parent)
        {
            Vector2 parentScale = parent->GetWorldScale();
            return Vector2(scale.x * parentScale.x, scale.y * parentScale.y);
        }
        return scale;
    }

private:
    //  matrix
    void MakeLocalMatrix();
    void MakeWorldMatrix();
    void MakeScreenMatrix();

public:
    //  matrix get
    const D2D1::Matrix3x2F& GetLocalMatrix() { return localMatrix; }
    const D2D1::Matrix3x2F& GetWorldMatrix(){ return worldMatrix; }
    const D2D1::Matrix3x2F& GetScreenMatrix() { return screenMatrix; }
};