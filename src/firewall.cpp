// [Bitcoin Firewall 1.2.4
// March 21, 2018 - Biznatch Enterprises & Profit Hunters Coin (PHC)
// https://github.com/BiznatchEnterprises/BitcoinFirewall
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



// * Function: BoolToString *
inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}


// * Function: CountArray *
int CountStringArray(string *ArrayName)
{
    int tmp_cnt;
    tmp_cnt = 0;

    while(ArrayName[tmp_cnt] != "")
    {
        tmp_cnt++;
    }


return tmp_cnt;
}


// * Function: CountArray *
int CountIntArray(int *ArrayName)
{
    int tmp_cnt;
    tmp_cnt = 0;

    while(ArrayName[tmp_cnt] > 0)
    {
        tmp_cnt++;
    }

return tmp_cnt;
}



string ModuleName = "[Bitcoin Firewall 1.2.4]";

// *** Firewall Controls (General) ***
bool FIREWALL_ENABLED = false;
bool FIREWALL_CLEAR_BLACKLIST = false;
bool FIREWALL_CLEAR_BANS = false;
double FIREWALL_TRAFFIC_TOLERANCE = 0.0001; // Reduce for minimal fluctuation
double FIREWALL_TRAFFIC_ZONE = 4; // + or - Traffic Range 

// *** Firewall Debug (Live Output) ***
bool FIREWALL_LIVE_DEBUG = false;
bool FIREWALL_LIVEDEBUG_EXAM = true;
bool FIREWALL_LIVEDEBUG_BANS = true;
bool FIREWALL_LIVEDEBUG_BLACKLIST = true;
bool FIREWALL_LIVEDEBUG_DISCONNECT = true;
bool FIREWALL_LIVEDEBUG_BANDWIDTHABUSE = true;
bool FIREWALL_LIVEDEBUG_NOFALSEPOSITIVE = true;
bool FIREWALL_LIVEDEBUG_INVALIDWALLET = true;
bool FIREWALL_LIVEDEBUG_FORKEDWALLET = true;
bool FIREWALL_LIVEDEBUG_FLOODINGWALLET = true;

// *** Firewall Controls (Bandwidth Abuse) ***
bool FIREWALL_DETECT_BANDWIDTHABUSE = true;
bool FIREWALL_BLACKLIST_BANDWIDTHABUSE = true;
bool FIREWALL_BAN_BANDWIDTHABUSE = true;
bool FIREWALL_NOFALSEPOSITIVE_BANDWIDTHABUSE = true;

// *** Firewall Controls (Invalid Peer Wallets) ***
bool FIREWALL_DETECT_INVALIDWALLET = true;
bool FIREWALL_BLACKLIST_INVALIDWALLET = true;
bool FIREWALL_BAN_INVALIDWALLET = true;

// *** Firewall Controls (Forked Peer Wallets) ***
bool FIREWALL_DETECT_FORKEDWALLET = true;
bool FIREWALL_BLACKLIST_FORKEDWALLET = true;
bool FIREWALL_BAN_FORKEDWALLET = true;

// *** Firewall Controls (Flooding Peer Wallets) ***
bool FIREWALL_DETECT_FLOODINGWALLET = true;
bool FIREWALL_BLACKLIST_FLOODINGWALLET = true;
bool FIREWALL_BAN_FLOODINGWALLET = true;

// * Firewall Settings (General) *
//int FIREWALL_CHECK_MAX = 3;  // minutes interval for some detection settings

// * Firewall Settings (Exam) *
int FIREWALL_AVERAGE_TOLERANCE = 2;    // Reduce for minimal fluctuation 2 Blocks tolerance
int FIREWALL_AVERAGE_RANGE = 100;   // + or - Starting Height Range

// *** Firewall Settings (Bandwidth Abuse) ***
int FIREWALL_BANTIME_BANDWIDTHABUSE = 0; // 24 hours
int FIREWALL_BANDWIDTHABUSE_MAXCHECK = 10;
double FIREWALL_BANDWIDTHABUSE_MINATTACK = 17.1;
double FIREWALL_BANDWIDTHABUSE_MAXATTACK = 17.2;

// * Firewall Settings (Invalid Wallet)
int FIREWALL_MINIMUM_PROTOCOL = MIN_PEER_PROTO_VERSION;
int FIREWALL_BANTIME_INVALIDWALLET = 2600000; // 30 days
int FIREWALL_INVALIDWALLET_MAXCHECK = 60; // seconds

// * Firewall Settings (Forked Wallet)
int FIREWALL_BANTIME_FORKEDWALLET = 2600000; // 30 days
// FORKLIST (ignore)
int FIREWALL_FORKED_NODEHEIGHT[256] =
{

};

// * Firewall Settings (Flooding Wallet)
int FIREWALL_BANTIME_FLOODINGWALLET = 0; // 24 hours
int FIREWALL_FLOODINGWALLET_MINBYTES = 1000000;
int FIREWALL_FLOODINGWALLET_MAXBYTES = 1000000;
// Flooding Patterns (WARNINGS)
string FIREWALL_FLOODPATTERNS[256] =
{
    "56810121416192123", 
    "57910121517202223",
    "57910121416202223"
};
double FIREWALL_FLOODINGWALLET_MINTRAFFICAVERAGE = 2000; // Ratio Up/Down
double FIREWALL_FLOODINGWALLET_MAXTRAFFICAVERAGE = 2000; // Ratio Up/Down
int FIREWALL_FLOODINGWALLET_MINCHECK = 30; // seconds
int FIREWALL_FLOODINGWALLET_MAXCHECK = 90; // seconds

