#include <Conqueror/Conqueror.h>
#include "Core/Time/Timer.h"
#include "Core/Time/Clock.h"
#include "Core/Tiering/QualitySettings.h"
#include "Core/Tiering/PlatformDetection.h"
#include "Core/Reflection/TypeInfo.h"

class PlayerController : public Conqueror::ScriptableEntity
{
public:
    float Speed = 5.0f;
    float JumpForce = 5.0f;
    
private:
    bool m_WasSpacePressed = false;
    Conqueror::Timer m_TestTimer;
    Conqueror::Clock m_TestClock;

public:
    void OnCreate() override
    {
        CQ_CORE_INFO("PlayerController::OnCreate - Script başlatıldı!");
        
        if (!HasComponent<Conqueror::RigidbodyComponent>())
        {
            CQ_CORE_ERROR("PlayerController: Entity'de RigidbodyComponent yok!");
        }
        else
        {
            CQ_CORE_INFO("PlayerController: RigidbodyComponent bulundu");
        }

        // Time System Test
        CQ_CORE_INFO("=== Time System Test ===");
        m_TestTimer = Conqueror::Timer(3.0f, false);
        m_TestTimer.SetCallback([]() {
            CQ_CORE_INFO("Timer finished! 3 seconds elapsed.");
        });
        m_TestTimer.Start();
        
        m_TestClock.Start();
        CQ_CORE_INFO("Clock started");
        CQ_CORE_INFO("Current time: {0}", Conqueror::Clock::GetCurrentTime());
        CQ_CORE_INFO("Current date: {0}", Conqueror::Clock::GetCurrentDate());
        CQ_CORE_INFO("Timestamp: {0}", Conqueror::Clock::GetTimestamp());

        // Tiering System Test
        CQ_CORE_INFO("=== Tiering System Test ===");
        Conqueror::PlatformDetection::Init();
        auto& sysInfo = Conqueror::PlatformDetection::GetSystemInfo();
        CQ_CORE_INFO("OS: {0}", sysInfo.OS);
        CQ_CORE_INFO("CPU: {0} ({1} cores)", sysInfo.CPUName, sysInfo.CPUCores);
        CQ_CORE_INFO("RAM: {0} MB", sysInfo.TotalRAM);
        
        Conqueror::QualitySettings::Init();
        auto recommended = Conqueror::PlatformDetection::RecommendQualityLevel();
        CQ_CORE_INFO("Recommended quality: {0}", (int)recommended);
        
        Conqueror::QualitySettings::SetQualityLevel(Conqueror::QualityLevel::High);
        auto& preset = Conqueror::QualitySettings::GetPreset();
        CQ_CORE_INFO("Shadow resolution: {0}", preset.ShadowResolution);
        CQ_CORE_INFO("SSAO enabled: {0}", preset.EnableSSAO);

        // Reflection System Test
        CQ_CORE_INFO("=== Reflection System Test ===");
        auto& registry = Conqueror::Reflection::TypeRegistry::Get();
        auto allTypes = registry.GetAllTypes();
        CQ_CORE_INFO("Registered types: {0}", allTypes.size());
        for (const auto* type : allTypes)
        {
            CQ_CORE_INFO("  Type: {0} ({1} properties)", type->Name, type->Properties.size());
        }
    }

    void OnUpdate(float deltaTime) override
    {
        // Mevcut velocity'yi al
        glm::vec3 velocity = Conqueror::Physics::GetLinearVelocity(GetEntity());
        
        // WASD hareket - Velocity bazlı
        glm::vec3 moveDir(0.0f);
        
        if (Conqueror::Input::IsKeyPressed(Conqueror::Key::W))
            moveDir.z -= 1.0f;
        
        if (Conqueror::Input::IsKeyPressed(Conqueror::Key::S))
            moveDir.z += 1.0f;
        
        if (Conqueror::Input::IsKeyPressed(Conqueror::Key::A))
            moveDir.x -= 1.0f;
        
        if (Conqueror::Input::IsKeyPressed(Conqueror::Key::D))
            moveDir.x += 1.0f;
        
        // Hareket varsa velocity'yi set et
        if (glm::length(moveDir) > 0.0f)
        {
            moveDir = glm::normalize(moveDir);
            velocity.x = moveDir.x * Speed;
            velocity.z = moveDir.z * Speed;
        }
        else
        {
            // Hareket yoksa yatay velocity'yi sıfırla (sürtünme)
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
        
        // Zıplama - Space tuşu (sadece bir kez, basılı tutunca tekrar zıplama)
        bool isSpacePressed = Conqueror::Input::IsKeyPressed(Conqueror::Key::Space);
        
        // Zemin kontrolü: Y velocity küçükse (düşmüyorsa) zemindedir
        bool isGrounded = glm::abs(velocity.y) < 0.1f;
        
        // Space'e yeni basıldıysa VE zemindeyse zıpla
        if (isSpacePressed && !m_WasSpacePressed && isGrounded)
        {
            velocity.y = JumpForce;
        }
        
        m_WasSpacePressed = isSpacePressed;
        
        // Velocity'yi set et
        Conqueror::Physics::SetLinearVelocity(GetEntity(), velocity);
    }

    void OnDestroy() override
    {
        CQ_CORE_INFO("PlayerController::OnDestroy - Script yok edildi");
    }
};

// Factory fonksiyonu - DLL/SO'dan çağrılacak
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

extern "C" EXPORT Conqueror::ScriptableEntity* CreatePlayerController()
{
    return new PlayerController();
}
