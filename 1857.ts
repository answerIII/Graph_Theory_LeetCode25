function largestPathValue(colors: string, edges: number[][]): number {
  const n = Math.max(...[...edges.flat(), 0]) + 1;
  const colorsCount = new Set(colors).size;
  const colorMap = new Map<string, number>();
  const isSource = Array.from({ length: n }, () => true);
  for (const char of colors) {
    if (!colorMap.has(char)) {
      colorMap.set(char, colorMap.size);
    }
  }
  const graph = Array.from({ length: n }, (): number[] => []);
  edges.forEach(([u, v]) => {
    graph[u].push(v);
    isSource[v] = false;
  });

  const TopSort = (): number[] | null => {
    enum NodeColor {
      White,
      Grey,
      Black,
    }
    const visited = Array.from({ length: n }, (): NodeColor => NodeColor.White);
    const ans: number[] = [];

    const dfs = (u: number): boolean => {
      if (visited[u] === NodeColor.Grey) return true;
      if (visited[u] === NodeColor.Black) return false;

      visited[u] = NodeColor.Grey;

      for (const v of graph[u]) {
        if (dfs(v)) {
          return true;
        }
      }

      ans.push(u);
      visited[u] = NodeColor.Black;

      return false;
    };

    for (let i = 0; i < n; ++i) {
      if (dfs(i)) {
        return null;
      }
    }
    return ans; // .reverse();
  };

  const dp = Array.from({ length: n }, () =>
    Array.from({ length: colorsCount }, () => 0),
  );
  const sortedGraph = TopSort();
  if (sortedGraph == null) {
    return -1;
  }

  // console.log(sortedGraph);

  for (let u = 0; u < n; ++u) {
    graph[sortedGraph[u]].forEach((v) => {
      for (let c = 0; c < colorsCount; ++c) {
        dp[sortedGraph[u]][c] = Math.max(dp[sortedGraph[u]][c], dp[v][c]);
        // console.log(sortedGraph[u], u, v, c);
      }
    });

    ++dp[sortedGraph[u]][colorMap.get(colors[sortedGraph[u]])!];
    // console.log(dp);
  }
  // console.log(dp);
  if (!dp) return 1;
  return Math.max(...dp.filter((_, i) => isSource[i]).flat());
}
