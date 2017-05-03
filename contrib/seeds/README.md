### Seeds ###

Utility to generate the seeds.txt list that is compiled into the client
<<<<<<< HEAD
(see [src/chainparamsseeds.h](/src/chainparamsseeds.h) and other utilities in [contrib/seeds](/contrib/seeds)).

The seeds compiled into the release are created from pooler's DNS seed data, like this:

    curl -s http://bata.midnightminer.net/seeds.txt > seeds_main.txt
    python makeseeds.py < seeds_main.txt > nodes_main.txt
    python generate-seeds.py . > ../../src/chainparamsseeds.h

=======
(see [src/chainparamsseeds.h](/src/chainparamsseeds.h) and [share/seeds](/share/seeds)).

The 512 seeds compiled into the 0.10 release were created from pooler's seed data, the seed list can be found here:

	https://www.litecoinpool.org/seeds.txt

The seed selection process is sorted by 30d availability, block height and version.
>>>>>>> upstream/0.10
