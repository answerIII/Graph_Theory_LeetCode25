use std::collections::{HashMap, HashSet};

struct Solution;

impl Solution {
    pub fn is_printable(target_grid: Vec<Vec<i32>>) -> bool {
        let (m, n) = (target_grid.len(), target_grid[0].len());
        let mut borders = HashMap::new();
        for i in 0..m {
            for j in 0..n {
                // [min_row, min_col, max_row, max_col]
                let border = borders.entry(target_grid[i][j]).or_insert([m, n, 0, 0]);
                border[0] = border[0].min(i);
                border[1] = border[1].min(j);
                border[2] = border[2].max(i);
                border[3] = border[3].max(j);
            }
        }
        let mut adj = HashMap::new();
        for (&color, &border) in &borders {
            for i in border[0]..=border[2] {
                for j in border[1]..=border[3] {
                    let adj = adj.entry(color).or_insert(HashSet::new());
                    if target_grid[i][j] != color {
                        adj.insert(target_grid[i][j]);
                    }
                }
            }
        }
        !Self::has_cycle(adj)
    }

    fn has_cycle(adj: HashMap<i32, HashSet<i32>>) -> bool {
        #[derive(PartialEq, Debug)]
        enum Color {
            White,
            Gray,
            Black,
        }
        fn dfs(
            node: i32,
            adj: &HashMap<i32, HashSet<i32>>,
            colors: &mut HashMap<i32, Color>,
        ) -> bool {
            colors.insert(node, Color::Gray);
            if let Some(neighbors) = adj.get(&node) {
                for &neighbor in neighbors {
                    match colors.get(&neighbor) {
                        Some(Color::White) => {
                            if dfs(neighbor, adj, colors) {
                                return true;
                            }
                        }
                        Some(Color::Gray) => return true,
                        _ => {}
                    }
                }
            }
            colors.insert(node, Color::Black);
            false
        }
        let mut colors = HashMap::new();
        for &node in adj.keys() {
            colors.insert(node, Color::White);
        }
        for &node in adj.keys() {
            if dfs(node, &adj, &mut colors) {
                return true;
            }
        }
        false
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_example_1() {
        let target_grid = vec![
            vec![1, 1, 1, 1],
            vec![1, 2, 2, 1],
            vec![1, 2, 2, 1],
            vec![1, 1, 1, 1],
        ];
        let result = Solution::is_printable(target_grid);
        assert_eq!(result, true);
    }

    #[test]
    fn test_example_2() {
        let target_grid = vec![
            vec![1, 1, 1, 1],
            vec![1, 1, 3, 3],
            vec![1, 1, 3, 4],
            vec![5, 5, 1, 4],
        ];
        let result = Solution::is_printable(target_grid);
        assert_eq!(result, true);
    }

    #[test]
    fn test_example_3() {
        let target_grid = vec![vec![1, 2, 1], vec![2, 1, 2], vec![1, 2, 1]];
        let result = Solution::is_printable(target_grid);
        assert_eq!(result, false);
    }

    #[test]
    fn test_example_4() {
        let target_grid = vec![
            vec![6, 2, 2, 5],
            vec![2, 2, 2, 5],
            vec![2, 2, 2, 5],
            vec![4, 3, 3, 4],
        ];
        let result = Solution::is_printable(target_grid);
        assert_eq!(result, true);
    }
}

fn main() {}
