// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "rpcserver.h"

#include "clientversion.h"
#include "main.h"
#include "net.h"
#include "netbase.h"
#include "protocol.h"
#include "sync.h"
#include "timedata.h"
#include "util.h"
#include "version.h"

#include <boost/foreach.hpp>

#include "json/json_spirit_value.h"

using namespace json_spirit;
using namespace std;

extern int CountStringArray(string *ArrayName);
extern int CountIntArray(int *ArrayName);

inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}


Value getconnectioncount(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getconnectioncount\n"
            "\nReturns the number of connections to other nodes.\n"
            "\nbResult:\n"
            "n          (numeric) The connection count\n"
            "\nExamples:\n"
            + HelpExampleCli("getconnectioncount", "")
            + HelpExampleRpc("getconnectioncount", "")
        );

    LOCK(cs_vNodes);
    return (int)vNodes.size();
}

Value ping(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "ping\n"
            "\nRequests that a ping be sent to all other nodes, to measure ping time.\n"
            "Results provided in getpeerinfo, pingtime and pingwait fields are decimal seconds.\n"
            "Ping command is handled in queue with all other commands, so it measures processing backlog, not just network ping.\n"
            "\nExamples:\n"
            + HelpExampleCli("ping", "")
            + HelpExampleRpc("ping", "")
        );

    // Request that each node send a ping during next message processing pass
    LOCK(cs_vNodes);
    BOOST_FOREACH(CNode* pNode, vNodes) {
        pNode->fPingQueued = true;
    }

    return Value::null;
}

static void CopyNodeStats(std::vector<CNodeStats>& vstats)
{
    vstats.clear();

    LOCK(cs_vNodes);
    vstats.reserve(vNodes.size());
    BOOST_FOREACH(CNode* pnode, vNodes) {
        CNodeStats stats;
        pnode->copyStats(stats);
        vstats.push_back(stats);
    }
}

Value getpeerinfo(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getpeerinfo\n"
            "\nReturns data about each connected network node as a json array of objects.\n"
            "\nbResult:\n"
            "[\n"
            "  {\n"
            "    \"id\": n,                   (numeric) Peer index\n"
            "    \"addr\":\"host:port\",      (string) The ip address and port of the peer\n"
            "    \"addrlocal\":\"ip:port\",   (string) local address\n"
            "    \"services\":\"xxxxxxxxxxxxxxxx\",   (string) The services offered\n"
            "    \"lastsend\": ttt,           (numeric) The time in seconds since epoch (Jan 1 1970 GMT) of the last send\n"
            "    \"lastrecv\": ttt,           (numeric) The time in seconds since epoch (Jan 1 1970 GMT) of the last receive\n"
            "    \"bytessent\": n,            (numeric) The total bytes sent\n"
            "    \"bytesrecv\": n,            (numeric) The total bytes received\n"
            "    \"conntime\": ttt,           (numeric) The connection time in seconds since epoch (Jan 1 1970 GMT)\n"
            "    \"pingtime\": n,             (numeric) ping time\n"
            "    \"pingwait\": n,             (numeric) ping wait\n"
            "    \"version\": v,              (numeric) The peer version, such as 7001\n"
            "    \"subver\": \"/Satoshi:0.8.5/\",  (string) The string version\n"
            "    \"inbound\": true|false,     (boolean) Inbound (true) or Outbound (false)\n"
            "    \"startingheight\": n,       (numeric) The starting height (block) of the peer\n"
            "    \"banscore\": n,             (numeric) The ban score\n"
            "    \"synced_headers\": n,       (numeric) The last header we have in common with this peer\n"
            "    \"synced_blocks\": n,        (numeric) The last block we have in common with this peer\n"
            "    \"inflight\": [\n"
            "       n,                        (numeric) The heights of blocks we're currently asking from this peer\n"
            "       ...\n"
            "    ]\n"
            "  }\n"
            "  ,...\n"
            "]\n"
            "\nExamples:\n"
            + HelpExampleCli("getpeerinfo", "")
            + HelpExampleRpc("getpeerinfo", "")
        );

    vector<CNodeStats> vstats;
    CopyNodeStats(vstats);

    Array ret;

    BOOST_FOREACH(const CNodeStats& stats, vstats) {
        Object obj;
        CNodeStateStats statestats;
        bool fStateStats = GetNodeStateStats(stats.nodeid, statestats);
        obj.push_back(Pair("id", stats.nodeid));
        obj.push_back(Pair("addr", stats.addrName));
        if (!(stats.addrLocal.empty()))
            obj.push_back(Pair("addrlocal", stats.addrLocal));
        obj.push_back(Pair("services", strprintf("%016x", stats.nServices)));
        obj.push_back(Pair("lastsend", stats.nLastSend));
        obj.push_back(Pair("lastrecv", stats.nLastRecv));
        obj.push_back(Pair("bytessent", stats.nSendBytes));
        obj.push_back(Pair("bytesrecv", stats.nRecvBytes));
        obj.push_back(Pair("conntime", stats.nTimeConnected));
        obj.push_back(Pair("pingtime", stats.dPingTime));
        if (stats.dPingWait > 0.0)
            obj.push_back(Pair("pingwait", stats.dPingWait));
        obj.push_back(Pair("version", stats.nVersion));
        // Use the sanitized form of subver here, to avoid tricksy remote peers from
        // corrupting or modifiying the JSON output by putting special characters in
        // their ver message.
        obj.push_back(Pair("subver", stats.cleanSubVer));
        obj.push_back(Pair("inbound", stats.fInbound));
        obj.push_back(Pair("startingheight", stats.nStartingHeight));
        if (fStateStats) {
            obj.push_back(Pair("banscore", statestats.nMisbehavior));
            obj.push_back(Pair("synced_headers", statestats.nSyncHeight));
            obj.push_back(Pair("synced_blocks", statestats.nCommonHeight));
            Array heights;
            BOOST_FOREACH(int height, statestats.vHeightInFlight) {
                heights.push_back(height);
            }
            obj.push_back(Pair("inflight", heights));
        }
        obj.push_back(Pair("whitelisted", stats.fWhitelisted));

        ret.push_back(obj);
    }

    return ret;
}

