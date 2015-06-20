![BATA](https://github.com/justinvforvendetta/BATA-SOURCE/blob/master/bata.png)

Coin Type: POW

Hashing Algorithm: SCRYPT

Difficulty Retargeting: Dark Gravity Wave

Time Between Blocks  30

Block Reward: 50

Block Reward Halving Rate: 50000

Premine: No

Total Coins: 5,000,000

rpc port: 5493

net port: 5784

Website: http://www.bata.money/ - Coming Soon!

Twitter: @BataCurrency

https://www.facebook.com/batacurrency

http://cryptooz.com/group/bta-bata-community-group/stream

https://cryptocointalk.com/topic/37776-pre-annbta-bata-scrypt-pow-modern-day-trade/

https://www.altcoincalendar.info/coins/1614-BTA

https://bitcointalk.org/index.php?topic=1040956


### instructions for compile: 

sudo apt-get git build-essential openssl libminiupnpc-dev libboost++-dev libboost-all-dev libssl-dev libminiupnpc8 libdb++-dev

git clone https://github.com/BTA-BATA/BATA-SOURCE bata && cd bata/src

make -f makefile.unix

./BATAd -daemon

(it will instruct you to make a BATA.conf file, so make the BATA.conf file and place in the .BATA/ folder.)

### example regular internet users' BATA.conf:

rpcuser=BATAusername

rpcpassword=BATAuserpass

### example for i2P users' BATA.conf:

rpcuser=BATAusername

rpcpassword=BATAuserpass

i2p=1

proxy=127.0.0.1:3643

connect=batai2pnodeaddress.b32.i2p

### example for tor users' BATA.conf:

rpcuser=BATAusername

rpcpassword=BATAuserpass

tor=1

proxy=127.0.0.1:9050

connect=batai2pnodeaddress.onion

