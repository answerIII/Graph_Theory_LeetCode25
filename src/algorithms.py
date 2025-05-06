from typing import Dict, Set, TypeVar, Optional, Callable, List, Any

T = TypeVar('T')

def DFS(
    nodes: Dict[T, Any],
    edge_list: Dict[T, Dict[T, Any]],
    used_set: Set[T] = None,
    on_in: Optional[Callable[[T], None]] = None,
    on_out: Optional[Callable[[T], None]] = None
  ) -> List[List[T]]:
  if (used_set is None):
    used_set = set()
  components = []
  for node in nodes:
    if node not in used_set:
      path = []
      stack = [(node, False)]
      while stack:
        u, is_processed = stack.pop()
        if is_processed:
          if on_out is not None:
            on_out(u)
          path.append(u)
        else:
          if on_in is not None:
            on_in(u)
          used_set.add(u)
          stack.append((u, True))
          for v in edge_list[u]:
            if v not in used_set:
              stack.append((v, False))
      components.append(path)
  return components
