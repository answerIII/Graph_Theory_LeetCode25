import re
from algorithms import DFS

class Graph:
  def __init__(self, edge_list = None, **attr):
    attr.setdefault("directed", False)
    self._graph = attr
    self._adj = dict()
    self._node = dict()
    if edge_list is not None:
      for u, v in edge_list:
        self.add_edge(u, v)

  def adj(self):
    return self._adj
    
  def nodes(self):
    return self._node
  
  def add_node(self, node, **attr):
    if node not in self._node:
      if node is None:
        raise ValueError("None is not a properly node")
      self._adj[node] = dict()
      self._node[node] = dict()
      self._node[node].update(attr)
    else:
      self._node[node].update(attr)

  def remove_node(self, node):
    try:
      neighbors = self._adj
      del self._node[node]
    except KeyError:
      raise ValueError(f"Node {node} is not in the graph")
    for u in neighbors:
      del self._adj[u][node]
    del self._adj[node]

  def has_node(self, node):
    return node in self._node
  
  def add_edge(self, u, v, **attr):
    if (u not in self._node):
      if u is None:
        raise ValueError("None is not a properly node")
      self._adj[u] = dict()
      self._node[u] = dict()
    if (v not in self._node):
      if v is None:
        raise ValueError("None is not a properly node")
      self._adj[v] = dict()
      self._node[v] = dict()

    if (self._adj[u].get(v) != None):
      print(f"multiEdge {u}-{v}")
    data = self._adj[u].get(v, dict())
    data.update(attr)
    self._adj[u][v] = data
    if (not self._graph['directed'] and u != v):
        self._adj[v][u] = data

  def remove_edge(self, u, v):
    try:
      del self._adj[u][v]
      if (not self._graph['directed']):
        del self._adj[v][u]
    except KeyError:
      raise ValueError(f"Edge {u}-{v} is not in the graph")
  
  def has_edge(self, u, v):
    try:
      return v in self._adj[u]
    except KeyError:
      return False
    
  def inversed(self):
    inversed_graph = Graph(**self._graph)
    for u, nbrdict in self._adj.items():
      for v, attr in nbrdict.items():
        inversed_graph.add_edge(v, u, **attr)
    return inversed_graph

  def degree(self, node):
    return len(self._adj[node])

  def number_of_nodes(self):
    return len(self._node)
  
  def number_of_edges(self):
    return (
      sum(self.degree(u) for u in self._node) 
      // (2 if not self._graph['directed'] else 1)
    )
  
  def density(self):
    return (
      self.number_of_edges()
      * (2 if not self._graph['directed'] else 1)
      / (self.number_of_nodes() * (self.number_of_nodes() - 1))
    )

  @staticmethod
  def from_file(file_name, **attr):
    with open(file_name) as inputFile:
      edge_list = []
      for line in inputFile:
        if line.startswith('#'): continue
        else: 
          edge_list.append(re.split(r'\s',line)[:2])
      return Graph(edge_list=edge_list, **attr)
  
  def find_wcc(self):
    if self._graph['directed']:
      raise ValueError("Can't find WCC in directed graph")
    return DFS(self.nodes(), self.adj())
  
  def find_scc(self):
    if not self._graph['directed']:
      raise ValueError("Can't find SCC in undirected graph")
    tout_dict = dict()
    time = 0

    def on_out(node):
      nonlocal time
      tout_dict[node] = time
      time += 1

    DFS(self.nodes(), self.adj(), on_out=on_out)
    return DFS(
      dict(reversed(tout_dict.items())),
      self.inversed().adj()
    )
    

# graph = Graph.from_file('directed/test.txt', directed=True)

# print(graph.find_scc())