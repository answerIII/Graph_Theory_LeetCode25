function countCompleteComponents(n: number, edges: number[][]): number {
  const graph = Array.from({ length: n }, (): number[] => []);
  edges.forEach(([u, v]) => {
    graph[u].push(v);
    graph[v].push(u);
  });

  const visited = new Array<boolean>(n).fill(false);

  const processComponent = (v: number): [number, number] => {
    let vertices = 0;
    let edges = 0;

    const dfs = (v: number) => {
      visited[v] = true;
      ++vertices;
      edges += graph[v].length;
      for (const u of graph[v]) {
        if (!visited[u]) {
          dfs(u);
        }
      }
    };
    dfs(v);
    return [vertices, edges];
  };

  let ans = 0;

  for (let v = 0; v < n; ++v) {
    if (!visited[v]) {
      const [vertices, edges] = processComponent(v);
      if (edges === vertices * (vertices - 1)) ++ans;
    }
  }

  return ans;
}
