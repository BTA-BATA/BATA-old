
# 10.5.1 Change Log:
- Firewall bug fixed: socket send error Bad file descriptor (9)
- Inactivity checking syntax changed
- ThreadSocketHandler - Disconnect unused nodes
- Bitcoin Miner code removal
- RefreshRecentConnections(Minutes) function
Commit:
https://github.com/JustinPercy/BATA-Development/commit/68703b29b8b11fb3fd98ddced7c4bb7da5ff9cd6

Results:
Maintains long-term out-going connections to peers/nodes with at a set interval automatically (avoiding wallet-restart for peer discovery).

