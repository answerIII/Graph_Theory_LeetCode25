const minReorder = require('./1466');

describe('1466. Reorder Routes to Make All Paths Lead to the City Zero', () => {
  test('Example 1', () => {
    const n = 6;
    const connections = [[0,1],[1,3],[2,3],[4,0],[4,5]];
    expect(minReorder(n, connections)).toBe(3);
  });

  test('Example 2', () => {
    const n = 5;
    const connections = [[1,0],[1,2],[3,2],[3,4]];
    expect(minReorder(n, connections)).toBe(2);
  });

  test('Example 3', () => {
    const n = 3;
    const connections = [[1,0],[2,0]];
    expect(minReorder(n, connections)).toBe(0);
  });
});