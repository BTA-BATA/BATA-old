// [Bitcoin Firewall 1.2.3
// Oct 14, 2017 - Biznatch Enterprises & Bata Development
// http://bata.io & https://github.com/BiznatchEnterprises/BitcoinFirewall
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "net.h"
#include "firewall.h"
#include <string>
#include "addrman.h"

#ifdef WIN32
#include <string.h>
#else
#include <fcntl.h>
#endif

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

bool ForceDisconnectNode(CNode *pnode, string FromFunction)
{

    // [Force Disconnection of node/peer]
    //
    //      Hard-disconnection function (Panic)

    TRY_LOCK(pnode->cs_vSend, lockSend);
        if (lockSend){
            // release outbound grant (if any)
            pnode->CloseSocketDisconnect();
            LogPrintStr(ModuleName + " - (" + FromFunction + ") Panic Disconnect: " + pnode->addrName.c_str() + "\n");
return true;  
        }
        else
        {
            pnode->vSendMsg.end();
        }

return false;
}


bool CheckBlackList(CNode *pnode)
{
int i;

    for (i = 1; i < BlackListCounter + 1; i++)
    {  
        if (pnode->addrName == BLACKLIST[i])
        {   
            // Banned IP FOUND!
            return true;
        }
    }

    // Banned IP not found
    return false;
}


bool AddToBlackList(CNode *pnode)
{
// [AddTo Blacklist]

        // Restart Blacklist count
        if (BlackListCounter >  255)
        {
            BlackListCounter = 0;
        }

        if (CheckBlackList(pnode) == false)
        {
            // increase Blacklist count
            BlackListCounter = BlackListCounter + 1;
            // Add node IP to blacklist
            BLACKLIST[BlackListCounter] = pnode->addrName;

            // Append Blacklist to debug.log
            LogPrintStr(ModuleName + " - Blacklisted: " + pnode->addrName.c_str() + "\n");

return true;

        }

return false;

}


