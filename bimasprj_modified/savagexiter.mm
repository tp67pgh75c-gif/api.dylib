#import "Esp/ImGuiDrawView.h"
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <Foundation/Foundation.h>
#import "APIClient.h"
// Icons defined in Helper/data.h
#include <iostream>
#include <UIKit/UIKit.h>
#include <vector>
#include "iconcpp.h"
#import "pthread.h"
#include <array>
#include <cmath>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#import "JRMemory.framework/Headers/MemScan.h"
#import "Esp/CaptainHook.h"
#import "Esp/ImGuiDrawView.h"
#import "IMGUI/imgui.h"
#import "IMGUI/imgui_internal.h"
#import "IMGUI/imgui_impl_metal.h"
#import "IMGUI/zzz.h"
#include "oxorany/oxorany_include.h"
#import "Helper/Mem.h"
#include "font.h"
#import "Esp/Includes.h"
#import "Helper/Vector3.h"
#import "Helper/Vector2.h"
#import "Helper/Quaternion.h"
#import "Helper/Monostring.h"
#include "Helper/font.h"
#include "Helper/data.h"
#include "ban.cpp"

ImFont* verdana_smol;
ImFont* pixel_big = {};
ImFont* pixel_smol = {};

#include "Helper/Obfuscate.h"
#import "Helper/Hooks.h"
#import "IMGUI/zzz.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <unistd.h>
#include <string.h>
#include "hook/hook.h"

// ─── ZOMBI MODE KEY SYSTEM ──────────────────────────────────────────────────
static bool isKeyVerified = false;

static void showKeyDialog();

static void verifyKey(NSString *key) {
    [[APIClient sharedAPIClient] onLogin:key
        onSuccess:^(NSDictionary *data) {
            isKeyVerified = true;
            [[NSUserDefaults standardUserDefaults] setObject:key forKey:@"zombi_saved_key"];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }
        onFailure:^(NSDictionary *error) {
            isKeyVerified = false;
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                showKeyDialog();
            });
        }
    ];
}

static void showKeyDialog() {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alert = [UIAlertController
            alertControllerWithTitle:@"🧟 ZOMBI MODE"
            message:@"Enter your key to unlock"
            preferredStyle:UIAlertControllerStyleAlert];

        [alert addTextFieldWithConfigurationHandler:^(UITextField *tf) {
            tf.placeholder = @"ZOMBI-DAY-XXXXXXXXXXXXXXXX";
            tf.autocorrectionType = UITextAutocorrectionTypeNo;
            tf.autocapitalizationType = UITextAutocapitalizationTypeAllCharacters;
        }];

        UIAlertAction *unlock = [UIAlertAction
            actionWithTitle:@"Unlock"
            style:UIAlertActionStyleDefault
            handler:^(UIAlertAction *action) {
                NSString *key = alert.textFields.firstObject.text;
                if (key.length > 0) {
                    verifyKey(key);
                } else {
                    showKeyDialog();
                }
            }
        ];

        [alert addAction:unlock];

        UIViewController *root = [UIApplication sharedApplication].windows.firstObject.rootViewController;
        while (root.presentedViewController) root = root.presentedViewController;
        [root presentViewController:alert animated:YES completion:nil];
    });
}

static void checkKeyOnStart() {
    NSString *savedKey = [[NSUserDefaults standardUserDefaults] stringForKey:@"zombi_saved_key"];
    if (savedKey.length > 0) {
        [[APIClient sharedAPIClient] onCheckPackage:^(NSDictionary *header) {
            isKeyVerified = true;
        } onFailure:^(NSDictionary *error) {
            isKeyVerified = false;
            showKeyDialog();
        }];
        // Pre-set the key so check-package works
        [[NSUserDefaults standardUserDefaults] setObject:savedKey forKey:@"zombi_saved_key"];
        verifyKey(savedKey);
    } else {
        showKeyDialog();
    }
}
// ────────────────────────────────────────────────────────────────────────────

// Simple menu state (no Security.h needed)
static bool _menuOpen = false;
static int activeTab = 0;
static bool StreamerMode = true;

