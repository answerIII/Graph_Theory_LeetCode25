function minScore(n: number, roads: number[][]): number {
  const graph = Array.from({ length: n + 1 }, (): number[][] => []);
  roads.forEach(([u, v, d]) => {
    graph[u].push([v, d]);
    graph[v].push([u, d]);
  });

  const visited = new Array<boolean>(n + 1).fill(false);
  let ans = Infinity;
  const dfs = (v: number): void => {
    visited[v] = true;
    for (const [u, d] of graph[v]) {
      ans = Math.min(ans, d);
      if (!visited[u]) {
        dfs(u);
      }
    }
  };
  dfs(1);
  return ans;
}