Value addnode(const Array& params, bool fHelp)
{
    string strCommand;
    if (params.size() == 2)
        strCommand = params[1].get_str();
    if (fHelp || params.size() != 2 ||
        (strCommand != "onetry" && strCommand != "add" && strCommand != "remove"))
        throw runtime_error(
            "addnode \"node\" \"add|remove|onetry\"\n"
            "\nAttempts add or remove a node from the addnode list.\n"
            "Or try a connection to a node once.\n"
            "\nArguments:\n"
            "1. \"node\"     (string, required) The node (see getpeerinfo for nodes)\n"
            "2. \"command\"  (string, required) 'add' to add a node to the list, 'remove' to remove a node from the list, 'onetry' to try a connection to the node once\n"
            "\nExamples:\n"
            + HelpExampleCli("addnode", "\"192.168.0.6:5784\" \"onetry\"")
            + HelpExampleRpc("addnode", "\"192.168.0.6:5784\", \"onetry\"")
        );

    string strNode = params[0].get_str();

    if (strCommand == "onetry")
    {
        CAddress addr;
        OpenNetworkConnection(addr, NULL, strNode.c_str());
        return Value::null;
    }

    LOCK(cs_vAddedNodes);
    vector<string>::iterator it = vAddedNodes.begin();
    for(; it != vAddedNodes.end(); it++)
        if (strNode == *it)
            break;

    if (strCommand == "add")
    {
        if (it != vAddedNodes.end())
            throw JSONRPCError(RPC_CLIENT_NODE_ALREADY_ADDED, "Error: Node already added");
        vAddedNodes.push_back(strNode);
    }
    else if(strCommand == "remove")
    {
        if (it == vAddedNodes.end())
            throw JSONRPCError(RPC_CLIENT_NODE_NOT_ADDED, "Error: Node has not been added.");
        vAddedNodes.erase(it);
    }

    return Value::null;
}

Value getaddednodeinfo(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "getaddednodeinfo dns ( \"node\" )\n"
            "\nReturns information about the given added node, or all added nodes\n"
            "(note that onetry addnodes are not listed here)\n"
            "If dns is false, only a list of added nodes will be provided,\n"
            "otherwise connected information will also be available.\n"
            "\nArguments:\n"
            "1. dns        (boolean, required) If false, only a list of added nodes will be provided, otherwise connected information will also be available.\n"
            "2. \"node\"   (string, optional) If provided, return information about this specific node, otherwise all nodes are returned.\n"
            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"addednode\" : \"192.168.0.201\",   (string) The node ip address\n"
            "    \"connected\" : true|false,          (boolean) If connected\n"
            "    \"addresses\" : [\n"
            "       {\n"
            "         \"address\" : \"192.168.0.201:5784\",  (string) The bata server host and port\n"
            "         \"connected\" : \"outbound\"           (string) connection, inbound or outbound\n"
            "       }\n"
            "       ,...\n"
            "     ]\n"
            "  }\n"
            "  ,...\n"
            "]\n"
            "\nExamples:\n"
            + HelpExampleCli("getaddednodeinfo", "true")
            + HelpExampleCli("getaddednodeinfo", "true \"192.168.0.201\"")
            + HelpExampleRpc("getaddednodeinfo", "true, \"192.168.0.201\"")
        );

    bool fDns = params[0].get_bool();

    list<string> laddedNodes(0);
    if (params.size() == 1)
    {
        LOCK(cs_vAddedNodes);
        BOOST_FOREACH(string& strAddNode, vAddedNodes)
            laddedNodes.push_back(strAddNode);
    }
    else
    {
        string strNode = params[1].get_str();
        LOCK(cs_vAddedNodes);
        BOOST_FOREACH(string& strAddNode, vAddedNodes)
            if (strAddNode == strNode)
            {
                laddedNodes.push_back(strAddNode);
                break;
            }
        if (laddedNodes.size() == 0)
            throw JSONRPCError(RPC_CLIENT_NODE_NOT_ADDED, "Error: Node has not been added.");
    }

    Array ret;
    if (!fDns)
    {
        BOOST_FOREACH(string& strAddNode, laddedNodes)
        {
            Object obj;
            obj.push_back(Pair("addednode", strAddNode));
            ret.push_back(obj);
        }
        return ret;
    }

    list<pair<string, vector<CService> > > laddedAddreses(0);
    BOOST_FOREACH(string& strAddNode, laddedNodes)
    {
        vector<CService> vservNode(0);
        if(Lookup(strAddNode.c_str(), vservNode, Params().GetDefaultPort(), fNameLookup, 0))
            laddedAddreses.push_back(make_pair(strAddNode, vservNode));
        else
        {
            Object obj;
            obj.push_back(Pair("addednode", strAddNode));
            obj.push_back(Pair("connected", false));
            Array addresses;
            obj.push_back(Pair("addresses", addresses));
        }
    }

    LOCK(cs_vNodes);
    for (list<pair<string, vector<CService> > >::iterator it = laddedAddreses.begin(); it != laddedAddreses.end(); it++)
    {
        Object obj;
        obj.push_back(Pair("addednode", it->first));

        Array addresses;
        bool fConnected = false;
        BOOST_FOREACH(CService& addrNode, it->second)
        {
            bool fFound = false;
            Object node;
            node.push_back(Pair("address", addrNode.ToString()));
            BOOST_FOREACH(CNode* pnode, vNodes)
                if (pnode->addr == addrNode)
                {
                    fFound = true;
                    fConnected = true;
                    node.push_back(Pair("connected", pnode->fInbound ? "inbound" : "outbound"));
                    break;
                }
            if (!fFound)
                node.push_back(Pair("connected", "false"));
            addresses.push_back(node);
        }
        obj.push_back(Pair("connected", fConnected));
        obj.push_back(Pair("addresses", addresses));
        ret.push_back(obj);
    }

    return ret;
}

Value clearbanned(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
                            "clearbanned\n"
                            "\nClear all banned IPs.\n"
                            "\nExamples:\n"
                            + HelpExampleCli("clearbanned", "")
                            + HelpExampleRpc("clearbanned", "")
                            );

    CNode::ClearBanned();
    //DumpBanlist(); //store banlist to disk
    //uiInterface.BannedListChanged();

    return Value::null;
}

Value getnettotals(const Array& params, bool fHelp)
{
    if (fHelp || params.size() > 0)
        throw runtime_error(
            "getnettotals\n"
            "\nReturns information about network traffic, including bytes in, bytes out,\n"
            "and current time.\n"
            "\nResult:\n"
            "{\n"
            "  \"totalbytesrecv\": n,   (numeric) Total bytes received\n"
            "  \"totalbytessent\": n,   (numeric) Total bytes sent\n"
            "  \"timemillis\": t        (numeric) Total cpu time\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("getnettotals", "")
            + HelpExampleRpc("getnettotals", "")
       );

    Object obj;
    obj.push_back(Pair("totalbytesrecv", CNode::GetTotalBytesRecv()));
    obj.push_back(Pair("totalbytessent", CNode::GetTotalBytesSent()));
    obj.push_back(Pair("timemillis", GetTimeMillis()));
    return obj;
}

