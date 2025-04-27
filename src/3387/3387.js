/**
 * @param {string} initialCurrency
 * @param {string[][]} pairs1
 * @param {number[]} rates1
 * @param {string[][]} pairs2
 * @param {number[]} rates2
 * @return {number}
 */
var maxAmount = function (initialCurrency, pairs1, rates1, pairs2, rates2) {
  // Функция для выполнения Беллмана-Форда
    const bellmanFord = (amounts, pairs, rates) => {
        const n = pairs.length;

        for (let i = 0; i < n; i++) {
            for (let j = 0; j < n; j++) {
                
                const start = pairs[j][0];
                const target = pairs[j][1];
                const rate = rates[j];
                
                const startVal = amounts.get(start) || 0;
                const targetVal = amounts.get(target) || 0;
                
                if (startVal > 0) {
                    amounts.set(target, Math.max(targetVal, startVal * rate));
                }

                if (targetVal > 0) {
                    amounts.set(start, Math.max(startVal, targetVal / rate));
                }
        }
    }
  };

  // Инициализируем суммы
  const amounts = new Map();
  amounts.set(initialCurrency, 1.0);
  bellmanFord(amounts, pairs1, rates1);
  
  const amountsDay1 = new Map(amounts);
  amounts.clear();
  
  amounts.set(initialCurrency, 1.0);
  const pairs2Reverse = pairs2.map(([start, target]) => [target, start]);
  bellmanFord(amounts, pairs2Reverse, rates2);

  let maxAmount = 1.0;
  
  for (const [currency, amount1] of amountsDay1) {
    
    const amount2 = amounts.get(currency) || 0;
    
    if (amount1 > 0 && amount2 > 0) {
        maxAmount = Math.max(maxAmount, amount1 * amount2);
    }
  }

  return Math.round(maxAmount * 100000) / 100000;
};

module.exports = maxAmount;