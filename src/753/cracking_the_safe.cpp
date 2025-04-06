/*
 There is a safe protected by a password. The password is a sequence of n digits
where each digit can be in the range [0, k - 1].

The safe has a peculiar way of checking the password. When you enter in a
sequence, it checks the most recent n digits that were entered each time you
type a digit.
Time complexity: O(k ^ n)
Additional space complexity: O(kMaxSymbols * k ^ (n - 1))
Idea: According to De Bruyne's sequence total password length in this problem is
always k ^ n + 1. Use dfs to iterate through all possible passwords
*/

#include <string>
#include <unordered_map>
#include <array>
#include <cmath>

class Solution {
public:
    static std::string crackSafe(int n, int k) {
        const int total_passwords = static_cast<int>(std::pow(k, n));
        std::string ans(n, '0');
        std::unordered_map<std::string, std::array<bool, kMaxSymbols>>
            used_passwords;
        ans.reserve(total_passwords + 1);
        used_passwords[std::string(n - 1, '0')][0] = true;
        const char symbol_limit = static_cast<char>(48 + k);
        int current_passwords = 1;
        getPasswordSeq(current_passwords, total_passwords, n, symbol_limit,
                       used_passwords, ans);
        return ans;
    }

private:
    static constexpr int kMaxSymbols = 10;

    // Return true if function found password_seq of min length, false otherwise
    static bool getPasswordSeq(
        int& current_passwords, const int& total_passwords, const int& n,
        const char& symbol_limit,
        std::unordered_map<std::string, std::array<bool, kMaxSymbols>>&
            used_passwords,
        std::string& ans) {
        if (current_passwords == total_passwords) {
            return true;
        }
        std::string new_base = ans.substr(ans.size() - n + 1);
        for (char new_symbol = '0'; new_symbol < symbol_limit; ++new_symbol) {
            if (!used_passwords.contains(new_base) ||
                !used_passwords[new_base][new_symbol - '0']) {
                used_passwords[new_base][new_symbol - '0'] = true;
                ++current_passwords;
                ans.push_back(new_symbol);
                if (getPasswordSeq(current_passwords, total_passwords, n,
                                   symbol_limit, used_passwords, ans)) {
                    return true;
                }
                ans.pop_back();
                --current_passwords;
                used_passwords[new_base][new_symbol - '0'] = false;
            }
        }
        return false;
    }
};
