#ifndef EX_HEADERS
#define EX_HEADERS
extern struct ImVec4 espv;
extern struct ImVec4 espi;
extern struct ImVec4 userColor;
extern struct ImVec4 nameColor;
extern struct ImVec4 distanceColor;
extern struct ImVec4 fovColor;
extern bool istelekill;
extern bool isfly;
inline void OtFovV1(float a, float b, float c, float d, float e, ImColor f, float g) {}
inline void drawcircleglow(ImDrawList *a, ImVec2 b, float c, ImColor d, int e, float f, int g) {}
inline void AddText(ImFont *font, float size, bool b1, bool b2, ImVec2 pos, ImColor col, const char *text)
{
    ImGui::GetBackgroundDrawList()->AddText(font, size, pos, col, text);
}
#endif

struct Vars_t
{
    bool Enable = {};
    bool Aimbot = {};
    float AimFov = 120.0f;
    int AimCheck = {};
    int AimType = {};
    int AimWhen = 0;
    bool isAimFov = {};
    int AimHitbox = 0;
    const char *aimHitboxes[3] = {"Head", "Neck", "Body"};
    const char *dir[4] = {"Always", "Fire", "Scope", "Fire + Scope"};
    float AimSpeed = 9999.0f;
    bool VisibleCheck = true;
    bool IgnoreKnocked = false;
    bool AimSilent = false;
    bool AimVectored = false;
    float AimVectoredSpeed = 1.0f;
} Vars;

namespace Save
{
    inline int AimFPS = (1000000 / 15);
    inline long AimDelay = 0;
}

class game_sdk_t
{
public:
    void init();
    int (*GetHp)(void *player);
    void *(*Curent_Match)();
    void *(*GetLocalPlayer)(void *Game);
    void *(*GetHeadPositions)(void *player);
    Vector3 (*get_position)(void *player);
    void *(*Component_GetTransform)(void *player);
    void *(*get_camera)();
    Vector3 (*WorldToScreenPoint)(void *, Vector3);
    bool (*get_isVisible)(void *player);
    bool (*get_isLocalTeam)(void *player);
    bool (*get_IsDieing)(void *player);
    int (*get_MaxHP)(void *player);
    Vector3 (*GetForward)(void *player);
    void (*set_aim)(void *player, Quaternion look);
    bool (*get_IsSighting)(void *player);
    bool (*get_IsFiring)(void *player);
    monoString *(*name)(void *player);
    void *(*_GetHeadPositions)(void *);
    void *(*_newHipMods)(void *);
    void *(*_GetLeftAnkleTF)(void *);
    void *(*_GetRightAnkleTF)(void *);
    void *(*_GetLeftToeTF)(void *);
    void *(*_GetRightToeTF)(void *);
    void *(*_getLeftHandTF)(void *);
    void *(*_getRightHandTF)(void *);
    void *(*_getLeftForeArmTF)(void *);
    void *(*_getRightForeArmTF)(void *);
};

game_sdk_t *game_sdk = new game_sdk_t();

void game_sdk_t::init()
{
    this->GetHp = (int (*)(void *))getRealOffset(oxo("0x104A8478C"));
    this->Curent_Match = (void *(*)())getRealOffset(oxo("0x104E355B0"));
    this->GetLocalPlayer = (void *(*)(void *))getRealOffset(oxo("0x1036051FC"));
    this->GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x104AA1A28"));
    this->get_position = (Vector3 (*)(void *))getRealOffset(oxo("0x108552BAC"));
    this->Component_GetTransform = (void *(*)(void *))getRealOffset(oxo("0x10854060C"));
    this->get_camera = (void *(*)())getRealOffset(oxo("0x1084E7148"));
    this->WorldToScreenPoint = (Vector3 (*)(void *, Vector3))getRealOffset(oxo("0x1084E6AC8"));
    this->get_isVisible = (bool (*)(void *))getRealOffset(oxo("0x104B8FEE8"));
    this->get_isLocalTeam = (bool (*)(void *))getRealOffset(oxo("0x104A38D90"));
    this->get_IsDieing = (bool (*)(void *))getRealOffset(oxo("0x104A02EA8"));
    this->get_MaxHP = (int (*)(void *))getRealOffset(oxo("0x104A8489C"));
    this->GetForward = (Vector3 (*)(void *))getRealOffset(oxo("0x1085534CC"));
    this->set_aim = (void (*)(void *, Quaternion))getRealOffset(oxo("0x104A1C91C"));
    this->get_IsSighting = (bool (*)(void *))getRealOffset(oxo("0x104A0FF18"));
    this->get_IsFiring = (bool (*)(void *))getRealOffset(oxo("0x104A05634"));
    this->name = (monoString * (*)(void *player)) getRealOffset(oxo("0x104A16D38"));
    this->_GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x104AA1A28"));
    this->_newHipMods = (void *(*)(void *))getRealOffset(oxo("0x104AA1BD8"));
    this->_GetLeftAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x104AA2028"));
    this->_GetRightAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x104AA2134"));
    this->_GetLeftToeTF = (void *(*)(void *))getRealOffset(oxo("0x104AA2240"));
    this->_GetRightToeTF = (void *(*)(void *))getRealOffset(oxo("0x104AA234C"));
    this->_getLeftHandTF = (void *(*)(void *))getRealOffset(oxo("0x104A1B9B4"));
    this->_getRightHandTF = (void *(*)(void *))getRealOffset(oxo("0x104A1BAB8"));
    this->_getLeftForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x104A1BBBC"));
    this->_getRightForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x104A1BCC0"));
}

namespace Camera$$WorldToScreen
{
    ImVec2 Regular(Vector3 pos)
    {
        auto cam = game_sdk->get_camera();
        if (!cam)
            return {0, 0};
        Vector3 worldPoint = game_sdk->WorldToScreenPoint(cam, pos);
        int ScreenWidth = ImGui::GetIO().DisplaySize.x;
        int ScreenHeight = ImGui::GetIO().DisplaySize.y;
        Vector3 location;
        location.x = ScreenWidth * worldPoint.x;
        location.y = ScreenHeight - worldPoint.y * ScreenHeight;
        location.z = worldPoint.z;
        return {location.x, location.y};
    }

    ImVec2 Checker(Vector3 pos, bool &checker)
    {
        auto cam = game_sdk->get_camera();
        if (!cam)
            return {0, 0};
        Vector3 worldPoint = game_sdk->WorldToScreenPoint(cam, pos);
        int ScreenWidth = ImGui::GetIO().DisplaySize.x;
        int ScreenHeight = ImGui::GetIO().DisplaySize.y;
        Vector3 location;
        location.x = ScreenWidth * worldPoint.x;
        location.y = ScreenHeight - worldPoint.y * ScreenHeight;
        location.z = worldPoint.z;
        checker = location.z > 1;
        return {location.x, location.y};
    }
}

