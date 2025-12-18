
// Read Me!
// 양우정 게임 엔진 소개
// 자세한 설명은 선언해둔 클래스 [F12] 타고 들어가서 봐주세요.


/*---------------------------------------------------------------------------------*/
/*--------------- 게임 콘텐츠 작성시 알아야 할 Cycle, Utility -----------------------*/
/*--------------------------------------------------------------------------------*/

class GameObject;
// 게임 콘텐츠 프로젝트에서는 GameObject를 상속받은 다양한 GameObject를 조립하고
// 씬에서는 해당 GameObject를 Create만 합니다.
// 유효성 검사는  ObjectTable::Get().IsValid(object)를 통해 이루어집니다.
{
	/* [GameObject Cycle] */		   // ! 얘는 안써도 됨. 간단한 게임오브젝트 같은 경우에 Script 컴포넌트를 만들기 귀찮기 때문에 냅둔 사이클
	virtual void Awake() {};           // 오브젝트가 생성될 때, 생성자 이후
	virtual void SceneStart() {};      // Scene의 Start -> Update중 SceneStart() 호출 보장 x
	virtual void Update() {};          // Scene의 Update
	virtual void Destroyed() {};       // Scene의 Exit, GameObject Delete


	/* [Utility] */
	// 1. name, tag
	// 생성자로 전달 가능하며 puplic 멤버변수라 바로 지정 가능함
	// 
	// 2. Find
	// name, tag로 게임오브젝트를 찾아 포인터를 받을 수 있음		
	// 
	// 3. Destory
	// - player.Destory()로 this 게임오브젝트 삭제 가능
	// - GameObject::Destroy(포인터)로 게임 오브젝트 삭제 가능
	//
	// 4. SetActive
	// - bool값을 전달해 게임오브젝트를 활성화/ 비활성화 할 수 있음
	// - 자식 오브젝트까지 모두 활성화/비활성화 됨
}


class Component;
// 모든 컴포넌트는 이 Component를 상속받아 만들어졌으며, 
// 유효성 검사는  ObjectTable::Get().IsValid(object)를 통해 이루어집니다.
{
	/* [Component Cycle] */
	virtual void OnEnable_Inner() = 0;    // 컴포넌트 활성화시
	virtual void OnDisable_Inner() = 0;   // 컴포넌트 비활성화시
	virtual void OnDestroy_Inner() = 0;   // 컴포넌트 or 게임오브젝트 파괴시

	// SetEnabled
	// 컴포넌트 활성화/비활성화 여부를 설정 수 있음
}


class Script;
// Component를 상속받아 만들어진 사용자 커스텀용 컴포넌트로, 유니티의 모노비헤이어라고 생각하면 됩니다.
// 게임 콘텐츠에서는 Script를 상속받은 여러 스크립트 컴포넌트를 만들어 게임의 로직을 작성하면 됩니다.
// 유효성 검사는  ObjectTable::Get().IsValid(object)를 통해 이루어집니다.
{
	/* [script component cycle] */
	virtual void OnEnable() {}          // 컴포넌트 활성화시
	virtual void OnDisable() {}         // 컴포넌트 비활성화시
	virtual void Awake() {}             // Update() 전에 1회 호출
	virtual void Start() {}             // Awake() 이후 Update() 직전 시점 1회 호출
	virtual void Update() {}            // 프레임 단위 반복 호출
	virtual void FixedUpdate() {}       // 물리 업데이트 0.02f 보장 반복 호출
	virtual void LateUpdate() {}		// 카메라 등
	virtual void OnDestroy() {}         // 컴포넌트 or 오브젝트 소멸 시점

	/* [collision event] */
	// trigger
	virtual void OnTriggerEnter(ICollider* other, const ContactInfo& contact) {}
	virtual void OnTriggerStay(ICollider* other, const ContactInfo& contact) {}
	virtual void OnTriggerExit(ICollider* other, const ContactInfo& contact) {}

	// collision
	virtual void OnCollisionEnter(ICollider* other, const ContactInfo& contact) {}
	virtual void OnCollisionStay(ICollider* other, const ContactInfo& contact) {}
	virtual void OnCollisionExit(ICollider* other, const ContactInfo& contact) {}
}



/*---------------------------------------------------------------*/
/*--------------- Component & Component System  -----------------*/
/*--------------- 안에 들어가서 설명 읽어주세요  ------------------*/
/*--------------------------------------------------------------*/
class TransformSystem;		// trasnfrom system
class Transform;
class RectTransform;

class RenderSystem;			// render system
class SpriteRenderer;
class ImageRenderer;
class WorldTextRenderer;
class ScreenTextRenderer;

class CameraSystem;			// camera system
class Camera;
	
class ScriptSystem;			// script system
class Script;

class PhysicsSystem;		// physics system
class Rigidbody;

class ColliderSystem;		// collider system
class BoxCollider;
class CircleCollider;

class AnimatorSystem;		// animator system
class Animator;

class UISystem;				// ui system
class Button;
class Slider;

