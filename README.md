자세한 이해를 위해서는 Friend_Engine/Friend_Engine/Friend_2D_EngineLib/ReadMe.cpp 를 읽어주세요.


# 🎮 Game Engine Guide

게임 콘텐츠 제작 시 필요한 **엔진의 주요 구조와 사용 방법**을 정리한 문서입니다.

이 엔진은 `GameObject - Component - Script` 구조를 기반으로 하며,
게임 콘텐츠에서는 `GameObject`를 조립하고 `Script` 컴포넌트를 작성하여 게임 로직을 구현하는 방식으로 사용합니다.

---

# 1. GameObject

게임 콘텐츠에서 사용하는 모든 게임 오브젝트의 기반 클래스입니다.

일반적으로 `GameObject`를 상속받아 필요한 컴포넌트를 조립한 뒤,
`Scene`에서는 완성된 GameObject를 생성하여 사용합니다.

오브젝트의 유효성 검사는 다음과 같이 할 수 있습니다.

```cpp
ObjectTable::Get().IsValid(object);
```

## GameObject Life Cycle

간단한 GameObject의 경우 별도의 Script 컴포넌트를 만들지 않고 아래 사이클을 직접 사용할 수도 있습니다.

| 함수             | 호출 시점                       |
| -------------- | --------------------------- |
| `Awake()`      | GameObject 생성 시 생성자 호출 이후   |
| `SceneStart()` | Scene 시작 시                  |
| `Update()`     | Scene Update 시              |
| `Destroyed()`  | Scene 종료 또는 GameObject 삭제 시 |

> `SceneStart()`는 Scene의 Start → Update 과정에서 반드시 호출되는 것을 보장하지 않습니다.

대부분의 게임 로직은 GameObject의 Life Cycle보다 아래에서 설명하는 **Script 컴포넌트**를 사용하는 것을 권장합니다.

## GameObject Utility

### Name / Tag

GameObject는 `name`, `tag`를 사용할 수 있습니다.

생성자를 통해 전달하거나 public 멤버를 직접 지정할 수 있습니다.

### Find

`name` 또는 `tag`를 기준으로 GameObject를 검색하여 포인터를 얻을 수 있습니다.

### Destroy

현재 GameObject를 삭제할 수 있습니다.

```cpp
player.Destroy();
```

또는 GameObject 포인터를 전달하여 삭제할 수 있습니다.

```cpp
GameObject::Destroy(object);
```

### SetActive

GameObject의 활성화 상태를 설정합니다.

```cpp
object->SetActive(true);
object->SetActive(false);
```

GameObject를 비활성화하면 **자식 GameObject까지 함께 활성화/비활성화**됩니다.

---

# 2. Component

엔진에서 사용하는 모든 컴포넌트의 기반 클래스입니다.

`Transform`, `Renderer`, `Collider`, `Rigidbody`, `Animator`, `AudioSource` 등의 기능은 모두 Component 기반으로 구성되어 있습니다.

Component 역시 다음 방법으로 유효성을 검사할 수 있습니다.

```cpp
ObjectTable::Get().IsValid(component);
```

## Component Life Cycle

| 함수                  | 호출 시점                      |
| ------------------- | -------------------------- |
| `OnEnable_Inner()`  | Component 활성화              |
| `OnDisable_Inner()` | Component 비활성화             |
| `OnDestroy_Inner()` | Component 또는 GameObject 파괴 |

## SetEnabled

Component 단위로 활성화/비활성화할 수 있습니다.

GameObject 전체를 끄는 `SetActive()`와 달리 특정 Component만 제어할 때 사용합니다.

---

# 3. Script

`Script`는 `Component`를 상속받은 **사용자 게임 로직 작성용 컴포넌트**입니다.

Unity를 사용해봤다면 `MonoBehaviour`와 비슷한 역할이라고 생각하면 됩니다.

게임 콘텐츠에서는 보통 Script를 상속받아 필요한 로직을 구현합니다.

