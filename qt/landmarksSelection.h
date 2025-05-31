#ifndef LANDMARKSSELECTION
#define LANDMARKSSELECTION

#include "undirectedGraph.h"
#include <numeric>
#include <algorithm>
#include <random>
#include <queue>


class LandmarksSelection {
	static inline int64 partition(const UndirectedGraph& graph, std::vector<uint32>& v, int64 leftEdge, int64 rightEdge) {
		int64 left = leftEdge;
		int64 right = rightEdge;
		std::swap(v[left + rand() % (right - left + 1)], v[right]);
		uint64 pivot = graph.internalDegree(v[right]);
		--right;
		while (left <= right) {
			while (left <= rightEdge && graph.internalDegree(v[left]) > pivot) { ++left; }
			while (right >= leftEdge && graph.internalDegree(v[right]) < pivot) { --right; }
			if (left <= right) {
				std::swap(v[left++], v[right--]);
			}
		}
		std::swap(v[left], v[rightEdge]);

		return left;
	}

	static void kth_element(const UndirectedGraph& graph, std::vector<uint32>& v, uint32 k) {
		int64 pivotPos;
		int64 leftEdge = 0;
		int64 rightEdge = v.size() - 1;
		while (leftEdge <= rightEdge) {
			pivotPos = partition(graph, v, leftEdge, rightEdge);
			if (pivotPos > k) {
				rightEdge = pivotPos - 1;
			}
			else if (pivotPos == k) {
				break;
			}
			else {
				leftEdge = pivotPos + 1;
			}
		}
	}

	static void shortestPath_(const UndirectedGraph& graph, std::vector<uint32>& path, uint32 s, uint32 t) {
		std::vector<uint32> p(graph.vertixNumber(), UndirectedGraph::maxVertixNumber);
		std::queue<uint32> q;
		uint32 v;
		q.push(s);
		while (!q.empty()) {
			v = q.front();
			q.pop();
			for (auto u : graph.internalAdj(v)) {
				if (p[u] == UndirectedGraph::maxVertixNumber) {
					p[u] = v;
					q.push(u);
				}
				if (u == t) {
					break;
				}
			}
		}
		if (p[t] != UndirectedGraph::maxVertixNumber) {
			v = t;
			while (v != s) {
				path.push_back(v);
				v = p[v];
			}
			path.push_back(v);
		}
	}

public:

	inline static const std::vector<std::string> availableSelectionMethods = { "highest-degree", "best-coverage", "random" };

	/*
	* @param[in] graph: contains graph, which is considered as undirected
	* @param[out] landmarks: arbitrary vector of any size and elements in it, 
	which will be filled by k landmarks with internal vertix indices
	* @param[in] k: desired number of landmarks, if k == 0, landmarks.size() == 0 guaranteed, k = min(k, graph.vertixNumber());
	*/
	static void highestDegreeSelection(const UndirectedGraph& graph, std::vector<uint32>& landmarks, uint32 k) {
		k = std::min(k, graph.vertixNumber());
		if (k == 0) {
			landmarks.resize(0);
			return;
		}
		landmarks.resize(graph.vertixNumber());
		std::iota(landmarks.begin(), landmarks.end(), 0);
		kth_element(graph, landmarks, k - 1);
		landmarks.resize(k);
	}


