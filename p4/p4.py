"""
A palindromic number reads the same both ways. The largest palindrome made from the product of two 2-digit numbers is 9009 = 91 x 99.

Find the largest palindrome made from the product of two 3-digit numbers.
"""

def is_palindrome(n):
    return str(n) == str(n)[::-1]

def largest_palindrome():
    max_palindrome = 0
    for i in range(999, 99, -1):
        for j in range(i, 99, -1):
            product = i * j
            if product < max_palindrome:
                break
            if is_palindrome(product):
                max_palindrome = product
    return max_palindrome

if __name__ == "__main__":
    print(largest_palindrome())