import sympy
from sympy import Matrix, expand

e_1 = Matrix([1, 2, 3]) / sympy.sqrt(14)
e_2 = Matrix([13, -2, -3]) / sympy.sqrt(182)
e_3 = Matrix([0, 3, -2]) / sympy.sqrt(13)

print(e_1)
print(e_2)
print(e_3)

Dot = lambda x, y: expand(x.transpose() * y)[0, 0]

basis = [e_1, e_2, e_3]

for i, e_i in enumerate(basis):
    for j, e_j in enumerate(basis):
        print(f"<e_{i + 1}, e_{j + 1}> = ", end="")
        print(Dot(e_i, e_j))
