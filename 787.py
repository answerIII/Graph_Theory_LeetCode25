# This is the first solution that passes the time limit
# Previous ones are too slow, which makes them invalid

class Solution(object):
    def findCheapestPrice(self, n, flights, src, dst, k):
        prices = [float('inf')] * n
        prices[src] = 0
        queue = [src]
        next_queue = []
        tmp = list(prices)
        step = 0
        while queue and (step <= k):
            step += 1
            for flight in flights:
                if (flight[0] in queue) and (prices[flight[0]] + flight[2] < tmp[flight[1]]):
                    next_queue.append(flight[1])
                    tmp[flight[1]] = prices[flight[0]] + flight[2]
            prices = list(tmp)
            queue = list(next_queue)
            next_queue = []
        if type(prices[dst]) == int:
            return prices[dst]
        return -1