bool CheckAttack(CNode *pnode)
{
// [Intelligent Attack Protection]

    bool DETECTED = false;
    int nTimeConnected = GetTime() - pnode->nTimeConnected;
    string AttackType = "";

    int NodeHeight;
    if (pnode->nSyncHeight == 0)
    {
        NodeHeight = pnode->nStartingHeight;
    }
    else
    {
        NodeHeight = pnode->nSyncHeight;
    }
    

    // ---Filter 1 -------------
    if (DETECT_BANDWIDTH_ABUSE == true)
    {
    // * Attack detection #2 & 3
        // Calculate the ratio between Recieved bytes and Sent Bytes
        // Detect a valid syncronizaion vs. a flood attack
        
        if (nTimeConnected > 10)
        {
            // * Attack detection #2
            // Node is further ahead on the chain than average minimum
            if (NodeHeight > CurrentAverageHeight_Min)
            {
                if (pnode->nTrafficAverage < CurrentAverageTraffic_Min)
                {
                        // too low bandiwidth ratio limits
                        DETECTED = true;
                        AttackType = "2-LowBW-HighHeight";
                }

                if (pnode->nTrafficAverage > CurrentAverageTraffic_Max)
                {
                        // too high bandiwidth ratio limits
                        DETECTED = true;
                        AttackType = "2-HighBW-HighHeight";
                }
            }

            // * Attack detection #3
            // Node is behind on the chain than average minimum
            if (NodeHeight < CurrentAverageHeight_Min)
            {  
                if (pnode->nTrafficAverage < CurrentAverageTraffic_Min)
                {
                        // too low bandiwidth ratio limits
                        DETECTED = true;
                        AttackType = "3-LowBW-LowHeight";
                }

                if (pnode->nTrafficAverage > CurrentAverageTraffic_Max)
                {

                    // too high bandiwidth ratio limits
                    DETECTED = true;
                    AttackType = "3-HighBW-LowHeight";
                }
            }
        }

        if (DETECTED == true)
        {
            if (BLACKLIST_BANDWIDTH_ABUSE == true)
            {
                BLACKLIST_ATTACK = true;
            }

            if (BAN_BANDWIDTH_ABUSE == true)
            {
                BAN_ATTACK = true;
            }

        }
    }
    // ----------------

    if (FALSE_POSITIVE_PROTECTION == true)
    {
        // ----------------
        // FALSE POSITIVE PROTECTION
        if (DETECTED == true)
        {

            if (AttackType == "2-LowBW-HighHeight")
            {
                AttackType = "";
                DETECTED = false;
            }   

            if (AttackType == "2-HighBW-HighHeight")
            {
                // Node/peer is in wallet sync (catching up to full blockheight)
                AttackType = "";
                DETECTED = false;
            }

            if (AttackType == "3-LowBW-LowHeight")
            {
                AttackType = "";
                DETECTED = false;
            }   

           if (AttackType == "3-HighBW-LowHeight")
            {
                double tnTraffic = pnode->nSendBytes / pnode->nRecvBytes;
                if (pnode->nTrafficAverage < CurrentAverageTraffic_Max)
                {
                    if (tnTraffic > 17.2 || tnTraffic < 17.1)
                    {
                        // wallet full sync
                        AttackType = "";
                        DETECTED = false;
                    }
                }

                if (pnode->nSendBytes > pnode->nRecvBytes)
                {
                    // wallet full sync
                    AttackType = "";
                    DETECTED = false;
                }
            }   
        }
    }
    // ----------------

    // ---Filter 2-------------
    if (DETECT_INVALID_WALLET == true)
    {
        // Start Height = -1
        // Check for more than ALL_CHECK_MAX minutes connection length
        if (nTimeConnected > ALL_CHECK_MAX * 60)
        {
            // Check for -1 blockheight
            if (pnode->nStartingHeight == -1)
            {
                // Trigger Blacklisting
                DETECTED = true;
                AttackType = "1-StartHeight-Invalid";
            }
        }

        // Check for -1 blockheight
        if (nTimeConnected > ALL_CHECK_MAX * 60)
        {
            // Check for -1 blockheight
            if (pnode->nStartingHeight < 0)
            {
                // Trigger Blacklisting
                DETECTED = true;
                AttackType = "1-StartHeight-Invalid";
            }
        }
        
        // (Protocol: 0
        // Check for more than ALL_CHECK_MAX minutes connection length
        if (nTimeConnected > ALL_CHECK_MAX * 60)
        {
            // Check for 0 protocol
            if (pnode->nRecvVersion == 0)
            {
                // Trigger Blacklisting
                DETECTED = true;
                AttackType = "1-Protocol-Invalid";
            }
        }

        // (Protocol: 0
        // Check for more than ALL_CHECK_MAX minutes connection length
        if (nTimeConnected > ALL_CHECK_MAX * 60)
        {
            // Check for 
            if (pnode->nRecvVersion < 1)
            {
                // Trigger Blacklisting
                DETECTED = true;
                AttackType = "1-Protocol-Invalid";
            }
        }

        // Resetting sync Height
        if (nTimeConnected > 60)
        {
            if (pnode->nSyncHeight > pnode->nSyncHeightOld)
            {
                pnode->nSyncHeightOld = pnode->nSyncHeight;
            }

            if (pnode->nSyncHeight < pnode->nSyncHeightOld)
            {
                // Trigger Blacklisting
                DETECTED = true;
                AttackType = "1-SyncReset";
            }

        }

        if (DETECTED == true)
        {
            if (BLACKLIST_INVALID_WALLET == true)
            {
                BLACKLIST_ATTACK = true;
            }

            if (BAN_INVALID_WALLET == true)
            {
                BAN_ATTACK = true;
            }

        }
    }

    if (INTERVAL_CHECK_ALL == true)
    {

        if (DETECTED == false)
        {
            int GlobalTimeOn = GetTime() - ALL_CHECK_TIMER;
            if (GlobalTimeOn > ALL_CHECK_MAX * 60)
            {
                ALL_CHECK_TIMER = GetTime();

                if (LIVE_DEBUG_OUTPUT == true)
                {
                    cout<<ModuleName<<" - Interval Check: "<<ALL_CHECK_MAX<<" minutes"<<endl;
                }

                BOOST_FOREACH(CNode* pnode, vNodes)
                {
                    int tnTimeConnected = GetTime() - pnode->nTimeConnected;
                    if (tnTimeConnected > 60)
                    {
                        DETECTED = false;
                        AttackType = "";

                        if (pnode->nSyncHeight == 0)
                        {
                            NodeHeight = pnode->nStartingHeight;
                        }
                        else
                        {
                            NodeHeight = pnode->nSyncHeight;
                        }

                        // Detect for invalid height
                        if (pnode->nStartingHeight < 0)
                        {
                            DETECTED = true;
                            AttackType = "1-Invalid-Height";

                        }

                        // Detect for Obsolete Version
                        if (pnode->nRecvVersion < MINIMUM_PROTOCOL)
                        {
                            DETECTED = true;
                            AttackType = "1-Obsolete-Version";
                        }

                        // Detect for Double-spend signatures
                        if (NodeHeight < CurrentAverageHeight_Min)
                        {
                            if (pnode->nTrafficAverage > CurrentAverageTraffic_Max)
                            {
                                double tnTraffic = pnode->nSendBytes / pnode->nRecvBytes;
                                if (tnTraffic > 17.1)
                                {
                                    if (tnTraffic < 17.12)
                                    {
                                    // Double Spend
                                    AttackType = "1-Double-Spend-a";
                                    DETECTED = true;
                                    }
                                }

                            }

                            if (tnTimeConnected > 200)
                            {
                                if (tnTimeConnected < 600)
                                {

                                    if (pnode->nSendBytes == pnode->nRecvBytes)
                                    {
                                        // Double Spend
                                        AttackType = "1-Double-Spend-b";
                                        DETECTED = true;  
                                    }

                                }
                            }

                        }

                        if (LIVE_DEBUG_OUTPUT == true)
                        {
                            cout<<ModuleName<<" [Check Node IP: "<<pnode->addrName.c_str()<<"] [Protocol: "<<pnode->nRecvVersion<<"] [Send: "<<pnode->nSendBytes<<"] [Recv: "<<pnode->nRecvBytes<<"] [SyncHeight: "<<pnode->nSyncHeight<<"] [ Detected: "<<DETECTED<<"]"<<endl;
                        }

                        if (DETECTED == true)
                        {

                            std::string NodeTrafficRatioStr = boost::lexical_cast<std::string>(pnode->nTrafficRatio);
                            std::string NodeTrafficAverageStr = boost::lexical_cast<std::string>(pnode->nTrafficAverage);
                            std::string CurrentAverageTrafficStr = boost::lexical_cast<std::string>(CurrentAverageTraffic);
                            std::string SendBytesStr = boost::lexical_cast<std::string>(pnode->nSendBytes);
                            std::string RecvBytesStr = boost::lexical_cast<std::string>(pnode->nRecvBytes);
                            std::string SyncHeight = boost::lexical_cast<std::string>(pnode->nSyncHeight);
                            std::string NodeProtocol = boost::lexical_cast<std::string>(pnode->nRecvVersion);

                            LogPrintStr(ModuleName + " - [Attack Type: " +  AttackType + "] [Detected from: " + pnode->addrName.c_str() + "] [Node Traffic: " + NodeTrafficRatioStr +  "] [Node Traffic Avrg: " + NodeTrafficAverageStr + "] [Traffic Avrg: " + CurrentAverageTrafficStr + "] [Sent Bytes: " + SendBytesStr + "] [Recv Bytes: " + RecvBytesStr + "] [Sync Height: " + SyncHeight + "] [Protocol: " + NodeProtocol + "\n");

                            if (BLACKLIST_INVALID_WALLET == true)
                            {   
                                AddToBlackList(pnode);
                            }
                            ForceDisconnectNode(pnode, "Interval");

                        }
                    }
                }
            }
        }

        if (DETECTED == true)
        {
            //if (BLACKLIST_INVALID_WALLET == true)
            //{
                BLACKLIST_ATTACK = true;
            //}

            //if (BAN_INVALID_WALLET == true)
            //{
                BAN_ATTACK = true;
            //}
        }
    }


    // ----------------
    // ATTACK DETECTED (TRIGGER)!
    if (DETECTED == true)
    {

        std::string NodeTrafficRatioStr = boost::lexical_cast<std::string>(pnode->nTrafficRatio);
        std::string NodeTrafficAverageStr = boost::lexical_cast<std::string>(pnode->nTrafficAverage);
        std::string CurrentAverageTrafficStr = boost::lexical_cast<std::string>(CurrentAverageTraffic);
        std::string SendBytesStr = boost::lexical_cast<std::string>(pnode->nSendBytes);
        std::string RecvBytesStr = boost::lexical_cast<std::string>(pnode->nRecvBytes);
        std::string NodeProtocol = boost::lexical_cast<std::string>(pnode->nRecvVersion);
        std::string NodeSyncHeight = boost::lexical_cast<std::string>(pnode->nSyncHeight);

        LogPrintStr(ModuleName + " - [Attack Type: " +  AttackType + "] [Detected from: " + pnode->addrName.c_str() + "] [Node Traffic: " + NodeTrafficRatioStr +  "] [Node Traffic Avrg: " + NodeTrafficAverageStr + "] [Traffic Avrg: " + CurrentAverageTrafficStr + "] [Sent Bytes: " + SendBytesStr + "] [Recv Bytes: " + RecvBytesStr + "] [Sync Height: " + NodeSyncHeight + "] [Protocol: " + NodeProtocol + "\n");

        // Blacklist IP on Attack detection
        // * add node/peer IP to blacklist
        if (BLACKLIST_ATTACK == true)
        {
            AddToBlackList(pnode);
        }

        // Peer/Node Ban if required
        if (BAN_ATTACK == true)
        {
            pnode->Ban(pnode->addr);
            //CNode::Ban(pnode->addr);
            LogPrintStr(ModuleName + " - Banned: " + pnode->addrName.c_str() + "\n");
        }

// ATTACK DETECTED!
return true;

    }
    else
    {
//NO ATTACK DETECTED...
return false;

    }
    // ----------------

}


