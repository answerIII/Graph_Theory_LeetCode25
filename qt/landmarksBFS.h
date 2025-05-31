#ifndef LANDMARKSBFSALGORITHM
#define LANDMARKSBFSALGORITHM

#include "graph.h"
#include <omp.h>

class LandmarksBFS {
	std::vector<std::vector<uint32>> SPTs_;
	std::vector<uint32> landmarks_;

	void BFSPrecompute_(const Graph& graph, uint32 s, uint32 index) {
		std::queue<uint32> q;
		q.push(s);
		uint32 v;
		while (!q.empty()) {
			v = q.front();
			q.pop();
			for (auto u : graph.internalAdj(v)) {
				if (SPTs_[index][u] == Graph::maxVertixNumber) {
					SPTs_[index][u] = v;
					q.push(u);
				}
			}
		}
	}

	uint32 BFSCompute_(const Graph& graph, std::vector<uint32>& dist, uint32 s, uint32 t) const {
		dist[s] = 0;
		std::queue<uint32> q;
		q.push(s);
		uint32 v;
		while (!q.empty()) {
			v = q.front();
			q.pop();
			for (auto u : graph.internalAdj(v)) {
				if (dist[u] == Graph::maxVertixNumber) {
					dist[u] = dist[v] + 1;
					q.push(u);
				}
				if (u == t) {
					return dist[u];
				}
			}
		}
		return dist[t];
	}

	void fillWithPath_(std::unordered_set<uint32>& pathVertices, uint64 w, uint32 index) const {
		if (SPTs_[index][w] == Graph::maxVertixNumber) {
			return;
		}
		uint32 v = w;
		while (v != landmarks_[index]) {
			v = SPTs_[index][v];
			pathVertices.emplace(v);
		}
	}

public:
	/*
	* @param[in] graph: contains graph, which is considered as undirected
	* @param[in] landmarkSelectionMethod: options - LandmarksSelection::availableSelectionMethods
	* @param[in] k: desired number of landmarks, k = min(k, graph.vertixNumber());
	* @param[in] M: desired number of shortest paths will be collected, specify only if best-coverage method selected
	*/
	LandmarksBFS(const Graph& graph, const std::string& landmarkSelectionMethod, uint32 k, uint32 M = 0) :
		SPTs_(k, std::vector<uint32>(graph.vertixNumber(), Graph::maxVertixNumber)) {

		Graph::LandmarksSelection::chooseMethodByString(graph, landmarks_, landmarkSelectionMethod, k, M);

		omp_set_num_threads(omp_get_max_threads());
#pragma omp parallel for
		for (int64 i = 0; i < landmarks_.size(); ++i) {
			BFSPrecompute_(graph, landmarks_[i], i);
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

		std::unordered_set<uint32> pathVertices;
		pathVertices.emplace(s);
		pathVertices.emplace(t);
		for (uint32 i = 0; i < landmarks_.size(); ++i) {
			fillWithPath_(pathVertices, s, i);
			fillWithPath_(pathVertices, t, i);
		}

		std::vector<uint32> dist(graph.vertixNumber(), 0);
		for (auto v : pathVertices) {
			dist[v] = Graph::maxVertixNumber;
		}
		return BFSCompute_(graph, dist, s, t);
	}
};


#endif
