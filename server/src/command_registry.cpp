#include "command_registry.hpp"

#include "commands/append_command.hpp"
#include "commands/dbsize_command.hpp"
#include "commands/decr_command.hpp"
#include "commands/del_command.hpp"
#include "commands/echo_command.hpp"
#include "commands/exists_command.hpp"
#include "commands/expire_command.hpp"
#include "commands/flushall_command.hpp"
#include "commands/flushdb_command.hpp"
#include "commands/get_command.hpp"
#include "commands/incr_command.hpp"
#include "commands/info_command.hpp"
#include "commands/keys_command.hpp"
#include "commands/llen_command.hpp"
#include "commands/lpop_command.hpp"
#include "commands/lpush_command.hpp"
#include "commands/lrange_command.hpp"
#include "commands/ping_command.hpp"
#include "commands/rename_command.hpp"
#include "commands/rpop_command.hpp"
#include "commands/rpush_command.hpp"
#include "commands/sadd_command.hpp"
#include "commands/scard_command.hpp"
#include "commands/select_command.hpp"
#include "commands/set_command.hpp"
#include "commands/shutdown_command.hpp"
#include "commands/sismember_command.hpp"
#include "commands/smembers_command.hpp"
#include "commands/srem_command.hpp"
#include "commands/ttl_command.hpp"
#include "commands/type_command.hpp"

void registerCommands(CommandHandler& handler) {
    handler.registerCommand("PING", std::make_unique<PingCommand>());
    handler.registerCommand("ECHO", std::make_unique<EchoCommand>());
    handler.registerCommand("DBSIZE", std::make_unique<DbSizeCommand>());
    handler.registerCommand("RENAME", std::make_unique<RenameCommand>());
    handler.registerCommand("FLUSHDB", std::make_unique<FlushDbCommand>());
    handler.registerCommand("FLUSHALL", std::make_unique<FlushAllCommand>());
    handler.registerCommand("INFO", std::make_unique<InfoCommand>());
    handler.registerCommand("SHUTDOWN", std::make_unique<ShutdownCommand>());
    handler.registerCommand("SELECT", std::make_unique<SelectCommand>());

    handler.registerCommand("EXISTS", std::make_unique<ExistsCommand>());
    handler.registerCommand("DEL", std::make_unique<DelCommand>());
    handler.registerCommand("TYPE", std::make_unique<TypeCommand>());
    handler.registerCommand("KEYS", std::make_unique<KeysCommand>());
    handler.registerCommand("EXPIRE", std::make_unique<ExpireCommand>());
    handler.registerCommand("TTL", std::make_unique<TTLCommand>());

    handler.registerCommand("SET", std::make_unique<SetCommand>());
    handler.registerCommand("GET", std::make_unique<GetCommand>());
    handler.registerCommand("INCR", std::make_unique<IncrCommand>());
    handler.registerCommand("DECR", std::make_unique<DecrCommand>());

    handler.registerCommand("APPEND", std::make_unique<AppendCommand>());

    handler.registerCommand("LPUSH", std::make_unique<LPushCommand>());
    handler.registerCommand("RPUSH", std::make_unique<RPushCommand>());
    handler.registerCommand("LPOP", std::make_unique<LPopCommand>());
    handler.registerCommand("RPOP", std::make_unique<RPopCommand>());
    handler.registerCommand("LRANGE", std::make_unique<LRangeCommand>());
    handler.registerCommand("LLEN", std::make_unique<LLenCommand>());

    handler.registerCommand("SADD", std::make_unique<SAddCommand>());
    handler.registerCommand("SREM", std::make_unique<SRemCommand>());
    handler.registerCommand("SMEMBERS", std::make_unique<SMembersCommand>());
    handler.registerCommand("SISMEMBER", std::make_unique<SIsMemberCommand>());
    handler.registerCommand("SCARD", std::make_unique<SCardCommand>());
}
