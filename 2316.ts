function countPairs(n: number, edges: number[][]): number {
  const graph = Array.from({ length: n }, (): number[] => []);
  edges.forEach(([u, v]) => {
    graph[u].push(v);
    graph[v].push(u);
  });

  const visited = new Array<boolean>(n).fill(false);

  const processComponent = (v: number) => {
    let nodesInComponent = 0;
    const dfs = (v: number) => {
      ++nodesInComponent;
      visited[v] = true;
      for (const u of graph[v]) {
        if (!visited[u]) {
          dfs(u);
        }
      }
    };
    dfs(v);
    return nodesInComponent;
  };

  const nodes: number[] = [];
  for (let v = 0; v < n; ++v) {
    if (!visited[v]) {
      nodes.push(processComponent(v));
    }
  }

  let ans = 0;
  for (let i = 0; i < nodes.length; ++i) {
    ans += nodes[i] * (n - nodes[i]);
  }

  return ans / 2;
}
