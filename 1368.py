class Solution:
    def minCost(self, grid: List[List[int]])->int:
        n=len(grid)
        m=len(grid[0])
        c=0
        i=0
        q=deque()
        mc=[[float("inf")]*m for _ in range(n)]
        self.dfs(grid, 0, 0, mc, c, q)
        while q:
            c=c+1
            s=len(q)
            while i<s:
                r,l=q.popleft()
                for dir_idx, (dx, dy) in enumerate(self.np):
                    self.dfs(grid, r+dx, l+dy, mc, c, q)
                i=i+1
            i=0
        return mc[n - 1][m - 1]
    def v(self, mc: List[List[int]], r:int, l:int)->bool:
        return (0<=r and r<len(mc) and 0<=l and l<len(mc[0]) and mc[r][l]==float("inf"))
    def dfs(self, grid:List[List[int]], r:int, l:int, mc:List[List[int]], c:int, q:deque)->None:
        if not self.v(mc,r,l):
            return
        mc[r][l] = c
        q.append((r,l))
        d=grid[r][l]-1
        dx,dy=self.np[d]
        self.dfs(grid, r+dx, l+dy, mc, c, q)
    np=[(0,1),(0,-1),(1,0),(-1,0)]