static Array GetNetworksInfo()
{
    Array networks;
    for(int n=0; n<NET_MAX; ++n)
    {
        enum Network network = static_cast<enum Network>(n);
        if(network == NET_UNROUTABLE)
            continue;
        proxyType proxy;
        Object obj;
        GetProxy(network, proxy);
        obj.push_back(Pair("name", GetNetworkName(network)));
        obj.push_back(Pair("limited", IsLimited(network)));
        obj.push_back(Pair("reachable", IsReachable(network)));
        obj.push_back(Pair("proxy", proxy.IsValid() ? proxy.ToStringIPPort() : string()));
        networks.push_back(obj);
    }
    return networks;
}

Value getnetworkinfo(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getnetworkinfo\n"
            "Returns an object containing various state info regarding P2P networking.\n"
            "\nResult:\n"
            "{\n"
            "  \"version\": xxxxx,                      (numeric) the server version\n"
            "  \"subversion\": \"/Satoshi:x.x.x/\",     (string) the server subversion string\n"
            "  \"protocolversion\": xxxxx,              (numeric) the protocol version\n"
            "  \"localservices\": \"xxxxxxxxxxxxxxxx\", (string) the services we offer to the network\n"
            "  \"timeoffset\": xxxxx,                   (numeric) the time offset\n"
            "  \"connections\": xxxxx,                  (numeric) the number of connections\n"
            "  \"networks\": [                          (array) information per network\n"
            "  {\n"
            "    \"name\": \"xxx\",                     (string) network (ipv4, ipv6 or onion)\n"
            "    \"limited\": true|false,               (boolean) is the network limited using -onlynet?\n"
            "    \"reachable\": true|false,             (boolean) is the network reachable?\n"
            "    \"proxy\": \"host:port\"               (string) the proxy that is used for this network, or empty if none\n"
            "  }\n"
            "  ,...\n"
            "  ],\n"
            "  \"relayfee\": x.xxxxxxxx,                (numeric) minimum relay fee for non-free transactions in bta/kb\n"
            "  \"localaddresses\": [                    (array) list of local addresses\n"
            "  {\n"
            "    \"address\": \"xxxx\",                 (string) network address\n"
            "    \"port\": xxx,                         (numeric) network port\n"
            "    \"score\": xxx                         (numeric) relative score\n"
            "  }\n"
            "  ,...\n"
            "  ]\n"
            "}\n"
            "\nExamples:\n"
            + HelpExampleCli("getnetworkinfo", "")
            + HelpExampleRpc("getnetworkinfo", "")
        );

    Object obj;
    obj.push_back(Pair("version",       CLIENT_VERSION));
    obj.push_back(Pair("subversion",
        FormatSubVersion(CLIENT_NAME, CLIENT_VERSION, std::vector<string>())));
    obj.push_back(Pair("protocolversion",PROTOCOL_VERSION));
    obj.push_back(Pair("localservices",       strprintf("%016x", nLocalServices)));
    obj.push_back(Pair("timeoffset",    GetTimeOffset()));
    obj.push_back(Pair("connections",   (int)vNodes.size()));
    obj.push_back(Pair("networks",      GetNetworksInfo()));
    obj.push_back(Pair("relayfee",      ValueFromAmount(::minRelayTxFee.GetFeePerK())));
    Array localAddresses;
    {
        LOCK(cs_mapLocalHost);
        BOOST_FOREACH(const PAIRTYPE(CNetAddr, LocalServiceInfo) &item, mapLocalHost)
        {
            Object rec;
            rec.push_back(Pair("address", item.first.ToString()));
            rec.push_back(Pair("port", item.second.nPort));
            rec.push_back(Pair("score", item.second.nScore));
            localAddresses.push_back(rec);
        }
    }
    obj.push_back(Pair("localaddresses", localAddresses));
    return obj;
}




Value firewallstatus(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() != 0)
        throw runtime_error(
                            "firewallstatus \"\n"
                            "\nGet the status of Bitcoin Firewall.\n"
                            );


    Object result;
    result.push_back(Pair("enabled", BoolToString(FIREWALL_ENABLED)));
    result.push_back(Pair("clear-blacklist", BoolToString(FIREWALL_CLEAR_BLACKLIST)));
    result.push_back(Pair("clear-banlist", BoolToString(FIREWALL_CLEAR_BANS)));
    result.push_back(Pair("live-debug", BoolToString(FIREWALL_LIVE_DEBUG)));
    result.push_back(Pair("live-debug-exam", BoolToString(FIREWALL_LIVEDEBUG_EXAM)));
    result.push_back(Pair("live-debug-bans", BoolToString(FIREWALL_LIVEDEBUG_BANS)));
    result.push_back(Pair("live-debug-blacklist", BoolToString(FIREWALL_LIVEDEBUG_BLACKLIST)));
    result.push_back(Pair("live-debug-disconnect", BoolToString(FIREWALL_LIVEDEBUG_DISCONNECT)));
    result.push_back(Pair("live-debug-bandwidthabuse", BoolToString(FIREWALL_LIVEDEBUG_BANDWIDTHABUSE)));
    result.push_back(Pair("live-debug-nofalsepositive", BoolToString(FIREWALL_LIVEDEBUG_NOFALSEPOSITIVE)));
    result.push_back(Pair("live-debug-invalidwallet", BoolToString(FIREWALL_LIVEDEBUG_INVALIDWALLET)));
    result.push_back(Pair("live-debug-forkedwallet", BoolToString(FIREWALL_LIVEDEBUG_FORKEDWALLET)));
    result.push_back(Pair("live-debug-floodingwallet", BoolToString(FIREWALL_LIVEDEBUG_FLOODINGWALLET)));
    result.push_back(Pair("detect-bandwidthabuse", BoolToString(FIREWALL_DETECT_BANDWIDTHABUSE)));
    result.push_back(Pair("nofalsepositive", BoolToString(FIREWALL_NOFALSEPOSITIVE_BANDWIDTHABUSE)));
    result.push_back(Pair("detect-invalidwallet", BoolToString(FIREWALL_DETECT_INVALIDWALLET)));
    result.push_back(Pair("detect-forkedwallet", BoolToString(FIREWALL_DETECT_FORKEDWALLET)));
    result.push_back(Pair("detect-floodingwallet", BoolToString(FIREWALL_DETECT_FLOODINGWALLET)));
    result.push_back(Pair("blacklist-bandwidthabuse", BoolToString(FIREWALL_BLACKLIST_BANDWIDTHABUSE)));
    result.push_back(Pair("blacklist-invalidwallet", BoolToString(FIREWALL_BLACKLIST_INVALIDWALLET)));
    result.push_back(Pair("blacklist-forkedwallet", BoolToString(FIREWALL_BLACKLIST_FORKEDWALLET)));
    result.push_back(Pair("blacklist-floodingwallet", BoolToString(FIREWALL_BLACKLIST_FLOODINGWALLET)));
    result.push_back(Pair("ban-bandwidthabuse", BoolToString(FIREWALL_BAN_BANDWIDTHABUSE)));
    result.push_back(Pair("ban-invalidwallet", BoolToString(FIREWALL_BAN_INVALIDWALLET)));
    result.push_back(Pair("ban-forkedwallet", BoolToString(FIREWALL_BAN_FORKEDWALLET)));
    result.push_back(Pair("ban-floodingwallet", BoolToString(FIREWALL_BAN_FLOODINGWALLET)));
    result.push_back(Pair("bantime-bandwidthabuse", (int64_t)FIREWALL_BANTIME_BANDWIDTHABUSE));
    result.push_back(Pair("bantime-invalidwallet", (int64_t)FIREWALL_BANTIME_INVALIDWALLET));
    result.push_back(Pair("bantime-forkedwallet", (int64_t)FIREWALL_BANTIME_FORKEDWALLET));
    result.push_back(Pair("bantime-floodingwallet", (int64_t)FIREWALL_BANTIME_FLOODINGWALLET));