// Aim fire control sync vars
static bool autoAim = true;
static bool aimFire = false;
static bool aimScope = false;
static bool fireScope = false;
static bool aimSilent = false;
static bool aimVectored = false;
static float aimVectoredSpeed = 10.0f;

ImVec4 userColor = ImVec4(0.0f, 0.7f, 1.0f, 1.0f);
ImVec4 espv = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
ImVec4 espi = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 fovColor = ImVec4(0.0f, 0.7f, 1.0f, 1.0f);
ImVec4 nameColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 distanceColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

#define kWidth  [UIScreen mainScreen].bounds.size.width
#define kHeight [UIScreen mainScreen].bounds.size.height

UIWindow *mainWindow;
UIButton *menuView;
bool istelekill = false;
bool isfly = false;

@interface ImGuiDrawView () <MTKViewDelegate>
@property (nonatomic, strong) id <MTLDevice> device;
@property (nonatomic, strong) id <MTLCommandQueue> commandQueue;
@end

@implementation ImGuiDrawView
ImFont *_espFont;
ImFont* verdanab;
ImFont* icons;
ImFont* interb;
ImFont* Urbanist;

// === Logic Run (aimbot only) ===
void Logic_Run() {
    try {
        if (!game_sdk) return;
        if (!game_sdk->Curent_Match || !game_sdk->GetLocalPlayer || !game_sdk->Component_GetTransform) return;
        
        void* match = game_sdk->Curent_Match();
        if (!match) return;
        void* local = game_sdk->GetLocalPlayer(match);
        if (!local) return;
        void* tf = game_sdk->Component_GetTransform(local);
        if (!tf) return;

        // Sync aim fire control
        if (Vars.Aimbot) {
            if (autoAim) Vars.AimWhen = 0;
            else if (aimFire) Vars.AimWhen = 1;
            else if (aimScope) Vars.AimWhen = 2;
            else if (fireScope) Vars.AimWhen = 3;
        }
        
        Vars.AimSilent = aimSilent;
        Vars.AimVectored = aimVectored;
        Vars.AimVectoredSpeed = aimVectoredSpeed;
    } catch(...) {}
}

bool antiban(void *instance) {
    return false;
}