class AudioSystem;			// audio system
class AudoiSource;




/*---------------------------------------------------------------*/
/*---------------------- Static System --------------------------*/
/*--------------------------------------------------------------*/
class Input;
class Time;
class InvokeSystem;



/*---------------------------------------------------------------*/
/*-------------------  리소스 재활용 방식 ------------------------*/
/*--------------- 안에 들어가서 설명 읽어주세요  ------------------*/
/*--------------------------------------------------------------*/
class ResourceManager;



/*------------------------------------------------------------*/
/*------------------ 그래픽 객체 종류 ------------------------*/
/*----------------------------------------------------------*/
class Texture2D;
class Sprite;



/*--------------------------------------------------------------*/
/*---------------- 미리 정의해둔 GameObject --------------------*/
/*------------------------------------------------------------*/
class UI_Text;
class UI_Image;
class UI_Button;
class UI_Slider;




/*-------------------------------------------------------------------*/
/*---------------------- Animatio FSM 시스템 ------------------------*/
/*------------------------------------------------------------------*/
/* Animator -> AnimatorController -> AnimationBaseState -> AnimationClip */

class Animator;						// "Component" 연결된 animator controller update, SpriteRenderer의 sprite를 직접 udpate
class AnimatorController;			// "일반 Class" 등록된 state에 대해 FSM 시스템 담당 -> current sprite update
class AnimationBaseState;			// "일반 Class" 각 animation clip에 대한 state, Enter(), Update(), Eixt()에 transition 코드 작성 가능
class AnimationClip;				// "일반 Class" animation clip asset



/*-------------------------------------------------------------------*/
/*-------------------------    Physics   ---------------------------*/
/*------------------------------------------------------------------*/
class Rigidbody;

// Collision Detection Mode
// 1. Discrete(Defualt) : 프레임마다 단순 위치 체크 (빠르게 가면 뚫고갈 수 있음)
// 2. Continuous : SweepTest를 이용하여 이전 프레임과 현재 프레임의 이동경로상으로 step씩 콜라이더를 밀면서 충돌 검사


/*-------------------------------------------------------------------*/
/*-------------------------   Collision  ---------------------------*/
/*-----------------------------------------------------------------*/
/* ColliderSystem -> ICollider(Box, Circle) -> Script Event Func */

class ColliderSystem;
// 콜라이더들이 생성되면 ColliderSystem에 등록되고,
// 물리 udpate 주기마다 각 콜라이더들이 가지고있는 aabb에 따라 정렬하고, 
// sap 알고리즘을 활용하여 모든 콜라이더에 대해 충돌 체크를 진행한다.
// 만약 두 콜라이더가 충돌했다면 해당 콜라이더들의 현재 프레임에 충돌한 콜라이더 map 컨테이너에 충돌 정보를 추가한다.

class ICollider;
class BoxCollider;
class CircleCollider;
// ColliderSystem에서 충돌 체크하라고 상대 콜라이더를 던져주면, 해당 콜라이더의 Type을 판별하여 알맞는 충돌 체크를 진행한다.
// 이때 충돌한 경우 ContactInfo(충돌 지점, 충돌 법선벡터, 깊이)를 함께 계산한다.
// 충돌한 콜라이더를 저장하기 위한 unordered_map<ICollider*, ContactInfo>을 이전 프레임 map, 현재 프레임 map으로 저장하며
// 이번 프레임의 충돌 계산이 모두 끝나면 이를 비교하여 Enter, Stay, Exit를 호출한다.
// => 이때 이 콜라이더가 등록되어있는 게임오브젝트의 Script 컴포넌트의 이벤트함수까지 호출해준다.

// OnCollision시에는 rigidbody가 있을 경우에만 Script에 Collision 이벤트를 전달하며
// rigidbody에 contact 정보를 넘겨 충돌한만큼 transform을 보정하게 한다. (kinematic이 아닐 경우에만)
// 또 normal > 0인 경우의 Enter와 Exit를 체크하여 rigidbody의 isgrounded 플래그를 제어하여 
// rigidbody 내부에서 중력을 초기화할 수 있도록 한다.

class Script;
// Script 컴포넌트에 있는 충돌 이벤트 함수를 맘 편하게 오버라이드해서 쓰면 된다! 
// 위에서 알아서 호출해준다.


// [Collision vs Trigger]
// - Collistion 이벤트는 무조건 Rigidbody가 있어야만 Script로 전달됩니다.
// - Collistion 이벤트 발생시 Rigidbody가 있어야만 충돌 보정이 됩니다. (kinematic이 아닌 경우에)

// - 충돌한 두 콜라이더중 하나라도 isTrigger라면 양쪽 모두에 Trigger 이벤트가 전달됩니다. (충돌 보정 x)
// - Tirgger 이벤트는 Rigidbody와 관계 없이 무조건 Script로 전달됩니다.

// 1. collision vs collision 
//	  => Rigidbody가 붙어있는 경우에 CollisionEvent 호출, 충돌 보정

