/**
 * @param {string} initialCurrency
 * @param {string[][]} pairs1
 * @param {number[]} rates1
 * @param {string[][]} pairs2
 * @param {number[]} rates2
 * @return {number}
 */
var maxAmount = function(initialCurrency, pairs1, rates1, pairs2, rates2) {
    
    // 1. Создаём графы для обоих дней
    const graph1 = {};
    const graph2 = {};
    const currencies = new Set([initialCurrency]);
    
    // Заполняем граф для первого дня
    for (let i = 0; i < pairs1.length; i++) {
        
        const [start, target] = pairs1[i];
        const rate = rates1[i];
        
        if (!graph1[start]) graph1[start] = [];
        if (!graph1[target]) graph1[target] = [];
        
        graph1[start].push([target, rate]);
        graph1[target].push([start, 1 / rate]);
        
        currencies.add(start);
        currencies.add(target);
    }
    
    // Заполняем граф для второго дня
    for (let i = 0; i < pairs2.length; i++) {
        
        const [start, target] = pairs2[i];
        const rate = rates2[i];
        
        if (!graph2[start]) graph2[start] = [];
        if (!graph2[target]) graph2[target] = [];
        
        graph2[start].push([target, rate]);
        graph2[target].push([start, 1 / rate]);
        
        currencies.add(start);
        currencies.add(target);
    }
    
    // 2. Функция для поиска максимального множителя с помощью Беллмана-Форда
    function findMaxMultiplier(graph, start, end) {
        
        const amounts = {};
        
        for (const currency of currencies) {
            amounts[currency] = 0; // Изначально 0 для всех валют
        }
        amounts[start] = 1; // Начинаем с 1 единицы
        
        // Беллман-Форд с умножением
        for (let i = 0; i < currencies.size - 1; i++) {
            for (const from of currencies) {
                
                if (!graph[from]) continue;
                
                for (const [to, rate] of graph[from]) {
                    
                    const newAmount = amounts[from] * rate;
                    
                    if (newAmount > amounts[to]) {
                        amounts[to] = newAmount;
                    }
                }
            }
        }
        
        return amounts[end];
    }
    
    // 3. Первый день: находим максимальное количество каждой валюты
    const amountsAfterDay1 = {};
    
    for (const currency of currencies) {
        amountsAfterDay1[currency] = findMaxMultiplier(graph1, initialCurrency, currency);
    }
    
    // 4. Второй день: максимизируем количество initialCurrency
    let maxFinalAmount = 0;
    
    for (const intermediate of currencies) {
        
        const amountAfterDay1 = amountsAfterDay1[intermediate];
        
        if (amountAfterDay1 === 0) continue; // Пропускаем недостижимые валюты
        
        const multiplierDay2 = findMaxMultiplier(graph2, intermediate, initialCurrency);
        const finalAmount = amountAfterDay1 * multiplierDay2;
        
        maxFinalAmount = Math.max(maxFinalAmount, finalAmount);
    }
    
    // 5. Учитываем случай, когда не делаем конвертаций
    return Math.max(maxFinalAmount, 1.0);
};

module.exports = maxAmount;