return result;
}


Value firewallenabled(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallenabled \"true|false\"\n"
                            "\nChange the status of Bitcoin Firewall.\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - true\n"
                            + HelpExampleCli("firewallenabled", "true")
                            + HelpExampleCli("firewallenabled", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_ENABLED = true;
    }
    else
    {
        FIREWALL_ENABLED = false;
    }


    Object result;
    result.push_back(Pair("enabled", strCommand));

return result;
}



Value firewallclearblacklist(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallclearblacklist \"true|false\"\n"
                            "\nBitcoin Firewall Clear Blacklist (session)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - false\n"
                            + HelpExampleCli("firewallclearblacklist", "true")
                            + HelpExampleCli("firewallclearblacklist", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_CLEAR_BLACKLIST = true;
    }
    else
    {
        FIREWALL_CLEAR_BLACKLIST = false;
    }


    Object result;
    result.push_back(Pair("clear-blacklist", strCommand));

return result;
}


Value firewallclearbanlist(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallclearbanlist \"true|false\"\n"
                            "\nBitcoin Firewall Clear Ban List (permenant)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - false\n"
                            + HelpExampleCli("firewallclearbanlist", "true")
                            + HelpExampleCli("firewallclearbanlist", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_CLEAR_BANS = true;
    }
    else
    {
        FIREWALL_CLEAR_BANS = false;
    }


    Object result;
    result.push_back(Pair("clear-banlist", strCommand));

return result;
}


Value firewalldebug(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebug \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - false\n"
                            + HelpExampleCli("firewalldebug", "true")
                            + HelpExampleCli("firewalldebug", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVE_DEBUG = true;
    }
    else
    {
        FIREWALL_LIVE_DEBUG = false;
    }


    Object result;
    result.push_back(Pair("live-debug", strCommand));

return result;
}

Value firewalldebugexam(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebugexam \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output - Exam\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - true\n"
                            + HelpExampleCli("firewalldebugexam", "true")
                            + HelpExampleCli("firewalldebugexam", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVEDEBUG_EXAM = true;
    }
    else
    {
        FIREWALL_LIVEDEBUG_EXAM = false;
    }


    Object result;
    result.push_back(Pair("live-debug-exam", strCommand));

return result;
}


Value firewalldebugbans(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebugbans \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output - Bans\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - true\n"
                            + HelpExampleCli("firewalldebugbans", "true")
                            + HelpExampleCli("firewalldebugbans", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVEDEBUG_BANS = true;
    }
    else
    {
        FIREWALL_LIVEDEBUG_BANS = false;
    }


    Object result;
    result.push_back(Pair("live-debug-bans", strCommand));

return result;
}

Value firewalldebugblacklist(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebugblacklist \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output - Blacklist\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - true\n"
                            + HelpExampleCli("firewalldebugblacklist", "true")
                            + HelpExampleCli("firewalldebugblacklist", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVEDEBUG_BLACKLIST = true;
    }
    else
    {
        FIREWALL_LIVEDEBUG_BLACKLIST = false;
    }


    Object result;
    result.push_back(Pair("live-debug-blacklist", strCommand));

return result;
}


Value firewalldebugdisconnect(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebugdisconnect \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output - Disconnect\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - true\n"
                            + HelpExampleCli("firewalldebugdisconnect", "true")
                            + HelpExampleCli("firewalldebugdisconnect", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVEDEBUG_DISCONNECT = true;
    }
    else
    {
        FIREWALL_LIVEDEBUG_DISCONNECT = false;
    }


    Object result;
    result.push_back(Pair("live-debug-disconnect", strCommand));

return result;
}


Value firewalldebugbandwidthabuse(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebugbandwidthabuse \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output - Bandwidth Abuse\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - true\n"
                            + HelpExampleCli("firewalldebugbandwidthabuse", "true")
                            + HelpExampleCli("firewalldebugbandwidthabuse", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVEDEBUG_BANDWIDTHABUSE = true;
    }
    else
    {
        FIREWALL_LIVEDEBUG_BANDWIDTHABUSE = false;
    }


    Object result;
    result.push_back(Pair("live-debug-bandwidthabuse", strCommand));

return result;
}


Value firewalldebugnofalsepositivebandwidthabuse(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebugnofalsepositivebandwidthabuse \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output - No False Positive (Bandwidth Abuse)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - true\n"
                            + HelpExampleCli("firewalldebugnofalsepositivebandwidthabuse", "true")
                            + HelpExampleCli("firewalldebugnofalsepositivebandwidthabuse", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVEDEBUG_NOFALSEPOSITIVE = true;
    }
    else
    {
        FIREWALL_LIVEDEBUG_NOFALSEPOSITIVE = false;
    }


    Object result;
    result.push_back(Pair("live-debug-nofalsepositive", strCommand));

return result;
}