	/*
	* @param[in] graph: contains graph, which is considered as undirected
	* @param[out] landmarks: arbitrary vector of any size and elements in it, 
	which will be filled by k landmarks with internal vertix indices
	* @param[in] M: desired number of shortest paths will be collected, if M == 0, highestDegreeSelection applied instead
	* @param[in] k: desired number of landmarks, if k == 0, landmarks.size() == 0 guaranteed, k = min(k, graph.vertixNumber());
	* @return actual number of landmarks been inserted using shortest paths, the rest vertices were inserted by highestDegreeSelection
	*/
	static uint32 bestCoverageSelection(const UndirectedGraph& graph, std::vector<uint32>& landmarks, uint32 M, uint32 k) {
		k = std::min(k, graph.vertixNumber());
		if (k == 0) {
			landmarks.resize(0);
			return 0;
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32> dist(0, graph.vertixNumber() - 1);

		std::vector<uint32> s_path;
		std::vector<std::vector<uint32>> paths;

		uint32 s;
		uint32 t;
		for (uint32 i = 0; i < M; ++i) { // may be parallelized
			s = dist(gen);
			t = dist(gen);
			if (s == t) {
				t = (t + 1) % graph.vertixNumber();
			}
			shortestPath_(graph, s_path, s, t); // O(n + m)
			if (!s_path.empty()) {
				paths.push_back(s_path);
			}
			s_path.resize(0);
		}

		std::unordered_map<uint32, std::unordered_set<uint32>> pathIndices;
		for (uint32 i = 0; i < paths.size(); ++i) {
			for (auto v : paths[i]) {
				pathIndices[v].emplace(i);
			}
		}

		uint32 maxValue;
		uint32 maxVertex;
		landmarks.resize(0);
		for (uint32 i = 0; i < k; ++i) {
			maxValue = 0;
			for (auto& pair : pathIndices) { // O(DM)
				if (maxValue < pair.second.size()) {
					maxValue = pair.second.size();
					maxVertex = pair.first;
				}
			}
			if (maxValue == 0) { // O(klogk + n) (2 ptrs), fill with maxDegree values
				std::vector<uint32> vertices;
				std::sort(landmarks.begin(), landmarks.end());
				uint32 landmark_ptr = 0;
				uint32 value = 0;
				while (landmark_ptr < landmarks.size() && value < graph.vertixNumber()) {
					if (value < landmarks[landmark_ptr]) {
						vertices.push_back(value);
					}
					else {
						++landmark_ptr;
					}
					++value;
				}
				while (value < graph.vertixNumber()) {
					vertices.push_back(value);
					++value;
				}
				uint32 rest = k - landmarks.size();
				kth_element(graph, vertices, rest - 1);
				for (uint32 j = 0; j < rest; ++j) {
					landmarks.push_back(vertices[j]);
				}
				return landmark_ptr;
			}
			landmarks.push_back(maxVertex);
			for (auto pathIndex : pathIndices[maxVertex]) { // O(DM)
				for (auto v : paths[pathIndex]) {
					if (v != maxVertex) {
						pathIndices[v].erase(pathIndex);
					}
				}
			}
			pathIndices[maxVertex].clear();
		}
		return landmarks.size();
	}

	/*
	* @param[in] graph: contains graph, which is considered as undirected
	* @param[out] landmarks: landmarks: arbitrary vector of any size and elements in it, 
	which will be filled by k landmarks with internal vertix indices, k = min(k, graph.vertixNumber());
	* @param[in] k: desired number of landmarks, if k == 0, landmarks.size() == 0 guaranteed
	*/
	static void randomSelection(const UndirectedGraph& graph, std::vector<uint32>& landmarks, uint32 k) {
		k = std::min(k, graph.vertixNumber());
		if (k == 0) {
			landmarks.resize(0);
			return;
		}
		landmarks.resize(graph.vertixNumber());
		std::random_device rd;
		std::mt19937 gen(rd());
		std::iota(landmarks.begin(), landmarks.end(), 0);
		std::shuffle(landmarks.begin(), landmarks.end(), gen);
		landmarks.resize(k);
	}

	/*
	* @param[in] graph: contains graph, which is considered as undirected
	* @param[in] landmarkSelectionMethod: options - LandmarksSelection::availableSelectionMethods
	* @param[in] k: desired number of landmarks
	* @param[in] M: desired number of shortest paths will be collected, specify only if best-coverage method selected
	*/
	static void chooseMethodByString(const UndirectedGraph& graph, std::vector<uint32>& landmarks, 
		const std::string& landmarkSelectionMethod, uint32 k, uint32 M) {

		if (!landmarkSelectionMethod.compare(availableSelectionMethods[0])) {
			highestDegreeSelection(graph, landmarks, k);
		}
		else if (!landmarkSelectionMethod.compare(availableSelectionMethods[1])) {
			bestCoverageSelection(graph, landmarks, M, k);
		}
		else if (!landmarkSelectionMethod.compare(availableSelectionMethods[2])) {
			randomSelection(graph, landmarks, k);
		}
		else {
			throw std::exception("Unknown landmark selection method, available options: LandmarksSelection::availableSelectionMethods");
		}

	}
};


#endif