Vector3 GetBonePosition(void *player, void *(*transformGetter)(void *))
{
    if (!player || !transformGetter)
        return Vector3();
    void *transform = transformGetter(player);
    if (!transform)
        return Vector3();
    void *comp = game_sdk->Component_GetTransform(transform);
    if (!comp)
        return Vector3();
    return game_sdk->get_position(comp);
}

Vector3 GetHitboxPosition(void *player, int hitbox)
{
    if (!player)
        return Vector3::zero();
    switch (hitbox)
    {
    case 0:
        return GetBonePosition(player, game_sdk->_GetHeadPositions);
    case 1:
    {
        Vector3 h = GetBonePosition(player, game_sdk->_GetHeadPositions);
        return h == Vector3::zero() ? h : Vector3(h.x, h.y - 0.05f, h.z);
    }
    case 2:
    {
        Vector3 h = GetBonePosition(player, game_sdk->_GetHeadPositions);
        return h == Vector3::zero() ? h : Vector3(h.x, h.y - 0.2f, h.z);
    }
    default:
        return GetBonePosition(player, game_sdk->_GetHeadPositions);
    }
}

Vector3 getPosition(void *player)
{
    if (!player)
        return Vector3();
    void *comp = game_sdk->Component_GetTransform(player);
    if (!comp)
        return Vector3();
    return game_sdk->get_position(comp);
}

static Vector3 GetHeadPosition(void *player)
{
    if (!player)
        return Vector3();
    void *head = game_sdk->GetHeadPositions(player);
    if (!head)
        return Vector3();
    return game_sdk->get_position(head);
}

static Vector3 CameraMain(void *player)
{
    auto cam = game_sdk->get_camera();
    if (!cam)
        return Vector3::zero();
    void *camTransform = game_sdk->Component_GetTransform(cam);
    if (!camTransform)
        return Vector3::zero();
    return game_sdk->get_position(camTransform);
}

Quaternion GetRotationToTheLocation(Vector3 Target, float Height, Vector3 MyEnemy)
{
    Vector3 direction = (Target + Vector3(0, Height, 0)) - MyEnemy;
    return Quaternion::LookRotation(direction, Vector3(0, 1, 0));
}

Quaternion GetCurrentRotation(void *player)
{
    void *transform = game_sdk->Component_GetTransform(player);
    if (!transform)
        return Quaternion();
    return Quaternion::LookRotation(game_sdk->GetForward(transform), Vector3(0, 1, 0));
}

#include "Helper/Ext.h"

class tanghinh
{
public:
    static Vector3 Transform_GetPosition(void *player)
    {
        if (game_sdk && game_sdk->get_position)
            return game_sdk->get_position(player);
        return Vector3::zero();
    }

    static void *Player_GetHeadCollider(void *player)
    {
        void *(*_Player_GetHeadCollider)(void *) = (void *(*)(void *))getRealOffset(oxo("0x104A1A9D4"));
        return _Player_GetHeadCollider(player);
    }

    static bool Physics_Raycast(Vector3 origin, Vector3 target, unsigned int layerMask, void *unused)
    {
        bool (*_Physics_Raycast)(Vector3, Vector3, float, int) = (bool (*)(Vector3, Vector3, float, int))getRealOffset(oxo("0x1085C9F40"));
        Vector3 direction = Vector3::Normalized(target - origin);
        float maxDistance = Vector3::Distance(origin, target);
        return _Physics_Raycast(origin, direction, maxDistance, (int)layerMask);
    }

    static bool isVisible(void *enemy)
    {
        if (enemy != NULL && game_sdk != NULL && game_sdk->get_isVisible)
            return game_sdk->get_isVisible(enemy);
        return false;
    }
};

bool isFov(Vector3 vec1, Vector3 vec2, int radius)
{
    float dx = vec1.x - vec2.x;
    float dy = vec1.y - vec2.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

void *GetClosestEnemy()
{
    try
    {
        float shortestDistance = 250.0f;
        void *closestEnemy = NULL;
        void *get_MatchGame = game_sdk->Curent_Match();
        if (!get_MatchGame)
            return NULL;
        void *LocalPlayer = game_sdk->GetLocalPlayer(get_MatchGame);
        if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
            return NULL;
        if (!Vars.Aimbot)
            return NULL;

        void *playersDict = *(void **)((long)get_MatchGame + oxo("0x148"));
        if (!playersDict)
            return NULL;
        IL2CppPlayerDict players(playersDict);

        Vector3 LocalPlayerPos = getPosition(LocalPlayer);
        ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);

        for (int u = 0; u < players.getCount(); u++)
        {
            void *Player = players.getPlayer(u);
            if (!Player || Player == LocalPlayer || !game_sdk->get_MaxHP(Player) || game_sdk->get_isLocalTeam(Player))
                continue;
            if (Vars.IgnoreKnocked && game_sdk->get_IsDieing(Player))
                continue;
            if (Vars.VisibleCheck && !tanghinh::isVisible(Player))
                continue;
            if (Vars.VisibleCheck)
            {
                Vector3 localHead = GetHeadPosition(LocalPlayer);
                Vector3 enemyHead = GetHitboxPosition(Player, 0);
                if (localHead != Vector3::zero() && enemyHead != Vector3::zero())
                    if (tanghinh::Physics_Raycast(localHead, enemyHead, 256, nullptr))
                        continue;
            }

            Vector3 PlayerPos = GetHitboxPosition(Player, Vars.AimHitbox);
            float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
            if (distance >= 300)
                continue;

            ImVec2 enemyScreenPos = Camera$$WorldToScreen::Regular(PlayerPos);
            bool isValidTarget = isFov(Vector3(enemyScreenPos.x, enemyScreenPos.y, 0), Vector3(center.x, center.y, 0), Vars.AimFov);
            if (isValidTarget && distance < shortestDistance)
            {
                shortestDistance = distance;
                closestEnemy = Player;
            }
        }
        return closestEnemy;
    }
    catch (...)
    {
        return NULL;
    }
}