- (instancetype)initWithNibName:(nullable NSString *)nibNameOrNil bundle:(nullable NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];

    _device = MTLCreateSystemDefaultDevice();
    _commandQueue = [_device newCommandQueue];

    if (!self.device) abort();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuiStyle& style = ImGui::GetStyle();

    ImVec4 accent    = ImVec4(0.00f, 0.70f, 1.00f, 1.00f);
    ImVec4 accentHover = ImVec4(0.00f, 0.60f, 0.90f, 1.00f);
    ImVec4 accentActive = ImVec4(0.00f, 0.80f, 1.00f, 1.00f);
    ImVec4 darkBg       = ImVec4(0.02f, 0.05f, 0.10f, 0.95f);
    ImVec4 elementBg    = ImVec4(0.05f, 0.15f, 0.30f, 0.54f);

    style.Colors[ImGuiCol_WindowBg] = darkBg;
    style.Colors[ImGuiCol_ChildBg]  = elementBg;
    style.Colors[ImGuiCol_PopupBg]  = darkBg;
    style.Colors[ImGuiCol_Border]   = accent;
    style.Colors[ImGuiCol_FrameBg]        = elementBg;
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]  = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]          = accent;
    style.Colors[ImGuiCol_TitleBgActive]    = accent;
    style.Colors[ImGuiCol_TitleBgCollapsed] = accent;
    style.Colors[ImGuiCol_Button]           = elementBg;
    style.Colors[ImGuiCol_ButtonHovered]    = accentHover;
    style.Colors[ImGuiCol_ButtonActive]     = accent;
    style.Colors[ImGuiCol_CheckMark]        = accent;
    style.Colors[ImGuiCol_SliderGrab]       = accent;
    style.Colors[ImGuiCol_SliderGrabActive] = accentHover;
    style.Colors[ImGuiCol_Header]           = accent;
    style.Colors[ImGuiCol_HeaderHovered]    = accentHover;
    style.Colors[ImGuiCol_HeaderActive]     = accentActive;
    style.Colors[ImGuiCol_Tab]              = elementBg;
    style.Colors[ImGuiCol_TabHovered]       = accentHover;
    style.Colors[ImGuiCol_TabActive]        = accent;
    style.Colors[ImGuiCol_ScrollbarBg]      = darkBg;
    style.Colors[ImGuiCol_ScrollbarGrab]    = accent;
    style.Colors[ImGuiCol_Separator]        = accent;

    style.WindowRounding = 12.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 12.0f;
    style.PopupRounding = 10.0f;
    style.ScrollbarRounding = 10.0f;
    style.TabRounding = 8.0f;
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.WindowPadding = ImVec2(16.0f, 16.0f);
    style.FramePadding = ImVec2(10.0f, 6.0f);
    style.ItemSpacing = ImVec2(12.0f, 8.0f);

    static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
    ImFontConfig icons_config;
    ImFontConfig CustomFont;
    CustomFont.FontDataOwnedByAtlas = false;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 21.f, &CustomFont);
    io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 19.0f, &icons_config, icons_ranges);
    io.Fonts->AddFontDefault();
    ImFont* font = io.Fonts->AddFontFromMemoryTTF(sansbold, sizeof(sansbold), 21.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    verdana_smol = io.Fonts->AddFontFromMemoryTTF(verdana, sizeof verdana, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());
    pixel_big = io.Fonts->AddFontFromMemoryTTF((void*)smallestpixel, sizeof smallestpixel, 400, NULL, io.Fonts->GetGlyphRangesCyrillic());
    pixel_smol = io.Fonts->AddFontFromMemoryTTF((void*)smallestpixel, sizeof smallestpixel, 10*2, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImGui_ImplMetal_Init(_device);

    return self;
}

+ (void)showChange:(BOOL)open {
    _menuOpen = open;
}

+ (BOOL)isMenuShowing {
    return _menuOpen;
}

- (MTKView *)mtkView {
    return (MTKView *)self.view;
}

- (void)loadView
{
    CGFloat w = [UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.width;
    CGFloat h = [UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.height;
    self.view = [[MTKView alloc] initWithFrame:CGRectMake(0, 0, w, h)];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.mtkView.device = self.device;
    self.mtkView.delegate = self;
    self.mtkView.clearColor = MTLClearColorMake(0, 0, 0, 0);
    self.mtkView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0];
    self.mtkView.clipsToBounds = YES;
}

#pragma mark - Interaction

- (void)updateIOWithTouchEvent:(UIEvent *)event
{
    UITouch *anyTouch = event.allTouches.anyObject;
    CGPoint touchLocation = [anyTouch locationInView:self.view];
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(touchLocation.x, touchLocation.y);

    BOOL hasActiveTouch = NO;
    for (UITouch *touch in event.allTouches)
    {
        if (touch.phase != UITouchPhaseEnded && touch.phase != UITouchPhaseCancelled)
        {
            hasActiveTouch = YES;
            break;
        }
    }
    io.MouseDown[0] = hasActiveTouch;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { [self updateIOWithTouchEvent:event]; }
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { [self updateIOWithTouchEvent:event]; }
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { [self updateIOWithTouchEvent:event]; }
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event { [self updateIOWithTouchEvent:event]; }

// ===== UI HELPERS =====
static bool ToggleSwitch(const char* label, bool* v) {
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float h = ImGui::GetFrameHeight() * 0.70f;
    float w = h * 1.75f;
    float r = h * 0.5f;
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    float totalW = w + 8.0f + labelSize.x;
    ImGui::InvisibleButton(label, ImVec2(totalW, h));
    bool clicked = ImGui::IsItemClicked();
    if (clicked) *v = !*v;
    ImU32 bg = *v ? IM_COL32(0, 210, 110, 255) : IM_COL32(50, 55, 68, 255);
    dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), bg, r);
    if (*v) {
        dl->AddRect(ImVec2(p.x-1, p.y-1), ImVec2(p.x+w+1, p.y+h+1), IM_COL32(0, 210, 110, 90), r+1, 0, 2.0f);
    }
    float kx = *v ? (p.x + w - r) : (p.x + r);
    dl->AddCircleFilled(ImVec2(kx, p.y + r), r - 2.0f, IM_COL32(255, 255, 255, 255));
    dl->AddText(ImVec2(p.x + w + 8, p.y + (h - ImGui::GetTextLineHeight()) * 0.5f), IM_COL32(220, 225, 235, 255), label);
    return clicked;
}

static void SectionHeader(const char* text, ImVec4 color) {
    ImGui::Spacing();
    ImGui::TextColored(color, "%s", text);
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
    float w = ImGui::GetContentRegionAvail().x;
    ImU32 lineCol = IM_COL32((int)(color.x*255), (int)(color.y*255), (int)(color.z*255), 60);
    dl->AddLine(ImVec2(p.x, p.y), ImVec2(p.x + w, p.y), lineCol, 1.0f);
    ImGui::Spacing();
}

// ===== SAVE / LOAD (aimbot-only) =====
static bool settingsSaved = false;
static bool settingsLoaded = false;
static float settingsMsgTimer = 0;

static void SaveAllSettings() {
    @try {
    NSMutableDictionary *d = [NSMutableDictionary dictionary];
    d[@"aimbot"] = @(Vars.Aimbot);
    d[@"vischeck"] = @(Vars.VisibleCheck);
    d[@"ignknock"] = @(Vars.IgnoreKnocked);
    d[@"aimfov"] = @(Vars.AimFov);
    d[@"hitbox"] = @(Vars.AimHitbox);
    d[@"aimwhen"] = @(Vars.AimWhen);
    d[@"drawfov"] = @(Vars.isAimFov);
    d[@"aimsilent"] = @(aimSilent);
    d[@"aimvec"] = @(aimVectored);
    d[@"aimvecspd"] = @(aimVectoredSpeed);
    d[@"fx"] = @(fovColor.x); d[@"fy"] = @(fovColor.y); d[@"fz"] = @(fovColor.z); d[@"fw"] = @(fovColor.w);
    NSString *path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"_fk5_cfg.plist"];
    [d writeToFile:path atomically:YES];
    [[NSUserDefaults standardUserDefaults] setObject:d forKey:@"_fk5_c"];
    [[NSUserDefaults standardUserDefaults] synchronize];
    settingsSaved = true; settingsMsgTimer = 3.0f;
    } @catch(...) {}
}