void Examination(CNode *pnode, string FromFunction)
{
// Calculate new Height Average from all peers connected

    bool UpdateNodeStats = false;
    int NodeHeight;

    if (CheckAttack(pnode) == true)
    { 
        // Peer/Node Panic Disconnect
        ForceDisconnectNode(pnode, FromFunction);
    }

    if (pnode->nSyncHeight == 0)
    {
        NodeHeight = pnode->nStartingHeight;
    }
    else
    {
        NodeHeight = pnode->nSyncHeight;
    }

    // ** Update current average if increased ****
    if (NodeHeight > CurrentAverageHeight) 
    {
        CurrentAverageHeight = CurrentAverageHeight + NodeHeight; 
        CurrentAverageHeight = CurrentAverageHeight / 2;
        CurrentAverageHeight = CurrentAverageHeight - AverageTolerance;      // reduce with tolerance
        CurrentAverageHeight_Min = CurrentAverageHeight - AverageRange;
        CurrentAverageHeight_Max = CurrentAverageHeight + AverageRange;
    }

    if (pnode->nRecvBytes > 0)
    {
        pnode->nTrafficRatio = pnode->nSendBytes / (double)pnode->nRecvBytes;

        if (pnode->nTrafficTimestamp == 0)
        {
            UpdateNodeStats = true;
        }

        if (GetTime() - pnode->nTrafficTimestamp > 5){
            UpdateNodeStats = true;
        }

            pnode->nTrafficAverage = pnode->nTrafficAverage + (double)pnode->nTrafficRatio / 2;
            pnode->nTrafficTimestamp = GetTime();

        if (UpdateNodeStats == true)
        {   
            CurrentAverageTraffic = CurrentAverageTraffic + (double)pnode->nTrafficAverage;
            CurrentAverageTraffic = CurrentAverageTraffic / (double)2;
            CurrentAverageTraffic = CurrentAverageTraffic - (double)TrafficTolerance;      // reduce with tolerance
            CurrentAverageTraffic_Min = CurrentAverageTraffic - (double)TrafficZone;
            CurrentAverageTraffic_Max = CurrentAverageTraffic + (double)TrafficZone;    
            CurrentAverageSend = CurrentAverageSend + pnode->nSendBytes / vNodes.size();
            CurrentAverageRecv = CurrentAverageRecv + pnode->nRecvBytes / vNodes.size();
            

            //std::ofstream fout(pathFirewallLog);

            if (LIVE_DEBUG_OUTPUT == true){
            cout<<ModuleName<<" [BlackListed Nodes/Peers: "<<BlackListCounter<<"]"<<endl;
            cout<<"[Traffic: "<<CurrentAverageTraffic<<"] [Traffic Min: "<<CurrentAverageTraffic_Min<<"] [Traffic Max: "<<CurrentAverageTraffic_Max<<"]"<<" [Safe Height: "<<CurrentAverageHeight<<"] [Height Min: "<<CurrentAverageHeight_Min<<"] [Height Max: "<<CurrentAverageHeight_Max<<"] [Send Avrg: "<<CurrentAverageSend<<"] [Rec Avrg: "<<CurrentAverageRecv<<"]"<<endl;
            cout<<"[Check Node IP: "<<pnode->addrName.c_str()<<"] [Traffic: "<<pnode->nTrafficRatio<<"] [Traffic Average: "<<pnode->nTrafficAverage<<"] [Starting Height: "<<pnode->nStartingHeight<<"] [Sync Height: "<<pnode->nSyncHeight<<"] [Node Sent: "<<pnode->nSendBytes<<"] [Node Recv: "<<pnode->nRecvBytes<<"] [Protocol: "<<pnode->nRecvVersion<<"]"<<endl;
            }

        }

    }

}


bool FireWall(CNode *pnode, string FromFunction)
{
    int i;

    if (ENABLE_FIREWALL == false)
    {
        return false;
    }

    for (i = 0; i < SeedNodeCounter; i++)
    {  
        // Check for Static Whitelisted Seed Node
        if (pnode->addrName == IgnoreSeedNode[i])
        {
            return false;
        }
    }

    // Check for Node Whitelisted status
    if (pnode->fWhitelisted == true)
    {
        return false;
    }

    if (FIREWALL_CLEAR_BANS == true)
    {
        pnode->ClearBanned();
    }

    // Check for 0 peer count (auto-unban)
    if (vNodes.size() == 0){
        pnode->ClearBanned();
    }


    if (CheckBlackList(pnode) == true)
    {
        FromFunction = "Blacklisted";

// Peer/Node Panic Disconnect
ForceDisconnectNode(pnode, FromFunction);
return true;

    }

    // Perform a Node consensus examination
Examination(pnode, FromFunction);

// Peer/Node Safe    
return false;

}
