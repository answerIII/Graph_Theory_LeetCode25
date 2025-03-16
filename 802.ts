function eventualSafeNodes(graph: number[][]): number[] {
  enum NodeColor {
    White,
    Grey,
    Black,
  }

  const n = graph.length;
  const visited = new Array<NodeColor>(n).fill(NodeColor.White);

  const dfs = (v: number): boolean => {
    if (visited[v] === NodeColor.Grey) return true;
    if (visited[v] === NodeColor.Black) return false;

    visited[v] = NodeColor.Grey;

    for (const u of graph[v]) {
      if (dfs(u)) {
        return true;
      }
    }
    visited[v] = NodeColor.Black;

    return false;
  };

  for (let v = 0; v < n; ++v) {
    dfs(v);
  }

  const ans: number[] = [];

  for (let v = 0; v < n; ++v) {
    if (visited[v] === NodeColor.Black) {
      ans.push(v);
    }
  }

  return ans;
}