static void LoadAllSettings() {
    @try {
    NSDictionary *d = nil;
    NSString *path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"_fk5_cfg.plist"];
    if ([[NSFileManager defaultManager] fileExistsAtPath:path]) {
        d = [NSDictionary dictionaryWithContentsOfFile:path];
    }
    if (!d) d = [[NSUserDefaults standardUserDefaults] dictionaryForKey:@"_fk5_c"];
    if (!d) { settingsLoaded = false; return; }
    Vars.Aimbot = [d[@"aimbot"] boolValue];
    Vars.VisibleCheck = [d[@"vischeck"] boolValue];
    Vars.IgnoreKnocked = [d[@"ignknock"] boolValue];
    Vars.AimFov = [d[@"aimfov"] floatValue];
    Vars.AimHitbox = [d[@"hitbox"] intValue];
    Vars.AimWhen = [d[@"aimwhen"] intValue];
    Vars.isAimFov = [d[@"drawfov"] boolValue];
    aimSilent = [d[@"aimsilent"] boolValue];
    aimVectored = [d[@"aimvec"] boolValue];
    aimVectoredSpeed = [d[@"aimvecspd"] floatValue];
    fovColor = ImVec4([d[@"fx"] floatValue], [d[@"fy"] floatValue], [d[@"fz"] floatValue], [d[@"fw"] floatValue]);
    if (Vars.AimWhen == 0) { autoAim = true; aimFire = false; aimScope = false; fireScope = false; }
    else if (Vars.AimWhen == 1) { autoAim = false; aimFire = true; aimScope = false; fireScope = false; }
    else if (Vars.AimWhen == 2) { autoAim = false; aimFire = false; aimScope = true; fireScope = false; }
    else if (Vars.AimWhen == 3) { autoAim = false; aimFire = false; aimScope = false; fireScope = true; }
    settingsLoaded = true; settingsMsgTimer = 3.0f;
    } @catch(...) {}
}

