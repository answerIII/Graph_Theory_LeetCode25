const possibleBipartition = require('./886');

describe('886. Possible Bipartition', () => {
  test('Example 1', () => {
    const n = 4;
    const dislikes = [[1,2],[1,3],[2,4]];
    expect(possibleBipartition(n, dislikes)).toBe(true);
  });

  test('Example 2', () => {
    const n = 3;
    const dislikes = [[1,2],[1,3],[2,3]];
    expect(possibleBipartition(n, dislikes)).toBe(false);
  });

  test('Empty dislikes', () => {
    const n = 5;
    const dislikes = [];
    expect(possibleBipartition(n, dislikes)).toBe(true);
  });
});