```cpp
class PlayerScript : public Script
{
public:
    void Awake() override
    {
        // 초기화
    }

    void Update() override
    {
        // 플레이어 로직
    }
};
```

## Script Life Cycle

| 함수              | 호출 시점                       |
| --------------- | --------------------------- |
| `OnEnable()`    | Component 활성화               |
| `OnDisable()`   | Component 비활성화              |
| `Awake()`       | Update 이전 1회                |
| `Start()`       | Awake 이후, 첫 Update 직전 1회    |
| `Update()`      | 매 프레임                       |
| `FixedUpdate()` | 물리 업데이트 주기마다 호출 (`0.02f`)   |
| `LateUpdate()`  | Update 이후 호출. 카메라 등의 처리에 사용 |
| `OnDestroy()`   | Component 또는 GameObject 소멸  |

일반적인 실행 흐름은 다음과 같습니다.

```text
Component 활성화
      │
      ▼
  OnEnable()
      │
      ▼
    Awake()
      │
      ▼
    Start()
      │
      ▼
┌───────────────┐
│    Update()   │
│ FixedUpdate() │
│ LateUpdate()  │
└───────────────┘
      │
      ▼
 OnDisable()
      │
      ▼
 OnDestroy()
```

## Collision Event

```cpp
void OnCollisionEnter(ICollider* other, const ContactInfo& contact) override;
void OnCollisionStay(ICollider* other, const ContactInfo& contact) override;
void OnCollisionExit(ICollider* other, const ContactInfo& contact) override;
```

## Trigger Event

```cpp
void OnTriggerEnter(ICollider* other, const ContactInfo& contact) override;
void OnTriggerStay(ICollider* other, const ContactInfo& contact) override;
void OnTriggerExit(ICollider* other, const ContactInfo& contact) override;
```

충돌 시스템에서 충돌 상태를 계산한 뒤 해당 GameObject에 등록된 Script까지 이벤트를 전달하므로, 콘텐츠 코드에서는 필요한 이벤트 함수만 override해서 사용하면 됩니다.

---

# 4. Component Systems

엔진의 주요 기능은 **Component + ComponentSystem** 구조로 관리됩니다.

## Transform

```text
TransformSystem
├─ Transform
└─ RectTransform
```

GameObject의 위치, 회전, 크기 및 UI Transform을 담당합니다.

## Render

```text
RenderSystem
├─ SpriteRenderer
├─ ImageRenderer
├─ WorldTextRenderer
└─ ScreenTextRenderer
```

게임 오브젝트 및 UI 렌더링을 담당합니다.

## Camera

```text
CameraSystem
└─ Camera
```

게임 화면의 Camera 처리를 담당합니다.

## Script

```text
ScriptSystem
└─ Script
```

등록된 Script들의 Life Cycle을 관리합니다.

## Physics

```text
PhysicsSystem
└─ Rigidbody
```

물리 업데이트와 Rigidbody 처리를 담당합니다.

## Collider

```text
ColliderSystem
├─ BoxCollider
└─ CircleCollider
```

Collider 등록, 충돌 검사 및 Script 충돌 이벤트 전달을 담당합니다.

## Animation

```text
AnimatorSystem
└─ Animator
```

Animation FSM 및 Sprite Animation을 관리합니다.

## UI

```text
UISystem
├─ Button
└─ Slider
```

UI 입력 및 상태 처리를 담당합니다.

## Audio

```text
AudioSystem
└─ AudioSource
```

FMOD 기반 오디오 재생 및 채널 관리를 담당합니다.

---

# 5. Static Systems

GameObject나 Component에 직접 붙이지 않고 전역적으로 사용하는 시스템입니다.

```cpp
Input
Time
InvokeSystem
```

### Input

키보드/마우스 등의 입력을 처리합니다.

### Time

프레임 및 시간 관련 정보를 제공합니다.

### InvokeSystem

지연 호출 등 시간 기반 함수 실행을 관리합니다.

---

# 6. Resource

