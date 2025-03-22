use std::collections::VecDeque;

struct Solution;

impl Solution {
    pub fn shortest_distance_after_queries(n: i32, queries: Vec<Vec<i32>>) -> Vec<i32> {
        let n = n as usize;
        let mut result = Vec::with_capacity(queries.len());
        let mut queue: VecDeque<usize> = VecDeque::with_capacity(n);
        let mut distances = vec![None; n];
        let mut adj: Vec<Vec<usize>> = vec![vec![]; n];
        for i in 0..n - 1 {
            adj[i].push(i + 1);
        }
        for query in queries {
            let from = query[0] as usize;
            let to = query[1] as usize;
            adj[from].push(to);
            distances[0] = Some(0);
            queue.push_back(0);
            while let Some(city) = queue.pop_front() {
                for &neighboor in &adj[city] {
                    if distances[neighboor].is_none() {
                        distances[neighboor] = Some(distances[city].unwrap() + 1);
                        queue.push_back(neighboor);
                    }
                }
            }
            result.push(distances[n - 1].unwrap());
            distances.fill(None);
            queue.clear();
        }
        result
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_example_1() {
        let n = 5;
        let queries = vec![vec![2, 4], vec![0, 2], vec![0, 4]];
        let result = Solution::shortest_distance_after_queries(n, queries);
        assert_eq!(result, vec![3, 2, 1]);
    }

    #[test]
    fn test_example_2() {
        let n = 4;
        let queries = vec![vec![0, 3], vec![0, 2]];
        let result = Solution::shortest_distance_after_queries(n, queries);
        assert_eq!(result, vec![1, 1]);
    }

    #[test]
    fn test_example_3() {
        let n = 6;
        let queries = vec![vec![1, 4], vec![0, 2]];
        let result = Solution::shortest_distance_after_queries(n, queries);
        assert_eq!(result, vec![3, 3]);
    }
}

fn main() {}
