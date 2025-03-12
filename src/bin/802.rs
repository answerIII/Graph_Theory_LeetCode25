struct Solution;

impl Solution {
    pub fn eventual_safe_nodes(graph: Vec<Vec<i32>>) -> Vec<i32> {
        let n = graph.len();
        let mut state: Vec<usize> = vec![0; n];
        let mut safe: Vec<bool> = vec![false; n];
        fn dfs(node: usize, graph: &Vec<Vec<i32>>, state: &mut Vec<usize>, safe: &mut Vec<bool>) -> bool {
            if state[node] == 1 { return false; }
            if state[node] == 2 { return safe[node]; }
            state[node] = 1;
            for &neighboor in &graph[node] {
                let neighboor = neighboor as usize;
                if !dfs(neighboor, graph, state, safe) {
                    return false;
                }
            }
            state[node] = 2;
            safe[node] = true;
            return true;
        }
        let mut result = Vec::with_capacity(n);
        for node in 0..n {
            if dfs(node, &graph, &mut state, &mut safe) {
                result.push(node as i32);
            }
        }
        result
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_example_1() {
        let graph = vec![
            vec![1, 2],
            vec![2, 3],
            vec![5],
            vec![0],
            vec![5],
            vec![],
            vec![],
        ];
        let result = Solution::eventual_safe_nodes(graph);
        assert_eq!(result, vec![2, 4, 5, 6]);
    }

    #[test]
    fn test_example_2() {
        let graph = vec![
            vec![1, 2, 3, 4],
            vec![1, 2],
            vec![3, 4],
            vec![0, 4],
            vec![],
        ];
        let result = Solution::eventual_safe_nodes(graph);
        assert_eq!(result, vec![4]);
    }
}

fn main() {}