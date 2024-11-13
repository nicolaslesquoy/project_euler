"""
The prime factors of 13195 are 5, 7, 13 and 29.

What is the largest prime factor of the number 600851475143 ?
"""

import numpy as np

def prime_factors(n):
    factors = []

    # Add the number of 2s that divide n
    while n % 2 == 0:
        factors.append(2)
        n = n // 2

    # n must be odd at this point
    for i in range(3, int(np.sqrt(n)) + 1, 2):
        while n % i == 0:
            factors.append(i)
            n = n // i

    # If n is a prime number greater than 2
    if n > 2:
        factors.append(n)

    return max(factors)

print(prime_factors(600851475143))