void ProcessAimbot()
{
    if (!Vars.Aimbot)
        return;
    void *CurrentMatch = game_sdk->Curent_Match();
    if (!CurrentMatch)
        return;
    void *LocalPlayer = game_sdk->GetLocalPlayer(CurrentMatch);
    if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
        return;
    void *closestEnemy = GetClosestEnemy();
    if (!closestEnemy || !game_sdk->Component_GetTransform(closestEnemy))
        return;

    Vector3 EnemyLocation = GetHitboxPosition(closestEnemy, Vars.AimHitbox);
    if (EnemyLocation == Vector3::zero())
        return;
    Vector3 PlayerLocation = CameraMain(LocalPlayer);
    if (PlayerLocation == Vector3::zero())
        return;

    bool IsScopeOn = game_sdk->get_IsSighting(LocalPlayer);
    bool IsFiring = game_sdk->get_IsFiring(LocalPlayer);
    bool shouldAim =
        (Vars.AimWhen == 0) ||
        (Vars.AimWhen == 1 && IsFiring) ||
        (Vars.AimWhen == 2 && IsScopeOn) ||
        (Vars.AimWhen == 3 && (IsFiring || IsScopeOn));

    if (shouldAim && (!Vars.VisibleCheck || tanghinh::isVisible(closestEnemy)))
    {
        if (game_sdk->get_IsDieing(closestEnemy) && Vars.IgnoreKnocked)
        {
            float shortestDistance = 9999.0f;
            void *newTarget = NULL;
            void *playersDict = *(void **)((long)CurrentMatch + oxo("0x148"));
            if (playersDict)
            {
                IL2CppPlayerDict players(playersDict);
                for (int u = 0; u < players.getCount(); u++)
                {
                    void *Player = players.getPlayer(u);
                    if (!Player || Player == LocalPlayer || !game_sdk->get_MaxHP(Player) || game_sdk->get_isLocalTeam(Player) || Player == closestEnemy)
                        continue;
                    if (Vars.IgnoreKnocked && game_sdk->get_IsDieing(Player))
                        continue;
                    if (Vars.VisibleCheck && !tanghinh::isVisible(Player))
                        continue;
                    Vector3 PlayerPos = GetHitboxPosition(Player, Vars.AimHitbox);
                    float distance = Vector3::Distance(PlayerLocation, PlayerPos);
                    if (distance < 300 && distance < shortestDistance)
                    {
                        shortestDistance = distance;
                        newTarget = Player;
                    }
                }
            }
            if (newTarget)
            {
                EnemyLocation = GetHitboxPosition(newTarget, Vars.AimHitbox);
                closestEnemy = newTarget;
            }
            else
                return;
        }

        Quaternion TargetLook = GetRotationToTheLocation(EnemyLocation, 0.0f, PlayerLocation);

        if (Vars.AimSilent)
        {
            if (game_sdk->get_IsFiring(LocalPlayer))
                game_sdk->set_aim(LocalPlayer, TargetLook);
        }
        else if (Vars.AimVectored)
        {
            Quaternion CurrentRot = GetCurrentRotation(LocalPlayer);
            float smoothFactor = 1.0f / Vars.AimVectoredSpeed;
            Quaternion SmoothedRot = Quaternion::Slerp(CurrentRot, TargetLook, smoothFactor);
            game_sdk->set_aim(LocalPlayer, SmoothedRot);
        }
        else
        {
            game_sdk->set_aim(LocalPlayer, TargetLook);
        }
    }
}

void aimbot()
{
    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
    if (!Vars.Aimbot)
        return;
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    if (!draw_list)
        return;
    void *Match = game_sdk->Curent_Match();
    if (!Match)
        return;

    if (Vars.isAimFov)
        draw_list->AddCircle(center, Vars.AimFov, ImColor(1.0f, 1.0f, 1.0f, 0.6f), 100, 1.5f);

    void *LocalPlayer = game_sdk->GetLocalPlayer(Match);
    if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
        return;
    void *playertarget = GetClosestEnemy();
    if (!playertarget)
        return;

    ImVec2 EnemyLocation = Camera$$WorldToScreen::Regular(GetHeadPosition(playertarget));
    draw_list->AddLine(center, EnemyLocation, ImColor(1.0f, 0.0f, 0.0f, 0.8f), 1.5f);
}

void get_players()
{
    try
    {
        if (Vars.Aimbot)
            ProcessAimbot();
    }
    catch (...)
    {
    }
}
#ifndef EX_HEADERS
#define EX_HEADERS
extern struct ImVec4 espv;
extern struct ImVec4 espi;
extern struct ImVec4 userColor;
extern struct ImVec4 nameColor;
extern struct ImVec4 distanceColor;
extern struct ImVec4 fovColor;
extern bool istelekill;
extern bool isfly;
#define FMT_HEADER_ONLY
#include "fmt/core.h"
inline void OtFovV1(float a, float b, float c, float d, float e, ImColor f, float g) {}
inline void drawcircleglow(ImDrawList *a, ImVec2 b, float c, ImColor d, int e, float f, int g) {}
inline void AddText(ImFont *font, float size, bool b1, bool b2, ImVec2 pos, ImColor col, const char *text)
{
    ImGui::GetBackgroundDrawList()->AddText(font, size, pos, col, text);
}
#endif

extern bool istelekill;
extern bool isfly;

static void Transform_INTERNAL_SetPosition(void *player, Vector3 inn)
{
    void (*_Transform_set_position)(void *transform, Vector3 value) = (void (*)(void *, Vector3))getRealOffset(oxo("0x108552C74"));
    _Transform_set_position(player, inn);
}

struct Vars_t
{
    bool UpPlayerOne = false;
    bool Enable = {};
    bool AimbotEnable = {};
    bool Aimbot = {};
    float AimFov = {};
    int AimCheck = {};
    int AimType = {};
    int AimWhen = {};
    bool isAimFov = {};
    int AimHitbox = 0; // 0: Head, 1: Neck, 2: Body
    const char *aimHitboxes[3] = {" CabeÃ§a", " PescoÃ§o", " Corpo"};
    const char *dir[4] = {" AutomÃ¡tico", " Disparo", " Escopo", " Disparo + Escopo"};
    bool lines = {};
    bool Box = {};
    bool Outline = {};
    bool Name = {};
    bool Health = {};
    bool Distance = {};
    bool fovaimglow = {};
    bool circlepos = {};
    bool skeleton = {};
    bool OOF = {};
    bool counts = {};
    ImVec4 boxColor = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    float AimSpeed = 9999.0f;
    bool VisibleCheck = true;
    bool IgnoreKnocked = false;

    // AIM KILL & VECTORED
    bool AimKill = false;
    bool AimSilent = false;
    bool AimVectored = false;
    float AimVectoredSpeed = 1.0f;
} Vars;

namespace Save
{
    inline int AimFPS = (1000000 / 15);
    inline long AimDelay = 0;
}

class game_sdk_t
{
public:
    void init();
    int (*GetHp)(void *player);
    void *(*Curent_Match)();
    void *(*GetLocalPlayer)(void *Game);
    void *(*GetHeadPositions)(void *player);
    Vector3 (*get_position)(void *player);
    void *(*Component_GetTransform)(void *player);
    void *(*get_camera)();
    Vector3 (*WorldToScreenPoint)(void *, Vector3);
    bool (*get_isVisible)(void *player);
    bool (*get_isLocalTeam)(void *player);
    bool (*get_IsDieing)(void *player);
    int (*get_MaxHP)(void *player);
    Vector3 (*GetForward)(void *player);
    void (*set_aim)(void *player, Quaternion look);
    bool (*get_IsSighting)(void *player);
    bool (*get_IsFiring)(void *player);
    monoString *(*name)(void *player);
    void *(*_GetHeadPositions)(void *);
    void *(*_newHipMods)(void *);
    void *(*_GetLeftAnkleTF)(void *);
    void *(*_GetRightAnkleTF)(void *);
    void *(*_GetLeftToeTF)(void *);
    void *(*_GetRightToeTF)(void *);
    void *(*_getLeftHandTF)(void *);
    void *(*_getRightHandTF)(void *);
    void *(*_getLeftForeArmTF)(void *);
    void *(*_getRightForeArmTF)(void *);
};

