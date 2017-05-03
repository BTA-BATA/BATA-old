Wallet Tools
---------------------

### [BitRPC](/contrib/bitrpc) ###
Allows for sending of all standard Bitcoin commands via RPC rather than as command line args.

### [SpendFrom](/contrib/spendfrom) ###

Use the raw transactions API to send coins received on a particular
address (or addresses).

Repository Tools
---------------------

### [Developer tools](/contrib/devtools) ###
Specific tools for developers working on this repository.
<<<<<<< HEAD
Contains the script `github-merge.py` for merging github pull requests securely and signing them using GPG.

### [Verify-Commits](/contrib/verify-commits) ###
Tool to verify that every merge commit was signed by a developer using the above `github-merge.py` script.
=======
Contains the script `github-merge.sh` for merging github pull requests securely and signing them using GPG.
>>>>>>> upstream/0.10

### [Linearize](/contrib/linearize) ###
Construct a linear, no-fork, best version of the blockchain.

### [Qos](/contrib/qos) ###

<<<<<<< HEAD
A Linux bash script that will set up traffic control (tc) to limit the outgoing bandwidth for connections to the Bata network. This means one can have an always-on batad instance running, and another local batad/bata-qt instance which connects to this node and receives blocks from it.
=======
A Linux bash script that will set up traffic control (tc) to limit the outgoing bandwidth for connections to the Bitcoin network. This means one can have an always-on bitcoind instance running, and another local bitcoind/bitcoin-qt instance which connects to this node and receives blocks from it.
>>>>>>> upstream/0.10

### [Seeds](/contrib/seeds) ###
Utility to generate the pnSeed[] array that is compiled into the client.

Build Tools and Keys
---------------------

### [Debian](/contrib/debian) ###
<<<<<<< HEAD
Contains files used to package batad/bata-qt
for Debian-based Linux systems. If you compile batad/bata-qt yourself, there are some useful files here.
=======
Contains files used to package bitcoind/bitcoin-qt
for Debian-based Linux systems. If you compile bitcoind/bitcoin-qt yourself, there are some useful files here.
>>>>>>> upstream/0.10

### [Gitian-descriptors](/contrib/gitian-descriptors) ###
Gavin's notes on getting gitian builds up and running using KVM.

### [Gitian-downloader](/contrib/gitian-downloader)
Various PGP files of core developers. 

### [MacDeploy](/contrib/macdeploy) ###
Scripts and notes for Mac builds. 

<<<<<<< HEAD
### [RPM](/contrib/rpm) ###
RPM spec file for building litecoin-core on RPM based distributions

=======
>>>>>>> upstream/0.10
Test and Verify Tools 
---------------------

### [TestGen](/contrib/testgen) ###
<<<<<<< HEAD
Utilities to generate test vectors for the data-driven Bata tests.
=======
Utilities to generate test vectors for the data-driven Bitcoin tests.
>>>>>>> upstream/0.10

### [Test Patches](/contrib/test-patches) ###
These patches are applied when the automated pull-tester
tests each pull and when master is tested using jenkins.

### [Verify SF Binaries](/contrib/verifysfbinaries) ###
This script attempts to download and verify the signature file SHA256SUMS.asc from SourceForge.
