
#pragma once

#include "IUpdatable.h"
#include "EventHandler/EventToken.h"
#include "System/Network.h"

#include "Network/INetworkPipe.h"
#include "MessageDispatcher.h"

#include "Math/Quad.h"

#include <unordered_map>
#include <memory>

namespace std
{
    template <>
    struct hash<network::Address>
    {
        size_t operator()(const network::Address& key) const
        {
            return key.host ^ key.port;
        }
    };
}

namespace network
{
    inline bool operator == (const network::Address& left, const network::Address& right)
    {
        return left.host == right.host && left.port == right.port;
    }
}

namespace game
{
    struct PingMessage;
    struct ConnectMessage;
    struct DisconnectMessage;
    struct HeartBeatMessage;
    struct ViewportMessage;

    struct Config;

    struct ClientData
    {
        uint32_t heartbeat_timestamp;
        math::Quad viewport;
    };

    class ServerManager : public mono::IUpdatable, public INetworkPipe
    {
    public:

        ServerManager(mono::EventHandler* event_handler, const game::Config* game_config);
        ~ServerManager();

        void SendMessage(const NetworkMessage& message) override;
        void SendMessageTo(const NetworkMessage& message, const network::Address& address) override;
        ConnectionInfo GetConnectionInfo() const override;

        void QuitServer();
        const std::unordered_map<network::Address, ClientData>& GetConnectedClients() const;
        const struct ConnectionStats& GetConnectionStats() const;

    private:

        void PurgeZombieClients();

        bool HandlePingMessage(const PingMessage& ping_message);
        bool HandleConnectMessage(const ConnectMessage& message);
        bool HandleDisconnectMessage(const DisconnectMessage& message);
        bool HandleHeartBeatMessage(const HeartBeatMessage& message);
        bool HandleViewportMessage(const ViewportMessage& message);

        void doUpdate(const mono::UpdateContext& update_context) override;

        mono::EventHandler* m_event_handler;
        const game::Config* m_game_config;
        MessageDispatcher m_dispatcher;
        std::unique_ptr<class RemoteConnection> m_remote_connection;
        network::Address m_broadcast_address;
        network::Address m_server_address;
        uint32_t m_beacon_timer;
        uint32_t m_server_time;

        std::unordered_map<network::Address, ClientData> m_connected_clients;

        mono::EventToken<PingMessage> m_ping_func_token;
        mono::EventToken<ConnectMessage> m_connect_token;
        mono::EventToken<DisconnectMessage> m_disconnect_token;
        mono::EventToken<HeartBeatMessage> m_heartbeat_token;
        mono::EventToken<ViewportMessage> m_viewport_token;
    };
}