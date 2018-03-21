// [Bitcoin Firewall 1.2.4
// March 21, 2018 - Biznatch Enterprises & Profit Hunters Coin (PHC)
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

// *** Firewall Controls (General) ***
extern bool FIREWALL_ENABLED;
extern bool FIREWALL_LIVE_DEBUG;
extern bool FIREWALL_CLEAR_BLACKLIST;
extern bool FIREWALL_CLEAR_BANS;

// *** Firewall Controls (General) ***
extern bool FIREWALL_LIVEDEBUG_EXAM;
extern bool FIREWALL_LIVEDEBUG_BANS;
extern bool FIREWALL_LIVEDEBUG_BLACKLIST;
extern bool FIREWALL_LIVEDEBUG_DISCONNECT;
extern bool FIREWALL_LIVEDEBUG_BANDWIDTHABUSE;
extern bool FIREWALL_LIVEDEBUG_NOFALSEPOSITIVE;
extern bool FIREWALL_LIVEDEBUG_INVALIDWALLET;
extern bool FIREWALL_LIVEDEBUG_FORKEDWALLET;
extern bool FIREWALL_LIVEDEBUG_FLOODINGWALLET;

// *** Firewall Controls (Bandwidth Abuse) ***
extern bool FIREWALL_DETECT_BANDWIDTHABUSE;
extern bool FIREWALL_BLACKLIST_BANDWIDTHABUSE;
extern bool FIREWALL_BAN_BANDWIDTHABUSE;
extern bool FIREWALL_NOFALSEPOSITIVE_BANDWIDTHABUSE;

// *** Firewall Controls (Invalid Peer Wallets) ***
extern bool FIREWALL_DETECT_INVALIDWALLET;
extern bool FIREWALL_BLACKLIST_INVALIDWALLET;
extern bool FIREWALL_BAN_INVALIDWALLET;

// *** Firewall Controls (Forked Peer Wallets) ***
extern bool FIREWALL_DETECT_FORKEDWALLET;
extern bool FIREWALL_BLACKLIST_FORKEDWALLET;
extern bool FIREWALL_BAN_FORKEDWALLET;

// *** Firewall Controls (Flooding Peer Wallets) ***
extern bool FIREWALL_DETECT_FLOODINGWALLET;
extern bool FIREWALL_BLACKLIST_FLOODINGWALLET;
extern bool FIREWALL_BAN_FLOODINGWALLET;

// * Firewall Settings (Exam) *
extern int FIREWALL_AVERAGE_TOLERANCE;
extern int FIREWALL_AVERAGE_RANGE;
extern double FIREWALL_TRAFFIC_TOLERANCE;
extern double FIREWALL_TRAFFIC_ZONE;
extern string FIREWALL_WHITELIST[];
extern string FIREWALL_BLACKLIST[];

// * Firewall Settings (Bandwidth Abuse) *
extern int FIREWALL_BANTIME_BANDWIDTHABUSE;
extern int FIREWALL_BANDWIDTHABUSE_MAXCHECK;
extern double FIREWALL_BANDWIDTHABUSE_MINATTACK;
extern double FIREWALL_BANDWIDTHABUSE_MAXATTACK;
extern int FIREWALL_BANTIME_BANDWIDTHABUSE;

// * Firewall Settings (Invalid Wallet)
extern int FIREWALL_MINIMUM_PROTOCOL;
extern int FIREWALL_BANTIME_INVALIDWALLET;
extern int FIREWALL_INVALIDWALLET_MAXCHECK;
extern int FIREWALL_BANTIME_INVALIDWALLET;

// * Firewall Settings (Forked Wallet)
extern int FIREWALL_BANTIME_FORKEDWALLET;
extern int FIREWALL_FORKED_NODEHEIGHT[];

// * Firewall Settings (Flooding Wallet)
extern int FIREWALL_BANTIME_FLOODINGWALLET;
extern int FIREWALL_FLOODINGWALLET_MINBYTES;
extern int FIREWALL_FLOODINGWALLET_MAXBYTES;
extern string FIREWALL_FLOODPATTERNS[];
extern double FIREWALL_FLOODINGWALLET_MINTRAFFICAVERAGE;
extern double FIREWALL_FLOODINGWALLET_MAXTRAFFICAVERAGE;
extern int FIREWALL_FLOODINGWALLET_MINCHECK;
extern int FIREWALL_FLOODINGWALLET_MAXCHECK;