Value firewalldebuginvalidwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebuginvalidwallet \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output - Invalid Wallet\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - true\n"
                            + HelpExampleCli("firewalldebuginvalidwallet", "true")
                            + HelpExampleCli("firewalldebuginvalidwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVEDEBUG_INVALIDWALLET = true;
    }
    else
    {
        FIREWALL_LIVEDEBUG_INVALIDWALLET = false;
    }


    Object result;
    result.push_back(Pair("live-debug-invalidwallet", strCommand));

return result;
}


Value firewalldebugforkedwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebugforkedwallet \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output - Forked Wallet\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - true\n"
                            + HelpExampleCli("firewalldebugforkedwallet", "true")
                            + HelpExampleCli("firewalldebugforkedwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVEDEBUG_FORKEDWALLET = true;
    }
    else
    {
        FIREWALL_LIVEDEBUG_FORKEDWALLET = false;
    }


    Object result;
    result.push_back(Pair("live-debug-forkedwallet", strCommand));

return result;
}


Value firewalldebugfloodingwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldebugfloodingwallet \"true|false\"\n"
                            "\nBitcoin Firewall Live Debug Output - Flooding Wallet\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewalldebugfloodingwallet", "true")
                            + HelpExampleCli("firewalldebugfloodingwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_LIVEDEBUG_FLOODINGWALLET = true;
    }
    else
    {
        FIREWALL_LIVEDEBUG_FLOODINGWALLET = false;
    }


    Object result;
    result.push_back(Pair("live-debug-floodingwallet", strCommand));

return result;
}


Value firewallaveragetolerance(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallaveragetolerance \"tolerance\"\n"
                            "\nBitcoin Firewall Exam Setting (Average Block Tolerance)\n"
                            "\nArguments:\n"
                            "Value: \"tolerance\" (double, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallaveragetolerance", "0.0001")
                            + HelpExampleCli("firewallaveragetolerance", "0.1")
                            );

    if (params.size() == 1)
    {
        FIREWALL_AVERAGE_TOLERANCE = strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("exam-average-tolerance", FIREWALL_AVERAGE_TOLERANCE));

return result;
}


Value firewallaveragerange(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallaveragerange \"zone\"\n"
                            "\nBitcoin Firewall Exam Setting (Average Block Range)\n"
                            "\nArguments:\n"
                            "Value: \"zone\" (integer), required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallaveragerange", "10")
                            + HelpExampleCli("firewallaveragerange", "50")
                            );

    if (params.size() == 1)
    {
        FIREWALL_AVERAGE_RANGE = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("exam-average-range", FIREWALL_AVERAGE_RANGE));

return result;
}


Value firewalltraffictolerance(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalltraffictolerance \"tolerance\"\n"
                            "\nBitcoin Firewall Exam Setting (Traffic Tolerance)\n"
                            "\nArguments:\n"
                            "Value: \"tolerance\" (double, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewalltraffictolerance", "0.0001")
                            + HelpExampleCli("firewalltraffictolerance", "0.1")
                            );

    if (params.size() == 1)
    {
        FIREWALL_TRAFFIC_TOLERANCE = strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("exam-traffic-tolerance", FIREWALL_TRAFFIC_TOLERANCE));

return result;
}


Value firewalltrafficzone(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalltrafficzone \"zone\"\n"
                            "\nBitcoin Firewall Exam Setting (Traffic Zone)\n"
                            "\nArguments:\n"
                            "Value: \"zone\" (double), required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewalltrafficzone", "10.10")
                            + HelpExampleCli("firewalltrafficzone", "50.50")
                            );

    if (params.size() == 1)
    {
        FIREWALL_TRAFFIC_ZONE = strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("exam-traffic-zone", FIREWALL_TRAFFIC_ZONE));

return result;
}


Value firewalladdtowhitelist(const Array& params, bool fHelp)
{
    string MSG;

    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalladdtowhitelist \"address\"\n"
                            "\nBitcoin Firewall Adds IP Address to General Rule\n"
                            "\nArguments:\n"
                            "Value: \"address\" (string, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewalladdtowhitelist", "IP")
                            + HelpExampleCli("firewalladdtowhitelist", "127.0.0.1")
                            );

    if (params.size() == 1)
    {
        if (CountStringArray(FIREWALL_WHITELIST) < 256)
        {
            FIREWALL_WHITELIST[CountStringArray(FIREWALL_WHITELIST)] = params[0].get_str();
            MSG = CountStringArray(FIREWALL_WHITELIST);
        }
        else
        {
            MSG = "Over 256 Max!";
        }
    }

    Object result;
    result.push_back(Pair("exam-whitelist-add", MSG));

return result;
}


Value firewalladdtoblacklist(const Array& params, bool fHelp)
{
    string MSG;

    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalladdtoblacklist \"address\"\n"
                            "\nBitcoin Firewall Adds IP Address to General Rule\n"
                            "\nArguments:\n"
                            "Value: \"address\" (string, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewalladdtoblacklist", "IP")
                            + HelpExampleCli("firewalladdtoblacklist", "127.0.0.1")
                            );

    if (params.size() == 1)
    {
        if (CountStringArray(FIREWALL_BLACKLIST) < 256)
        {
            FIREWALL_BLACKLIST[CountStringArray(FIREWALL_BLACKLIST)] = params[0].get_str();
            MSG = CountStringArray(FIREWALL_BLACKLIST);
        }
        else
        {
            MSG = "Over 256 Max!";
        }
    }

    Object result;
    result.push_back(Pair("exam-blacklist-add", MSG));

return result;
}


Value firewalldetectbandwidthabuse(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldetectbandwidthabuse \"true|false\"\n"
                            "\nBitcoin Firewall Detect Bandwidth Abuse Rule #1\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewalldetectbandwidthabuse", "true")
                            + HelpExampleCli("firewalldetectbandwidthabuse", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_DETECT_BANDWIDTHABUSE = true;
    }
    else
    {
        FIREWALL_DETECT_BANDWIDTHABUSE = false;
    }


    Object result;
    result.push_back(Pair("detect-bandwidthabuse", strCommand));

return result;
}