game_sdk_t *game_sdk = new game_sdk_t();

void game_sdk_t::init()
{
    this->GetHp = (int (*)(void *))getRealOffset(oxo("0x104A8478C"));                           // public int get_CurHP() { }
    this->Curent_Match = (void *(*)())getRealOffset(oxo("0x104E355B0"));                        // public static NFJPHMKKEBF CurrentMatch() { }
    this->GetLocalPlayer = (void *(*)(void *))getRealOffset(oxo("0x1036051FC"));                // private static Player GetLocalPlayer() { }
    this->GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x104AA1A28"));              // public virtual Transform GetHeadTF() { }
    this->get_position = (Vector3 (*)(void *))getRealOffset(oxo("0x108552BAC"));                // public Vector3 get_position() { }
    this->Component_GetTransform = (void *(*)(void *))getRealOffset(oxo("0x10854060C"));        // public Transform get_transform() { }
    this->get_camera = (void *(*)())getRealOffset(oxo("0x1084E7148"));                          // public static Camera get_main() { }
    this->WorldToScreenPoint = (Vector3 (*)(void *, Vector3))getRealOffset(oxo("0x1084E6AC8")); // public Vector3 WorldToViewportPoint(Vector3 position) { }
    this->get_isVisible = (bool (*)(void *))getRealOffset(oxo("0x104B8FEE8"));                  // public override bool IsVisible() { }
    this->get_isLocalTeam = (bool (*)(void *))getRealOffset(oxo("0x104A38D90"));                // public virtual bool IsLocalTeammate(bool isCheckSocial3pEffect = False) { }
    this->get_IsDieing = (bool (*)(void *))getRealOffset(oxo("0x104A02EA8"));                   // public bool get_IsDieing() { }
    this->get_MaxHP = (int (*)(void *))getRealOffset(oxo("0x104A8489C"));                       // public int get_MaxHP() { }
    this->GetForward = (Vector3 (*)(void *))getRealOffset(oxo("0x1085534CC"));                  // public Vector3 get_forward() { }
    this->set_aim = (void (*)(void *, Quaternion))getRealOffset(oxo("0x104A1C91C"));            // public void SetAimRotation(Quaternion JGOGIAFGCFC) { }
    this->get_IsSighting = (bool (*)(void *))getRealOffset(oxo("0x104A0FF18"));                 // public bool get_IsSighting() { }
    this->get_IsFiring = (bool (*)(void *))getRealOffset(oxo("0x104A05634"));                   // public bool IsFiring() { }
    this->name = (monoString * (*)(void *player)) getRealOffset(oxo("0x104A16D38"));            // public string get_NickName() { }
    // skeleton
    this->_GetHeadPositions = (void *(*)(void *))getRealOffset(oxo("0x104AA1A28"));  // GetHeadTF
    this->_newHipMods = (void *(*)(void *))getRealOffset(oxo("0x104AA1BD8"));        // GetHipTF
    this->_GetLeftAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x104AA2028"));    // GetLeftAnkleTF
    this->_GetRightAnkleTF = (void *(*)(void *))getRealOffset(oxo("0x104AA2134"));   // GetRightAnkleTF
    this->_GetLeftToeTF = (void *(*)(void *))getRealOffset(oxo("0x104AA2240"));      // GetLeftToeTF
    this->_GetRightToeTF = (void *(*)(void *))getRealOffset(oxo("0x104AA234C"));     // GetRightToeTF
    this->_getLeftHandTF = (void *(*)(void *))getRealOffset(oxo("0x104A1B9B4"));     // get_LeftHandTF
    this->_getRightHandTF = (void *(*)(void *))getRealOffset(oxo("0x104A1BAB8"));    // get_RightHandTF
    this->_getLeftForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x104A1BBBC"));  // get_LeftForeArmTF
    this->_getRightForeArmTF = (void *(*)(void *))getRealOffset(oxo("0x104A1BCC0")); // get_RightForeArmTF
}

namespace Camera$$WorldToScreen
{
    ImVec2 Regular(Vector3 pos)
    {
        auto cam = game_sdk->get_camera();
        if (!cam)
            return {0, 0};
        Vector3 worldPoint = game_sdk->WorldToScreenPoint(cam, pos);
        Vector3 location;
        int ScreenWidth = ImGui::GetIO().DisplaySize.x;
        int ScreenHeight = ImGui::GetIO().DisplaySize.y;
        location.x = ScreenWidth * worldPoint.x;
        location.y = ScreenHeight - worldPoint.y * ScreenHeight;
        location.z = worldPoint.z;
        return {location.x, location.y};
    }

    ImVec2 Checker(Vector3 pos, bool &checker)
    {
        auto cam = game_sdk->get_camera();
        if (!cam)
            return {0, 0};
        Vector3 worldPoint = game_sdk->WorldToScreenPoint(cam, pos);
        Vector3 location;
        int ScreenWidth = ImGui::GetIO().DisplaySize.x;
        int ScreenHeight = ImGui::GetIO().DisplaySize.y;
        location.x = ScreenWidth * worldPoint.x;
        location.y = ScreenHeight - worldPoint.y * ScreenHeight;
        location.z = worldPoint.z;
        checker = location.z > 1;
        return {location.x, location.y};
    }
}

Vector3 GetBonePosition(void *player, void *(*transformGetter)(void *))
{
    if (!player || !transformGetter)
        return Vector3();
    void *transform = transformGetter(player);
    if (!transform)
        return Vector3();
    void *comp = game_sdk->Component_GetTransform(transform);
    if (!comp)
        return Vector3();
    return game_sdk->get_position(comp);
}

Vector3 GetHitboxPosition(void *player, int hitbox)
{
    if (!player)
        return Vector3::zero();

    switch (hitbox)
    {
    case 0:
        return GetBonePosition(player, game_sdk->_GetHeadPositions); // Head
    case 1:
    {
        Vector3 headPos = GetBonePosition(player, game_sdk->_GetHeadPositions);
        return headPos == Vector3::zero() ? headPos : Vector3(headPos.x, headPos.y - 0.05f, headPos.z); // Neck
    }
    case 2:
    {
        Vector3 headPos = GetBonePosition(player, game_sdk->_GetHeadPositions);
        return headPos == Vector3::zero() ? headPos : Vector3(headPos.x, headPos.y - 0.2f, headPos.z); // Body
    }
    default:
        return GetBonePosition(player, game_sdk->_GetHeadPositions);
    }
}

