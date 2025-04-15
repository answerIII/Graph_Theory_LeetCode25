const isBipartite = require('./785');

describe('785. Is Graph Bipartite?', () => {
  test('Example 1', () => {
    const graph = [[1,2,3],[0,2],[0,1,3],[0,2]];
    expect(isBipartite(graph)).toBe(false);
  });

  test('Example 2', () => {
    const graph = [[1,3],[0,2],[1,3],[0,2]];
    expect(isBipartite(graph)).toBe(true);
  });
});