// Firewall Whitelist (ignore)
string FIREWALL_WHITELIST[256] =
{

};

// * Firewall BlackList Settings *
string FIREWALL_BLACKLIST[256] =
{

};

// * Global Firewall Variables *
bool Firewall_FirstRun = false;
int Firewall_AverageHeight = 0;
int Firewall_AverageHeight_Min = 0;
int Firewall_AverageHeight_Max = 0;
double Firewall_AverageTraffic = 0;
double Firewall_AverageTraffic_Min = 0;
double Firewall_AverageTraffic_Max = 0;
int Firewall_AverageSend = 0;
int Firewall_AverageRecv = 0;
int ALL_CHECK_TIMER = GetTime();

// * Function: LoadFirewallSettings (phc.conf)*
void LoadFirewallSettings()
{
    // *** Firewall Controls (General) ***
    FIREWALL_ENABLED = GetBoolArg("-firewallenabled", FIREWALL_ENABLED);
    FIREWALL_CLEAR_BLACKLIST = GetBoolArg("-firewallclearblacklist", FIREWALL_CLEAR_BLACKLIST);
    FIREWALL_CLEAR_BANS = GetBoolArg("-firewallclearbanlist", FIREWALL_CLEAR_BANS);

    // *** Firewall Debug (Live Output) ***
    FIREWALL_LIVE_DEBUG = GetBoolArg("-firewalldebug", FIREWALL_LIVE_DEBUG);
    FIREWALL_LIVEDEBUG_EXAM = GetBoolArg("-firewalldebugexam", FIREWALL_LIVEDEBUG_EXAM);
    FIREWALL_LIVEDEBUG_BANS = GetBoolArg("-firewalldebugbans", FIREWALL_LIVEDEBUG_BANS);
    FIREWALL_LIVEDEBUG_BLACKLIST = GetBoolArg("-firewalldebugblacklist", FIREWALL_LIVEDEBUG_BLACKLIST);
    FIREWALL_LIVEDEBUG_DISCONNECT = GetBoolArg("-firewalldebugdisconnect", FIREWALL_LIVEDEBUG_DISCONNECT);
    FIREWALL_LIVEDEBUG_BANDWIDTHABUSE = GetBoolArg("-firewalldebugbandwidthabuse", FIREWALL_LIVEDEBUG_BANDWIDTHABUSE);
    FIREWALL_LIVEDEBUG_NOFALSEPOSITIVE = GetBoolArg("-firewalldebugnofalsepositivebandwidthabuse", FIREWALL_LIVEDEBUG_NOFALSEPOSITIVE);
    FIREWALL_LIVEDEBUG_INVALIDWALLET = GetBoolArg("-firewalldebuginvalidwallet", FIREWALL_LIVEDEBUG_INVALIDWALLET);
    FIREWALL_LIVEDEBUG_FORKEDWALLET = GetBoolArg("-firewalldebugforkedwallet", FIREWALL_LIVEDEBUG_FORKEDWALLET);
    FIREWALL_LIVEDEBUG_FLOODINGWALLET = GetBoolArg("-firewalldebugfloodingwallet", FIREWALL_LIVEDEBUG_FLOODINGWALLET);

    // *** Firewall Controls (Bandwidth Abuse) ***
    FIREWALL_DETECT_BANDWIDTHABUSE = GetBoolArg("-firewalldetectbandwidthabuse", FIREWALL_DETECT_BANDWIDTHABUSE);
    FIREWALL_BLACKLIST_BANDWIDTHABUSE = GetBoolArg("-firewallblacklistbandwidthabuse", FIREWALL_BLACKLIST_BANDWIDTHABUSE);
    FIREWALL_BAN_BANDWIDTHABUSE = GetBoolArg("-firewallbanbandwidthabuse", FIREWALL_BAN_BANDWIDTHABUSE);
    FIREWALL_NOFALSEPOSITIVE_BANDWIDTHABUSE = GetBoolArg("-firewallnofalsepositivebandwidthabuse", FIREWALL_NOFALSEPOSITIVE_BANDWIDTHABUSE);

    // *** Firewall Controls (Invalid Peer Wallets) ***
    FIREWALL_DETECT_INVALIDWALLET = GetBoolArg("-firewalldetectinvalidwallet", FIREWALL_DETECT_INVALIDWALLET);
    FIREWALL_BLACKLIST_INVALIDWALLET = GetBoolArg("-firewallblacklistinvalidwallet", FIREWALL_BLACKLIST_INVALIDWALLET);
    FIREWALL_BAN_INVALIDWALLET = GetBoolArg("-firewallbaninvalidwallet", FIREWALL_BAN_INVALIDWALLET);

    // *** Firewall Controls (Forked Peer Wallets) ***
    FIREWALL_DETECT_FORKEDWALLET = GetBoolArg("-firewalldetectforkedwallet", FIREWALL_DETECT_FORKEDWALLET);
    FIREWALL_BLACKLIST_FORKEDWALLET = GetBoolArg("-firewallblacklistforkedwallet", FIREWALL_BLACKLIST_FORKEDWALLET);
    FIREWALL_BAN_FORKEDWALLET = GetBoolArg("-firewallbanforkedwallet", FIREWALL_BAN_FORKEDWALLET);

    // *** Firewall Controls (Flooding Peer Wallets) ***
    FIREWALL_DETECT_FLOODINGWALLET = GetBoolArg("-firewalldetectfloodingwallet", FIREWALL_DETECT_FLOODINGWALLET);
    FIREWALL_BLACKLIST_FLOODINGWALLET = GetBoolArg("-firewallblacklistfloodingwallet", FIREWALL_BLACKLIST_FLOODINGWALLET);
    FIREWALL_BAN_FLOODINGWALLET = GetBoolArg("-firewallbanfloodingwallet", FIREWALL_BAN_FLOODINGWALLET);

    // * Firewall Settings (Exam) *
    FIREWALL_TRAFFIC_TOLERANCE = GetArg("-firewalltraffictolerance", FIREWALL_TRAFFIC_TOLERANCE);
    FIREWALL_TRAFFIC_ZONE = GetArg("-firewalltrafficzone", FIREWALL_TRAFFIC_ZONE);

    // * Firewall Settings (Bandwidth Abuse) *
    FIREWALL_BANTIME_BANDWIDTHABUSE = GetArg("-firewallbantimebandwidthabuse", FIREWALL_BANTIME_BANDWIDTHABUSE);
    FIREWALL_BANDWIDTHABUSE_MAXCHECK = GetArg("-firewallbandwidthabusemaxcheck", FIREWALL_BANDWIDTHABUSE_MAXCHECK);
    FIREWALL_BANDWIDTHABUSE_MINATTACK = GetArg("-firewallbandwidthabuseminattack", FIREWALL_BANDWIDTHABUSE_MINATTACK);
    FIREWALL_BANDWIDTHABUSE_MAXATTACK = GetArg("-firewallbandwidthabusemaxattack", FIREWALL_BANDWIDTHABUSE_MAXATTACK);

    // * Firewall Settings (Invalid Wallet)
    FIREWALL_MINIMUM_PROTOCOL = GetArg("-firewallinvalidwalletminprotocol", FIREWALL_MINIMUM_PROTOCOL);
    FIREWALL_BANTIME_INVALIDWALLET = GetArg("-firewallbantimeinvalidwallet", FIREWALL_BANTIME_INVALIDWALLET);
    FIREWALL_INVALIDWALLET_MAXCHECK = GetArg("-firewallinvalidwalletmaxcheck", FIREWALL_INVALIDWALLET_MAXCHECK);

    // * Firewall Settings (Forked Wallet)
    FIREWALL_BANTIME_FORKEDWALLET = GetArg("-firewallbantimeforkedwallet", FIREWALL_BANTIME_FORKEDWALLET);

    // * Firewall Settings (Flooding Wallet)
    FIREWALL_BANTIME_FLOODINGWALLET = GetArg("-firewallbantimefloodingwallet", FIREWALL_BANTIME_FLOODINGWALLET);
    FIREWALL_FLOODINGWALLET_MINBYTES = GetArg("-firewallfloodingwalletminbytes", FIREWALL_FLOODINGWALLET_MINBYTES);
    FIREWALL_FLOODINGWALLET_MAXBYTES = GetArg("-firewallfloodingwalletmaxbytes", FIREWALL_FLOODINGWALLET_MAXBYTES);

    if (GetArg("-firewallfloodingwalletattackpattern", "-") != "-")
    {
        FIREWALL_FLOODPATTERNS[CountStringArray(FIREWALL_FLOODPATTERNS)] = GetArg("-firewallfloodingwalletattackpattern", "");
    }

    FIREWALL_FLOODINGWALLET_MINTRAFFICAVERAGE = GetArg("-firewallfloodingwalletmintrafficavg", FIREWALL_FLOODINGWALLET_MINTRAFFICAVERAGE);
    FIREWALL_FLOODINGWALLET_MAXTRAFFICAVERAGE = GetArg("-firewallfloodingwalletmaxtrafficavg", FIREWALL_FLOODINGWALLET_MAXTRAFFICAVERAGE);
    FIREWALL_FLOODINGWALLET_MINCHECK = GetArg("-firewallfloodingwalletmincheck", FIREWALL_FLOODINGWALLET_MINCHECK);
    FIREWALL_FLOODINGWALLET_MAXCHECK = GetArg("-firewallfloodingwalletmaxcheck", FIREWALL_FLOODINGWALLET_MAXCHECK);

return;
}


