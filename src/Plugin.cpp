#include "API/ARK/Ark.h"

#include <fstream>
#include <Permissions.h>
#include <Points.h>

#include "Plugin.h"

#include "Utils.h"

// other functions includes
#include "Tests.h"

#include "Hooks.h"

#include "Timer.h"

#include "Commands.h"

#pragma comment(lib, "AsaApi.lib")
#pragma comment(lib, "Permissions.lib")
#pragma comment(lib, "ArkShop.lib")





// Called when the server is ready, do post-"server ready" initialization here
void OnServerReady()
{
    Log::GetLog()->info("Hello, world!");

    LoadConfig();
    AddRemoveCommands();
}

// Hook that triggers once when the server is ready
DECLARE_HOOK(AShooterGameMode_BeginPlay, void, AShooterGameMode*);
void Hook_AShooterGameMode_BeginPlay(AShooterGameMode* _this)
{
    AShooterGameMode_BeginPlay_original(_this);

    OnServerReady();
}

// Called by AsaApi when the plugin is loaded, do pre-"server ready" initialization here
extern "C" __declspec(dllexport) void Plugin_Init()
{
    Log::Get().Init(PROJECT_NAME);

    AsaApi::GetHooks().SetHook("AShooterGameMode.BeginPlay()", Hook_AShooterGameMode_BeginPlay,
        &AShooterGameMode_BeginPlay_original);

    if (AsaApi::GetApiUtils().GetStatus() == AsaApi::ServerStatus::Ready)
        OnServerReady();
}

// Called by AsaApi when the plugin is unloaded, do cleanup here
extern "C" __declspec(dllexport) void Plugin_Unload()
{
    AsaApi::GetHooks().DisableHook("AShooterGameMode.BeginPlay()", Hook_AShooterGameMode_BeginPlay);

    AddRemoveCommands(false);
}