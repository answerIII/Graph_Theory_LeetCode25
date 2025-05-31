#ifndef LANDMARKSBASICALGORITHM
#define LANDMARKSBASICALGORITHM


#include "graph.h"
#include <omp.h>


class LandmarksBasic {
	std::vector<std::vector<uint32>> distances_;

	void BFSPrecompute_(const Graph& graph, uint32 s, uint32 index) {
		distances_[index][s] = 0;
		std::queue<uint32> q;
		q.push(s);
		uint32 v;
		while (!q.empty()) {
			v = q.front();
			q.pop();
			for (auto u : graph.internalAdj(v)) {
				if (distances_[index][u] == Graph::maxVertixNumber) {
					distances_[index][u] = distances_[index][v] + 1;
					q.push(u);
				}
			}
		}
	}

public:
	/*
	* @param[in] graph: contains graph, which is considered as undirected
	* @param[in] landmarkSelectionMethod: options - LandmarksSelection::availableSelectionMethods
	* @param[in] k: desired number of landmarks, k = min(k, graph.vertixNumber());
	* @param[in] M: desired number of shortest paths will be collected, specify only if best-coverage method selected
	*/
	LandmarksBasic(const Graph& graph, const std::string& landmarkSelectionMethod, uint32 k, uint32 M = 0) :
		distances_(k, std::vector<uint32>(graph.vertixNumber(), Graph::maxVertixNumber)) {

		std::vector<uint32> landmarks;
		Graph::LandmarksSelection::chooseMethodByString(graph, landmarks, landmarkSelectionMethod, k, M);
		

		omp_set_num_threads(omp_get_max_threads());
#pragma omp parallel for
		for (int64 i = 0; i < landmarks.size(); ++i) {
			BFSPrecompute_(graph, landmarks[i], i);
		}
	}

	uint32 computeApproximateDistance(const Graph& graph, uint64 s, uint64 t, bool internalVertixRepresentationPassed = false) const {
		if (s == t) {
			return 0;
		}
		if (!internalVertixRepresentationPassed) {
			s = graph.internalVertixName(s);
			t = graph.internalVertixName(t);
			if (s == Graph::maxVertixNumber || t == Graph::maxVertixNumber) {
				throw std::runtime_error("One of the vertices (s, t) doesn't exist.");
			}
		}
		else {
			if (s >= graph.vertixNumber() || t >= graph.vertixNumber()) {
				throw std::runtime_error("One of the vertices (s, t) doesn't exist.");
			}
		}

		uint32 minDist = Graph::maxVertixNumber;
		uint32 currentDist;
		for (uint32 i = 0; i < distances_.size(); ++i) {
			if (distances_[i][s] < Graph::maxVertixNumber &&
				distances_[i][t] < Graph::maxVertixNumber) {
				currentDist = distances_[i][s] + distances_[i][t];
				if (minDist > currentDist) {
					minDist = currentDist;
				}
			}
		}
		return minDist;
	}
};


#endif