// * Function: ForceDisconnectNode *
bool ForceDisconnectNode(CNode *pnode, string FromFunction)
{
    TRY_LOCK(pnode->cs_vSend, lockSend);
        if (lockSend){

            // release outbound grant (if any)
            pnode->CloseSocketDisconnect();
            LogPrint("net", "%s (%s) Panic Disconnect: %s\n", ModuleName.c_str(), FromFunction, pnode->addrName.c_str());

            if (FIREWALL_LIVE_DEBUG == true)
            {
                if (FIREWALL_LIVEDEBUG_DISCONNECT == true)
                {
                    cout << ModuleName << "Panic Disconnect: " << pnode->addrName << "]\n" << endl;
                }
            }

return true;  
        }
        else
        {
            pnode->vSendMsg.end();
        }

return false;
}


// * Function: CheckBlackList *
bool CheckBlackList(CNode *pnode)
{
    int i;
    int TmpBlackListCount;
    TmpBlackListCount = CountStringArray(FIREWALL_BLACKLIST);

    if (TmpBlackListCount > 0)
    {
        for (i = 0; i < TmpBlackListCount; i++)
        {  
            if (pnode->addrName == FIREWALL_BLACKLIST[i])
            {   
// Banned IP FOUND!
return true;
            }
        }
    }

// Banned IP not found
return false;
}


// * Function:  *
bool CheckBanned(CNode *pnode)
{
    if (CNode::IsBanned(pnode->addr) == true)
    {
// Yes Banned!
return true;
    }

// No Banned!
return false;
}


