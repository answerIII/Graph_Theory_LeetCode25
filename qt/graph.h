#ifndef GraphDATASTRUCTURE
#define GraphDATASTRUCTURE


#include "fastReader.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>
#include <queue>
#include <numeric>
#include <algorithm>
#include <random>
#include <climits>

#include <stack>
#include <map>

class Graph {
	std::vector<std::vector<uint32>> graph_;
	std::unordered_map<uint64, uint32> renaming_;
	uint32 currentRenamingId_;
	bool isDirected_;

	///////////////////////////////////////////////
	uint32 max_WCC_size = 0;
	std::vector<uint32> sets;
	uint32 max_WCC_id = 0;
	///////////////////////////////////////////////

	void doRenaming(uint64& vertexId) {
		auto res = renaming_.emplace(vertexId, currentRenamingId_);
		vertexId = res.second ? currentRenamingId_++ : res.first->second;
	}

	class LandmarksSelection {
		static inline int64 partition(const Graph& graph, std::vector<uint32>& v, int64 leftEdge, int64 rightEdge) {
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

		static void kth_element(const Graph& graph, std::vector<uint32>& v, uint32 k) {
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

		static void shortestPath_(const Graph& graph, std::vector<uint32>& path, uint32 s, uint32 t) {
			std::vector<uint32> p(graph.vertixNumber(), Graph::maxVertixNumber);
			std::queue<uint32> q;
			uint32 v;
			q.push(s);
			while (!q.empty()) {
				v = q.front();
				q.pop();
				for (auto u : graph.internalAdj(v)) {
					if (p[u] == Graph::maxVertixNumber) {
						p[u] = v;
						q.push(u);
					}
					if (u == t) {
						break;
					}
				}
			}
			if (p[t] != Graph::maxVertixNumber) {
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
		static void highestDegreeSelection(const Graph& graph, std::vector<uint32>& landmarks, uint32 k) {
			k = std::min(k, graph.vertixNumber());
			if (k == 0) {
				landmarks.resize(0);
				return;
			}
			landmarks.resize(graph.vertixNumber());
			std::iota(landmarks.begin(), landmarks.end(), 0);
			kth_element(graph, landmarks, k - 1);
			std::nth_element(landmarks.begin(), landmarks.begin() + k - 1, landmarks.end(), [&graph](uint32 i, uint32 j) {return graph.internalDegree(i) > graph.internalDegree(j); });
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
		static uint32 bestCoverageSelection(const Graph& graph, std::vector<uint32>& landmarks, uint32 M, uint32 k) {
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
		static void randomSelection(const Graph& graph, std::vector<uint32>& landmarks, uint32 k) {
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

		static void chooseMethodByString(const Graph& graph, std::vector<uint32>& landmarks,
			const std::string& landmarkSelectionMethod, uint32 k, uint32 M) {

			if (!landmarkSelectionMethod.compare(LandmarksSelection::availableSelectionMethods[0])) {
				LandmarksSelection::highestDegreeSelection(graph, landmarks, k);
			}
			else if (!landmarkSelectionMethod.compare(LandmarksSelection::availableSelectionMethods[1])) {
				LandmarksSelection::bestCoverageSelection(graph, landmarks, M, k);
			}
			else if (!landmarkSelectionMethod.compare(LandmarksSelection::availableSelectionMethods[2])) {
				LandmarksSelection::randomSelection(graph, landmarks, k);
			}
			else {
				throw std::runtime_error("Unknown landmark selection method, available options: LandmarksSelection::availableSelectionMethods");
			}
		}

	};
	
	friend class LandmarksBasic;
	friend class LandmarksBFS;

public:
	static inline const uint32 maxVertixNumber = UINT32_MAX;

	Graph(const std::string& path, bool ignoreDirection = false) : currentRenamingId_(0) {
		FastGraphReader reader(path);
		uint64 vertices;
		uint64 edges;
		char direction;
		reader.parseFirstLine(vertices, edges, direction);
		isDirected_ = (direction == 'd' && !ignoreDirection);

		if (vertices > maxVertixNumber) {
			throw std::runtime_error(("The number of vertices is limited by " + std::to_string(maxVertixNumber)).c_str());
		}
		graph_.resize(vertices);
		renaming_.reserve(vertices);

		uint64 fromId;
		uint64 toId;
		for (uint64 i = 0; i < edges; ++i) {
			fromId = reader.parseInt();
			toId = reader.parseInt();
			doRenaming(fromId);
			doRenaming(toId);
			graph_[fromId].push_back(toId);
			if (!isDirected_) {
				graph_[toId].push_back(fromId);
			}
		}

		std::vector<uint32>::iterator it;
		for (uint64 i = 0; i < graph_.size(); ++i) {
			std::sort(graph_[i].begin(), graph_[i].end());
			it = std::unique(graph_[i].begin(), graph_[i].end());
			graph_[i].resize(std::distance(graph_[i].begin(), it));
		}
	}

    bool isDirected() const {
        return isDirected_;
    }

	uint32 vertixNumber() const {
		return graph_.size();
	}

	uint32 internalVertixName(uint64 v) const {
		auto it = renaming_.find(v);
		if (it == renaming_.end()) {
			return maxVertixNumber;
		}
		return it->second;
	}

	const std::vector<uint32>& internalAdj(uint32 v) const {
		return graph_[v];
	}

	uint64 internalDegree(uint32 v) const {
		return graph_[v].size();
	}

	uint32 findShortestPath(uint32 s, uint32 t, bool internalVertixRepresentationPassed = false) const {
		if (!internalVertixRepresentationPassed) {
			s = internalVertixName(s);
			t = internalVertixName(t);
			if (s == Graph::maxVertixNumber || t == Graph::maxVertixNumber) {
				throw std::runtime_error("One of the vertices (s, t) doesn't exist.");
			}
		}
		else {
			if (s >= vertixNumber() || t >= vertixNumber()) {
				throw std::runtime_error("One of the vertices (s, t) doesn't exist.");
			}
		}

		std::vector<uint32> dist(vertixNumber(), maxVertixNumber);
		dist[s] = 0;
		std::queue<uint32> q;
		q.push(s);
		uint32 v;
		while (!q.empty()) {
			v = q.front();
			q.pop();
			for (auto u : graph_[v]) {
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

	////////////////////////////////////////////////////////////////////////////////////////
	void printAdjacencyList() {
		for (size_t i = 0; i < graph_.size(); ++i) {
			std::cout << i << ": ";
			for (size_t j = 0; j < graph_[i].size(); ++j) {
				std::cout << graph_[i][j];
				if (j != graph_[i].size() - 1) {
					std::cout << " ";
				}
			}
			std::cout << "\n";
		}
	}

	void DFS(uint32 node, const std::vector<std::vector<uint32>>& adj_dir,
		std::vector<bool>& visited, uint32& visited_counter,
		std::vector<uint32>& ordered) {
		std::stack<std::pair<uint32, bool>> stack;
		stack.push({ node, false });

		while (!stack.empty()) {
			auto [current, processed] = stack.top();
			stack.pop();

			if (processed) {
				ordered[visited_counter] = current;
				++visited_counter;
				continue;
			}

			if (visited[current]) continue;
			visited[current] = true;

			stack.push({ current, true });

			for (auto it = adj_dir[current].rbegin(); it != adj_dir[current].rend(); ++it) {
				if (!visited[*it]) {
					stack.push({ *it, false });
				}
			}
		}
	}
	void reverse_list(const std::vector<std::vector<uint32>>& adj_dir, std::vector<std::vector<uint32>>& adj_dir_reverse) {
		for (size_t i = 0; i < adj_dir.size(); ++i) {
			for (size_t j = 0; j < adj_dir[i].size(); ++j) {
				adj_dir_reverse[adj_dir[i][j]].push_back(i);
			}
		}
	}

	void DFS_reverse(uint32 node, const std::vector<std::vector<uint32>>& adj_dir_reverse,
		std::vector<bool>& visited, uint32& cur_size_dir,
		uint32& num_of_sets_dir, std::vector<uint32>& sets_dir) {
		std::stack<uint32> stack;
		stack.push(node);

		while (!stack.empty()) {
			uint32 current = stack.top();
			stack.pop();

			if (visited[current]) continue;
			visited[current] = true;
			sets_dir[current] = num_of_sets_dir;
			++cur_size_dir;

			for (uint32 neighbor : adj_dir_reverse[current]) {
				if (!visited[neighbor]) {
					stack.push(neighbor);
				}
			}
		}
	}
	void find_SCC(uint32& max_size_dir, uint32& num_of_sets_dir, std::vector<uint32>& sets_dir, const size_t nodes, const std::vector<std::vector<uint32>>& adj_dir_reverse, const std::vector<uint32>& ordered, std::vector<bool>& visited) {
		for (uint32 i = 0; i < ordered.size(); ++i) {
			if (!visited[ordered[i]]) {
				uint32 cur_size = 0;
				DFS_reverse(ordered[i], adj_dir_reverse, visited, cur_size, num_of_sets_dir, sets_dir);
				++num_of_sets_dir;
				if (max_size_dir < cur_size) {
					max_size_dir = cur_size;
				}
			}
		}
	}

	struct AnalyzeSCCResponse {
		uint32 num_of_sets_dir;
		uint32 max_size_dir;
		double part;
	};

	AnalyzeSCCResponse Analyze_SCC() {
		uint32 max_size_dir = 0;
		uint32 num_of_sets_dir = 0;
		std::vector<uint32> sets_dir(graph_.size());
		std::vector<bool> visited(graph_.size());
		uint32 visited_counter = 0;
		std::vector<uint32> ordered(graph_.size());
		std::vector<std::vector<uint32>> adj_dir_reverse(graph_.size());

		for (uint32 i = 0; i < graph_.size(); ++i) {
			if (!visited[i]) {
				DFS(i, graph_, visited, visited_counter, ordered);
			}
		}
		for (uint32 i = 0; i < graph_.size(); ++i) {
			visited[i] = false;
		}
		reverse_list(graph_, adj_dir_reverse);
		reverse(ordered.begin(), ordered.end());

		find_SCC(max_size_dir, num_of_sets_dir, sets_dir, graph_.size(), adj_dir_reverse, ordered, visited);

		return AnalyzeSCCResponse {
			num_of_sets_dir,
			max_size_dir,
			static_cast<double>(max_size_dir) / graph_.size()
		};
	}

	struct AnalyzeWCCResponse {
		uint32 num_of_sets;
		uint32 max_WCC_size;
		double part;
	};

	AnalyzeWCCResponse Analyze_WCC() {
		max_WCC_size = 0;
		uint32 num_of_sets = 0;

		sets.resize(graph_.size());
		std::queue<uint32> q;
		uint32 cur_size = 0;
		for (size_t i = 0; i < graph_.size(); ++i) {
			sets[i] = 0;
		}
		for (size_t i = 0; i < graph_.size(); ++i) {
			if (sets[i] == 0) {
				++num_of_sets;
				sets[i] = num_of_sets;
				q.push(i);
				++cur_size;
				uint32 current_node;
				while (!q.empty()) {
					current_node = q.front();
					for (int i = 0; i < graph_[current_node].size(); ++i) {
						if (sets[graph_[current_node][i]] == 0) {
							sets[graph_[current_node][i]] = num_of_sets;
							q.push(graph_[current_node][i]);
							++cur_size;
						}
					}
					q.pop();
				}
				if (max_WCC_size < cur_size) {
					max_WCC_id = num_of_sets;
					max_WCC_size = cur_size;
				}
				cur_size = 0;
			}
		}

		return AnalyzeWCCResponse{
			num_of_sets,
			max_WCC_size,
			static_cast<double>(max_WCC_size) / graph_.size()
		};
	}

	double LocalClusteringCoefficient(uint32 u, uint32& triangles_total) {
		const uint32 ku = graph_[u].size();
		if (ku < 2) return 0.0;

		uint32 triangles = 0;
		const auto& neighbors = graph_[u];

		for (uint32 i = 0; i < neighbors.size(); ++i) {
			uint32 v = neighbors[i];
			for (uint32 j = i + 1; j < neighbors.size(); ++j) {
				uint32 w = neighbors[j];
				if (binary_search(graph_[v].begin(), graph_[v].end(), w)) {
					++triangles;
				}
			}
		}
		triangles_total += triangles;
		const double max_possible = ku * (ku - 1) / 2.0;
		return triangles / max_possible;
	}

	struct AverageClusteringCoefficientResponse {
		uint32 triangles_total;
		double average_coefficient;
		double WCC_coefficient;
	};

	AverageClusteringCoefficientResponse AverageClusteringCoefficient() {
		for (auto& neighbors : graph_) {
			sort(neighbors.begin(), neighbors.end());
		}

		double total = 0.0;
		const uint32 N = graph_.size();

		double total_WCC = 0.0;
		uint32 triangles_total = 0;
		for (uint32 u = 0; u < N; ++u) {
			const double coeff = LocalClusteringCoefficient(u, triangles_total);
			total += coeff;
			if (sets[u] == max_WCC_id) {
				total_WCC += coeff;
			}
		}
		double WCC_coefficient = 0;
		double average_coefficient = 0;

		if (N != 0) {
			WCC_coefficient = total_WCC / max_WCC_size;
			average_coefficient = total / N;
		}
		triangles_total /= 3;

		return AverageClusteringCoefficientResponse {
			triangles_total,
			average_coefficient,
			WCC_coefficient,
		};
	}

	double GlobalClusteringCoefficient() {
		for (auto& neighbors : graph_) {
			sort(neighbors.begin(), neighbors.end());
		}

		uint32 triangles = 0;
		uint32 connectedTriples = 0;
		const uint32 N = graph_.size();

		for (uint32 u = 0; u < N; ++u) {
			uint32 ku = graph_[u].size();
			connectedTriples += ku * (ku - 1) / 2;

			for (uint32 v : graph_[u]) {
				if (v <= u) continue;
				std::vector<uint32> commonNeighbors;
				set_intersection(
					graph_[u].begin(), graph_[u].end(),
					graph_[v].begin(), graph_[v].end(),
					back_inserter(commonNeighbors)
				);
				triangles += commonNeighbors.size();
			}
		}
		double global_coefficient = 0;
		if (connectedTriples > 0) {
			global_coefficient = (3.0 * triangles) / connectedTriples;
		}
		return global_coefficient;
	}

	uint32 find_furthest(uint32 node_id, uint32& dist) {
		uint32 furthest_id = 0;

		uint32 sz = graph_.size();
		std::vector<uint32> level(sz);
		std::queue<uint32> q;
		q.push(node_id);
		for (uint32 i = 0; i < sz; ++i) {
			level[i] = -1;
		}
		level[node_id] = 0;
		while (!q.empty()) {
			uint32 cur_node = q.front();
			uint32 cur_distance = level[cur_node];
			for (uint32 i = 0; i < graph_[cur_node].size(); ++i) {
				if (level[graph_[cur_node][i]] == -1) {
					level[graph_[cur_node][i]] = cur_distance + 1;
					q.push(graph_[cur_node][i]);
					furthest_id = graph_[cur_node][i];
				}
			}
			q.pop();
		}

		dist = level[furthest_id];
		return furthest_id;
	}

	uint32 Analyze_diameter() {
		static std::random_device rd;
		static std::mt19937_64 gen(rd());
		std::uniform_int_distribution<uint32> dist(0, max_WCC_size);
		uint32 node_num = dist(gen);
		uint32 in_set_counter = 0;
		uint32 node_id = 0;
		uint32 diameter = 0;



		for (uint32 i = 0; i < sets.size(); ++i) {
			if (sets[i] == max_WCC_id) {
				if (in_set_counter == node_num) {
					node_id = i;
					break;
				}
				++in_set_counter;
			}
		}
		node_id = find_furthest(node_id, diameter);
		node_id = find_furthest(node_id, diameter);
		return diameter;
	}
	uint32 shortest_path(uint32 node_1, uint32 node_2) {
		uint32 furthest_id = 0;

		uint32 sz = graph_.size();
		std::vector<uint32> level(sz);
		std::queue<uint32> q;
		q.push(node_1);
		for (uint32 i = 0; i < sz; ++i) {
			level[i] = -1;
		}
		level[node_1] = 0;
		while (!q.empty()) {
			uint32 cur_node = q.front();
			uint32 cur_distance = level[cur_node];
			for (uint32 i = 0; i < graph_[cur_node].size(); ++i) {
				if (level[graph_[cur_node][i]] == -1) {
					level[graph_[cur_node][i]] = cur_distance + 1;
					if (graph_[cur_node][i] == node_2) {
						return level[graph_[cur_node][i]];
					}
					q.push(graph_[cur_node][i]);
					furthest_id = graph_[cur_node][i];
				}
			}
			q.pop();
		}
		std::cout << "path not found\n";
		return 0;
	}
	std::vector<std::pair<uint32_t, uint32_t>> getRandomPairs(uint32_t numPairs = 500) {
		if (max_WCC_size < 2) return {};

		std::vector<std::pair<uint32_t, uint32_t>> pairs;
		std::unordered_set<uint64_t> pairSet;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> dist(0, max_WCC_size - 1);

		while (pairs.size() < numPairs) {
			uint32_t u = dist(gen);
			uint32_t v = dist(gen);
			if (u == v) continue;

			uint64_t packed = (u < v)
				? (static_cast<uint64_t>(u) << 32) | v
				: (static_cast<uint64_t>(v) << 32) | u;

			if (pairSet.insert(packed).second) {
				pairs.emplace_back(u, v);
			}
		}
		return pairs;
	}

	uint32 Analyze_diameter_b() {

		std::vector<uint32> WCC_nodes;
		std::vector<uint32> dist;

		for (uint32 i = 0; i < sets.size(); ++i) {
			if (sets[i] == max_WCC_id) {
				WCC_nodes.push_back(i);
			}
		}
		std::vector<std::pair<uint32_t, uint32_t>> pairs = getRandomPairs();
		for (uint32 i = 0; i < pairs.size(); ++i) {
			uint32 node1 = WCC_nodes[pairs[i].first];
			uint32 node2 = WCC_nodes[pairs[i].second];
			dist.push_back(shortest_path(node1, node2));
		}


		sort(dist.begin(), dist.end());
		uint32 index90 = (uint32)(0.9 * dist.size());
		uint32 diameter_b = dist[index90];

		return diameter_b;
	}
	std::vector<std::vector<uint32>> cut_graph(const std::vector<uint32>& nodesToRemove) {
		std::vector<std::vector<uint32>> cut_graph = graph_;
		std::unordered_set<uint32> nodesToRemoveSet(nodesToRemove.begin(), nodesToRemove.end());

		for (uint32 node : nodesToRemoveSet) {
			if (node < cut_graph.size()) {
				cut_graph[node].clear();
			}
		}

		for (auto& neighbors : cut_graph) {
			neighbors.erase(
				std::remove_if(neighbors.begin(), neighbors.end(),
					[&nodesToRemoveSet](uint32_t node) {
						return nodesToRemoveSet.count(node) > 0;
					}),
				neighbors.end()
			);
		}
		return cut_graph;
	}
	void snowball_distances(uint32 node_id, std::vector<std::vector<uint32>> snowball_graph, std::vector<uint32>& distances) {
		if (snowball_graph[node_id].size() == 0) {
			return;
		}

		uint32 sz = snowball_graph.size();
		std::vector<uint32> level(sz);
		std::queue<uint32> q;
		q.push(node_id);
		for (uint32 i = 0; i < sz; ++i) {
			level[i] = -1;
		}
		level[node_id] = 0;
		while (!q.empty()) {
			uint32 cur_node = q.front();
			uint32 cur_distance = level[cur_node];
			for (uint32 i = 0; i < snowball_graph[cur_node].size(); ++i) {
				if (level[snowball_graph[cur_node][i]] == -1) {
					level[snowball_graph[cur_node][i]] = cur_distance + 1;
					if (node_id < snowball_graph[cur_node][i]) {
						distances.push_back(cur_distance + 1);
					}
					q.push(snowball_graph[cur_node][i]);
				}
			}
			q.pop();
		}
	}
	std::vector<uint32> make_snowball(uint32 node_id, uint32 snowball_sz = 500) {
		std::vector<uint32> snowball;
		uint32 counter = 0;

		uint32 sz = graph_.size();
		std::vector<uint32> level(sz);
		std::queue<uint32> q;
		q.push(node_id);
		snowball.push_back(node_id);
		++counter;
		for (uint32 i = 0; i < sz; ++i) {
			level[i] = -1;
		}
		level[node_id] = 0;
		while (!q.empty()) {
			uint32 cur_node = q.front();
			uint32 cur_distance = level[cur_node];
			for (uint32 i = 0; i < graph_[cur_node].size(); ++i) {
				if (level[graph_[cur_node][i]] == -1) {
					level[graph_[cur_node][i]] = cur_distance + 1;
					q.push(graph_[cur_node][i]);
					snowball.push_back(graph_[cur_node][i]);
					++counter;
					if (counter == 500) {
						return snowball;
					}
				}
			}
			q.pop();
		}
		return snowball;
	}
	uint32 Analyze_diameter_c() {
		static std::random_device rd;
		static std::mt19937_64 gen(rd());
		std::uniform_int_distribution<uint32> dist(0, max_WCC_size);
		uint32 node_num = dist(gen);
		uint32 in_set_counter = 0;
		uint32 node_id = 0;
		uint32 diameter = 0;



		for (uint32 i = 0; i < sets.size(); ++i) {
			if (sets[i] == max_WCC_id) {
				if (in_set_counter == node_num) {
					node_id = i;
					break;
				}
				++in_set_counter;
			}
		}

		std::vector<uint32> snowball = make_snowball(node_id);
		std::vector<uint32> distances;

		std::vector<uint32> to_remove;
		for (uint32 i = 0; i < graph_.size(); ++i) {
			if ((std::find(snowball.begin(), snowball.end(), i) == snowball.end())) {
				to_remove.push_back(i);
			}
		}

		std::vector<std::vector<uint32>> cut = cut_graph(to_remove);
		//std::cout << cut.size();
		for (uint32 i = 0; i < cut.size(); ++i) {
			snowball_distances(i, cut, distances);
		}

		sort(distances.begin(), distances.end());
		uint32 index90 = (uint32)(0.9 * distances.size());
		uint32 diameter_c = distances[index90];

		return diameter_c;

	}

	struct AnanlyzeDegreesResponse {
		unsigned long long minDegree;
		unsigned long long maxDegree;
		double avgDegree;
		std::map<unsigned long long, unsigned long long> degreeCount;
	};
	AnanlyzeDegreesResponse Analyze_degrees() {

		size_t numNodes = graph_.size();
		unsigned long long minDegree = ULLONG_MAX;
		unsigned long long maxDegree = 0;
		double totalDegree = 0.0;

		std::map<unsigned long long, unsigned long long> degreeCount;

		for (const auto& neighbors : graph_) {
			unsigned long long degree = neighbors.size();

			if (degree < minDegree) minDegree = degree;
			if (degree > maxDegree) maxDegree = degree;
			totalDegree += degree;

			degreeCount[degree]++;
		}

		double avgDegree = totalDegree / numNodes;

		return AnanlyzeDegreesResponse {
			minDegree,
			maxDegree,
			avgDegree,
			std::move(degreeCount)
		};

		/*std::cout << "Min degree: " << minDegree << std::endl;
		std::cout << "Max degree: " << maxDegree << std::endl;
		std::cout << "Average degree: " << avgDegree << std::endl;

		std::cout << "\nDegree distribution (normal scale):\n";
		std::cout << "Degree\tCount\tProbability\n";
		for (const auto& entry : degreeCount) {
			double probability = static_cast<double>(entry.second) / numNodes;
			std::cout << entry.first << "\t" << entry.second << "\t" << probability << std::endl;
		}

		std::cout << "\nDegree distribution (log-log scale):\n";
		std::cout << "log(Degree)\tlog(Probability)\n";
		for (const auto& entry : degreeCount) {
			if (entry.first == 0) continue;
			double probability = static_cast<double>(entry.second) / numNodes;
			if (probability <= 0) continue;
			std::cout << log10(entry.first) << "\t" << log10(probability) << std::endl;
		}*/
	}
	std::vector<uint32> generateUniqueRandomNumbers(uint32_t N) {
		std::vector<uint32> numbers(graph_.size());
		std::iota(numbers.begin(), numbers.end(), 0);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::ranges::shuffle(numbers, gen);

		numbers.resize(N);
		return numbers;
	}

	void Random_cut(int percentage) {
		uint32 num_to_cut = (graph_.size() * percentage)/100;
		std::vector<uint32> to_remove = generateUniqueRandomNumbers(num_to_cut);
		std::vector<std::vector<uint32>> cut = cut_graph(to_remove);
		graph_ = cut;
	}

	std::vector<uint32> Max_cut_nodes(uint32_t N) {

		std::vector<std::pair<uint32_t, uint32_t>> degreeNodePairs;

		for (uint32_t node = 0; node < graph_.size(); ++node) {
			uint32_t degree = static_cast<uint32_t>(graph_[node].size());
			degreeNodePairs.emplace_back(degree, node);
		}

		std::sort(degreeNodePairs.begin(), degreeNodePairs.end(), [](const auto& a, const auto& b) { return a.first > b.first; });

		std::vector<uint32_t> result;
		for (uint32_t node = 0; node < N; ++node) {
			result.push_back(degreeNodePairs[node].second);
		}

		return result;
	}

	void Max_cut(int percentage) {
		uint32 num_to_cut = (graph_.size() * percentage) / 100;
		std::vector<uint32> to_remove = Max_cut_nodes(num_to_cut);
		std::vector<std::vector<uint32>> cut = cut_graph(to_remove);
		graph_ = cut;
	}

	////////////////////////////////////////////////////////////////////////////////////////
};





#endif
