function minimumCost(
  source: string,
  target: string,
  original: string[],
  changed: string[],
  cost: number[],
): number {
  const graph = Array.from({ length: 26 }, (_, i): number[] =>
    Array.from({ length: 26 }, (_, j): number => (i === j ? 0 : Infinity)),
  );
  for (let i = 0; i < original.length; ++i) {
    const from = original[i].charCodeAt(0) - 97;
    const to = changed[i].charCodeAt(0) - 97;
    graph[from][to] = Math.min(graph[from][to], cost[i]);
  }

  const floydWarshall = () => {
    for (let k = 0; k < 26; ++k) {
      for (let i = 0; i < 26; ++i) {
        for (let j = 0; j < 26; ++j) {
          graph[i][j] = Math.min(graph[i][j], graph[i][k] + graph[k][j]);
        }
      }
    }
  };

  floydWarshall();

  let ans = 0;

  for (let i = 0; i < source.length; ++i) {
    const dist = graph[source.charCodeAt(i) - 97][target.charCodeAt(i) - 97];
    if (dist === Infinity) return -1;
    ans += dist;
  }
  return ans;
}