Value firewallblacklistbandwidthabuse(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallblacklistbandwidthabuse \"true|false\"\n"
                            "\nBitcoin Firewall Blacklist Bandwidth Abuse Rule #1 (session)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallblacklistbandwidthabuse", "true")
                            + HelpExampleCli("firewallblacklistbandwidthabuse", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_BLACKLIST_BANDWIDTHABUSE = true;
    }
    else
    {
        FIREWALL_BLACKLIST_BANDWIDTHABUSE = false;
    }


    Object result;
    result.push_back(Pair("blacklist-bandwidthabuse", strCommand));

return result;
}

Value firewallbanbandwidthabuse(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbanbandwidthabuse \"true|false\"\n"
                            "\nBitcoin Firewall Ban Bandwidth Abuse Rule #1 (permenant)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallbanbandwidthabuse", "true")
                            + HelpExampleCli("firewallbanbandwidthabuse", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_BAN_BANDWIDTHABUSE = true;
    }
    else
    {
        FIREWALL_BAN_BANDWIDTHABUSE = false;
    }


    Object result;
    result.push_back(Pair("ban-bandwidthabuse", strCommand));

return result;
}

Value firewallnofalsepositivebandwidthabuse(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallnofalsepositivebandwidthabuse \"true|false\"\n"
                            "\nBitcoin Firewall False Positive Protection Rule #1\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallnofalsepositivebandwidthabuse", "true")
                            + HelpExampleCli("firewallnofalsepositivebandwidthabuse", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_NOFALSEPOSITIVE_BANDWIDTHABUSE = true;
    }
    else
    {
        FIREWALL_NOFALSEPOSITIVE_BANDWIDTHABUSE = false;
    }


    Object result;
    result.push_back(Pair("firewallnofalsepositivebandwidthabuse", strCommand));

return result;
}


Value firewallbantimebandwidthabuse(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbantimebandwidthabuse \"seconds\"\n"
                            "\nBitcoin Firewall Ban Time Bandwidth Abuse Rule #1\n"
                            "\nArguments:\n"
                            "Value: \"0|10000\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - 24h\n"
                            + HelpExampleCli("firewallbantimebandwidthabuse", "0")
                            + HelpExampleCli("firewallbantimebandwidthabuse", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_BANTIME_BANDWIDTHABUSE = (int)strtod(params[0].get_str().c_str(), NULL);
    }


    Object result;
    result.push_back(Pair("bantime-bandwidthabuse", FIREWALL_BANTIME_BANDWIDTHABUSE));

return result;
}


Value firewallbandwidthabusemaxcheck(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbandwidthabusemaxcheck \"seconds\"\n"
                            "\nBitcoin Firewall Max Check Bandwidth Abuse Rule #1\n"
                            "\nArguments:\n"
                            "Seconds: \"0|10000\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default\n"
                            + HelpExampleCli("firewallbandwidthabusemaxcheck", "0")
                            + HelpExampleCli("firewallbandwidthabusemaxcheck", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_BANDWIDTHABUSE_MAXCHECK = (int)strtod(params[0].get_str().c_str(), NULL);
    }


    Object result;
    result.push_back(Pair("maxcheck-bandwidthabuse", FIREWALL_BANDWIDTHABUSE_MAXCHECK));

return result;
}

Value firewallbandwidthabuseminattack(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbandwidthabuseminattack \"value\"\n"
                            "\nBitcoin Firewall Min Attack Bandwidth Abuse Rule #1\n"
                            "\nArguments:\n"
                            "Value: \"17.1\" (double, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - 17.1\n"
                            + HelpExampleCli("firewallbandwidthabuseminattack", "17.1")
                            + HelpExampleCli("firewallbandwidthabuseminattack", "17.005")
                            );

    if (params.size() == 1)
    {
        FIREWALL_BANDWIDTHABUSE_MINATTACK = strtod(params[0].get_str().c_str(), NULL);
    }


    Object result;
    result.push_back(Pair("minattack-bandwidthabuse", FIREWALL_BANDWIDTHABUSE_MINATTACK));

return result;
}

Value firewallbandwidthabusemaxattack(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbandwidthabusemaxattack \"ratio\"\n"
                            "\nBitcoin Firewall Max Attack Bandwidth Abuse Rule #1\n"
                            "\nArguments:\n"
                            "Value: \"17.2\" (double, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - 17.2\n"
                            + HelpExampleCli("firewallbandwidthabusemaxattack", "17.2")
                            + HelpExampleCli("firewallbandwidthabusemaxattack", "18.004")
                            );

    if (params.size() == 1)
    {
        FIREWALL_BANDWIDTHABUSE_MAXATTACK = strtod(params[0].get_str().c_str(), NULL);
    }


    Object result;
    result.push_back(Pair("maxattack-bandwidthabuse", FIREWALL_BANDWIDTHABUSE_MAXATTACK));

return result;
}


Value firewalldetectinvalidwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldetectinvalidwallet \"true|false\"\n"
                            "\nBitcoin Firewall Detect Invalid Wallet Rule #2\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewalldetectinvalidwallet", "true")
                            + HelpExampleCli("firewalldetectinvalidwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_DETECT_INVALIDWALLET  = true;
    }
    else
    {
        FIREWALL_DETECT_INVALIDWALLET  = false;
    }


    Object result;
    result.push_back(Pair("detect-invalidwallet", strCommand));

return result;
}


Value firewallblacklistinvalidwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallblacklistinvalidwallet \"true|false\"\n"
                            "\nBitcoin Firewall Blacklist Invalid Wallet Rule #2 (session)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallblacklistinvalidwallet", "true")
                            + HelpExampleCli("firewallblacklistinvalidwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_BLACKLIST_INVALIDWALLET = true;
    }
    else
    {
        FIREWALL_BLACKLIST_INVALIDWALLET = false;
    }


    Object result;
    result.push_back(Pair("blacklist-invalidwallet", strCommand));

return result;
}


Value firewallbaninvalidwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbaninvalidwallet \"true|false\"\n"
                            "\nBitcoin Firewall Ban Invalid Wallet Rule #2 (permenant)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallbaninvalidwallet", "true")
                            + HelpExampleCli("firewallbaninvalidwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_BAN_INVALIDWALLET = true;
    }
    else
    {
        FIREWALL_BAN_INVALIDWALLET = false;
    }


    Object result;
    result.push_back(Pair("ban-invalidwallet", strCommand));

return result;
}


Value firewallbantimeinvalidwallet(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbantimeinvalidwallet \"seconds\"\n"
                            "\nBitcoin Firewall Ban Time Invalid Wallet Rule #2\n"
                            "\nArguments:\n"
                            "Value: \"0|100000\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - 24h\n"
                            + HelpExampleCli("firewallbantimeinvalidwallet", "0")
                            + HelpExampleCli("firewallbantimeinvalidwallet", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_BANTIME_INVALIDWALLET = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("bantime-invalidwallet", FIREWALL_BANTIME_INVALIDWALLET));

return result;
}