// * Function: AddToBlackList *
bool AddToBlackList(CNode *pnode)
{
    int TmpBlackListCount;
    TmpBlackListCount = CountStringArray(FIREWALL_BLACKLIST);

        // Restart Blacklist count
        if (TmpBlackListCount >  255)
        {
            TmpBlackListCount = 0;
        }

        if (CheckBlackList(pnode) == false)
        {
            // increase Blacklist count
            TmpBlackListCount = TmpBlackListCount + 1;
            // Add node IP to blacklist
            FIREWALL_BLACKLIST[TmpBlackListCount] = pnode->addrName;

            if (FIREWALL_LIVE_DEBUG == true)
            {
                if (FIREWALL_LIVEDEBUG_BLACKLIST == true)
                {
                    cout << ModuleName << "Blacklisted: " << pnode->addrName << "]\n" << endl;
                }
            }

            // Append Blacklist to debug.log
            LogPrint("net", "%s Blacklisted: %s\n", ModuleName.c_str(), pnode->addrName.c_str());

return true;
        }

return false;
}


// * Function: AddToBanList *
bool AddToBanList(CNode *pnode, BanReason BAN_REASON, int BAN_TIME)
{
    CNode::Ban(pnode->addr, BAN_REASON, BAN_TIME, false);
    //DumpBanlist();

    LogPrint("net", "%s Banned: %s\n", ModuleName.c_str(), pnode->addrName);

    if (FIREWALL_LIVE_DEBUG == true)
    {
        if (FIREWALL_LIVEDEBUG_BANS == true)
        {
            cout << ModuleName << "Banned: " << pnode->addrName << "]\n" << endl;
        }
    }

return true;
}