리소스는 `ResourceManager`를 통해 관리합니다.

```cpp
class ResourceManager;
```

Texture, Sprite, AudioClip 등의 Asset을 직접 중복 생성하기보다 ResourceManager를 통해 생성 및 재사용하는 구조입니다.

주요 그래픽 리소스는 다음과 같습니다.

```cpp
Texture2D
Sprite
```

---

# 7. Predefined GameObjects

UI 제작 편의를 위해 미리 정의된 GameObject가 제공됩니다.

```cpp
UI_Text
UI_Image
UI_Button
UI_Slider
```

필요한 UI 요소를 직접 처음부터 GameObject + Component로 조립하지 않고 해당 클래스를 활용할 수 있습니다.

---

# 8. Animation FSM

Animation은 다음 구조로 동작합니다.

```text
Animator
   │
   ▼
AnimatorController
   │
   ▼
AnimationBaseState
   │
   ▼
AnimationClip
```

### Animator

GameObject에 부착되는 **Component**입니다.

연결된 `AnimatorController`를 업데이트하며 최종적으로 `SpriteRenderer`의 Sprite를 변경합니다.

### AnimatorController

일반 클래스이며 등록된 Animation State들을 기반으로 **FSM을 관리**합니다.

현재 State를 업데이트하고 Animation 전환을 담당합니다.

### AnimationBaseState

각 AnimationClip에 대응되는 State입니다.

```cpp
Enter();
Update();
Exit();
```

등의 함수에서 State 진입/업데이트/종료 및 Transition 로직을 작성할 수 있습니다.

### AnimationClip

실제 Animation 데이터를 가지고 있는 Asset입니다.

---

# 9. Physics

물리 처리는 `Rigidbody`를 중심으로 이루어집니다.

## Collision Detection Mode

### Discrete (Default)

프레임마다 현재 위치를 기준으로 충돌을 검사합니다.

```text
Previous Position        Current Position
       ● --------------------> ●
```

속도가 너무 빠른 객체는 프레임 사이에 Collider를 통과하여 충돌을 놓칠 수 있습니다.

### Continuous

이전 프레임과 현재 프레임 사이의 이동 경로를 대상으로 Sweep Test를 수행합니다.

```text
Previous                  Current
   ● → ○ → ○ → ○ → ○ → ●
```

이동 경로를 따라 Collider를 Step 단위로 이동시키며 충돌을 검사하기 때문에 빠르게 움직이는 객체의 충돌 누락을 줄일 수 있습니다.

---

# 10. Collision System

전체 충돌 처리 흐름은 다음과 같습니다.

```text
ColliderSystem
      │
      ▼
ICollider
 ├─ BoxCollider
 └─ CircleCollider
      │
      ▼
Collision Detection
      │
      ▼
ContactInfo
      │
      ▼
Enter / Stay / Exit
      │
      ▼
Script Event
```

## ColliderSystem

Collider가 생성되면 `ColliderSystem`에 등록됩니다.

물리 업데이트마다 각 Collider의 AABB를 기준으로 정렬한 뒤 **SAP(Sweep And Prune)** 알고리즘을 활용해 충돌 후보를 검사합니다.

두 Collider가 충돌하면 해당 프레임의 충돌 Map에 충돌 정보가 저장됩니다.

## ICollider

ColliderSystem으로부터 상대 Collider를 전달받으면 Collider Type에 따라 적절한 충돌 검사를 수행합니다.

현재 지원되는 Collider는 다음과 같습니다.

```cpp
BoxCollider
CircleCollider
```

충돌이 발생하면 `ContactInfo`를 계산합니다.

```text
ContactInfo
├─ 충돌 지점
├─ 충돌 Normal Vector
└─ 충돌 깊이
```

Collider는 충돌 정보를 다음과 같이 관리합니다.

```cpp
unordered_map<ICollider*, ContactInfo>
```

이전 프레임의 Collision Map과 현재 프레임의 Collision Map을 비교하여

