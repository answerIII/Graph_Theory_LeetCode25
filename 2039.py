'''
Objective: determine the earliest point in time, starting from which no message transmission 
           occurs in the server network thus the network becomes inactive. need to take into account 
           the delivery time of messages to and from the main server, as well as the frequency of their resending, 
           depending on the patience parameter of each server
Idea of solution: uses BFS to find the shortest distances from each server to the main one (0), then calculates the time for each server to receive the last response, taking into account its patience[i]. The answer will be the maximum completion time of all messages + 1, as the network becomes inactive the next moment.
Complexity: O(V + E)
'''

from collections import deque

class Solution:
    def networkBecomesIdle(self, edges, patience):

        # initialization of the graph as adjacency list

        n = len(patience) # amount of servers
        adj = [[] for _ in range(n)]
        for u, v in edges:
            adj[u].append(v)
            adj[v].append(u)
        
        # need to find the shortest distances from the main server (0) to all other servers. 
        # for this we will use dist — an array where dist[i] stores the distance from server i to the main server 
        # and the BFS algorithm, which uses a queue for implementation.

        dist = [0] * n
        visited = [False] * n

        q = deque()
        q.append(0)
        visited[0] = True
        
        while q:
            u = q.popleft()

            for v in adj[u]:
                if not visited[v]:

                    dist[v] = dist[u] + 1
                    visited[v] = True
                    q.append(v)
        
        # for each server, we determine when it will receive the last response, taking into account its patience.

        ans = 0
        for i in range(1, n):

            # determine the time of the full cycle of sending a message and receiving a response (2 * dist[i]) 
            # and write the result to array time.

            time  = 2 * dist[i]

            # if patience[i] is greater than or equal to time, the server will not resend the message (last = 0)

            if patience[i] >= time:
                last = 0

            # else - > the last resend occurs at last = ((time - 1) // patience[i]) * patience[i]

            else:
                last = (time - 1) // patience[i] * patience[i]

            # ans is updated with the maximum value of last + time among all servers.    

            ans = max(ans, last + time)

        # network becomes idle the next second after the last message is received, so 1 is added to ans.

        return ans + 1