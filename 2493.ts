function magnificentSets(n: number, edges: number[][]): number {
  const graph = Array.from({ length: n }, (): number[] => []);
  for (let [u, v] of edges) {
    --u;
    --v;
    graph[v].push(u);
    graph[u].push(v);
  }

  const isBipartite = (): number => {
    enum Colors {
      Red,
      Black,
    }
    const colors = new Array<Colors>(graph.length);
    let ans = 0;

    const dfs = (v: number): number => {
      for (const u of graph[v]) {
        //console.log(v, u, colors[v], colors[u]);
        if (colors[u] === undefined) {
          colors[u] = colors[v] === Colors.Red ? Colors.Black : Colors.Red;
          const maxDistance = dfs(u);
          if (maxDistance === -1) return -1;
          maxDistances[v] = Math.max(maxDistances[v], maxDistance);
        } else if (colors[v] === colors[u]) return -1;
      }
      return maxDistances[v];
    };

    for (let i = 0; i < graph.length; ++i) {
      if (colors[i] === undefined) {
        colors[i] = Colors.Red;
        const maxDistance = dfs(i);
        if (maxDistance === -1) return -1;
        ans += maxDistance;
      }
    }
    return ans;
  };

  const getMaxDistances = (): number[] => {
    const bfs = (v: number): number => {
      const visited = new Array<boolean>(n).fill(false);
      const queue = [v];
      visited[v] = true;
      let maxDistance = 0;
      while (queue.length > 0) {
        const layerSize = queue.length;
        for (let _ = 0; _ < layerSize; ++_) {
          v = queue.shift()!;
          for (const u of graph[v]) {
            if (!visited[u]) {
              visited[u] = true;
              queue.push(u);
            }
          }
        }
        ++maxDistance;
      }
      return maxDistance;
    };
    return [...Array(graph.length)].map((_, index) => bfs(index));
  };

  const maxDistances = getMaxDistances();

  return isBipartite();
}
