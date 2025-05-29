from typing import Tuple, Set, List, Dict

from bfs import createSPTBFS


def createSPTList(
    undir_adj_list: Tuple[Set[int], ...], landmark_lst: List[int]
) -> List[Dict[int, int]]:
    """
    Create list of shortest path trees where spt_lst[i] is SPT for
    landmark_lst[i]
    """
    spt_lst: List[Dict[int, int]] = []
    for landmark_ind in landmark_lst:
        spt_lst.append(createSPTBFS(undir_adj_list, landmark_ind))
    return spt_lst