Vector3 getPosition(void *player)
{
    if (!player)
        return Vector3();
    void *comp = game_sdk->Component_GetTransform(player);
    if (!comp)
        return Vector3();
    return game_sdk->get_position(comp);
}

static Vector3 GetHeadPosition(void *player)
{
    if (!player)
        return Vector3();
    void *head = game_sdk->GetHeadPositions(player);
    if (!head)
        return Vector3();
    return game_sdk->get_position(head);
}

static Vector3 CameraMain(void *player)
{
    auto cam = game_sdk->get_camera();
    if (!cam)
        return Vector3::zero();
    void *camTransform = game_sdk->Component_GetTransform(cam);
    if (!camTransform)
        return Vector3::zero();
    return game_sdk->get_position(camTransform);
}

Quaternion GetRotationToTheLocation(Vector3 Target, float Height, Vector3 MyEnemy)
{
    Vector3 direction = (Target + Vector3(0, Height, 0)) - MyEnemy;
    return Quaternion::LookRotation(direction, Vector3(0, 1, 0));
}

Quaternion GetCurrentRotation(void *player)
{
    void *transform = game_sdk->Component_GetTransform(player);
    if (!transform)
        return Quaternion();
    return Quaternion::LookRotation(game_sdk->GetForward(transform), Vector3(0, 1, 0));
}

#include "Helper/Ext.h"

class tanghinh
{
public:
    static Vector3 Transform_GetPosition(void *player)
    {
        if (game_sdk && game_sdk->get_position)
        {
            return game_sdk->get_position(player);
        }
        return Vector3::zero();
    }

    static void *Player_GetHeadCollider(void *player)
    {
        void *(*_Player_GetHeadCollider)(void *players) = (void *(*)(void *))getRealOffset(oxo("0x104A1A9D4"));
        return _Player_GetHeadCollider(player);
    }

    static bool Physics_Raycast(Vector3 origin, Vector3 target, unsigned int layerMask, void *unused)
    {
        // Physics.Raycast(Vector3 origin, Vector3 direction, float maxDistance, int layerMask)
        bool (*_Physics_Raycast)(Vector3, Vector3, float, int) = (bool (*)(Vector3, Vector3, float, int))getRealOffset(oxo("0x1085C9F40"));
        Vector3 direction = Vector3::Normalized(target - origin);
        float maxDistance = Vector3::Distance(origin, target);
        return _Physics_Raycast(origin, direction, maxDistance, (int)layerMask);
    }

    static bool isVisible(void *enemy)
    {
        if (enemy != NULL && game_sdk != NULL && game_sdk->get_isVisible)
        {
            return game_sdk->get_isVisible(enemy);
        }
        return false;
    }
};

void DrawLine(ImDrawList *drawList, ImVec2 start, ImVec2 end, float thickness, bool isDead = false, bool isVisible = false)
{
    if (!drawList)
        return;
    ImColor color = isDead ? ImColor(espi) : isVisible ? ImColor(espv)
                                                       : ImColor(espi);
    drawList->AddLine(start, end, color, thickness);
}

void DrawHealthBar(ImDrawList *drawList, ImVec2 start, ImVec2 end, float healthMultiplier, float thickness, bool isDead = false)
{
    if (!drawList)
        return;

    float totalHeight = end.y - start.y;
    float healthHeight = totalHeight * healthMultiplier;

    drawList->AddRectFilled(
        ImVec2(start.x - thickness / 2, start.y),
        ImVec2(start.x + thickness / 2, end.y),
        ImColor(50, 50, 50, 200));

    if (healthMultiplier > 0)
    {
        ImColor color = isDead ? ImColor(255, 0, 0) : ImColor(0, 255, 0);
        drawList->AddRectFilled(
            ImVec2(start.x - thickness / 2, end.y - healthHeight),
            ImVec2(start.x + thickness / 2, end.y),
            color);
    }

    if (Vars.Outline)
    {
        drawList->AddRect(
            ImVec2(start.x - thickness / 2 - 1, start.y - 1),
            ImVec2(start.x + thickness / 2 + 1, end.y + 1),
            ImColor(0, 255, 0));
    }
}

void DrawSkeleton(void *player, ImDrawList *drawList)
{
    if (!player || !drawList)
        return;
    bool isPlayerVisible = tanghinh::isVisible(player);
    bool isPlayerDead = game_sdk->get_IsDieing(player);

    Vector3 headPos = GetBonePosition(player, game_sdk->_GetHeadPositions);
    Vector3 hipPos = GetBonePosition(player, game_sdk->_newHipMods);
    Vector3 leftAnklePos = GetBonePosition(player, game_sdk->_GetLeftAnkleTF);
    Vector3 rightAnklePos = GetBonePosition(player, game_sdk->_GetRightAnkleTF);
    Vector3 leftToePos = GetBonePosition(player, game_sdk->_GetLeftToeTF);
    Vector3 rightToePos = GetBonePosition(player, game_sdk->_GetRightToeTF);
    Vector3 leftHandPos = GetBonePosition(player, game_sdk->_getLeftHandTF);
    Vector3 rightHandPos = GetBonePosition(player, game_sdk->_getRightHandTF);
    Vector3 leftForeArmPos = GetBonePosition(player, game_sdk->_getLeftForeArmTF);
    Vector3 rightForeArmPos = GetBonePosition(player, game_sdk->_getRightForeArmTF);
    bool visible;
    ImVec2 headScreen = Camera$$WorldToScreen::Checker(headPos, visible);
    if (!visible)
        return;
    ImVec2 hipScreen = Camera$$WorldToScreen::Regular(hipPos);
    ImVec2 leftAnkleScreen = Camera$$WorldToScreen::Regular(leftAnklePos);
    ImVec2 rightAnkleScreen = Camera$$WorldToScreen::Regular(rightAnklePos);
    ImVec2 leftToeScreen = Camera$$WorldToScreen::Regular(leftToePos);
    ImVec2 rightToeScreen = Camera$$WorldToScreen::Regular(rightToePos);
    ImVec2 leftHandScreen = Camera$$WorldToScreen::Regular(leftHandPos);
    ImVec2 rightHandScreen = Camera$$WorldToScreen::Regular(rightHandPos);
    ImVec2 leftForeArmScreen = Camera$$WorldToScreen::Regular(leftForeArmPos);
    ImVec2 rightForeArmScreen = Camera$$WorldToScreen::Regular(rightForeArmPos);
    float thickness = 1.0f;
    ImColor color = isPlayerDead ? ImColor(espi) : isPlayerVisible ? ImColor(espv)
                                                                   : ImColor(espi);

    drawList->AddCircle(headScreen, 2.0f, color, 12, thickness);
    drawList->AddLine(headScreen, hipScreen, color, thickness);
    drawList->AddLine(headScreen, leftForeArmScreen, color, thickness);
    drawList->AddLine(headScreen, rightForeArmScreen, color, thickness);
    drawList->AddLine(leftForeArmScreen, leftHandScreen, color, thickness);
    drawList->AddLine(rightForeArmScreen, rightHandScreen, color, thickness);
    drawList->AddLine(hipScreen, leftAnkleScreen, color, thickness);
    drawList->AddLine(hipScreen, rightAnkleScreen, color, thickness);
    drawList->AddLine(leftAnkleScreen, leftToeScreen, color, thickness);
    drawList->AddLine(rightAnkleScreen, rightToeScreen, color, thickness);
}

