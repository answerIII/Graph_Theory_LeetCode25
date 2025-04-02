use std::collections::VecDeque;

struct Solution;

impl Solution {
    pub fn most_profitable_path(edges: Vec<Vec<i32>>, bob: i32, amount: Vec<i32>) -> i32 {
        let n = amount.len();
        let bob = bob as usize;
        let mut adj: Vec<Vec<usize>> = vec![vec![]; n];
        for edge in edges {
            let u = edge[0] as usize;
            let v = edge[1] as usize;
            adj[u].push(v);
            adj[v].push(u);
        }
        let b_paths = Self::bfs(&adj, bob).unwrap();
        let mut b_path_to_root = vec![n; n];
        let mut current = 0;
        while let Some((next, time)) = b_paths[current] {
            b_path_to_root[current] = time;
            if bob == current {
                break;
            }
            current = next;
        }
        let mut queue = VecDeque::with_capacity(n);
        queue.push_back((0, 0, 0));
        let mut visited = vec![false; n];
        let mut max = i32::MIN;
        while let Some((current, time, mut income)) = queue.pop_front() {
            if b_path_to_root[current] == n || time < b_path_to_root[current] {
                income += amount[current];
            } else if time == b_path_to_root[current] {
                income += amount[current] / 2;
            }
            if current != 0 && adj[current].len() == 1 {
                max = max.max(income);
            }
            for &next in &adj[current] {
                if !visited[next] {
                    queue.push_back((next, time + 1, income));
                }
            }
            visited[current] = true;
        }
        max
    }
    fn bfs(adj: &Vec<Vec<usize>>, start: usize) -> Option<Vec<Option<(usize, usize)>>> {
        let mut queue = VecDeque::with_capacity(adj.len());
        queue.push_back(start);
        let mut paths = vec![None; adj.len()];
        paths[start] = Some((start, 0));
        while let Some(current) = queue.pop_front() {
            for &next in &adj[current] {
                if paths[next].is_none() {
                    paths[next] = Some((current, paths[current]?.1 + 1));
                    queue.push_back(next);
                }
            }
        }
        Some(paths)
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_example_1() {
        let edges = vec![vec![0, 1], vec![1, 2], vec![1, 3], vec![3, 4]];
        let bob = 3;
        let amount = vec![-2, 4, 2, -4, 6];
        let result = Solution::most_profitable_path(edges, bob, amount);
        assert_eq!(result, 6);
    }

    #[test]
    fn test_example_2() {
        let edges = vec![vec![0, 1]];
        let bob = 1;
        let amount = vec![-7280, 2350];
        let result = Solution::most_profitable_path(edges, bob, amount);
        assert_eq!(result, -7280);
    }
}

fn main() {}
