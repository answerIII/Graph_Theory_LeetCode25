const maxProbability = require('./1514');

describe('1514. Path with Maximum Probability', () => {
  test('Example 1', () => {
    const n = 3;
    const edges = [[0,1],[1,2],[0,2]];
    const succProb = [0.5,0.5,0.2];
    const start = 0, end = 2;
    expect(maxProbability(n, edges, succProb, start, end)).toBeCloseTo(0.25, 5);
  });

  test('Example 2', () => {
    const n = 3;
    const edges = [[0,1],[1,2],[0,2]];
    const succProb = [0.5,0.5,0.3];
    const start = 0, end = 2;
    expect(maxProbability(n, edges, succProb, start, end)).toBeCloseTo(0.3, 5);
  });

  test('Example 3', () => {
    const n = 3;
    const edges = [[0,1]];
    const succProb = [0.5];
    const start = 0, end = 2;
    expect(maxProbability(n, edges, succProb, start, end)).toBeCloseTo(0.0, 5);
  });
});