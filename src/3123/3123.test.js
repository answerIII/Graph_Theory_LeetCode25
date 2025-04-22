const findAnswer = require('./3123');

describe('3123. Find Edges in Shortest Paths', () => {
  test('Example 1', () => {
    const n = 6;
    const edges = [[0,1,4],[0,2,1],[1,3,2],[1,4,3],[1,5,1],[2,3,1],[3,5,3],[4,5,2]];
    expect(findAnswer(n, edges)).toEqual([true,true,true,false,true,true,true,false]);
  });

  test('Example 2', () => {
    const n = 4;
    const edges = [[2,0,1],[0,1,1],[0,3,4],[3,2,2]];
    expect(findAnswer(n, edges)).toEqual([true,false,false,true]);
  });
});