bool isFov(Vector3 vec1, Vector3 vec2, int radius)
{
    float dx = vec1.x - vec2.x;
    float dy = vec1.y - vec2.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

void *GetClosestEnemy()
{
    try
    {
        float shortestDistance = 250.0f;
        void *closestEnemy = NULL;
        void *get_MatchGame = game_sdk->Curent_Match();
        if (!get_MatchGame)
            return NULL;
        void *LocalPlayer = game_sdk->GetLocalPlayer(get_MatchGame);
        if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
            return NULL;
        if (!Vars.Aimbot && !Vars.Enable)
            return NULL;

        void *playersDict = *(void **)((long)get_MatchGame + oxo("0x148"));
        if (!playersDict)
            return NULL;
        IL2CppPlayerDict players(playersDict);

        Vector3 LocalPlayerPos = getPosition(LocalPlayer);
        ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);

        for (int u = 0; u < players.getCount(); u++)
        {
            void *Player = players.getPlayer(u);
            if (!Player || Player == LocalPlayer || !game_sdk->get_MaxHP(Player) || game_sdk->get_isLocalTeam(Player))
                continue;

            if (Vars.IgnoreKnocked && game_sdk->get_IsDieing(Player))
                continue;
            if (Vars.VisibleCheck && !tanghinh::isVisible(Player))
                continue;

            // Raycast line-of-sight check: skip if obstacle blocks view
            if (Vars.VisibleCheck)
            {
                Vector3 localHead = GetHeadPosition(LocalPlayer);
                Vector3 enemyHead = GetHitboxPosition(Player, 0);
                if (localHead != Vector3::zero() && enemyHead != Vector3::zero())
                {
                    if (tanghinh::Physics_Raycast(localHead, enemyHead, 256, nullptr))
                        continue;
                }
            }

            Vector3 PlayerPos = GetHitboxPosition(Player, Vars.AimHitbox);
            float distance = Vector3::Distance(LocalPlayerPos, PlayerPos);
            if (distance >= 300)
                continue;

            ImVec2 enemyScreenPos = Camera$$WorldToScreen::Regular(PlayerPos);
            bool isValidTarget = isFov(Vector3(enemyScreenPos.x, enemyScreenPos.y, 0), Vector3(center.x, center.y, 0), Vars.AimFov);

            if (isValidTarget && distance < shortestDistance)
            {
                shortestDistance = distance;
                closestEnemy = Player;
            }
        }
        return closestEnemy;
    }
    catch (...)
    {
        return NULL;
    }
}

void UpOneEnemy()
{
    if (!Vars.Enable || !Vars.UpPlayerOne)
        return;

    void *match = game_sdk->Curent_Match();
    if (!match)
        return;

    void *local = game_sdk->GetLocalPlayer(match);
    if (!local || !game_sdk->Component_GetTransform(local))
        return;

    void *playersDict = *(void **)((long)match + 0x148);
    if (!playersDict)
        return;
    IL2CppPlayerDict players(playersDict);

    for (int i = 0; i < players.getCount(); i++)
    {
        void *enemy = players.getPlayer(i);
        if (!enemy || enemy == local)
            continue;
        if (!game_sdk->Component_GetTransform(enemy))
            continue;
        if (!game_sdk->get_MaxHP(enemy))
            continue;
        if (game_sdk->get_IsDieing(enemy))
            continue;
        if (game_sdk->GetHp(enemy) <= 0)
            continue;
        if (game_sdk->get_isLocalTeam(enemy))
            continue;

        void *enemyTF = game_sdk->Component_GetTransform(enemy);
        void *localTF = game_sdk->Component_GetTransform(local);
        if (!enemyTF || !localTF)
            continue;

        Vector3 enemyPos = game_sdk->get_position(enemyTF);
        Vector3 localPos = game_sdk->get_position(localTF);
        float distance = Vector3::Distance(localPos, enemyPos);
        if (distance <= 10.0f)
            continue;

        float groundY = enemyPos.y;
        float targetY = groundY + 5.7f;

        float step = 0.35f;

        if (enemyPos.y < targetY - 0.1f)
            enemyPos.y += step;
        else if (enemyPos.y > targetY + 0.1f)
            enemyPos.y -= step;

        Transform_INTERNAL_SetPosition(enemyTF, Vector3(enemyPos.x, enemyPos.y, enemyPos.z));
    }
}