// 2. collision vs trigger 
//	  => TriggerEnvet 호출, 충돌 보정 x

// 3. trigger vs trigger 
//	  => TriggerEnvet 호출, 충돌 보정 x


// Q. 만약 kinematic이라면?

// 1. Rigidbody + collision     vs     Rigidbody + collision
//  -> Collision이벤트 + 충돌보정ㅇ      -> Collision이벤트 + 충돌보정x

// 2. Rigidbody(kinematic) + collision   vs    Rigidbody + collision
// -> Collision이벤트 + 충돌보정x               -> Collision이벤트 + 충돌보정ㅇ 

// 3. Rigidbody(kinematic) + collision   vs    Rigidbody(kinematic) + collision
// -> Collision이벤트 + 충돌보정x               -> Collision이벤트 + 충돌보정x 

// 즉 kinematic일 경우에는 collision 이벤트가 호출 되지만, 충돌 보정은 되지 않는다.



/*------------------------------------------------------------------*/
/*-------------------------   Ray Cast  ---------------------------*/
/*-----------------------------------------------------------------*/
struct Ray;
struct RaycastHit;
class ColliderSystem;

// 게임 콘텐츠에서 Ray를 생성하고(Ray::origin, Ray::direction)
// ColliderSystem::Raycast() 함수를 호출하여 Ray를 던지면,
// 충돌 정보가 담긴 RaycastHit 구조체를 반환합니다. (가장 가까운 콜라이더 1개)
// Raycast는 isTrigger false인 콜라이더에 대해서만 충돌 체크를 진행합니다.





/*-------------------------------------------------------------------*/
/*-------------------------   Render    ----------------------------*/
/*-----------------------------------------------------------------*/
class RenderSystem;
class IRenderer;
class SpriteRenderer;
class ImageRenderer;			// ImageRenderer는 추가로 Filltype 지정이 가능하다

// RenderType에 따라 GameObject를 먼저 그리고 UI를 그린다.
// RenderMode에 따라 다른 렌더 체인을 제공하며 기본은 Unlit이다.
// 이때 Text 관련 렌더러들은 RenderMode에 영향을 받지 않으며,
// 무조건 bitmap Image만 적용이 된다. (SpriteRenderer, ImageRenderer의 sprite 적용시)


// SpriteRenderer와 ImageRenderer에서 sprite의 이펙트 효과를 받고싶다면
// 아래의 렌더모드를 지정해주어야한다!

// 1. Unlit	(Default)
//    - Draw Bitmap
//    - 일반 Bitmap 이미지를 그린다.
//    - 색상 변경은 불가능하며 Alpha값만 조정 가능하다.

// 2. UnlitColorTint
//    - Draw Image (Crop + ColorEffect)
//    - bitmap 이미지에 ColorMatirx를 적용시켜 색상 변환을 한 뒤 그린다.
//    - R, G, B, A 조정이 가능하다. + 채도 조절 추가!

// 3. Lit_Glow
//    - Draw Image (Crop + BlurEffect)
//    - bitmap 이미지에 Blur효과를 준 이미지를 그리고, 이미지를 그려 후광 효과를 준다.
//    - ColorMatrix를 쓰는 비용을 줄이기 위해 만든거라 RGBA, 채도 모두 지원하지 않는다.
//      DrawIamge에서 A를 사용하려면 무조건 colormatrix 연산 해야함

// 4. Lit_ColorTint			
//    - Draw Image (Crop + ColorEffect + BlurEffect)
//    - bitmap 이미지에 Blur효과를 준 이미지를 그리고, 이미지를 그려 후광 효과를 준다.
//    - ColorMatrix를 통해 bitmap의 색상을 변경하면 그에 따라 광원도 조정된다.
//    - R, G, B, A 조정이 가능하다. + 채도 조절 추가!


/*-------------------------------------------------------------------*/
/*--------------------------   Sound    ----------------------------*/
/*-----------------------------------------------------------------*/
// Sound는 FMOD라이브러리를 활용해 구현하였습니다. 
// 엔진라이브러리 폴더의 Extern 폴더에 FMOD inc폴더와 lib가 모두 있어야 실행됩니다.
class AudioSystem;		// componet system
class AudioSource;		// component
class AudioClip;		// asset

// AudioSystem에서 FMOD 시스템을 생성하고 전체 오디오가 잘 재생되도록 시스템을 매 프레임 update해줍니다.
// AudioSystem에 기본적으로 master채널, bgm채널, sfx채널이 구현되어있습니다.
// AudioSource는 각각 하나의 출력 채널을 가지며, 하나의 채널에는 두개의 sound가 동시에 재생될 수 없습니다.
// AudioSystem의 bgm채널, sfx채널에 각 AudioSource채널을 등록하여 전체 볼륨을 믹싱할 수 있습니다.
// AudioCilp은 사운드 파일 리소스로 리소스 매니저를 통해 생성해야합니다. 
// 생성한 AudioClip을 AudioSource에 set하여 사운드를 플레이하면 됩니다!