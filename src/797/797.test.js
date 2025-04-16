const allPathsSourceTarget = require('./797');

describe('797. All Paths From Source to Target', () => {
  test('Example 1', () => {
    const graph = [[1,2],[3],[3],[]];
    const expected = [[0,1,3],[0,2,3]];
    expect(allPathsSourceTarget(graph)).toEqual(expect.arrayContaining(expected));
  });

  test('Example 2', () => {
    const graph = [[4,3,1],[3,2,4],[3],[4],[]];
    const expected = [[0,4],[0,3,4],[0,1,3,4],[0,1,2,3,4],[0,1,4]];
    expect(allPathsSourceTarget(graph)).toEqual(expect.arrayContaining(expected));
  });
});