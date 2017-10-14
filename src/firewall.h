// [Bitcoin Firewall 1.2.3
// Oct 14, 2017 - Biznatch Enterprises & Bata Development (http://bata.io & )
// https://github.com/BiznatchEnterprises/BitcoinFirewall
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "addrman.h"
#include <string>

#ifdef WIN32
#include <string.h>
#else
#include <fcntl.h>
#endif

using namespace std;

string ModuleName = "[Bitcoin Firewall 1.2.3]";
//boost::filesystem::path pathFirewallLog = GetDataDir()  / "firewall.log";

// * FireWall Controls *
bool ENABLE_FIREWALL = true;
bool LIVE_DEBUG_OUTPUT = false;
bool INTERVAL_CHECK_ALL = true;
bool DETECT_INVALID_WALLET = true;
bool BLACKLIST_INVALID_WALLET = true;
bool BAN_INVALID_WALLET = true;
bool DETECT_BANDWIDTH_ABUSE =  true;
bool BLACKLIST_BANDWIDTH_ABUSE = true;
bool BAN_BANDWIDTH_ABUSE = true;
bool FALSE_POSITIVE_PROTECTION =  true;
bool FIREWALL_CLEAR_BANS = false;

// * Global Firewall Variables *
int CurrentAverageHeight = 0;
int CurrentAverageHeight_Min = 0;
int CurrentAverageHeight_Max = 0;
double CurrentAverageTraffic = 0;
double CurrentAverageTraffic_Min = 0;
double CurrentAverageTraffic_Max = 0;
int CurrentAverageSend = 0;
int CurrentAverageRecv = 0;
int ALL_CHECK_TIMER = GetTime();
int ALL_CHECK_MAX = 3;  // minutes interval
int MINIMUM_PROTOCOL = GETHEADERS_VERSION;
// Not Used: int CurrentAverageHeight_Max = 0;

// * BlackList node/peers Array
string BLACKLIST[256];
bool BLACKLIST_ATTACK = false;  // global temp variables
bool BAN_ATTACK = false; //global temp variables
int BlackListCounter = 0;

// WHITELIST (ignore)
int SeedNodeCounter = 2;
string IgnoreSeedNode[2] = { 
    "68.71.58.226:5784", 
    "91.134.137.252:5784" 
    };

// * Attack Detection Settings *
int AverageTolerance = 2;    // Reduce for minimal fluctuation 2 Blocks tolerance
int AverageRange = 500;   // + or - Starting Height Range

/// Bandwidth monitoring ranges
double TrafficTolerance = 0.0001; // Reduce for minimal fluctuation
double TrafficZone = 4; // + or - Traffic Range