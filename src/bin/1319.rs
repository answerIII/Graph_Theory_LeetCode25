struct UnionFind {
    parent: Vec<usize>,
    rank: Vec<usize>,
    extra_cables: usize,
}

impl UnionFind {
    fn new(n: usize) -> Self {
        Self {
            parent: (0..n).collect(),
            rank: vec![0; n],
            extra_cables: 0,
        }
    }
    fn find(&self, mut u: usize) -> usize {
        while self.parent[u] != u {
            u = self.parent[u];
        }
        self.parent[u]
    }
    fn union(&mut self, u: usize, v: usize) {
        let mut u = self.find(u);
        let mut v = self.find(v);
        if u != v {
            if self.rank[u] < self.rank[v] {
                std::mem::swap(&mut u, &mut v);
            }
            self.parent[v] = u;
            if self.rank[u] == self.rank[v] {
                self.rank[u] += 1;
            }
        } else {
            self.extra_cables += 1;
        }
    }
}

struct Solution;

impl Solution {
    pub fn make_connected(n: i32, connections: Vec<Vec<i32>>) -> i32 {
        let n = n as usize;
        let mut uf = UnionFind::new(n);
        for conn in connections {
            uf.union(conn[0] as usize, conn[1] as usize);
        }
        let components = (0..n).filter(|&i| uf.parent[i] == i).count();
        let needed_cables = components - 1;
        if uf.extra_cables >= needed_cables {
            return needed_cables as i32;
        }
        -1
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_example_1() {
        let n = 4;
        let connections = vec![vec![0, 1], vec![0, 2], vec![1, 2]];
        let result = Solution::make_connected(n, connections);
        assert_eq!(result, 1)
    }

    #[test]
    fn test_example_2() {
        let n = 6;
        let connections = vec![vec![0, 1], vec![0, 2], vec![0, 3], vec![1, 2], vec![1, 3]];
        let result = Solution::make_connected(n, connections);
        assert_eq!(result, 2)
    }

    #[test]
    fn test_example_3() {
        let n = 6;
        let connections = vec![vec![0, 1], vec![0, 2], vec![0, 3], vec![1, 2]];
        let result = Solution::make_connected(n, connections);
        assert_eq!(result, -1)
    }

    #[test]
    fn test_example_4() {
        let n = 5;
        let connections = vec![vec![0, 1], vec![0, 2], vec![3, 4], vec![2, 3]];
        let result = Solution::make_connected(n, connections);
        assert_eq!(result, 0)
    }

    #[test]
    fn test_example_5() {
        let n = 12;
        let connections = vec![
            vec![1, 5],
            vec![1, 7],
            vec![1, 2],
            vec![1, 4],
            vec![3, 7],
            vec![4, 7],
            vec![3, 5],
            vec![0, 6],
            vec![0, 1],
            vec![0, 4],
            vec![2, 6],
            vec![0, 3],
            vec![0, 2],
        ];
        let result = Solution::make_connected(n, connections);
        assert_eq!(result, 4)
    }
}

fn main() {}
