#pragma once

#include <KAI/Console/ConsoleColor.h>
#include <KAI/Core/Tree.h>
#include <KAI/Executor/Compiler.h>
#include <KAI/Executor/Executor.h>
#include <KAI/Language.h>
#include <KAI/Language/Common/TranslatorCommon.h>
#include <KAI/Network/Transport.h>

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

KAI_BEGIN

struct Coloriser;
class BinaryStream;

enum class NetworkMessageType : unsigned char {
    ConsoleCommand = net::kUserPacketStart + 10,
    ConsoleResult = net::kUserPacketStart + 11,
    ConsoleBroadcast = net::kUserPacketStart + 12,
    ConsoleLanguageSwitch = net::kUserPacketStart + 13,
    ConsoleBinary = net::kUserPacketStart + 14
};

struct NetworkConsoleMessage {
    std::string senderId;
    std::string command;
    std::string result;
    Language language{Language::Pi};
    long timestamp{0};

    NetworkConsoleMessage() {}
};

class Console : public Reflected {
    Tree tree_;
    Registry *reg_;
    Pointer<Executor> executor_;
    Pointer<Compiler> compiler_;
    std::shared_ptr<memory::IAllocator> alloc_;
    Language language_;
    std::shared_ptr<TranslatorCommon> translator_;

    std::vector<std::string> commandHistory_;
    std::string historyFile_;
    static const size_t kMaxHistorySize = 1000;

    // Network members
    std::unique_ptr<net::NetPeer> peer_;
    std::mutex peersMutex_;
    std::vector<net::NetAddress> connectedPeers_;
    std::thread messageThread_;
    bool networkingEnabled_;
    bool networkRunning_;
    int listenPort_;
    std::string consoleId_;
    std::vector<NetworkConsoleMessage> messageHistory_;
    std::function<void(const NetworkConsoleMessage&)> messageCallback_;
    mutable std::mutex peerExecutorsMutex_;
    std::unordered_map<std::string, Pointer<Executor>> peerExecutors_;
    std::unordered_map<std::string, std::string> peerConsoleIds_;

   public:
    Console();
    Console(std::shared_ptr<memory::IAllocator>);
    ~Console() override;

    void SetLanguage(Language lang);
    void SetLanguage(int lang);
    Language GetLanguage() const;

    void SetTranslator(std::shared_ptr<TranslatorCommon> trans);
    std::shared_ptr<TranslatorCommon> GetTranslator() const {
        return translator_;
    }

    void WritePrompt(std::ostream &out) const;
    String GetPrompt() const;
    String Process(const String &);
    String ProcessShellCommand(const String &text);
    String ExpandShellCommands(const String &text);
    String ProcessZshCommand(const String &text);
    String ExpandHistoryReferences(const String &text);
    String ParseHistoryExpansion(const String &text);
    std::vector<std::string> SplitIntoWords(const std::string &text);
    String ApplyWordDesignators(const std::string &command,
                                const std::string &designators);
    String ApplyModifiers(const String &text, const std::string &modifiers);
    String ProcessQuickSubstitution(const String &text);
    String SearchHistoryAnywhere(const String &pattern);
    String ProcessHistoryModifier(const String &text, char modifier);
    String ProcessSubstitutionModifier(const String &text,
                                       const std::string &pattern);
    std::string ExtractFilePath(const std::string &text);
    std::string currentCommand;  // For !# support
    bool shellMode = false;      // Toggle for shell mode
    Registry &GetRegistry() const { return *reg_; }
    Tree &GetTree() {
        return tree_;
    }
    Tree const &GetTree() const {
        return tree_;
    }

    Object GetRoot() const {
        return tree_.GetRoot();
    }

    Pointer<Executor> GetExecutor() const {
        return executor_;
    }
    Pointer<Compiler> GetCompiler() const {
        return compiler_;
    }

