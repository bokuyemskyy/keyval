#pragma once

#include <chrono>
#include <cstddef>
#include <string>

class Session {
   public:
    Session() { m_connected_at = std::chrono::steady_clock::now(); }

    void   setDb(size_t db) noexcept { m_db = db; }
    size_t db() const noexcept { return m_db; }

    void authenticate(const std::string& user) {
        m_authenticated = true;
        m_user          = user;
    }

    bool               isAuthenticated() const noexcept { return m_authenticated; }
    const std::string& user() const noexcept { return m_user; }

    void requestShutdown() noexcept { m_shutdown_requested = true; }
    bool isShutdownRequested() const noexcept { return m_shutdown_requested; }

    std::chrono::steady_clock::time_point connectedAt() const noexcept { return m_connected_at; }

   private:
    size_t                                m_db{};
    bool                                  m_authenticated{};
    std::string                           m_user;
    std::chrono::steady_clock::time_point m_connected_at;

    bool m_shutdown_requested{false};
};