void ProcessAimbot()
{
    if (!Vars.Aimbot)
        return;
    void *CurrentMatch = game_sdk->Curent_Match();
    if (!CurrentMatch)
        return;
    void *LocalPlayer = game_sdk->GetLocalPlayer(CurrentMatch);
    if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
        return;
    void *closestEnemy = GetClosestEnemy();
    if (!closestEnemy || !game_sdk->Component_GetTransform(closestEnemy))
        return;

    Vector3 EnemyLocation = GetHitboxPosition(closestEnemy, Vars.AimHitbox);
    if (EnemyLocation == Vector3::zero())
        return;
    Vector3 PlayerLocation = CameraMain(LocalPlayer);
    if (PlayerLocation == Vector3::zero())
        return;

    bool IsScopeOn = game_sdk->get_IsSighting(LocalPlayer);
    bool IsFiring = game_sdk->get_IsFiring(LocalPlayer);
    bool shouldAim =
        (Vars.AimWhen == 0) ||                          // Always
        (Vars.AimWhen == 1 && IsFiring) ||              // Firing
        (Vars.AimWhen == 2 && IsScopeOn) ||             // Scoping
        (Vars.AimWhen == 3 && (IsFiring || IsScopeOn)); // Fire & Scope

    if (shouldAim && (!Vars.VisibleCheck || tanghinh::isVisible(closestEnemy)))
    {
        if (game_sdk->get_IsDieing(closestEnemy) && Vars.IgnoreKnocked)
        {
            float shortestDistance = 9999.0f;
            void *newTarget = NULL;

            void *playersDict = *(void **)((long)CurrentMatch + oxo("0x148"));
            if (playersDict)
            {
                IL2CppPlayerDict players(playersDict);
                for (int u = 0; u < players.getCount(); u++)
                {
                    void *Player = players.getPlayer(u);
                    if (!Player || Player == LocalPlayer || !game_sdk->get_MaxHP(Player) || game_sdk->get_isLocalTeam(Player) || Player == closestEnemy)
                        continue;

                    if (Vars.IgnoreKnocked && game_sdk->get_IsDieing(Player))
                        continue;
                    if (Vars.VisibleCheck && !tanghinh::isVisible(Player))
                        continue;

                    Vector3 PlayerPos = GetHitboxPosition(Player, Vars.AimHitbox);
                    float distance = Vector3::Distance(PlayerLocation, PlayerPos);
                    if (distance < 300 && distance < shortestDistance)
                    {
                        shortestDistance = distance;
                        newTarget = Player;
                    }
                }
            }

            if (newTarget)
            {
                EnemyLocation = GetHitboxPosition(newTarget, Vars.AimHitbox);
                closestEnemy = newTarget;
            }
            else
            {
                return;
            }
        }

        // Calculate target rotation (Height = 0, clean aim)
        Quaternion TargetLook = GetRotationToTheLocation(EnemyLocation, 0.0f, PlayerLocation);

        if (Vars.AimSilent)
        {
            // Silent Aim: instant snap only during firing frames
            if (game_sdk->get_IsFiring(LocalPlayer))
            {
                game_sdk->set_aim(LocalPlayer, TargetLook);
            }
        }
        else if (Vars.AimVectored)
        {
            // Vectored Aim: smooth Slerp from current rotation to target
            Quaternion CurrentRot = GetCurrentRotation(LocalPlayer);
            float smoothFactor = 1.0f / Vars.AimVectoredSpeed;
            Quaternion SmoothedRot = Quaternion::Slerp(CurrentRot, TargetLook, smoothFactor);
            game_sdk->set_aim(LocalPlayer, SmoothedRot);
        }
        else
        {
            // Normal aim: instant snap
            game_sdk->set_aim(LocalPlayer, TargetLook);
        }
    }
}

void get_players()
{
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    int numberOfPlayersAround = 0;
    if (!draw_list)
        return;

    // Aimbot runs independently of ESP
    try
    {
        if (Vars.Aimbot)
        {
            ProcessAimbot();
        }

        if (Vars.UpPlayerOne)
        {
            UpOneEnemy();
        }
    }
    catch (...)
    {
    }

    // ESP drawing - skip everything below if ESP is disabled
    if (!Vars.Enable)
        return;

    try
    {
        void *current_Match = game_sdk->Curent_Match();
        if (!current_Match)
            return;

        void *local_player = game_sdk->GetLocalPlayer(current_Match);
        if (!local_player)
            return;

        void *playersDict = *(void **)((long)current_Match + 0x148);
        if (!playersDict)
            return;
        IL2CppPlayerDict players(playersDict);

        void *camera = game_sdk->get_camera();
        if (!camera)
            return;

        for (int u = 0; u < players.getCount(); u++)
        {
            void *closestEnemy = players.getPlayer(u);
            if (!closestEnemy ||
                !game_sdk->Component_GetTransform(closestEnemy) ||
                closestEnemy == local_player ||
                !game_sdk->get_MaxHP(closestEnemy) ||
                game_sdk->get_isLocalTeam(closestEnemy))
            {
                continue;
            }
            numberOfPlayersAround++;

            Vector3 pos = getPosition(closestEnemy);
            Vector3 pos2 = getPosition(local_player);
            float distance = Vector3::Distance(pos, pos2);
            if (distance > 200.0f)
                continue;

            bool isEnemyDead = game_sdk->get_IsDieing(closestEnemy);
            bool isEnemyVisible = tanghinh::isVisible(closestEnemy);

            bool w2sc;
            ImVec2 top_pos = Camera$$WorldToScreen::Regular(pos + Vector3(0, 1.6, 0));
            ImVec2 bot_pos = Camera$$WorldToScreen::Regular(pos);
            ImVec2 pos_3 = Camera$$WorldToScreen::Checker(pos, w2sc);
            auto pmtXtop = top_pos.x;
            auto pmtXbottom = bot_pos.x;
            if (top_pos.x > bot_pos.x)
            {
                pmtXtop = bot_pos.x;
                pmtXbottom = top_pos.x;
            }
            Camera$$WorldToScreen::Checker(pos + Vector3(0, 0.75f, 0), w2sc);
            float calculatedPosition = fabs((top_pos.y - bot_pos.y) * (0.0092f / 0.019f) / 2);

            ImRect rect(
                ImVec2(pmtXtop - calculatedPosition, top_pos.y),
                ImVec2(pmtXbottom + calculatedPosition, bot_pos.y));
            Vector3 viewpos = Vector3::zero();
            {
                void *_cam = game_sdk->get_camera();
                if (_cam)
                {
                    void *_camT = game_sdk->Component_GetTransform(_cam);
                    if (_camT)
                        viewpos = game_sdk->get_position(_camT);
                }
            }

            if (w2sc)
            {
                if (Vars.lines)
                {
                    DrawLine(draw_list, ImVec2(ImGui::GetIO().DisplaySize.x / 2, 15), ImVec2(rect.GetCenter().x, rect.Min.y), 1.0f, isEnemyDead, isEnemyVisible);
                }

                if (Vars.Box)
                {
                    DrawLine(draw_list, rect.Min, ImVec2(rect.Max.x, rect.Min.y), 1.0f, isEnemyDead, isEnemyVisible);
                    DrawLine(draw_list, ImVec2(rect.Max.x, rect.Min.y), rect.Max, 1.0f, isEnemyDead, isEnemyVisible);
                    DrawLine(draw_list, rect.Max, ImVec2(rect.Min.x, rect.Max.y), 1.0f, isEnemyDead, isEnemyVisible);
                    DrawLine(draw_list, ImVec2(rect.Min.x, rect.Max.y), rect.Min, 1.0f, isEnemyDead, isEnemyVisible);

                    if (Vars.Outline)
                    {
                        draw_list->AddRect(ImVec2(rect.Min.x - 1, rect.Min.y - 1), ImVec2(rect.Max.x + 1, rect.Max.y + 1), ImColor(0, 0, 0), 0.70, 0, 1);
                        draw_list->AddRect(ImVec2(rect.Min.x + 1, rect.Min.y + 1), ImVec2(rect.Max.x - 1, rect.Max.y - 1), ImColor(0, 0, 0), 0.70, 0, 1);
                    }
                }

                if (Vars.Health)
                {
                    auto health = game_sdk->GetHp(closestEnemy);
                    auto maxhealth = game_sdk->get_MaxHP(closestEnemy);
                    float health_multiplier = (float)health / (float)maxhealth;
                    if (health_multiplier < 0)
                        health_multiplier = 0;
                    if (health_multiplier > 1)
                        health_multiplier = 1;
                    float health_bar_pos = rect.Min.x - 4;
                    DrawHealthBar(draw_list, ImVec2(health_bar_pos, rect.Min.y - 1), ImVec2(health_bar_pos, rect.Max.y), health_multiplier, 3.0f, isEnemyDead);
                }

                if (Vars.Name)
                {
                    auto pname = game_sdk->name(closestEnemy);
                    std::string names = "null";
                    if (pname)
                        names = pname->toCPPString();
                    std::transform(names.begin(), names.end(), names.begin(), ::tolower);
                    std::string name = names;
                    ImVec2 text_size = verdana_smol->CalcTextSizeA(8, FLT_MAX, 0, names.c_str());
                    ImVec2 name_pos = {
                        rect.Min.x + (rect.GetWidth() / 2) - text_size.x / 2,
                        rect.Min.y - 2 - text_size.y};
                    draw_list->AddText(verdana_smol, 8.0f, name_pos, ImColor(nameColor), name.c_str());
                }

                if (Vars.Distance)
                {
                    std::string distancestr = fmt::format(oxorany("{}M"), static_cast<int>(distance));
                    ImVec2 distance_pos = {
                        rect.Max.x + 4,
                        rect.Min.y};
                    AddText(verdana_smol, 8, false, false, distance_pos, ImColor(distanceColor), distancestr.c_str());
                }

                if (Vars.skeleton)
                {
                    DrawSkeleton(closestEnemy, draw_list);
                }
            }

            if (Vars.OOF)
            {

                struct Disp
                {
                    float width;
                    float height;
                };
                Disp disp = {ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y};
                if ((pos_3.x < 0 || pos_3.x > disp.width) || (pos_3.y < 0 || pos_3.y > disp.height) || !w2sc)
                {
                    constexpr int maxpixels = 150;
                    int pixels = maxpixels;
                    if (w2sc)
                    {
                        if (pos_3.x < 0)
                            pixels = clamp((int)-pos_3.x, 0, (int)maxpixels);
                        if (pos_3.y < 0)
                            pixels = clamp((int)-pos_3.y, 0, (int)maxpixels);

                        if (pos_3.x > disp.width)
                            pixels = clamp((int)pos_3.x - (int)disp.width, 0, (int)maxpixels);
                        if (pos_3.y > disp.height)
                            pixels = clamp((int)pos_3.y - (int)disp.height, 0, (int)maxpixels);
                    }

                    float opacity = (float)pixels / (float)maxpixels;

                    float size = 3.5f;
                    Vector3 viewdir = Vector3::zero();
                    {
                        void *_cam2 = game_sdk->get_camera();
                        if (_cam2)
                        {
                            void *_camT2 = game_sdk->Component_GetTransform(_cam2);
                            if (_camT2)
                                viewdir = game_sdk->GetForward(_camT2);
                        }
                    }
                    Vector3 targetdir = Vector3::Normalized(pos - viewpos);

                    float viewangle = atan2(viewdir.z, viewdir.x) * Rad2Deg;
                    float targetangle = atan2(targetdir.z, targetdir.x) * Rad2Deg;

                    if (viewangle < 0)
                        viewangle += 360;
                    if (targetangle < 0)
                        targetangle += 360;

                    float angle = targetangle - viewangle;

                    while (angle < 0)
                        angle += 360;
                    while (angle > 360)
                        angle -= 360;

                    angle = 360 - angle;
                    angle -= 90;
                    OtFovV1(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2, 90 + distance * 2,
                            angle - size,
                            angle + size,
                            ImColor(1.f, 1.f, 1.f, 1.f * opacity), 1);
                }
            }
        }

        if (Vars.counts)
        {
            ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
            ImVec2 numberPosition(ImGui::GetIO().DisplaySize.x / 2.0f, 12);
            std::string countText = "Inimigos: " + std::to_string(numberOfPlayersAround);
            ImVec2 textSize = verdana_smol->CalcTextSizeA(25.0f, FLT_MAX, 0, countText.c_str());
            ImVec2 textPos(
                numberPosition.x - textSize.x / 2.0f,
                numberPosition.y);

            draw_list->AddText(verdana_smol, 25.0f, textPos, ImColor(255, 255, 255, 255), countText.c_str());
        }
    }
    catch (...)
    {
        return;
    }
}