// * Function: CheckAttack *
// Artificially Intelligent Attack Detection & Mitigation
bool CheckAttack(CNode *pnode, string FromFunction)
{
    bool DETECTED_ATTACK = false;
    
    bool BLACKLIST_ATTACK = false;

    int BAN_TIME = 0; // Default 24 hours
    bool BAN_ATTACK = false;

    BanReason BAN_REASON;

    string ATTACK_CHECK_NAME;
    string ATTACK_CHECK_LOG;

    int nTimeConnected = GetTime() - pnode->nTimeConnected;
    string ATTACK_TYPE = "";

    int NodeHeight;

    if (pnode->nSyncHeight == 0)
    {
        NodeHeight = pnode->nStartingHeight;
    }
    else
    {
        NodeHeight = pnode->nSyncHeight;
    }

    if (pnode->nSyncHeight < pnode->nStartingHeight)
    {
        NodeHeight = pnode->nStartingHeight;
    }
   

    // ---Filter 1 -------------
    if (FIREWALL_DETECT_BANDWIDTHABUSE == true)
    {
        ATTACK_CHECK_NAME = "Bandwidth Abuse";

        // ### Attack Detection ###
        // Calculate the ratio between Recieved bytes and Sent Bytes
        // Detect a valid syncronizaion vs. a flood attack
        
        if (nTimeConnected > FIREWALL_BANDWIDTHABUSE_MAXCHECK)
        {
            // * Attack detection #2
            // Node is further ahead on the chain than average minimum
            if (NodeHeight > Firewall_AverageHeight_Min)
            {
                if (pnode->nTrafficAverage < Firewall_AverageTraffic_Min)
                {
                    // too low bandiwidth ratio limits
                    DETECTED_ATTACK = true;
                    ATTACK_TYPE = "2-LowBW-HighHeight";
                }

                if (pnode->nTrafficAverage > Firewall_AverageTraffic_Max)
                {
                    // too high bandiwidth ratio limits
                    DETECTED_ATTACK = true;
                    ATTACK_TYPE = "2-HighBW-HighHeight";
                }
            }

            // * Attack detection #3
            // Node is behind on the chain than average minimum
            if (NodeHeight < Firewall_AverageHeight_Min)
            {  
                if (pnode->nTrafficAverage < Firewall_AverageTraffic_Min)
                {
                    // too low bandiwidth ratio limits
                    DETECTED_ATTACK = true;
                    ATTACK_TYPE = "3-LowBW-LowHeight";
                }

                if (pnode->nTrafficAverage > Firewall_AverageTraffic_Max)
                {

                    // too high bandiwidth ratio limits
                    DETECTED_ATTACK = true;
                    ATTACK_TYPE = "3-HighBW-LowHeight";
                }
            }
        }

        if (FIREWALL_LIVEDEBUG_BANDWIDTHABUSE == true)
        {
            ATTACK_CHECK_LOG = ATTACK_CHECK_LOG  + " {" +  ATTACK_CHECK_NAME + ":" + BoolToString(DETECTED_ATTACK) + "}";
        }

        // ### Attack Mitigation ###
        if (DETECTED_ATTACK == true)
        {
            if (FIREWALL_BLACKLIST_BANDWIDTHABUSE == true)
            {
                BLACKLIST_ATTACK = true;
            }

            if (FIREWALL_BAN_BANDWIDTHABUSE == true)
            {
                BAN_ATTACK = true;
                BAN_TIME = FIREWALL_BANTIME_BANDWIDTHABUSE;
                BAN_REASON = BanReasonBandwidthAbuse;
            }

        }
        // ##########################
    }
    // ----------------

    if (FIREWALL_NOFALSEPOSITIVE_BANDWIDTHABUSE == true)
    {
        ATTACK_CHECK_NAME = "No False Positive - Bandwidth Abuse";

        // ### AVOID FALSE POSITIVE FROM BANDWIDTH ABUSE ###
        if (DETECTED_ATTACK == true)
        {

            if (ATTACK_TYPE == "2-LowBW-HighHeight")
            {
                ATTACK_TYPE = "";
                DETECTED_ATTACK = false;
            }   

            if (ATTACK_TYPE == "2-HighBW-HighHeight")
            {
                // Node/peer is in wallet sync (catching up to full blockheight)
                ATTACK_TYPE = "";
                DETECTED_ATTACK = false;
            }

            if (ATTACK_TYPE == "3-LowBW-LowHeight")
            {
                ATTACK_TYPE = "";
                DETECTED_ATTACK = false;
            }   

            if (ATTACK_TYPE == "3-HighBW-LowHeight")
            {
                double tnTraffic = pnode->nSendBytes / pnode->nRecvBytes;
                if (pnode->nTrafficAverage < Firewall_AverageTraffic_Max)
                {
                    if (tnTraffic < FIREWALL_BANDWIDTHABUSE_MINATTACK || tnTraffic > FIREWALL_BANDWIDTHABUSE_MAXATTACK)
                    {
                        // wallet full sync
                        ATTACK_TYPE = "";
                        DETECTED_ATTACK = false;
                    }
                }

                if (pnode->nSendBytes > pnode->nRecvBytes)
                {
                    // wallet full sync
                    ATTACK_TYPE = "";
                    DETECTED_ATTACK = false;
                }
            }   
        }
        
        if (FIREWALL_LIVEDEBUG_NOFALSEPOSITIVE == true)
        {
            ATTACK_CHECK_LOG = ATTACK_CHECK_LOG  + " {" +  ATTACK_CHECK_NAME + ":" + BoolToString(DETECTED_ATTACK) + "}";
        }

        // ##########################
    }
    // ----------------

    // ---Filter 2-------------
    if (FIREWALL_DETECT_INVALIDWALLET == true)
    {
        ATTACK_CHECK_NAME = "Invalid Wallet";

        // ### Attack Detection ###
        // Start Height = -1
        // Check for more than FIREWALL_INVALIDWALLET_MAXCHECK minutes connection length
        if (nTimeConnected > FIREWALL_INVALIDWALLET_MAXCHECK)
        {
            // Check for -1 blockheight
            if (pnode->nStartingHeight == -1)
            {
                // Trigger Blacklisting
                DETECTED_ATTACK = true;
                ATTACK_TYPE = "1-StartHeight-Invalid";
            }
        }

        // Check for -1 blockheight
        if (nTimeConnected > FIREWALL_INVALIDWALLET_MAXCHECK)
        {
            // Check for -1 blockheight
            if (pnode->nStartingHeight < 0)
            {
                // Trigger Blacklisting
                DETECTED_ATTACK = true;
                ATTACK_TYPE = "1-StartHeight-Invalid";
            }
        }
        
        // (Protocol: 0
        // Check for more than FIREWALL_INVALIDWALLET_MAXCHECK minutes connection length
        if (nTimeConnected > FIREWALL_INVALIDWALLET_MAXCHECK)
        {
            // Check for 0 protocol
            if (pnode->nRecvVersion == 0)
            {
                // Trigger Blacklisting
                DETECTED_ATTACK = true;
                ATTACK_TYPE = "1-Protocol-Invalid";
            }
        }

        // (Protocol: 0
        // Check for more than FIREWALL_INVALIDWALLET_MAXCHECK minutes connection length
        if (nTimeConnected > FIREWALL_INVALIDWALLET_MAXCHECK)
        {
            // Check for 
            if (pnode->nRecvVersion < 1)
            {
                // Trigger Blacklisting
                DETECTED_ATTACK = true;
                ATTACK_TYPE = "1-Protocol-Invalid";
            }
        }

        //// Resetting sync Height
        //if (nTimeConnected > 60)
        //{
            //if (pnode->nSyncHeight > pnode->nSyncHeightOld)
            //{
                //pnode->nSyncHeightOld = pnode->nSyncHeight;
            //}

            //if (pnode->nSyncHeight < pnode->nSyncHeightOld - FIREWALL_AVERAGE_RANGE)
            //{
                // Trigger Blacklisting
                //DETECTED = true;
                //ATTACK_TYPE = "1-SyncReset";
            //}

        //}
        // ##########################

        if (FIREWALL_LIVEDEBUG_INVALIDWALLET == true)
        {
            ATTACK_CHECK_LOG = ATTACK_CHECK_LOG  + " {" +  ATTACK_CHECK_NAME + ":" + BoolToString(DETECTED_ATTACK) + "}";
        }

        // ### Attack Mitigation ###
        if (DETECTED_ATTACK == true)
        {
            if (FIREWALL_BLACKLIST_INVALIDWALLET == true)
            {
                BLACKLIST_ATTACK = true;
            }

            if (FIREWALL_BAN_INVALIDWALLET == true)
            {
                BAN_ATTACK = true;
                BAN_TIME = FIREWALL_BANTIME_INVALIDWALLET;
                BAN_REASON = BanReasonInvalidWallet;
            }
        }
        // ##########################
    }
    //--------------------------


    // ---Filter 3-------------
    if (FIREWALL_DETECT_FORKEDWALLET == true)
    {

        ATTACK_CHECK_NAME = "Forked Wallet";

        // ### Attack Detection ###

        int i;
        int TmpNodeHeightCount;
        TmpNodeHeightCount = CountIntArray(FIREWALL_FORKED_NODEHEIGHT) - 2;
        
        if (TmpNodeHeightCount > 0)
        {
            for (i = 0; i < TmpNodeHeightCount; i++)
            { 
                // Check for Forked Wallet (stuck on blocks)
                if (pnode->nStartingHeight == (int)FIREWALL_FORKED_NODEHEIGHT[i])
                {
                    DETECTED_ATTACK = true;
                    ATTACK_TYPE = ATTACK_CHECK_NAME;
                }
                // Check for Forked Wallet (stuck on blocks)
                if (pnode->nSyncHeight == (int)FIREWALL_FORKED_NODEHEIGHT[i])
                {
                    DETECTED_ATTACK = true;
                    ATTACK_TYPE = ATTACK_CHECK_NAME;
                }
            }          
        }
        // #######################

        // ### LIVE DEBUG OUTPUT ####
        if (FIREWALL_LIVEDEBUG_FORKEDWALLET == true)
        {
            ATTACK_CHECK_LOG = ATTACK_CHECK_LOG  + " {" +  ATTACK_CHECK_NAME + ":" + BoolToString(DETECTED_ATTACK) + "}";
        }
        // #######################

        // ### Attack Mitigation ###
        if (DETECTED_ATTACK == true)
        {
            if (FIREWALL_BLACKLIST_FORKEDWALLET == true)
            {
                BLACKLIST_ATTACK = true;
            }

            if (FIREWALL_BAN_FORKEDWALLET == true)
            {
                BAN_ATTACK = true;

                BAN_TIME = FIREWALL_BANTIME_FORKEDWALLET;
                BAN_REASON = BanReasonForkedWallet;
            }
        }
        // #######################

    }
    //--------------------------


    // ---Filter 4-------------
    if (FIREWALL_DETECT_FLOODINGWALLET == true)
    {
        ATTACK_CHECK_NAME = "Flooding Wallet";
        std::size_t FLOODING_MAXBYTES = FIREWALL_FLOODINGWALLET_MAXBYTES;
        std::size_t FLOODING_MINBYTES = FIREWALL_FLOODINGWALLET_MINBYTES;
        
        string WARNINGS = "";

        // WARNING #1 - Too high of bandwidth with low BlockHeight
        if (NodeHeight < Firewall_AverageHeight_Min)
        {  
            if (pnode->nTrafficAverage > Firewall_AverageTraffic_Max)
            {
                WARNINGS = WARNINGS + "1";
            }
        }
        
        // WARNING #2 - Send Bytes below minimum
        if (pnode->nSendBytes < FLOODING_MINBYTES)
        {
            WARNINGS = WARNINGS + "2";
        }

        // WARNING #3 - Send Bytes above minimum
        if (pnode->nSendBytes < FLOODING_MINBYTES)
        {
            WARNINGS = WARNINGS + "3";
        }

        // WARNING #4 - Send Bytes below maximum
        if (pnode->nSendBytes < FLOODING_MAXBYTES)
        {
            WARNINGS = WARNINGS + "4";
        }

        // WARNING #5 - Send Bytes above maximum
        if (pnode->nSendBytes > FLOODING_MAXBYTES)
        {
            WARNINGS = WARNINGS + "5";
        }

        // WARNING #6 - Recv Bytes above min 
        if (pnode->nRecvBytes > FLOODING_MINBYTES / 2)
        {
            WARNINGS = WARNINGS + "6";
        }

        // WARNING #7 - Recv Bytes below min
        if (pnode->nRecvBytes < FLOODING_MINBYTES / 2)
        {
            WARNINGS = WARNINGS + "7";
        }

        // WARNING #8 - Recv Bytes above max 
        if (pnode->nRecvBytes > FLOODING_MAXBYTES / 2)
        {
            WARNINGS = WARNINGS + "8";
        }

        // WARNING #9 - Recv Bytes below max
        if (pnode->nRecvBytes < FLOODING_MAXBYTES / 2)
        {
            WARNINGS = WARNINGS + "9";
        }

        // WARNING #10 - Recv Bytes above min 
        if (pnode->nSendBytes > FLOODING_MINBYTES / 2)
        {
            WARNINGS = WARNINGS + "10";
        }

        // WARNING #11 - Recv Bytes below min
        if (pnode->nSendBytes < FLOODING_MINBYTES / 2)
        {
            WARNINGS = WARNINGS + "11";
        }

        // WARNING #12 - Recv Bytes above max 
        if (pnode->nSendBytes > FLOODING_MINBYTES / 2)
        {
            WARNINGS = WARNINGS + "12";
        }

        // WARNING #13 - Recv Bytes below max
        if (pnode->nSendBytes < FLOODING_MINBYTES / 2)
        {
            WARNINGS = WARNINGS + "13";
        }

        // WARNING #14 - 
        if (pnode->nTrafficAverage > FIREWALL_FLOODINGWALLET_MINTRAFFICAVERAGE)
        {
            WARNINGS = WARNINGS + "14";
        }

        // WARNING #15 - 
        if (pnode->nTrafficAverage < FIREWALL_FLOODINGWALLET_MINTRAFFICAVERAGE)
        {
            WARNINGS = WARNINGS + "15";
        }

        // WARNING #16 - 
        if (pnode->nTrafficAverage > FIREWALL_FLOODINGWALLET_MAXTRAFFICAVERAGE)
        {
            WARNINGS = WARNINGS + "16";
        }

        // WARNING #17 - 
        if (pnode->nTrafficAverage < FIREWALL_FLOODINGWALLET_MAXTRAFFICAVERAGE)
        {
            WARNINGS = WARNINGS + "17";
        }

        // WARNING #18 - Starting Height = SyncHeight above max
        if (pnode->nStartingHeight == pnode->nSyncHeight)
        {
            WARNINGS = WARNINGS + "18";
        }

        // WARNING #19 - Connected Time above min
        if (nTimeConnected > FIREWALL_FLOODINGWALLET_MINCHECK * 60)
        {
            WARNINGS = WARNINGS + "19";
        }

        // WARNING #20 - Connected Time below min
        if (nTimeConnected < FIREWALL_FLOODINGWALLET_MINCHECK * 60)
        {
            WARNINGS = WARNINGS + "20";
        }

        // WARNING #21 - Connected Time above max
        if (nTimeConnected > FIREWALL_FLOODINGWALLET_MAXCHECK * 60)
        {
            WARNINGS = WARNINGS + "21";
        }

        // WARNING #22 - Connected Time below max
        if (nTimeConnected < FIREWALL_FLOODINGWALLET_MAXCHECK * 60)
        {
            WARNINGS = WARNINGS + "22";
        }

        // WARNING #23 - Current BlockHeight
        if (NodeHeight > Firewall_AverageHeight)
        {  
            if (NodeHeight < Firewall_AverageHeight_Max)
            {  
                WARNINGS = WARNINGS + "23";
            }
        }

        // WARNING #24 - 
        if (pnode->nSyncHeight < Firewall_AverageTraffic_Max)
        {
            if (pnode->nSyncHeight > Firewall_AverageHeight_Min)
            {
                WARNINGS = WARNINGS + "24";
            }
        }

        // WARNING #25 - 
        if (DETECTED_ATTACK == true)
        {
            WARNINGS = WARNINGS + "25";
        }      
    
        // IF WARNINGS is matches pattern for ATTACK = TRUE
        int i;
        int TmpFloodPatternsCount;

        TmpFloodPatternsCount = CountStringArray(FIREWALL_FLOODPATTERNS);

        if (TmpFloodPatternsCount > 0)
        {
            for (i = 0; i < TmpFloodPatternsCount; i++)
            {  
                // Check for Static Whitelisted Seed Node
                if (WARNINGS == FIREWALL_FLOODPATTERNS[i])
                {
                    DETECTED_ATTACK = true;
                    ATTACK_TYPE = ATTACK_CHECK_NAME;
                }

            }
        }

        // ### LIVE DEBUG OUTPUT ####
        if (FIREWALL_LIVEDEBUG_FLOODINGWALLET == true)
        {
            ATTACK_CHECK_LOG = ATTACK_CHECK_LOG  + " {" +  ATTACK_CHECK_NAME + ":" + WARNINGS + ":" + BoolToString(DETECTED_ATTACK) + "}";
        }
        // #######################

        if (DETECTED_ATTACK == true)
        {
            if (FIREWALL_BLACKLIST_FLOODINGWALLET == true)
            {
                BLACKLIST_ATTACK = true;
            }

            if (FIREWALL_BAN_FLOODINGWALLET == true)
            {
                BAN_ATTACK = true;
                BAN_TIME = FIREWALL_BANTIME_FLOODINGWALLET;
                BAN_REASON = BanReasonFloodingWallet;
            }

        }
    }
    //--------------------------

    // ---Filter 5-------------
    //if (DETECT_HIGH_BANSCORE == true)
    //{
        //DETECTED_ATTACK = false;

        //nMisbehavior
        //checkbanned function integration *todo*

        //if (DETECTED_ATTACK == true)
        //{
            //if (BLACKLIST_HIGH_BANSCORE == true)
            //{
                //BLACKLIST_ATTACK = true;
            //}

            //if (BAN_HIGH_BANSCORE == true)
            //{
                //BAN_ATTACK = true;
                //BAN_TIME = BANTIME_HIGH_BANSCORE;
            //}

        //}
    //}
    //--------------------------

        if (FIREWALL_LIVE_DEBUG == true)
        {
            cout << ModuleName << " [Checking: " << pnode->addrName << "] [Attacks: " << ATTACK_CHECK_LOG << "]\n" << endl;
        }

    // ----------------
    // ATTACK DETECTED (TRIGGER)!
    if (DETECTED_ATTACK == true)
    {
        if (FIREWALL_LIVE_DEBUG == true)
        {
            cout << ModuleName << " [Attack Type: " << ATTACK_TYPE << "] [Detected from: " << pnode->addrName << "] [Node Traffic: " << pnode->nTrafficRatio << "] [Node Traffic Avrg: " << pnode->nTrafficAverage << "] [Traffic Avrg: " << Firewall_AverageTraffic << "] [Sent Bytes: " << pnode->nSendBytes << "] [Recv Bytes: " << pnode->nRecvBytes << "] [Sync Height: " << pnode->nSyncHeight << "] [Protocol: " << pnode->nRecvVersion <<"\n" << endl;
        }

        LogPrint("net", "%s [Attack Type: %s] [Detected from: %s] [Node Traffic: %d] [Node Traffic Avrg: %d] [Traffic Avrg: %d] [Sent Bytes: %d] [Recv Bytes: %d] [Sync Height: %i] [Protocol: %i\n", ModuleName.c_str(), ATTACK_TYPE.c_str(), pnode->addrName.c_str(), pnode->nTrafficRatio, pnode->nTrafficAverage, Firewall_AverageTraffic, pnode->nSendBytes, pnode->nRecvBytes, pnode->nSyncHeight, pnode->nRecvVersion);

        // Blacklist IP on Attack detection
        // * add node/peer IP to blacklist
        if (BLACKLIST_ATTACK == true)
        {
            AddToBlackList(pnode);
        }

        // Peer/Node Ban if required
        if (BAN_ATTACK == true)
        {
            AddToBanList(pnode, BAN_REASON, BAN_TIME);
        }

        // Peer/Node Panic Disconnect
        ForceDisconnectNode(pnode, FromFunction);

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

// * Function: Examination *
void Examination(CNode *pnode, string FromFunction)
{
// Calculate new Height Average from all peers connected

    bool UpdateNodeStats = false;
    int NodeHeight;

    if (pnode->nSyncHeight == 0)
    {
        NodeHeight = pnode->nStartingHeight;
    }
    else
    {
        NodeHeight = pnode->nSyncHeight;
    }

    if (pnode->nSyncHeight < pnode->nStartingHeight)
    {
        NodeHeight = pnode->nStartingHeight;
    }


    // ** Update current average if increased ****
    if (NodeHeight > Firewall_AverageHeight) 
    {
        Firewall_AverageHeight = Firewall_AverageHeight + NodeHeight; 
        Firewall_AverageHeight = Firewall_AverageHeight / 2;
        Firewall_AverageHeight = Firewall_AverageHeight - FIREWALL_AVERAGE_TOLERANCE;      // reduce with tolerance
        Firewall_AverageHeight_Min = Firewall_AverageHeight - FIREWALL_AVERAGE_RANGE;
        Firewall_AverageHeight_Max = Firewall_AverageHeight + FIREWALL_AVERAGE_RANGE;
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
            Firewall_AverageTraffic = Firewall_AverageTraffic + (double)pnode->nTrafficAverage;
            Firewall_AverageTraffic = Firewall_AverageTraffic / (double)2;
            Firewall_AverageTraffic = Firewall_AverageTraffic - (double)FIREWALL_TRAFFIC_TOLERANCE;      // reduce with tolerance
            Firewall_AverageTraffic_Min = Firewall_AverageTraffic - (double)FIREWALL_TRAFFIC_ZONE;
            Firewall_AverageTraffic_Max = Firewall_AverageTraffic + (double)FIREWALL_TRAFFIC_ZONE;    
            Firewall_AverageSend = Firewall_AverageSend + pnode->nSendBytes / vNodes.size();
            Firewall_AverageRecv = Firewall_AverageRecv + pnode->nRecvBytes / vNodes.size();        

            if (FIREWALL_LIVE_DEBUG == true)
            {
                if (FIREWALL_LIVEDEBUG_EXAM == true)
                {
                    cout << ModuleName << " [BlackListed Nodes/Peers: " << CountStringArray(FIREWALL_BLACKLIST) << "] [Traffic: " << Firewall_AverageTraffic << "] [Traffic Min: " << Firewall_AverageTraffic_Min << "] [Traffic Max: " << Firewall_AverageTraffic_Max << "]" << " [Safe Height: " << Firewall_AverageHeight << "] [Height Min: " << Firewall_AverageHeight_Min << "] [Height Max: " << Firewall_AverageHeight_Max <<"] [Send Avrg: " << Firewall_AverageSend<< "] [Rec Avrg: " << Firewall_AverageRecv << "]\n" <<endl;

                    cout << ModuleName << "[Check Node IP: " << pnode->addrName.c_str() << "] [Traffic: " << pnode->nTrafficRatio << "] [Traffic Average: " << pnode->nTrafficAverage << "] [Starting Height: " << pnode->nStartingHeight << "] [Sync Height: " << NodeHeight << "] [Node Sent: " << pnode->nSendBytes << "] [Node Recv: " << pnode->nRecvBytes << "] [Protocol: " << pnode->nRecvVersion << "]\n" << endl;
                }

            }

        }

    CheckAttack(pnode, FromFunction);
    }
}

// * Function: FireWall *
bool FireWall(CNode *pnode, string FromFunction)
{

    if (Firewall_FirstRun == false)
    {
        Firewall_FirstRun = true;
        LoadFirewallSettings();
    }

    if (FIREWALL_ENABLED == false)
    {
        return false;
    }

    int i;
    int TmpWhiteListCount;

    TmpWhiteListCount = CountStringArray(FIREWALL_WHITELIST);

    if (TmpWhiteListCount > 0)
    {
        for (i = 0; i < TmpWhiteListCount; i++)
        {  
            // Check for Static Whitelisted Seed Node
            if (pnode->addrName == FIREWALL_WHITELIST[i])
            {
               return false;
            }
        }
    }

    // Check for Node Whitelisted status
    //if (pnode->fWhitelisted == true)
    //{
    //    return false;
    //}

    //cout<<"Firewall called: "<<FromFunction<<endl;

    if (FIREWALL_CLEAR_BANS == true)
    {
        pnode->ClearBanned();
        LogPrint("net", "%s Cleared ban: %s\n", ModuleName.c_str(), pnode->addrName.c_str());
    }

    // Check for 0 peer count (auto-unban)
    //if (vNodes.size() < 2){
    //    pnode->ClearBanned();
    //}


    if (CheckBlackList(pnode) == true)
    {
        FromFunction = "CheckBlackList";

        LogPrint("net", "%s Disconnected Blacklisted IP: %s\n", ModuleName.c_str(), pnode->addrName.c_str());

// Peer/Node Panic Disconnect
ForceDisconnectNode(pnode, FromFunction);
return true;

    }

    if (CheckBanned(pnode) == true)
    {
        FromFunction = "CheckBanned";

        LogPrint("net", "%s Disconnected Banned IP: %s\n", ModuleName.c_str(), pnode->addrName.c_str());

// Peer/Node Panic Disconnect
ForceDisconnectNode(pnode, FromFunction);
return true;

    }

    // Perform a Node consensus examination
    Examination(pnode, FromFunction);

// Peer/Node Safe    
return false;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||