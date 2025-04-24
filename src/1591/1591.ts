function isPrintable(targetGrid: number[][]): boolean {

    const m: number = targetGrid.length;
    const n: number = targetGrid[0].length;

    // 1. Находим границы прямоугольников для каждого цвета
    interface Bounds {
        minRow: number;
        maxRow: number;
        minCol: number;
        maxCol: number;
        exists: boolean;
    }

    const colorBounds: Bounds[] = Array(61)
        .fill(null)
        .map(() => ({
            minRow: Infinity,
            maxRow: -Infinity,
            minCol: Infinity,
            maxCol: -Infinity,
            exists: false,
        }));

    const colorsPresent: number[] = []; // Массив присутствующих цветов

    for (let i = 0; i < m; i++) {

        for (let j = 0; j < n; j++) {
            
            const color: number = targetGrid[i][j];
            
            if (!colorBounds[color].exists) {
                colorsPresent.push(color);
                colorBounds[color].exists = true;
            }
            
            colorBounds[color].minRow = Math.min(colorBounds[color].minRow, i);
            colorBounds[color].maxRow = Math.max(colorBounds[color].maxRow, i);
            colorBounds[color].minCol = Math.min(colorBounds[color].minCol, j);
            colorBounds[color].maxCol = Math.max(colorBounds[color].maxCol, j);
        }
    }

    // 2. Строим зависимости между цветами
    const dependencies: number[] = Array(61).fill(0); // Сколько цветов зависят от данного
    const dependents: number[][] = Array(61)
        .fill(null)
        .map(() => []); // Массив цветов, зависящих от данного

    for (const c1 of colorsPresent) {
        
        const bounds: Bounds = colorBounds[c1];
        const colorsInRect: boolean[] = Array(61).fill(false); // Цвета внутри прямоугольника c1
        
        // Собираем цвета внутри прямоугольника
        for (let i = bounds.minRow; i <= bounds.maxRow; i++) {
            for (let j = bounds.minCol; j <= bounds.maxCol; j++) {
                
                const c2: number = targetGrid[i][j];
                
                if (c2 !== c1) {
                    colorsInRect[c2] = true;
                }
            }
        }
        
        // Добавляем зависимости: c2 должен быть напечатан раньше c1
        for (let c2 = 1; c2 <= 60; c2++) {
            
            if (colorsInRect[c2]) {
                
                dependents[c2].push(c1);
                dependencies[c1]++;
            }
        }
    }

    // 3. Топологическая сортировка
    const queue: number[] = [];
    
    for (const color of colorsPresent) {
        
        if (dependencies[color] === 0) {
            queue.push(color);
        }
    }

    let processed: number = 0;
    
    while (queue.length > 0) {
        
        const color: number = queue.shift()!;
        processed++;
        
        // Уменьшаем зависимости
        for (const dependent of dependents[color]) {
            
            dependencies[dependent]--;
            
            if (dependencies[dependent] === 0) {
                queue.push(dependent);
            }
        }
    }

    // Если обработали не все цвета, есть цикл
    return processed === colorsPresent.length;
}