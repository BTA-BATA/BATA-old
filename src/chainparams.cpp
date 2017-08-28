// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */



//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress> &vSeedsOut, const SeedSpec6 *data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7*24*60*60;
    for (unsigned int i = 0; i < count; i++)
    {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */
static Checkpoints::MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (  10000, uint256("0xcbadbae8bb99bce9dc5c79808bcbeee8bb8f2c579dbbdfb38df925b88baa4963"))
		(  50000, uint256("0xcdeded5b25e5fab3b04c64c248c89daa678998e710b5699513dece49f2421061"))
		( 100000, uint256("0xd2847b94207e312fdc6614299646f59854e36232310a7727d1360d48ebd96c0f"))
		( 200000, uint256("0x342b70493e9b3a1e0c51c1e8cf76c7893cd5b9092b25b159f862829928d03d00"))
		( 300000, uint256("0xd2c28a813de750a0a191bddf36188c9c128aaca5df6f479511d8c36ac712b8ee"))
		( 400000, uint256("0xd99d4288cf1b05b4918e6bbe8a92a6d2893a1b7ad50a180c1060e17d0dd86a93"))
		( 440000, uint256("0xac145d581b0c014764400f6265fbc70e1e72f62ec37506be4ad52806315c6e18"))
		( 446000, uint256("0x6353453ab70c8f8d7c2a427848b31815b403954b35897fc171cdcb90efdc876c"))
		( 550000, uint256("0xc3e04a21969b9b7c8b058336a752829de20f60381ab421dc65d8e7dc652fe47e"))
		( 700000, uint256("0x3dde1aa60ab78ac29e84fa2baf641638dff95514af72e8c1dcf9fec89da84700"))
        ;

static const Checkpoints::CCheckpointData data = {
        &mapCheckpoints,
		1498771810, // * UNIX timestamp of last checkpoint block
        797741,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        960        // * estimated number of transactions per day after checkpoint
    };

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        ( 1, uint256("0xc1976360739525c4ea0949fc4a1b0086284614198a0f133b1513d79c55b5466b"))
        ;
static const Checkpoints::CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
		1503885814,
        2,
        960
    };

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
        boost::assign::map_list_of
        ( 0, uint256("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"))
        ;
static const Checkpoints::CCheckpointData dataRegtest = {
        &mapCheckpointsRegtest,
        0,
        0,
        0
    };

class CMainParams : public CChainParams {
public:
    CMainParams() {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /** 
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x34;
        pchMessageStart[1] = 0xc3;
        pchMessageStart[2] = 0xaf;
        pchMessageStart[3] = 0xeb;
        vAlertPubKey = ParseHex("040184710fa689ad5023690c80f3a49c8f13f8d45b8c857fbcbc8bc4a8e4d3eb4b10f4d4604fa08dce601aaf0f470216fe1b51850b4acf21b179c45070ac7b03a9");
        nDefaultPort = 5784;
        bnProofOfWorkLimit = ~uint256(0) >> 20;
        nSubsidyHalvingInterval = 100000;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 4 * 60 * 60; // 4 hours
        nTargetSpacing = 1.5 * 60; // 1.5 minutes
        nMaxTipAge = 24 * 60 * 60;

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         * 
         * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
         *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
         *   vMerkleTree: 4a5e1e
         */
        const char* pszTimestamp = "BATA";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 88 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("040184710fa689ad5023690c80f3a49c8f13f8d45b8c857fbcbc8bc4a8e4d3eb4b10f4d4604fa08dce601aaf0f470216fe1b51850b4acf21b179c45070ac7b03a9") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1410001075;   //  Sat, 06 Sep 2014 10:57:55 GMT
        genesis.nBits    = 0x1e0ffff0;
        genesis.nNonce   = 20946;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0xb4bee36fd54a6176fd832f462641415c142d50e4b378f71c041870c2b1186bc8"));
        assert(genesis.hashMerkleRoot == uint256("0x3164764afd26106715d7e58de57468e236b167d639095be545459d2bcf94afe7"));
        base58Prefixes[PUBKEY_ADDRESS] = boost::assign::list_of(25).convert_to_container<std::vector<unsigned char> >();						// Bata addresses start with 'B'
        base58Prefixes[SCRIPT_ADDRESS] = boost::assign::list_of(5)(85).convert_to_container<std::vector<unsigned char> >();						// Bata script addresses start with '3'
//        base58Prefixes[SCRIPT_ADDRESS2] = list_of(85);						// Bata script addresses start with 'b'
        base58Prefixes[SECRET_KEY] =     boost::assign::list_of(188).convert_to_container<std::vector<unsigned char> >();						// Bata private keys start with '2'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0xA4)(0x0C)(0x86)(0xFA).convert_to_container<std::vector<unsigned char> >();	// Bata BIP32 pubkeys start with 'bpub'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0xA4)(0x0B)(0x91)(0xBD).convert_to_container<std::vector<unsigned char> >();	// Bata BIP32 prvkeys start with 'bprv'
		base58Prefixes[EXT_COIN_TYPE]  = boost::assign::list_of(0x8000000b).convert_to_container<std::vector<unsigned char> >();				// Bata BIP44 coin type is 'b'

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("bata.io", "list.batadnsseed.bata.io"));
        vSeeds.push_back(CDNSSeedData("midnightnet.net", "batadnsseed.midnightminer.net"));
        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        // Bata: Mainnet v2 enforced as of block 710k
        nEnforceV2AfterHeight = 710000;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0xba;
        pchMessageStart[1] = 0xad;
        pchMessageStart[2] = 0xaf;
        pchMessageStart[3] = 0xc5;
        vAlertPubKey = ParseHex("0449623fc74489a947c4b15d579115591add020e53b3490bf47297dfa3762250625f8ecc2fb4fc59f69bdce8f7080f3167808276ed2c79d297054367566038aa82");
        nDefaultPort = 33813;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 3.5 * 24 * 60 * 60; // 3.5 days
        nTargetSpacing = 2.5 * 60; // 2.5 minutes
        nMaxTipAge = 0x7fffffff;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1503885814;
        genesis.nNonce = 698199;

