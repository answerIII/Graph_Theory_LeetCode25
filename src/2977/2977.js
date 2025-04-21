/**
 * @param {string} source
 * @param {string} target
 * @param {string[]} original
 * @param {string[]} changed
 * @param {number[]} cost
 * @return {number}
 */
var minimumCost = function(source, target, original, changed, cost) {
    
    // 1. Собираем длины подстрок
    const lengths = new Set();
    
    for (const str of original) lengths.add(str.length);
    
    const lengthList = Array.from(lengths).sort((a, b) => a - b);
    
    // 2. Построим граф преобразований
    const substrings = new Set([...original, ...changed]);
    const substringList = Array.from(substrings);
    const m = substringList.length;
    const substringIndex = new Map();
    
    for (let i = 0; i < m; i++) {
        substringIndex.set(substringList[i], i);
    }
    
    const INF = Number.MAX_SAFE_INTEGER;
    const dist = Array.from({ length: m }, () => Array(m).fill(INF));
    
    for (let i = 0; i < m; i++) dist[i][i] = 0;
    
    for (let i = 0; i < original.length; i++) {
        
        const from = substringIndex.get(original[i]);
        const to = substringIndex.get(changed[i]);
        dist[from][to] = Math.min(dist[from][to], cost[i]);
    }
    
    // Флойд-Уоршелл
    for (let k = 0; k < m; k++) {
        for (let i = 0; i < m; i++) {
            for (let j = 0; j < m; j++) {
                
                if (dist[i][k] !== INF && dist[k][j] !== INF) {
                    dist[i][j] = Math.min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }
    
    // 3. Динамическое программирование
    const n = source.length;
    const dp = new Array(n + 1).fill(INF);
    dp[0] = 0;
    
    // Для каждой позиции i
    for (let i = 1; i <= n; i++) {
        
        // Если символы совпадают
        if (source[i - 1] === target[i - 1]) {
            dp[i] = Math.min(dp[i], dp[i - 1]);
        }
        
        // Пробуем подстроки фиксированных длин, заканчивающиеся в i
        for (const len of lengthList) {
            
            const j = i - len;
            
            if (j < 0) continue;
            if (dp[j] === INF) continue;
            
            const srcSub = source.slice(j, i);
            const tgtSub = target.slice(j, i);
            
            if (substringIndex.has(srcSub) && substringIndex.has(tgtSub)) {
                
                const from = substringIndex.get(srcSub);
                const to = substringIndex.get(tgtSub);
                
                if (dist[from][to] !== INF) {
                    dp[i] = Math.min(dp[i], dp[j] + dist[from][to]);
                }
            }
        }
    }
    
    return dp[n] === INF ? -1 : dp[n];
};

module.exports = minimumCost;