
import numpy as np
import matplotlib.pyplot as plt

MOD_CONSTANT = 19*3*13*7*5*11*17*2

class monkey:
    def __init__(self, items, inspection, throw_rule, p1):
        self.items = items
        self.inspection = inspection
        self.throw_rule = throw_rule
        self.inspection_count = 0
        self.p1 = p1

    def take_turn(self, other_monkeys):
        for item in self.items:
            item = self.inspection(item) % MOD_CONSTANT
            self.inspection_count += 1
            if self.p1:
                item = item // 3
            next_monkey = self.throw_rule(item)
            other_monkeys[next_monkey].items.append(item)
        self.items = []

    def __str__(self):
        return f"{self.inspection_count}: {self.items}"

    def __repr__(self):
        return str(self)


if __name__ == "__main__":
    p1_monkeys = []
    p1_monkeys.append(monkey([85, 77, 77], lambda x: x*7, lambda x: 6 if x % 19 == 0 else 7, True))
    p1_monkeys.append(monkey([80, 99], lambda x: x*11, lambda x: 3 if x % 3 == 0 else 5, True))
    p1_monkeys.append(monkey([74, 60, 74, 63, 86, 92, 80], lambda x: x+8, lambda x: 0 if x % 13 == 0 else 6, True))
    p1_monkeys.append(monkey([71, 58, 93, 65, 80, 68, 54, 71], lambda x: x+7, lambda x: 2 if x % 7 == 0 else 4, True))
    p1_monkeys.append(monkey([97, 56, 79, 65, 58], lambda x: x+5, lambda x: 2 if x % 5 == 0 else 0, True))
    p1_monkeys.append(monkey([77], lambda x: x+4, lambda x: 4 if x % 11 == 0 else 3, True))
    p1_monkeys.append(monkey([99, 90, 84, 50], lambda x: x*x, lambda x: 7 if x % 17 == 0 else 1, True))
    p1_monkeys.append(monkey([50, 66, 61, 92, 64, 78], lambda x: x+3, lambda x: 5 if x % 2 == 0 else 1, True))

    for _ in range(20):
        for monk in p1_monkeys:
            monk.take_turn(p1_monkeys)

    vals = sorted([m.inspection_count for m in p1_monkeys], reverse=True)
    print(vals[0]*vals[1])


    p2_monkeys = []
    p2_monkeys.append(monkey([85, 77, 77], lambda x: x*7, lambda x: 6 if x % 19 == 0 else 7, False))
    p2_monkeys.append(monkey([80, 99], lambda x: x*11, lambda x: 3 if x % 3 == 0 else 5, False))
    p2_monkeys.append(monkey([74, 60, 74, 63, 86, 92, 80], lambda x: x+8, lambda x: 0 if x % 13 == 0 else 6, False))
    p2_monkeys.append(monkey([71, 58, 93, 65, 80, 68, 54, 71], lambda x: x+7, lambda x: 2 if x % 7 == 0 else 4, False))
    p2_monkeys.append(monkey([97, 56, 79, 65, 58], lambda x: x+5, lambda x: 2 if x % 5 == 0 else 0, False))
    p2_monkeys.append(monkey([77], lambda x: x+4, lambda x: 4 if x % 11 == 0 else 3, False))
    p2_monkeys.append(monkey([99, 90, 84, 50], lambda x: x*x, lambda x: 7 if x % 17 == 0 else 1, False))
    p2_monkeys.append(monkey([50, 66, 61, 92, 64, 78], lambda x: x+3, lambda x: 5 if x % 2 == 0 else 1, False))

    for _ in range(10_000):
        for monk in p2_monkeys:
            monk.take_turn(p2_monkeys)

    vals = sorted([m.inspection_count for m in p2_monkeys], reverse=True)
    print(vals[0]*vals[1])