    Pointer<Continuation> Compile(const char *, Structure);
    void Execute(const String &text, Structure st = Structure::Program);
    bool ExecuteFile(const char *);
    void Execute(Pointer<Continuation> cont);
    void ExecuteWithExecutor(Pointer<Continuation> cont,
                             Pointer<Executor> targetExecutor);
    void ExecuteWithExecutor(const String &text,
                             Pointer<Executor> targetExecutor,
                             Structure st = Structure::Program);

    String WriteStack() const;
    String WriteStackForExecutor(Pointer<Executor> exec) const;
    void ShowColoredStack() const;
    void ControlC();
    void ClearScreen() const;
    String ReadLineWithDynamicColor();
    void ExecuteShellCommandWithColor(const std::string &command);
    static void Register(Registry &);

    // Help system
    void ShowHelp(const std::string &topic = "") const;
    void ShowBasicHelp() const;
    void ShowHistoryHelp() const;
    void ShowLanguageHelp(const std::string &lang) const;
    void ShowBuiltinCommands() const;
    bool ProcessBuiltinCommand(const std::string &command);

    // History management
    void LoadHistory();
    void SaveHistory() const;
    void AddToHistory(const std::string &command);

    // Network functionality
    bool StartNetworking(int listenPort = 14600);
    bool ConnectToPeer(const std::string& host, int port);
    void StopNetworking();
    bool SendCommandToPeer(const std::string& peerAddr, const std::string& command);
    void BroadcastCommand(const std::string& command);
    std::vector<std::string> GetConnectedPeers() const;
    std::vector<NetworkConsoleMessage> GetNetworkHistory() const;
    void SetNetworkMessageCallback(std::function<void(const NetworkConsoleMessage&)> callback);
    bool SendBinaryToPeer(const std::string& peerAddr,
                          const BinaryStream& payload);
    String ProcessNetworkCommand(const String& command);
    void ShowNetworkHelp() const;
    String WriteStackForPeer(const std::string& peerId) const;
    bool IsNetworkingEnabled() const { return networkingEnabled_; }

    int Run();

    // Helper method to detect incomplete structures for multi-line input
    bool IsStructureIncomplete(const String &text) const;

   protected:
       void Create() override;
       void CreateTree();
       void RegisterTypes();
       void ExposeTypesToTree(Object types);

       // Network protected methods
       void ProcessNetworkMessages();
       void HandleNetworkPacket(const net::NetPacket& packet);
       void HandleConsoleCommand(const net::NetPacket& packet);
       void HandleConsoleResult(const net::NetPacket& packet);
       void HandleConsoleBroadcast(const net::NetPacket& packet);
       void HandleLanguageSwitch(const net::NetPacket& packet);
       void HandleConsoleBinary(const net::NetPacket& packet);
       void SendResultToPeer(const net::NetAddress& peer, const std::string& command, const std::string& result);
       void AddPeer(const net::NetAddress& address);
       void RemovePeer(const net::NetAddress& address);
       void LogNetworkMessage(const NetworkConsoleMessage& message);
       std::string GenerateConsoleId();
       std::string AddressToString(const net::NetAddress& addr) const;
       net::NetAddress FindPeerByAddress(const std::string& addr) const;
       std::string MakePeerKey(const net::NetAddress& addr) const;
       Pointer<Executor> GetOrCreatePeerExecutor(const net::NetAddress& addr);
       Pointer<Executor> GetOrCreatePeerExecutor(const std::string& peerKey);
       void AssignPeerConsoleId(const net::NetAddress& addr, const std::string& consoleId);
       Pointer<Executor> GetPeerExecutorByConsoleId(const std::string& consoleId) const;
       void RemovePeerExecutor(const net::NetAddress& addr);
       void ClearPeerExecutors();
       void CopyMainStackToExecutor(Pointer<Executor> target) const;
       void CopyExecutorStackToMain(Pointer<Executor> source);
       std::string SimplifyStackDump(const std::string& dump) const;

   private:
    bool end_ = false;
    int endCode_ = 0;
};

KAI_TYPE_TRAITS(Console, Number::Console, Properties::Reflected);

KAI_END