```text
새롭게 충돌      → Enter
계속 충돌 중     → Stay
충돌 종료        → Exit
```

이벤트를 결정하고, 최종적으로 GameObject에 등록된 Script의 이벤트 함수를 호출합니다.

---

# 11. Collision vs Trigger

Collision과 Trigger는 동작 방식이 다릅니다.

## Collision

Collision 이벤트가 Script에 전달되려면 **Rigidbody가 필요합니다.**

```text
Collider + Rigidbody
        │
        ▼
Collision Event
        │
        ▼
ContactInfo
        │
        ▼
Transform 충돌 보정
```

Rigidbody가 `Kinematic`이 아니라면 ContactInfo를 기반으로 Transform 위치를 보정합니다.

Ground Normal 조건을 만족하는 Enter/Exit가 발생하면 Rigidbody의 `isGrounded` 플래그도 갱신되며, Rigidbody 내부에서 중력 상태를 처리할 수 있도록 합니다.

## Trigger

충돌한 두 Collider 중 **하나라도 `isTrigger == true`이면 양쪽 모두 Trigger 이벤트**를 받습니다.

Trigger는 Rigidbody 존재 여부와 관계없이 Script에 전달됩니다.

충돌 보정은 하지 않습니다.

| 조합                    | 발생 이벤트          | 충돌 보정              |
| --------------------- | --------------- | ------------------ |
| Collision ↔ Collision | Collision Event | Rigidbody 조건에 따라 O |
| Collision ↔ Trigger   | Trigger Event   | X                  |
| Trigger ↔ Trigger     | Trigger Event   | X                  |

---

# 12. Kinematic Rigidbody

`Kinematic` Rigidbody도 Collision 이벤트 자체는 받을 수 있지만 **충돌 보정은 수행하지 않습니다.**

### Rigidbody ↔ Rigidbody

```text
A : Rigidbody + Collider
B : Rigidbody + Collider

A → Collision Event + 충돌 보정
B → Collision Event + 충돌 보정
```

### Kinematic ↔ Rigidbody

```text
A : Rigidbody(Kinematic) + Collider
B : Rigidbody + Collider

A → Collision Event + 충돌 보정 X
B → Collision Event + 충돌 보정 O
```

### Kinematic ↔ Kinematic

```text
A : Rigidbody(Kinematic) + Collider
B : Rigidbody(Kinematic) + Collider

A → Collision Event + 충돌 보정 X
B → Collision Event + 충돌 보정 X
```

즉,

> **Kinematic은 Collision Event는 발생하지만 Transform 충돌 보정 대상에서는 제외됩니다.**

---

# 13. Raycast

Raycast 관련 주요 타입은 다음과 같습니다.

```cpp
Ray
RaycastHit
ColliderSystem
```

게임 콘텐츠에서 Ray를 생성합니다.

```cpp
Ray ray;
ray.origin = ...;
ray.direction = ...;
```

이후 `ColliderSystem::Raycast()`를 호출하여 충돌 검사를 수행합니다.

```text
Ray
 │
 ├─ origin
 └─ direction
 │
 ▼
ColliderSystem::Raycast()
 │
 ▼
RaycastHit
```

Ray와 충돌한 Collider 중 **가장 가까운 Collider 1개**의 정보를 `RaycastHit`으로 반환합니다.

> Raycast는 `isTrigger == false`인 Collider만 검사합니다.

---

# 14. Render System

렌더링 구조는 다음과 같습니다.

```text
RenderSystem
      │
      ▼
IRenderer
 ├─ SpriteRenderer
 └─ ImageRenderer
```

`ImageRenderer`는 추가로 FillType을 지정할 수 있습니다.

RenderType에 따라 **GameObject를 먼저 렌더링한 뒤 UI를 렌더링**합니다.

RenderMode에 따라 서로 다른 렌더 체인을 사용하며 기본값은 `Unlit`입니다.

Text Renderer는 RenderMode의 영향을 받지 않습니다.

