use std::collections::VecDeque;

struct Solution;

impl Solution {
    pub fn remaining_methods(n: i32, k: i32, invocations: Vec<Vec<i32>>) -> Vec<i32> {
        let n = n as usize;
        let k = k as usize;
        let mut adj: Vec<Vec<usize>> = vec![vec![]; n];
        for invocation in &invocations {
            let u = invocation[0] as usize;
            let v = invocation[1] as usize;
            adj[u].push(v);
        }
        let mut queue: VecDeque<usize> = VecDeque::with_capacity(n);
        let mut visited = vec![false; n];
        visited[k] = true;
        queue.push_back(k);
        while let Some(current) = queue.pop_front() {
            for &next in &adj[current] {
                if !visited[next] {
                    visited[next] = true;
                    queue.push_back(next);
                }
            }
        }
        for invocation in &invocations {
            let u = invocation[0] as usize;
            let v = invocation[1] as usize;
            if visited[u] != visited[v] {
                return (0..n as i32).collect();
            }
        }
        (0..n as i32)
            .filter(|&method| !visited[method as usize])
            .collect()
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_example_1() {
        let n = 4;
        let k = 1;
        let invocations = vec![vec![1, 2], vec![0, 1], vec![3, 2]];
        let result = Solution::remaining_methods(n, k, invocations);
        assert_eq!(result, vec![0, 1, 2, 3]);
    }

    #[test]
    fn test_example_2() {
        let n = 5;
        let k = 0;
        let invocations = vec![vec![1, 2], vec![0, 2], vec![0, 1], vec![3, 4]];
        let result = Solution::remaining_methods(n, k, invocations);
        assert_eq!(result, vec![3, 4]);
    }

    #[test]
    fn test_example_3() {
        let n = 3;
        let k = 2;
        let invocations = vec![vec![1, 2], vec![0, 1], vec![2, 0]];
        let result = Solution::remaining_methods(n, k, invocations);
        assert_eq!(result, vec![]);
    }
}

fn main() {}
