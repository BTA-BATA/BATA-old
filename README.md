![BATA](https://bata.money/images/bta_square.png)

- Coin Type: POW
- Hashing Algorithm: SCRYPT
- Difficulty Retargeting: Dark Gravity Wave
- Time Between Blocks  90
- Block Reward: 25
- Block Reward Halving Rate: 100000
- Premine: No
- Total Coins: 5,000,000
- rpc port: 5493
- net port: 5784

### What is BATA (BTA)?

The predecessor of barter trade dollars. Soon to release new POW/POS Hybrid.

Bata (BTA) is a currency that has been developed to replace trade dollars in traditional Barter Systems. As the "Sharing Economy" grows, so does the decentralized market.

Bata aims to change way the way we trade, by introducing Barter on the Blockchain.

A Barter Currency has been re-invented on a blockchain, decentralized, secure and widely available in the form of BTA, replacing a Trade Dollar. Trade Dollars are just like cash and for that matter are based on regular currency, but in current Barter Systems they are controlled and issued by a central party.

BTA is traded on multiple cryptocurrency exchanges right across the globe.

### Website: 

https://bata.io

### For Developments info: 
https://bata.io/development

### Bootstrap: 

http://crypt.midnightminer.net/bootstraps/bata/bootstrap.zip

Unzip the file into the same directory as the bata-qt.exe

Then with a shortcut or in the command prompt:

bata-qt.exe -loadblock=bootstrap.dat

### Seednode

in your bata.conf file just at this line

- addnode=68.71.58.226
- addnode=68.71.58.229
- addnode=149.202.98.160
- addnode=157.161.128.61
- addnode=158.69.248.93
- addnode=171.25.193.78
- addnode=172.104.140.15
- addnode=176.126.252.12
- addnode=178.238.224.213
- addnode=185.173.206.30
- addnode=188.165.203.89
- addnode=188.165.235.208
- addnode=197.231.221.211
- addnode=198.27.81.114
- addnode=36.81.211.3
- addnode=46.4.85.241
- addnode=52.14.92.197
- addnode=54.149.206.22
- addnode=73.76.129.232
- addnode=77.247.181.163
- addnode=85.236.188.28
- addnode=88.99.68.228
- addnode=95.211.110.151
- addnode=149.202.98.160
- addnode=157.161.128.61
- addnode=158.69.248.93
- addnode=171.25.193.78
- addnode=172.104.140.15
- addnode=176.126.252.12
- addnode=178.238.224.213
- addnode=185.173.206.30
- addnode=188.165.203.89
- addnode=188.165.235.208
- addnode=197.231.221.211
- addnode=198.27.81.114
- addnode=36.81.211.3
- addnode=46.4.85.241
- addnode=52.14.92.197
- addnode=54.149.206.22
- addnode=68.71.58.226
- addnode=68.71.58.229
- addnode=73.76.129.232
- addnode=77.247.181.163
- addnode=85.236.188.28
- addnode=88.99.68.228
- addnode=95.211.110.151
- addnode=138.207.159.225
- addnode=209.37.22.54
- addnode=47.94.45.200
- addnode=59.11.125.143
- addnode=69.195.231.227
- addnode=73.11.158.217
- addnode=86.0.36.153


(Create in AppData\Roaming\bata)

### LinkedIn: 

https://www.linkedin.com/company/bata-money

### Blockchain Explorers

- https://chainz.cryptoid.info/bta/
- http://bata.midnightminer.net/


### Official Social Media

- https://twitter.com/Bata_Money
- https://www.facebook.com/batacurrency
- https://www.reddit.com/r/batamoney


### Forums:

- https://bitcointalk.org/index.php?topic=1040956
- https://cryptocointalk.com/topic/37776-pre-annbta-bata-scrypt-pow-modern-day-trade/
- http://bitcoingarden.tk/forum/index.php?topic=3479.msg90749#msg90749
- https://www.cryptocompare.com/coins/bta/forum
- https://www.cryptopia.co.nz/Forum/Thread/377


### Group Chat:

- Slack: https://bata-blockchain.slack.com
- Telegram: https://t.me/batacurrency


### install Linux dependencies:

- General: 

>sudo apt-get install git build-essential openssl libminiupnpc-dev  build-essential openssl libminiupnpc-dev  automake libtool libprotobuf-dev qrencode autoconf-tools cmake libevent-dev libcurl4-openssl-dev

- QT Creator 4:

>sudo apt-get install  libqt4-dev qt4-dev-tools

- QT Creator 5:

>sudo apt-get install libqt5-dev qt5-dev-tools qttools5-dev-tools qt5-default  libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools 

- LibBoost 4.8:

>sudo apt-get install software-properties-common

>sudo add-apt-repository ppa:bitcoin/bitcoin

>sudo apt-get update

>sudo apt-get install libdb4.8-dev libdb4.8++-dev


- Latest version of LibBoost:

>sudo apt-get install libboost++-dev libboost-all-dev libssl-dev libdb++-dev


### instructions for Linux compile: 


>git clone https://github.com/BTA-BATA/BATA-SOURCE

>cd BATA-SOURCE

>sudo chmod +777 *

>./autogen.sh

>./configure

>make

(it will instruct you to make a bata.conf file, so make the bata.conf file and place in the .bata/ folder.)

### example regular internet users' bata.conf:

>rpcuser=BATAusername

>rpcpassword=BATAuserpass

### example for i2P users' BATA.conf:

(you will need to download the latest i2p software)
https://geti2p.net/en/download

>rpcuser=BATAusername

>rpcpassword=BATAuserpass

>i2p=1

>proxy=127.0.0.1:3643

>connect=lv4llpu75ydlfwxgx3ej5t6dpcnyi47px4wnluf7pyxpncd5trca.b32.i2p

### example for tor users' bata.conf:

(you will need to download tor software)
https://www.torproject.org/download/

>rpcuser=BATAusername

>rpcpassword=BATAuserpass

>tor=1

>proxy=127.0.0.1:9050

>connect=eoekwl6m56nus2a7.onion

### possible errors during compile:

>virtual memory exhausted: Cannot allocate memory

>make: [obj/bitcoinrpc.o] Error 1

here, we just need to set up a swap file, because we ran out of memory:

>sudo fallocate -l 4G /swapfile

>sudo chmod 600 /swapfile

>sudo mkswap /swapfile

>sudo swapon /swapfile

### Make the Swap File Permanent

We have our swap file enabled, but when we reboot, the server will not automatically enable the file. We can change that though by modifying the fstab file.

Edit the file with root privileges in your text editor:

>sudo nano /etc/fstab

At the bottom of the file, you need to add a line that will tell the operating system to automatically use the file you created:

>/swapfile   none    swap    sw    0   0

Save and close the file when you are finished.

### Development process
-------------------

Developers work in their own trees, then submit pull requests when they think
their feature or bug fix is ready.

If it is a simple/trivial/non-controversial change, then one of the Bata
development team members simply pulls it.

If it is a *more complicated or potentially controversial* change, then the patch
submitter will be asked to start a discussion (if they haven't already) on the
[mailing list](https://groups.google.com/forum/#!forum/bata-dev).

The patch will be accepted if there is broad consensus that it is a good thing.
Developers should expect to rework and resubmit patches if the code doesn't
match the project's coding conventions (see [doc/coding.md](doc/coding.md)) or are
controversial.

The `master-0.10` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/bata-project/bata/tags) are created
regularly to indicate new official, stable release versions of Bata.

### Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Manual Quality Assurance (QA) Testing

Large changes should have a test plan, and should be tested by somebody other
than the developer who wrote the code.
Creating a thread in the [Bata discussion forum](https://bitcointalk.org/index.php?topic=1040956.0) will allow the Bata
development team members to review your proposal and to provide assistance with creating a test plan. 

### Bata Cross-compile Instructions

Guide by: Midnight Miner

- Based on Ubutntu 16.04
- Get your required packages to cross-compile Windows executables.

#### Bata Source Code

Clone the Github into a directory for building I will use this as an example ~/Bata-build/

>cd ~/Bata-build

>git clone https://github.com/BTA-BATA/BATA-SOURCE.git

>cd BATA-SOURCE

#### Windows 32 bit
==============

>sudo apt-get install g++-mingw-w64-i686 mingw-w64-i686-dev g++-mingw-w64-x86-64 mingw-w64-x86-64-dev curl

>sh autogen.sh 

>cd depends/

>make HOST=i686-w64-mingw32 -j4

>cd ..

>./configure --prefix=`pwd`/depends/i686-w64-mingw32 --with-gui=qt5

>make -j4

You will find the executables:
- ./src/batad.exe
- ./src/bata-cli.exe
- ./src/bata-tx.exe
- ./src/test/test_bata.exe
- ./src/qt/test/test_bata-qt.exe
- ./src/qt/bata-qt.exe

Copy them to another directory before compiling another platform.

#### Windows 64 bit
==============

>sh autogen.sh 

>cd depends/

>make HOST=x86_64-w64-mingw32 -j4

>cd ..

>./configure --prefix=`pwd`/depends/x86_64-w64-mingw32 --with-gui=qt5

>make -j4

You will find the executables:
- ./src/batad.exe
- ./src/bata-cli.exe
- ./src/bata-tx.exe
- ./src/test/test_bata.exe
- ./src/qt/test/test_bata-qt.exe
- ./src/qt/bata-qt.exe

Copy them to another directory before compiling another platform.


#### Mac Cross-Compile
=================

>sudo apt install libpython-all-dev python-all python-all-dev python-pip python-pip-whl python-pkg-resources python-setuptools python-wheel cmake libcap-dev

>sh autogen.sh 

>make download-osx

>cd depends/

>make HOST=x86_64-apple-darwin11 -j4

>cd ..

>./configure --prefix=`pwd`/depends/x86_64-apple-darwin11 --with-gui=qt5

>make -j4

#### Linux
=================

>sh autogen.sh 

>cd depends/

>make HOST=x86_64-unknown-linux-gnu -j4

>cd ..

>./configure --prefix=`pwd`/depends/x86_64-unknown-linux-gnu --with-gui=qt5

>make -j4

You will find the executables:
- ./src/batad
- ./src/bata-cli
- ./src/bata-tx
- ./src/test/test_bata
- ./src/qt/test/test_bata-qt
- ./src/qt/bata-qt


### Translations
------------

Important We do not accept translation changes as GitHub pull requests because the next
pull from Transifex would automatically overwrite them again.

We only accept translation fixes that are submitted through [Bitcoin Core's Transifex page](https://www.transifex.com/projects/p/bitcoin/).
Translations are converted to Bata periodically.

### Development tips and tricks
---------------------------

#### compiling for debugging

- Run configure with the --enable-debug option, then make.
- Or run configure with
CXXFLAGS="-g -ggdb -O0" or whatever debug flags you need.

#### debug.log

If the code is behaving strangely, take a look in the debug.log file in the data directory;
error and debugging messages are written there.

The -debug=... command-line option controls debugging; running with just -debug will turn
on all categories (and give you a very large debug.log file).

The Qt code routes qDebug() output to debug.log under category "qt": run with -debug=qt
to see it.

#### testnet and regtest modes 

Run with the -testnet option to run with "play batas" on the test network, if you
are testing multi-machine code that needs to operate across the internet.

If you are testing something that can run on one machine, run with the -regtest option.
In regression test mode, blocks can be created on-demand; see qa/rpc-tests/ for tests
that run in -regtest mode.

#### DEBUG_LOCKORDER

Bata Core is a multithreaded application, and deadlocks or other multithreading bugs
can be very difficult to track down. Compiling with -DDEBUG_LOCKORDER (configure
CXXFLAGS="-DDEBUG_LOCKORDER -g") inserts run-time checks to keep track of which locks
are held, and adds warnings to the debug.log file if inconsistencies are detected.

### BATA License

- Bata Core is released under the terms of the MIT license.
- See [COPYING](COPYING) for more information or see http://opensource.org/licenses/MIT.