Value firewallinvalidwalletminprotocol(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallinvalidwalletminprotocol \"protocol\"\n"
                            "\nBitcoin Firewall Min Protocol Invalid Wallet Rule #2\n"
                            "\nArguments:\n"
                            "Value: \"0|100000\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewallinvalidwalletminprotocol", "0")
                            + HelpExampleCli("firewallinvalidwalletminprotocol", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_MINIMUM_PROTOCOL = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("minprotocol-invalidwallet", FIREWALL_MINIMUM_PROTOCOL));

return result;
}


Value firewallinvalidwalletmaxcheck(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallinvalidwalletmaxcheck \"seconds\"\n"
                            "\nBitcoin Firewall Max Check Invalid Wallet Rule #2\n"
                            "\nArguments:\n"
                            "Value: \"0|100000\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewallinvalidwalletmaxcheck", "0")
                            + HelpExampleCli("firewallinvalidwalletmaxcheck", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_INVALIDWALLET_MAXCHECK = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("maxcheck-invalidwallet", FIREWALL_INVALIDWALLET_MAXCHECK));

return result;
}


Value firewalldetectforkedwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldetectforkedwallet \"true|false\"\n"
                            "\nBitcoin Firewall Detect Forked Wallet Rule #3\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewalldetectforkedwallet", "true")
                            + HelpExampleCli("firewalldetectforkedwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_DETECT_FORKEDWALLET = true;
    }
    else
    {
        FIREWALL_DETECT_FORKEDWALLET = false;
    }


    Object result;
    result.push_back(Pair("detect-forkedwallet", strCommand));

return result;
}


Value firewallblacklistforkedwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallblacklistforkedwallet \"true|false\"\n"
                            "\nBitcoin Firewall Blacklist Forked Wallet Rule #3 (session)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallblacklistforkedwallet", "true")
                            + HelpExampleCli("firewallblacklistforkedwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_BLACKLIST_FORKEDWALLET = true;
    }
    else
    {
        FIREWALL_BLACKLIST_FORKEDWALLET = false;
    }


    Object result;
    result.push_back(Pair("blacklist-forkedwallet", strCommand));

return result;
}


Value firewallbanforkedwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbanforkedwallet \"true|false\"\n"
                            "\nBitcoin Firewall Ban Forked Wallet Rule #3 (permenant)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallbanforkedwallet", "true")
                            + HelpExampleCli("firewallbanforkedwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_BAN_FORKEDWALLET = true;
    }
    else
    {
        FIREWALL_BAN_FORKEDWALLET = false;
    }


    Object result;
    result.push_back(Pair("ban-forkedwallet", strCommand));

return result;
}


Value firewallbantimeforkedwallet(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbantimeforkedwallet \"seconds\"\n"
                            "\nBitcoin Firewall Ban Time Forked Wallet Rule #3\n"
                            "\nArguments:\n"
                            "Value: \"seconds\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - 24h\n"
                            + HelpExampleCli("firewallbantimeinvalidwallet", "0")
                            + HelpExampleCli("firewallbantimeinvalidwallet", "10000000")
                            );

    if (params.size() == 1)
    {
         FIREWALL_BANTIME_FORKEDWALLET = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("bantime-forkedwallet", FIREWALL_BANTIME_FORKEDWALLET));

return result;
}


Value firewallforkedwalletnodeheight(const Array& params, bool fHelp)
{
    string MSG;

    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallforkedwalletnodeheight \"blockheight\"\n"
                            "\nBitcoin Firewall Adds Forked NodeHeight Flooding Wallet Rule #3\n"
                            "\nArguments:\n"
                            "Value: \"blockheight\" (int, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewallforkedwalletnodeheight", "0")
                            + HelpExampleCli("firewallforkedwalletnodeheight", "10000000")
                            );

    if (params.size() == 1)
    {
        if (CountIntArray(FIREWALL_FORKED_NODEHEIGHT) < 256)
        {
            FIREWALL_FORKED_NODEHEIGHT[CountIntArray(FIREWALL_FORKED_NODEHEIGHT)] = (int)strtod(params[0].get_str().c_str(), NULL);
            MSG = CountIntArray(FIREWALL_FORKED_NODEHEIGHT);
        }
        else
        {
            MSG = "Over 256 Max!";
        }
    }

    Object result;
    result.push_back(Pair("attackpattern-forkedwallet-nodeheight-add", MSG));

return result;
}


Value firewalldetectfloodingwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewalldetectfloodingwallet \"true|false\"\n"
                            "\nBitcoin Firewall Detect Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewalldetectfloodingwallet", "true")
                            + HelpExampleCli("firewalldetectfloodingwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_DETECT_FLOODINGWALLET = true;
    }
    else
    {
        FIREWALL_DETECT_FLOODINGWALLET = false;
    }


    Object result;
    result.push_back(Pair("detect-floodingwallet", strCommand));

return result;
}


Value firewallblacklistfloodingwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallblacklistfloodingwallet \"true|false\"\n"
                            "\nBitcoin Firewall Blacklist Flooding Wallet Rule #4 (session)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallblacklistfloodingwallet", "true")
                            + HelpExampleCli("firewallblacklistfloodingwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_BLACKLIST_FLOODINGWALLET = true;
    }
    else
    {
        FIREWALL_BLACKLIST_FLOODINGWALLET = false;
    }


    Object result;
    result.push_back(Pair("blacklist-floodingwallet", strCommand));

return result;
}


Value firewallbanfloodingwallet(const Array& params, bool fHelp)
{
    string strCommand = "true";
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbanfloodingwallet \"true|false\"\n"
                            "\nBitcoin Firewall Ban Flooding Wallet Rule #4 (permenant)\n"
                            "\nArguments:\n"
                            "Status: \"true|false\" (bool, required)\n"
                            "\nExamples:\n"
                            + HelpExampleCli("firewallbanfloodingwallet", "true")
                            + HelpExampleCli("firewallbanfloodingwallet", "false")
                            );

    if (params.size() == 1)
    {
        strCommand = params[0].get_str();
    }

    if (strCommand == "true")
    {
        FIREWALL_BAN_FLOODINGWALLET = true;
    }
    else
    {
        FIREWALL_BAN_FLOODINGWALLET = false;
    }


    Object result;
    result.push_back(Pair("ban-floodingwallet", strCommand));

