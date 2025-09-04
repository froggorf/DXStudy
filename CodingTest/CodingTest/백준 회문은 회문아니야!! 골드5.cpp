#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

bool isPalindrome(const string& s) {
    string rev = s;
    reverse(rev.begin(), rev.end());
    return s == rev;
}

int main() {
    string s;
    cin >> s;
    int n = s.length();

    // 전체가 팰린드롬이 아니면 전체 길이
    if (!isPalindrome(s)) {
        cout << n << endl;
        return 0;
    }

    // 양 끝 하나씩 제거해서 확인
    if (n > 1) {
        // 앞 문자 제거
        if (!isPalindrome(s.substr(1))) {
            cout << n - 1 << endl;
            return 0;
        }
        // 뒤 문자 제거
        if (!isPalindrome(s.substr(0, n-1))) {
            cout << n - 1 << endl;
            return 0;
        }
    }

    cout << -1 << endl;
    return 0;
}