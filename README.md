Bata Core integration/staging tree
=====================================

https://bata.io

What is Bata?
----------------

Bata is an experimental new digital currency that enables instant payments to
anyone, anywhere in the world. Bata uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried
out collectively by the network. Bata Core is the name of open source
software which enables the use of this currency.

For more information, as well as an immediately useable, binary version of
the Bata Core software, see https://bata.io

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

Website: http://www.bata.io

Twitter: @Bata_Money

https://www.facebook.com/batacurrency



https://cryptocointalk.com/topic/37776-pre-annbta-bata-scrypt-pow-modern-day-trade/

https://www.altcoincalendar.info/coins/1614-BTA

https://bitcointalk.org/index.php?topic=1040956


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
https://www.torproject.org/download/

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

License
-------

Bata Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see http://opensource.org/licenses/MIT.

Development process
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

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Manual Quality Assurance (QA) Testing

Large changes should have a test plan, and should be tested by somebody other
than the developer who wrote the code.
Creating a thread in the [Bata discussion forum](https://batatalk.org/index.php?board=2.0) will allow the Bata
development team members to review your proposal and to provide assistance with creating a test plan. 


Translations
------------

**Important**: We do not accept translation changes as GitHub pull requests because the next
pull from Transifex would automatically overwrite them again.

We only accept translation fixes that are submitted through [Bitcoin Core's Transifex page](https://www.transifex.com/projects/p/bitcoin/).
Translations are converted to Bata periodically.

Development tips and tricks
---------------------------

**compiling for debugging**

Run configure with the --enable-debug option, then make. Or run configure with
CXXFLAGS="-g -ggdb -O0" or whatever debug flags you need.

**debug.log**

If the code is behaving strangely, take a look in the debug.log file in the data directory;
error and debugging messages are written there.

The -debug=... command-line option controls debugging; running with just -debug will turn
on all categories (and give you a very large debug.log file).

The Qt code routes qDebug() output to debug.log under category "qt": run with -debug=qt
to see it.

**testnet and regtest modes**

Run with the -testnet option to run with "play batas" on the test network, if you
are testing multi-machine code that needs to operate across the internet.

If you are testing something that can run on one machine, run with the -regtest option.
In regression test mode, blocks can be created on-demand; see qa/rpc-tests/ for tests
that run in -regtest mode.

**DEBUG_LOCKORDER**

Bata Core is a multithreaded application, and deadlocks or other multithreading bugs
can be very difficult to track down. Compiling with -DDEBUG_LOCKORDER (configure
CXXFLAGS="-DDEBUG_LOCKORDER -g") inserts run-time checks to keep track of which locks
are held, and adds warnings to the debug.log file if inconsistencies are detected.