SpriteRenderer / ImageRenderer에 적용되는 이미지는 Bitmap Image 기반으로 처리됩니다.

## RenderMode

### 1. Unlit (Default)

```text
Draw Bitmap
```

일반 Bitmap 이미지를 렌더링합니다.

* RGB 색상 변경 불가능
* Alpha 조절 가능

### 2. UnlitColorTint

```text
Draw Image
  ├─ Crop
  └─ ColorEffect
```

Bitmap에 ColorMatrix를 적용하여 색상을 변경합니다.

지원 기능:

* R
* G
* B
* A
* Saturation

### 3. Lit_Glow

```text
Draw Image
  ├─ Crop
  └─ BlurEffect
```

Blur가 적용된 이미지를 먼저 렌더링하고 원본 이미지를 추가로 그려 Glow 효과를 만듭니다.

ColorMatrix 연산 비용을 줄이기 위한 RenderMode이므로 다음 기능은 지원하지 않습니다.

* RGBA Color Tint
* Saturation

### 4. Lit_ColorTint

```text
Draw Image
  ├─ Crop
  ├─ ColorEffect
  └─ BlurEffect
```

ColorMatrix와 Blur를 모두 적용합니다.

Bitmap 색상이 변경되면 Glow 색상 역시 함께 변경됩니다.

지원 기능:

* R
* G
* B
* A
* Saturation
* Glow

---

# 15. Sound System

Sound System은 **FMOD Library**를 기반으로 구현되어 있습니다.

실행을 위해 엔진 라이브러리의 `Extern` 폴더에 FMOD의 include/lib 파일이 필요합니다.

구조는 다음과 같습니다.

```text
AudioSystem
    │
    ├─ Master Channel
    ├─ BGM Channel
    └─ SFX Channel
         │
         ▼
    AudioSource
         │
         ▼
     AudioClip
```

## AudioSystem

FMOD System을 생성하고 매 프레임 Update하여 전체 오디오 재생을 관리합니다.

기본적으로 다음 채널 그룹이 구현되어 있습니다.

```text
Master
BGM
SFX
```

각 AudioSource의 Channel을 BGM/SFX Channel에 등록하여 전체 볼륨을 믹싱할 수 있습니다.

## AudioSource

GameObject에 부착되는 Component입니다.

각 AudioSource는 하나의 출력 Channel을 사용합니다.

> 하나의 Channel에서는 두 개의 Sound를 동시에 재생할 수 없습니다.

## AudioClip

사운드 파일을 나타내는 Asset입니다.

AudioClip은 `ResourceManager`를 통해 생성해야 합니다.

기본적인 사용 흐름은 다음과 같습니다.

```text
ResourceManager
      │
      ▼
 AudioClip 생성
      │
      ▼
AudioSource에 설정
      │
      ▼
     Play
```

---

# 전체 엔진 구조 요약

```text
Scene
 │
 └─ GameObject
      │
      ├─ Transform
      │
      ├─ Renderer
      │    ├─ SpriteRenderer
      │    └─ ImageRenderer
      │
      ├─ Script
      │
      ├─ Rigidbody
      │
      ├─ Collider
      │    ├─ BoxCollider
      │    └─ CircleCollider
      │
      ├─ Animator
      │
      └─ AudioSource
```

각 Component는 대응되는 System에 의해 관리됩니다.

```text
Transform     → TransformSystem
Renderer      → RenderSystem
Camera        → CameraSystem
Script        → ScriptSystem
Rigidbody     → PhysicsSystem
Collider      → ColliderSystem
Animator      → AnimatorSystem
UI            → UISystem
AudioSource   → AudioSystem
```

게임 콘텐츠를 작성할 때는 크게 다음 구조를 기억하면 됩니다.

```text
GameObject를 생성
      ↓
필요한 Component 조립
      ↓
Script를 상속하여 게임 로직 작성
      ↓
각 Component System이 자동으로 Update
      ↓
Render / Physics / Collision / Animation / Audio 처리
```

