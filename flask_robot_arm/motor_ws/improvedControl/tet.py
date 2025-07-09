def test():
    enable_results = {1: True, 2: True, 3: True, 4: True, 5: True, 6: True}
    return False in enable_results.values()
print(test())