void RunTelekill()
{
    if (!istelekill)
        return;

    void *match = game_sdk->Curent_Match();
    if (!match)
        return;

    void *local = game_sdk->GetLocalPlayer(match);
    if (!local || !game_sdk->Component_GetTransform(local))
        return;

    void *playersDict = *(void **)((long)match + 0x148);
    if (!playersDict)
        return;
    IL2CppPlayerDict players(playersDict);

    void *localTF = game_sdk->Component_GetTransform(local);
    Vector3 localPos = game_sdk->get_position(localTF);
    Vector3 forward = game_sdk->GetForward(localTF);

    for (int i = 0; i < players.getCount(); i++)
    {
        void *enemy = players.getPlayer(i);
        if (!enemy || enemy == local)
            continue;
        if (!game_sdk->Component_GetTransform(enemy))
            continue;
        if (!game_sdk->get_MaxHP(enemy))
            continue;
        if (game_sdk->get_IsDieing(enemy))
            continue;
        if (game_sdk->GetHp(enemy) <= 0)
            continue;
        if (game_sdk->get_isLocalTeam(enemy))
            continue;

        void *enemyTF = game_sdk->Component_GetTransform(enemy);
        Vector3 enemyPos = game_sdk->get_position(enemyTF);
        float distance = Vector3::Distance(localPos, enemyPos);

        if (distance > 8.0f)
            continue;

        Vector3 stableFront = localPos + forward * 1.2f;
        stableFront.y = localPos.y;

        Transform_INTERNAL_SetPosition(enemyTF, Vector3(stableFront.x, stableFront.y, stableFront.z));
    }
}

void aimbot()
{
    ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
    if (!Vars.Aimbot)
        return;
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    if (!draw_list)
        return;
    void *Match = game_sdk->Curent_Match();
    if (!Match)
        return;

    if (Vars.Aimbot && Vars.isAimFov)
    {
        if (Vars.fovaimglow)
            drawcircleglow(draw_list, center, Vars.AimFov, ImColor(1.0f, 1.0f, 1.0f, 1.0f), 999, 1, 12);
        else
            draw_list->AddCircle(center, Vars.AimFov, ImColor(1.0f, 1.0f, 1.0f, 1.0f), 100);
    }

    void *LocalPlayer = game_sdk->GetLocalPlayer(Match);
    if (!LocalPlayer || !game_sdk->Component_GetTransform(LocalPlayer))
        return;

    void *playertarget = GetClosestEnemy();
    if (!playertarget)
        return;

    ImVec2 EnemyLocation = Camera$$WorldToScreen::Regular(GetHitboxPosition(playertarget, Vars.AimHitbox));
    ImColor aimbotColor = ImColor(255, 255, 255);

    ProcessAimbot();
}