#pragma mark - MTKViewDelegate

- (void)drawInMTKView:(MTKView*)view
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = view.bounds.size.width;
    io.DisplaySize.y = view.bounds.size.height;
    CGFloat framebufferScale = view.window.screen.nativeScale ?: UIScreen.mainScreen.nativeScale;
    io.DisplayFramebufferScale = ImVec2(framebufferScale, framebufferScale);
    io.DeltaTime = 1 / float(view.preferredFramesPerSecond ?: 60);

    Logic_Run();

    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
    
    hideRecordTextfield.secureTextEntry = StreamerMode;

    // Block menu if key not verified
    if (!isKeyVerified) _menuOpen = false;

    if (_menuOpen) 
    {
        [self.view setUserInteractionEnabled:YES];
        [menuTouchView setUserInteractionEnabled:YES];
        if(menuView) menuView.hidden = NO;
    } 
    else
    {
        [self.view setUserInteractionEnabled:NO];
        [menuTouchView setUserInteractionEnabled:NO];
    }

    MTLRenderPassDescriptor* renderPassDescriptor = view.currentRenderPassDescriptor;
    if (renderPassDescriptor != nil) 
    {
        id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        [renderEncoder pushDebugGroup:@"ImGui Jane"];

        ImGui_ImplMetal_NewFrame(renderPassDescriptor);
        ImGui::NewFrame();
        ImVec4 col_main = ImVec4(0.0f, 0.7f, 1.0f, 1.0f);
        ImGuiStyle& style = ImGui::GetStyle();
        ImFont* font = ImGui::GetFont();
        font->Scale = 16.f / font->FontSize;

        CGFloat x = (([UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.width) - 420) / 2;
        CGFloat y = (([UIApplication sharedApplication].windows[0].rootViewController.view.frame.size.height) - 340) / 2;

        ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420, 340), ImGuiCond_FirstUseEver);

        static bool _uiOpen = false;
        _uiOpen = _menuOpen;
        if (_uiOpen)
        {
            ImGuiWindowFlags wflags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
            ImGui::Begin("##_p", &_uiOpen, wflags);
            if (!_uiOpen) _menuOpen = false;
            
            // Glassmorphism
            style.WindowRounding = 14.0f;
            style.ChildRounding = 10.0f;
            style.FrameRounding = 8.0f;
            style.GrabRounding = 12.0f;
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.06f, 0.12f, 0.92f);
            style.Colors[ImGuiCol_ChildBg] = ImVec4(0.04f, 0.08f, 0.16f, 0.60f);
            style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.7f, 1.0f, 0.15f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.12f, 0.22f, 0.70f);
            style.Colors[ImGuiCol_CheckMark] = col_main;
            style.Colors[ImGuiCol_SliderGrab] = col_main;
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.85f, 1.0f, 1.0f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.06f, 0.12f, 0.22f, 0.70f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.7f, 1.0f, 0.60f);
            style.Colors[ImGuiCol_ButtonActive] = col_main;
            
            // Neon border
            {
                ImVec2 wp = ImGui::GetWindowPos();
                ImVec2 ws = ImGui::GetWindowSize();
                ImDrawList* wdl = ImGui::GetWindowDrawList();
                float rnd = style.WindowRounding;
                wdl->AddRect(wp, ImVec2(wp.x+ws.x, wp.y+ws.y), IM_COL32(0, 180, 255, 160), rnd, 0, 1.5f);
                wdl->AddRect(ImVec2(wp.x-2, wp.y-2), ImVec2(wp.x+ws.x+2, wp.y+ws.y+2), IM_COL32(0, 180, 255, 50), rnd+2, 0, 2.5f);
            }
            
            // Header
            ImGui::TextColored(ImVec4(0.0f, 0.85f, 1.0f, 1.0f), "@FAKHERDDIN5");
            ImGui::SameLine(ImGui::GetWindowWidth() - 80);
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.4f, 1.0f), "Aimbot");
            {
                ImVec2 sp = ImGui::GetCursorScreenPos();
                float sw = ImGui::GetContentRegionAvail().x;
                ImDrawList* sdl = ImGui::GetWindowDrawList();
                sdl->AddLine(sp, ImVec2(sp.x + sw, sp.y), IM_COL32(0, 180, 255, 100), 1.0f);
            }
            ImGui::Spacing();
            
            // Sidebar + Content
            ImGui::Columns(2, "MainLayout", false);
            ImGui::SetColumnWidth(0, 110);
            
            // Sidebar
            {
                ImVec4 tabColors[] = {
                    ImVec4(0.20f, 0.50f, 1.00f, 1.00f),
                    ImVec4(0.20f, 0.75f, 0.40f, 1.00f),
                };
                const char* sLabels[] = { ICON_FA_CROSSHAIRS "  Aim", ICON_FA_SLIDERS "  Setting" };
                for (int i = 0; i < 2; i++) {
                    ImVec4 c = tabColors[i];
                    if (activeTab == i) {
                        ImGui::PushStyleColor(ImGuiCol_Button, c);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(c.x*1.1f, c.y*1.1f, c.z*1.1f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, c);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                    } else {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(c.x*0.22f, c.y*0.22f, c.z*0.22f, 0.45f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(c.x*0.50f, c.y*0.50f, c.z*0.50f, 0.65f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(c.x*0.80f, c.y*0.80f, c.z*0.80f, 0.85f));
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.65f, 0.75f, 1.0f));
                    }
                    if (ImGui::Button(sLabels[i], ImVec2(96, 34))) activeTab = i;
                    ImGui::PopStyleColor(4);
                    if (i < 1) ImGui::Spacing();
                }
            }
            
            ImGui::NextColumn();
            ImGui::BeginChild("ContentArea", ImVec2(0, 0), true, ImGuiWindowFlags_None);
            
            // ========= TAB 0: AIM =========
            if (activeTab == 0)
            {
                SectionHeader("[ AIM SYSTEM ]", col_main);
                
                ToggleSwitch("Enable Aim Assist", &Vars.Aimbot);
                ToggleSwitch("Visible Check", &Vars.VisibleCheck);
                ToggleSwitch("Ignore Knocked", &Vars.IgnoreKnocked);
                
                ImGui::Spacing();
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.0f, 0.75f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.0f, 0.90f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.08f, 0.15f, 0.25f, 0.80f));
                ImGui::PushItemWidth(-1);
                ImGui::SliderFloat("##AimFov", &Vars.AimFov, 0.0f, 360.0f, "Aim Strength: %.0f%%");
                ImGui::PopItemWidth();
                ImGui::PopStyleColor(3);
                
                ImGui::Spacing();
                ImGui::Combo("Target", &Vars.AimHitbox, Vars.aimHitboxes, 3);
                
                SectionHeader("[ FIRE CONTROL ]", col_main);
                
                ImGui::RadioButton("Auto Aim", &Vars.AimWhen, 0);
                ImGui::RadioButton("On Fire", &Vars.AimWhen, 1);
                ImGui::RadioButton("On Scope", &Vars.AimWhen, 2);
                ImGui::RadioButton("Fire + Scope", &Vars.AimWhen, 3);
                
                ImGui::Spacing();
                ToggleSwitch("Draw FOV Circle", &Vars.isAimFov);
                
                SectionHeader("[ ADVANCED AIM ]", ImVec4(0.90f, 0.30f, 0.90f, 1.0f));
                
                ToggleSwitch("Aim Silent", &aimSilent);
                ToggleSwitch("Aim Vectored", &aimVectored);
                if (aimVectored) {
                    ImGui::Indent(20);
                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.90f, 0.30f, 0.90f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.40f, 1.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.08f, 0.18f, 0.80f));
                    ImGui::PushItemWidth(-1);
                    ImGui::SliderFloat("##VecSpeed", &aimVectoredSpeed, 1.0f, 20.0f, "Snap Speed: %.0f");
                    ImGui::PopItemWidth();
                    ImGui::PopStyleColor(3);
                    ImGui::Unindent(20);
                }
            }
            
            // ========= TAB 1: SETTING =========
            else if (activeTab == 1)
            {
                SectionHeader("[ STREAM MODE ]", ImVec4(0.90f, 0.30f, 0.30f, 1.0f));
                ToggleSwitch("Hide from Recording", &StreamerMode);
                
                SectionHeader("[ COLORS ]", col_main);
                ImGui::PushItemWidth(40.0f);
                ImGui::ColorEdit4("FOV Color", (float*)&fovColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview);
                ImGui::PopItemWidth();
                
                SectionHeader("[ SETTINGS ]", ImVec4(0.20f, 0.75f, 0.40f, 1.0f));
                
                static bool doSave = false;
                static bool doLoad = false;
                if (ToggleSwitch("Save Settings", &doSave)) {
                    if (doSave) { SaveAllSettings(); doSave = false; }
                }
                if (ToggleSwitch("Load Settings", &doLoad)) {
                    if (doLoad) { LoadAllSettings(); doLoad = false; }
                }
                
                if (settingsMsgTimer > 0) {
                    settingsMsgTimer -= ImGui::GetIO().DeltaTime;
                    ImGui::Spacing();
                    if (settingsSaved) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.4f, 1.0f), "Settings Saved!");
                    else if (settingsLoaded) ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "Settings Loaded!");
                    if (settingsMsgTimer <= 0) { settingsSaved = false; settingsLoaded = false; }
                }
                
                ImGui::Spacing();
                SectionHeader("[ EMERGENCY ]", ImVec4(0.90f, 0.10f, 0.10f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.80f, 0.05f, 0.05f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.10f, 0.10f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.60f, 0.0f, 0.0f, 1.0f));
                if (ImGui::Button("PANIC: DISABLE ALL & HIDE", ImVec2(-1, 40))) {
                    Vars.Aimbot = false;
                    aimSilent = false;
                    aimVectored = false;
                    _menuOpen = false;
                }
                ImGui::PopStyleColor(3);
            }
            
            ImGui::EndChild();
            ImGui::Columns(1);
            ImGui::End();
        }
        
        // Draw aimbot overlay
        get_players();
        aimbot();
        game_sdk->init();

        if (Vars.isAimFov && Vars.AimFov > 0) {
            ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
            ImGui::GetBackgroundDrawList()->AddCircle(center, Vars.AimFov, ImColor(fovColor), 100, 2.0f);
        }

        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        ImGui_ImplMetal_RenderDrawData(draw_data, commandBuffer, renderEncoder);
        [renderEncoder popDebugGroup];
        [renderEncoder endEncoding];
        [commandBuffer presentDrawable:view.currentDrawable];
        [commandBuffer commit];
    } 
} 

- (void)mtkView:(MTKView*)view drawableSizeWillChange:(CGSize)size {}

void hooking() {
    // Antiban hook only
    void* address[] = {
        (void*)getRealOffset(ENCRYPTOFFSET("0x101CFACB4"))
    };
    void* function[] = {
        (void*)antiban                                                     
    };
    hook(address, function, 1);
}

void *hack_thread(void *) {
    sleep(5);
    hooking();
    // Check key after hooks are loaded
    dispatch_async(dispatch_get_main_queue(), ^{
        checkKeyOnStart();
    });
    pthread_exit(nullptr);
    return nullptr;
}

void __attribute__((constructor)) initialize() {
    pthread_t hacks;
    pthread_create(&hacks, NULL, hack_thread, NULL); 
}
@end
