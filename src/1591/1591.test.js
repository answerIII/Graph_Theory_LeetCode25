const isPrintable = require('./1591');

describe('1591. Strange Printer II', () => {
  test('Example 1', () => {
    const targetGrid = [
      [1,1,1,1],
      [1,2,2,1],
      [1,2,2,1],
      [1,1,1,1]
    ];
    expect(isPrintable(targetGrid)).toBe(true);
  });

  test('Example 2', () => {
    const targetGrid = [
      [1,1,1,1],
      [1,1,3,3],
      [1,1,3,4],
      [5,5,1,4]
    ];
    expect(isPrintable(targetGrid)).toBe(true);
  });

  test('Example 3', () => {
    const targetGrid = [
      [1,2,1],
      [2,1,2],
      [1,2,1]
    ];
    expect(isPrintable(targetGrid)).toBe(false);
  });
});