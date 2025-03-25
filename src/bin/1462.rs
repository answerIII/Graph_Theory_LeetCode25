use std::collections::{HashSet, VecDeque};

struct Solution;

impl Solution {
    pub fn check_if_prerequisite(
        num_courses: i32,
        prerequisites: Vec<Vec<i32>>,
        queries: Vec<Vec<i32>>,
    ) -> Vec<bool> {
        let n = num_courses as usize;
        let mut adj: Vec<Vec<usize>> = vec![vec![]; n];
        for prerequisite in prerequisites {
            adj[prerequisite[0] as usize].push(prerequisite[1] as usize);
        }
        let mut reachable: Vec<HashSet<usize>> = vec![HashSet::new(); n];
        let mut queue: VecDeque<usize> = VecDeque::with_capacity(n);
        let mut visited = vec![false; n];
        for start in 0..n {
            visited[start] = true;
            queue.push_back(start);
            while let Some(current) = queue.pop_front() {
                for &next in &adj[current] {
                    if !visited[next] {
                        visited[next] = true;
                        reachable[start].insert(next);
                        queue.push_back(next);
                    }
                }
            }
            queue.clear();
            visited.fill(false);
        }
        queries
            .into_iter()
            .map(|query| {
                let u = query[0] as usize;
                let v = query[1] as usize;
                reachable[u].contains(&v)
            })
            .collect()
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_example_1() {
        let num_courses = 2;
        let prerequisites = vec![vec![1, 0]];
        let queries = vec![vec![0, 1], vec![1, 0]];
        let result = Solution::check_if_prerequisite(num_courses, prerequisites, queries);
        assert_eq!(result, vec![false, true]);
    }

    #[test]
    fn test_example_2() {
        let num_courses = 2;
        let prerequisites = vec![];
        let queries = vec![vec![1, 0], vec![0, 1]];
        let result = Solution::check_if_prerequisite(num_courses, prerequisites, queries);
        assert_eq!(result, vec![false, false]);
    }

    #[test]
    fn test_example_3() {
        let num_courses = 3;
        let prerequisites = vec![vec![1, 2], vec![1, 0], vec![2, 0]];
        let queries = vec![vec![1, 0], vec![1, 2]];
        let result = Solution::check_if_prerequisite(num_courses, prerequisites, queries);
        assert_eq!(result, vec![true, true]);
    }

    #[test]
    fn test_example_4() {
        let num_courses = 5;
        let prerequisites = vec![vec![0, 1], vec![1, 2], vec![2, 3], vec![3, 4]];
        let queries = vec![vec![0, 4], vec![4, 0], vec![1, 3], vec![3, 0]];
        let result = Solution::check_if_prerequisite(num_courses, prerequisites, queries);
        assert_eq!(result, vec![true, false, true, false]);
    }
}

fn main() {}
