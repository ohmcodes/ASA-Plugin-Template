#include "API/ARK/Ark.h"

#include <fstream>

#include "Plugin.h"

#pragma comment(lib, "AsaApi.lib")

template <typename T>
T GetText(const std::string& section, const std::string& value)
{
    try
    {
        return Plugin::config[section][value].get<T>();
    }
    catch (const nlohmann::json::exception& error)
    {
        // Check if the exception is due to a missing key
        if (error.id == 302)
        {
            return T{};
        }

        Log::GetLog()->error("GetText() error: {}", error.what());
        throw;
    }
}



void FunctionCallback(AShooterPlayerController* player_controller, FString*, int, int)
{
    return;
}

void ReadConfig()
{
    const std::string config_path = AsaApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/" + PROJECT_NAME + "/config.json";
    std::ifstream file{ config_path };
    if (!file.is_open())
    {
        throw std::runtime_error("Can't open config.json");
    }

    file >> Plugin::config;

    Log::GetLog()->info("{} config file loaded", PROJECT_NAME);
    file.close();

    Plugin::isDebug = (GetText<bool>("General", "Debug") == true) ? true : false;
}

void LoadConfig()
{
    try
    {
        ReadConfig();
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error("LoadConfig error: {}", error.what());
        throw;
    }
}

void Reload()
{
    ReadConfig();
}

void ReloadConfig(APlayerController* player_controller, FString* /*unused*/, bool /*unused*/)
{
    auto* shooter_controller = static_cast<AShooterPlayerController*>(player_controller);

    try
    {
        Reload();
    }
    catch (const std::exception& error)
    {
        //Log::GetLog()->error(error.what());
        AsaApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::Red, "Failed to reload config Error: {}", error.what());
        return;
    }

    AsaApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::Green, "Reloaded config");
}

void ReloadConfigRcon(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld* /*unused*/)
{
    FString reply;

    try
    {
        Reload();
    }
    catch (const std::exception& error)
    {
        //Log::GetLog()->error(error.what());
        reply = error.what();
        rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
        return;
    }

    reply = "Reloaded config";
    rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
}

void AddRemoveCommands(bool addCmd = true)
{
    const FString command = GetText<std::string>("Commands", "CMD1").c_str();
    FString reload(FString(PROJECT_NAME) + ".Reload");

    // Add command if true
    if (addCmd)
    {
        if (!command.IsEmpty())
        {
            AsaApi::GetCommands().AddChatCommand(command, &FunctionCallback);
        }

        AsaApi::GetCommands().AddConsoleCommand(reload, &ReloadConfig);
        AsaApi::GetCommands().AddRconCommand(reload, &ReloadConfigRcon);

    }
    else
    {
        if (!command.IsEmpty())
        {
            AsaApi::GetCommands().RemoveChatCommand(command);
        }
        
        AsaApi::GetCommands().RemoveConsoleCommand(reload);
        AsaApi::GetCommands().RemoveRconCommand(reload);
    }
}

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