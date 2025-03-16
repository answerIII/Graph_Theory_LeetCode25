function loudAndRich(richer: number[][], quiet: number[]): number[] {
  const graph = Array.from({ length: quiet.length }, (): number[] => []);
  for (const [u, v] of richer) {
    graph[v].push(u);
  }

  const ans = new Array<number>(quiet.length).fill(-1);

  const dfs = (v: number): number => {
    if (ans[v] !== -1) {
      return ans[v];
    }
    let richestNode = v;

    for (const u of graph[v]) {
      const richerNode = dfs(u);
      if (quiet[richestNode] > quiet[richerNode]) {
        richestNode = richerNode;
      }
      //console.log(v, richerNode, richestNode);
    }

    ans[v] = richestNode;
    return richestNode;
  };

  for (let i = 0; i < quiet.length; ++i) {
    dfs(i);
  }

  return ans;
}