        if (false && genesis.GetHash() != hashGenesisBlock)
                {
                    printf("recalculating params for testnet.\n");
//                    printf("old testnet genesis nonce: %s\n", (unsigned)genesis.nNonce.ToString().c_str());
                    cout << "old testnet genesis nonce: " << (unsigned)genesis.nNonce << "\n" ;
                    printf("old testnet genesis hash:  %s\n", hashGenesisBlock.ToString().c_str());
                    printf("old testnet genesis nBits: %d\n", genesis.nBits);
                    printf("recalculating params for testnet.\n");
                    // deliberately empty for loop finds nonce value.
                    for(genesis.nNonce == 0; genesis.GetHash() > bnProofOfWorkLimit; genesis.nNonce++){ }
                    printf("new testnet genesis merkle root: %s\n", genesis.hashMerkleRoot.ToString().c_str());
//                    printf("new testnet genesis nonce: %s\n", (unsigned)genesis.nNonce);
                    cout << "new testnet genesis nonce: " << (unsigned)genesis.nNonce << "\n" ;
                    printf("new testnet genesis hash: %s\n", genesis.GetHash().ToString().c_str());
                    printf("new testnet genesis nBits: %d\n", genesis.nBits);
                }

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x00000a6800e5e2b43515cd12da56861eb482ebcdfa6ee92c1a8d7836ff654500"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("bata.io", "testnet-seed.bata.io"));
        vSeeds.push_back(CDNSSeedData("midnightminer.net", "testnet-bata.midnightminer.net"));
//        vSeeds.push_back(CDNSSeedData("wemine-testnet.com", "dnsseed.wemine-testnet.com"));

        base58Prefixes[PUBKEY_ADDRESS] = boost::assign::list_of(111).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SCRIPT_ADDRESS] = boost::assign::list_of(196).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[SECRET_KEY]     = boost::assign::list_of(239).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = true;
        fTestnetToBeDeprecatedFieldRPC = true;

        // Bata: v2 enforced using Bitcoin's supermajority rule
        nEnforceV2AfterHeight = -1;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 3.5 * 24 * 60 * 60; // 3.5 days
        nTargetSpacing = 2.5 * 60; // 2.5 minutes
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        nMaxTipAge = 24 * 60 * 60;
        genesis.nTime = 1296688602;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 0;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 33814;
//        assert(hashGenesisBlock == uint256("0x530827f38f93b43ed12af0b3ad25a288dc02ed74d6d7857862df51fc56c416f9"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = true;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        // Bata: v2 enforced using Bitcoin's supermajority rule
        nEnforceV2AfterHeight = -1;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams {
public:
    CUnitTestParams() {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 33815;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Unit test mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;

        // Bata: v2 enforced using Bitcoin's supermajority rule
        nEnforceV2AfterHeight = -1;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const 
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval)  { nSubsidyHalvingInterval=anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority)  { nEnforceBlockUpgradeMajority=anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority)  { nRejectBlockOutdatedMajority=anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority)  { nToCheckBlockUpgradeMajority=anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks)  { fDefaultConsistencyChecks=afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) {  fAllowMinDifficultyBlocks=afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams *pCurrentParams = 0;

CModifiableParams *ModifiableParams()
{
   assert(pCurrentParams);
   assert(pCurrentParams==&unitTestParams);
   return (CModifiableParams*)&unitTestParams;
}

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        case CBaseChainParams::UNITTEST:
            return unitTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
