use std::collections::HashMap;

struct Solution;

impl Solution {
    pub fn max_amount(
        initial_currency: String,
        pairs1: Vec<Vec<String>>,
        rates1: Vec<f64>,
        pairs2: Vec<Vec<String>>,
        rates2: Vec<f64>,
    ) -> f64 {
        fn bellman_ford(d: &mut HashMap<String, f64>, pairs: Vec<Vec<String>>, rates: Vec<f64>) {
            for _ in 0..pairs.len() {
                for (pair, &rate) in pairs.iter().zip(rates.iter()) {
                    let (start, target) = (&pair[0], &pair[1]);
                    let start_val = *d.entry(start.clone()).or_insert(0.0);
                    let target_val = *d.entry(target.clone()).or_insert(0.0);
                    d.insert(start.clone(), start_val.max(target_val / rate));
                    d.insert(target.clone(), target_val.max(start_val * rate));
                }
            }
        }
        let mut d: HashMap<String, f64> = HashMap::from([(initial_currency.clone(), 1.0)]);
        bellman_ford(&mut d, pairs1, rates1);
        bellman_ford(&mut d, pairs2, rates2);
        d[&initial_currency]
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_example1() {
        let initial_currency = "EUR".to_string();
        let pairs1 = vec![
            vec!["EUR".to_string(), "USD".to_string()],
            vec!["USD".to_string(), "JPY".to_string()],
        ];
        let rates1 = vec![2.0, 3.0];
        let pairs2 = vec![
            vec!["JPY".to_string(), "USD".to_string()],
            vec!["USD".to_string(), "CHF".to_string()],
            vec!["CHF".to_string(), "EUR".to_string()],
        ];
        let rates2 = vec![4.0, 5.0, 6.0];
        let result = Solution::max_amount(initial_currency, pairs1, rates1, pairs2, rates2);
        assert_eq!(result, 720.00000);
    }

    #[test]
    fn test_example2() {
        let initial_currency = "ACB".to_string();
        let pairs1 = vec![
            vec!["SFE".to_string(), "Z".to_string()],
            vec!["ACB".to_string(), "SFE".to_string()],
        ];
        let rates1 = vec![9.1, 2.4];
        let pairs2 = vec![
            vec!["SFE".to_string(), "Z".to_string()],
            vec!["ACB".to_string(), "SFE".to_string()],
        ];
        let rates2 = vec![2.8, 5.0];
        let result = Solution::max_amount(initial_currency, pairs1, rates1, pairs2, rates2);
        assert_eq!(result, 1.56000);
    }
}

fn main() {}
