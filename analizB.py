import networkx as nx
import random
import matplotlib.pyplot as plt

def simulate_removal(G, x_values):
    n = G.number_of_nodes()
    results_random = []
    results_targeted = []

    for x in x_values:
        num_remove = int(n * x / 100)
        G_rand = G.copy()
        nodes_to_remove = random.sample(list(G_rand.nodes), num_remove)
        G_rand.remove_nodes_from(nodes_to_remove)
        largest_cc_size = len(max(nx.connected_components(G_rand), key=len))
        results_random.append(largest_cc_size / G_rand.number_of_nodes())
        G_target = G.copy()
        degrees = dict(G_target.degree())
        high_deg_nodes = sorted(degrees, key=degrees.get, reverse=True)[:num_remove]
        G_target.remove_nodes_from(high_deg_nodes)
        largest_cc_size = len(max(nx.connected_components(G_target), key=len))
        results_targeted.append(largest_cc_size / G_target.number_of_nodes())
    return results_random, results_targeted

file_path = 'Email-EuAll.txt'
G = nx.Graph()
with open(file_path, 'r') as f:
    for line in f:
        u, v = map(int, line.strip().split())
        G.add_edge(u, v)
x_values = list(range(0, 55, 5))
random_result, targeted_result = simulate_removal(G, x_values)
plt.plot(x_values, random_result, label="Случайное удаление", marker='o')
plt.plot(x_values, targeted_result, label="Удаление по степени", marker='s')
plt.xlabel("Процент удалённых узлов")
plt.ylabel("Доля в наибольшей компоненте")
plt.title("Устойчивость сети к удалению узлов")
plt.legend()
plt.show()