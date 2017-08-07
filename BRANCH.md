
# 10.5.1 Change Log:
- Bitcoin Miner code removal

(SAFE) Miner Removal - Commit: https://github.com/JustinPercy/BATA-Development/commit/5f1f79b5e4a64d1abd306872050c107f6194ae7f

Results:
0/64 - bata-qt.exe https://virustotal.com/en/file/411632683a428b0a281e42e11ddd5bab03f4034e0b41385338664e4800914907/analysis/1501485086/
1/64 - batad.exe https://virustotal.com/en/file/f3cd9fabcb9eb7d45c8e0d749bd85d16aef4f185c853752b2745dc4585b90144/analysis/1501484870/

- RefreshRecentConnections(Minutes) function
Commit:
https://github.com/JustinPercy/BATA-Development/commit/68703b29b8b11fb3fd98ddced7c4bb7da5ff9cd6

Results:
Maintains long-term out-going connections to peers/nodes with at a set interval automatically (avoiding wallet-restart for peer discovery).
=======
# 10.5 Change Log
- Protocol Version: 80008
- Fixed random crashing from net.cpp (Windows & Linux)
- Bitcoin Firewall - Debug.log output
- Optimized Bitcoin Firewall 1.0 settings

