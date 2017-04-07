![BATA](https://bata.money/images/bta_square.png)

Coin Type: POW

Hashing Algorithm: SCRYPT

Difficulty Retargeting: Dark Gravity Wave

Time Between Blocks  90

Block Reward: 25

Block Reward Halving Rate: 100000

Premine: No

Total Coins: 5,000,000

rpc port: 5493

net port: 5784

Website: https://bata.io

Slack : https://bata-blockchain.signup.team/

Bootstrap available here:

http://crypt.midnightminer.net/bootstraps/bata/bootstrap.zip

Unzip the file into the same directory as the BATA-qt.exe

Then with a shortcut or in the command prompt:

BATA-qt.exe -loadblock=bootstrap.dat

Seednode

in your BATA.conf file just at this line

addnode=68.71.58.226


What is BATA (BTA)?


The predecessor of barter trade dollars. Soon to release new POW/POS Hybrid.

Bata (BTA) is a currency that has been developed to replace trade dollars in traditional Barter Systems. As the "Sharing Economy" grows, so does the decentralized market.

Bata aims to change way the way we trade, by introducing Barter on the Blockchain.

A Barter Currency has been re-invented on a blockchain, decentralized, secure and widely available in the form of BTA, replacing a Trade Dollar. Trade Dollars are just like cash and for that matter are based on regular currency, but in current Barter Systems they are controlled and issued by a central party.

BTA is traded on multiple cryptocurrency exchanges right across the globe.

Blockchain Explorer
https://chainz.cryptoid.info/bta/

Official Social Media

https://twitter.com/Bata_Money

https://www.facebook.com/batacurrency

https://www.reddit.com/r/batamoney

Forums:

https://bitcointalk.org/index.php?topic=1040956

https://cryptocointalk.com/topic/37776-pre-annbta-bata-scrypt-pow-modern-day-trade/

http://bitcoingarden.tk/forum/index.php?topic=3479.msg90749#msg90749

https://www.cryptocompare.com/coins/bta/forum


### instructions for compile: 

sudo apt-get install git build-essential openssl libminiupnpc-dev libboost++-dev libboost-all-dev libssl-dev libminiupnpc8 libdb++-dev

git clone https://github.com/BTA-BATA/BATA-SOURCE bata && cd bata/src

make -f makefile.unix

./BATAd -daemon

(it will instruct you to make a BATA.conf file, so make the BATA.conf file and place in the .BATA/ folder.)

### example regular internet users' BATA.conf:

rpcuser=BATAusername

rpcpassword=BATAuserpass

### example for i2P users' BATA.conf:
(you will need to download the latest i2p software)
https://geti2p.net/en/download

rpcuser=BATAusername

rpcpassword=BATAuserpass

i2p=1

proxy=127.0.0.1:3643

connect=lv4llpu75ydlfwxgx3ej5t6dpcnyi47px4wnluf7pyxpncd5trca.b32.i2p

### example for tor users' BATA.conf:
(you will need to download tor software)
http://www.softpedia.com/dyn-postdownload.php/71b643564f9a522d006160b023235a46/558695b8/db6e/0/1

rpcuser=BATAusername

rpcpassword=BATAuserpass

tor=1

proxy=127.0.0.1:9050

connect=eoekwl6m56nus2a7.onion

### possible errors during compile:

virtual memory exhausted: Cannot allocate memory

make: *** [obj/bitcoinrpc.o] Error 1

here, we just need to set up a swap file, because we ran out of memory:

sudo fallocate -l 4G /swapfile

sudo chmod 600 /swapfile

sudo mkswap /swapfile

sudo swapon /swapfile

### Make the Swap File Permanent

We have our swap file enabled, but when we reboot, the server will not automatically enable the file. We can change that though by modifying the fstab file.

Edit the file with root privileges in your text editor:

sudo nano /etc/fstab

At the bottom of the file, you need to add a line that will tell the operating system to automatically use the file you created:

/swapfile   none    swap    sw    0   0

Save and close the file when you are finished.
