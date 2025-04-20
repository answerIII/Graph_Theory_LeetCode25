function minEdgeReversals(n: number, edges: number[][]): number[] {
  const directedGraph = new Set<string>();
  const undirectedGraph = Array.from({ length: n }, (): number[] => []);
  edges.forEach(([u, v]) => {
    directedGraph.add(`${u} ${v}`);
    undirectedGraph[u].push(v);
    undirectedGraph[v].push(u);
  });
  const visited = Array.from({ length: n }, () => false);
  const rootReverses = (u: number): number => {
    let ans = 0;
    visited[u] = true;
    for (const v of undirectedGraph[u]) {
      if (!visited[v]) {
        ans += rootReverses(v) + (directedGraph.has(`${u} ${v}`) ? 0 : 1);
      }
    }
    return ans;
  };

  const ans = Array.from({ length: n }, () => 0);
  ans[0] = rootReverses(0);

  visited.fill(false);

  const totalReverses = (u: number) => {
    visited[u] = true;
    for (const v of undirectedGraph[u]) {
      if (!visited[v]) {
        ans[v] = ans[u] + (directedGraph.has(`${u} ${v}`) ? 1 : -1);
        totalReverses(v);
      }
    }
  };

  totalReverses(0);

  return ans;
}