return result;
}


Value firewallbantimefloodingwallet(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbantimefloodingwallet \"seconds\"\n"
                            "\nBitcoin Firewall Ban Time Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Value: \"seconds\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - 24h\n"
                            + HelpExampleCli("firewallbantimefloodingwallet", "0")
                            + HelpExampleCli("firewallbantimefloodingwallet", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_BANTIME_FLOODINGWALLET = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("bantime-floodingwallet", FIREWALL_BANTIME_FLOODINGWALLET));

return result;
}


Value firewallfloodingwalletminbytes(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallfloodingwalletminbytes \"bytes\"\n"
                            "\nBitcoin Firewall Min Bytes Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Value: \"Bytes\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - h\n"
                            + HelpExampleCli("firewallfloodingwalletminbytes", "0")
                            + HelpExampleCli("firewallfloodingwalletminbytes", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_FLOODINGWALLET_MINBYTES = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("minbytes-floodingwallet", FIREWALL_FLOODINGWALLET_MINBYTES));

return result;
}


Value firewallfloodingwalletmaxbytes(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallfloodingwalletmaxbytes \"bytes\"\n"
                            "\nBitcoin Firewall Max Bytes Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Value: \"bytes\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewallfloodingwalletmaxbytes", "0")
                            + HelpExampleCli("firewallfloodingwalletmaxbytes", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_FLOODINGWALLET_MAXBYTES = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("bantime-floodingwallet", FIREWALL_FLOODINGWALLET_MAXBYTES));

return result;
}


Value firewallfloodingwalletattackpatternadd(const Array& params, bool fHelp)
{
    string MSG;

    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallfloodingwalletattackpatternadd \"warnings\"\n"
                            "\nBitcoin Firewall Adds Attack Pattern Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Value: \"warnings\" (string, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewallfloodingwalletattackpatternadd", "0")
                            + HelpExampleCli("firewallfloodingwalletattackpatternadd", "10000000")
                            );

    if (params.size() == 1)
    {
        if (CountStringArray(FIREWALL_FLOODPATTERNS) < 256)
        {
            FIREWALL_FLOODPATTERNS[CountStringArray(FIREWALL_FLOODPATTERNS)] = params[0].get_str().c_str();
            MSG = CountStringArray(FIREWALL_FLOODPATTERNS);
        }
        else
        {
            MSG = "Over 256 Max!";
        }
    }

    Object result;
    result.push_back(Pair("attackpattern-floodingwallet-attackpattern-add", MSG));

return result;
}


Value firewallfloodingwalletattackpatternremove(const Array& params, bool fHelp)
{
    string MSG;
    int i;

    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallfloodingwalletattackpatternremove \"warnings\"\n"
                            "\nBitcoin Firewall Remove Attack Pattern Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Value: \"warnings\" (string, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewallfloodingwalletattackpatternremove", "0")
                            + HelpExampleCli("firewallfloodingwalletattackpatternremove", "10000000")
                            );

    if (params.size() == 1)
    {
        string WARNING;
        int TmpFloodPatternsCount;
        WARNING = params[0].get_str().c_str();
        TmpFloodPatternsCount = CountStringArray(FIREWALL_FLOODPATTERNS);

        MSG = "Not Found";

        for (i = 0; i < TmpFloodPatternsCount; i++)
        {  
            if (WARNING == FIREWALL_FLOODPATTERNS[i])
            {
                MSG = FIREWALL_FLOODPATTERNS[i];
                FIREWALL_FLOODPATTERNS[i] = "";
            }

        }
    }


    Object result;
    result.push_back(Pair("attackpattern-floodingwallet-attackpattern-remove", MSG));

return result;
}


Value firewallfloodingwalletmintrafficavg(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallfloodingwalletmintrafficavg \"ratio\"\n"
                            "\nBitcoin Firewall Min Traffic Average Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Value: \"ratio\" (double, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - 2000\n"
                            + HelpExampleCli("firewallfloodingwalletmintrafficav", "20000.01")
                            + HelpExampleCli("firewallfloodingwalletmintrafficav", "12000.014")
                            );

    if (params.size() == 1)
    {
        FIREWALL_FLOODINGWALLET_MINTRAFFICAVERAGE = strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("mintrafficavg-floodingwallet", FIREWALL_FLOODINGWALLET_MINTRAFFICAVERAGE));

return result;
}


Value firewallfloodingwalletmaxtrafficavg(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallbantimefloodingwallet \"ratio\"\n"
                            "\nBitcoin Firewall Max Traffic Average Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Value: \"ratio\" (double, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewallfloodingwalletmaxtrafficavg", "100.10")
                            + HelpExampleCli("ffirewallfloodingwalletmaxtrafficavg", "10.8")
                            );

    if (params.size() == 1)
    {
        FIREWALL_FLOODINGWALLET_MAXTRAFFICAVERAGE = strtod(params[0].get_str().c_str(), NULL);;
    }

    Object result;
    result.push_back(Pair("trafficavg-floodingwallet", FIREWALL_FLOODINGWALLET_MAXTRAFFICAVERAGE));

return result;
}


Value firewallfloodingwalletmincheck(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallfloodingwalletmincheck \"seconds\"\n"
                            "\nBitcoin Firewall Ban Time Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Value: \"seconds\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewallfloodingwalletmincheck", "0")
                            + HelpExampleCli("firewallfloodingwalletmincheck", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_FLOODINGWALLET_MINCHECK = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("mincheck-floodingwallet", FIREWALL_FLOODINGWALLET_MINCHECK));

return result;
}


Value firewallfloodingwalletmaxcheck(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
                            "firewallfloodingwalletmaxcheck \"seconds\"\n"
                            "\nBitcoin Firewall Max Check Flooding Wallet Rule #4\n"
                            "\nArguments:\n"
                            "Value: \"seconds\" (integer, required)\n"
                            "\nExamples:\n"
                            "\n0 = default - \n"
                            + HelpExampleCli("firewallfloodingwalletmaxcheck", "0")
                            + HelpExampleCli("firewallfloodingwalletmaxcheck", "10000000")
                            );

    if (params.size() == 1)
    {
        FIREWALL_FLOODINGWALLET_MAXCHECK = (int)strtod(params[0].get_str().c_str(), NULL);
    }

    Object result;
    result.push_back(Pair("maxcheck-floodingwallet", FIREWALL_FLOODINGWALLET_MAXCHECK));

return result;
}
