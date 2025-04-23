/**
 * @param {number[][]} targetGrid
 * @return {boolean}
 */
var isPrintable = function(targetGrid) {
    
    const m = targetGrid.length;
    const n = targetGrid[0].length;
    
    // 1. Найдём прямоугольники для каждого цвета
    const colorBounds = Array(61).fill(null).map(() => ({
        minRow: Infinity,
        maxRow: -Infinity,
        minCol: Infinity,
        maxCol: -Infinity,
        exists: false
    }));
    
    const colorsPresent = new Set();
    
    for (let i = 0; i < m; i++) {
        
        for (let j = 0; j < n; j++) {
            
            const color = targetGrid[i][j];
            
            colorsPresent.add(color);
            colorBounds[color].minRow = Math.min(colorBounds[color].minRow, i);
            colorBounds[color].maxRow = Math.max(colorBounds[color].maxRow, i);
            colorBounds[color].minCol = Math.min(colorBounds[color].minCol, j);
            colorBounds[color].maxCol = Math.max(colorBounds[color].maxCol, j);
            colorBounds[color].exists = true;
        }
    }
    
    // 2. Построим зависимости
    const dependencies = Array(61).fill(0); // Сколько цветов зависят от данного
    const dependents = Array(61).fill(null).map(() => new Set()); // Какие цвета зависят от данного
    
    for (const c1 of colorsPresent) {
        
        const bounds1 = colorBounds[c1];
        const colorsInRect = new Set();
        // Собираем все цвета внутри прямоугольника c1
        for (let i = bounds1.minRow; i <= bounds1.maxRow; i++) {
            
            for (let j = bounds1.minCol; j <= bounds1.maxCol; j++) {
                
                const c2 = targetGrid[i][j];
                
                if (c2 !== c1) {
                    colorsInRect.add(c2);
                }
            }
        }
        // Для каждого цвета c2 внутри прямоугольника c1: c2 должен быть напечатан раньше
        for (const c2 of colorsInRect) {
            
            dependents[c2].add(c1);
            dependencies[c1]++;
        }
    }
    
    // 3. Топологическая сортировка с помощью очереди
    const queue = [];
    
    for (const color of colorsPresent) {
        
        if (dependencies[color] === 0) {
            queue.push(color);
        }
    }
    
    let processed = 0;
    
    while (queue.length > 0) {
        
        const color = queue.shift();
        processed++;
        
        // Уменьшаем зависимости для всех цветов, которые зависят от текущего
        for (const dependent of dependents[color]) {
            
            dependencies[dependent]--;
            
            if (dependencies[dependent] === 0) {
                queue.push(dependent);
            }
        }
    }
    
    // Если обработали не все цвета, значит есть цикл
    return processed === colorsPresent.size;
};

// Экспортируем функцию
module.exports = isPrintable;