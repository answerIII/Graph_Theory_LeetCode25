const maxAmount = require('./3387');

describe('3387. Maximize Amount After Two Days of Conversions', () => {
  test('Example 1', () => {
    const initialCurrency = "EUR";
    const pairs1 = [["EUR","USD"],["USD","JPY"]];
    const rates1 = [2.0,3.0];
    const pairs2 = [["JPY","USD"],["USD","CHF"],["CHF","EUR"]];
    const rates2 = [4.0,5.0,6.0];
    expect(maxAmount(initialCurrency, pairs1, rates1, pairs2, rates2)).toBeCloseTo(720.0, 5);
  });

  test('Example 2', () => {
    const initialCurrency = "NGN";
    const pairs1 = [["NGN","EUR"]];
    const rates1 = [9.0];
    const pairs2 = [["NGN","EUR"]];
    const rates2 = [6.0];
    expect(maxAmount(initialCurrency, pairs1, rates1, pairs2, rates2)).toBeCloseTo(1.5, 5);
  });

  test('Example 3', () => {
    const initialCurrency = "USD";
    const pairs1 = [["USD","EUR"]];
    const rates1 = [1.0];
    const pairs2 = [["EUR","JPY"]];
    const rates2 = [10.0];
    expect(maxAmount(initialCurrency, pairs1, rates1, pairs2, rates2)).toBeCloseTo(1.0, 5);
  });

  test('Example 766', () => {
    const initialCurrency = "S";
    const pairs1 = [["S","Z"],["Z","FYN"],["FYN","TW"],["TW","V"],["V","OO"]];
    const rates1 = [10.0,10.0,10.0,10.0,10.0];
    const pairs2 = [["OO","CTV"],["CTV","X"],["X","CI"],["CI","R"],["R","S"]];
    const rates2 = [10.0,10.0,10.0,10.0,10.0];
    expect(maxAmount(initialCurrency, pairs1, rates1, pairs2, rates2)).toBeCloseTo(10000000000.0, 5);
  });
});