#include <climits>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>

struct Edge {
    int64_t to;
    int weight;  // synapses cnt
};

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> fields;
    std::string current_string = "";
    for (const auto& c : line) {
        if (c == ',') {
            fields.emplace_back(current_string);
            current_string = "";
        } else {
            current_string.push_back(c);
        }
    }
    fields.emplace_back(current_string);
    return fields;
}

void strip_cr(std::string& line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
}

int64_t get_id(const int64_t id, std::vector<int64_t>& idx_to_id, std::unordered_map<int64_t, int>& id_to_idx, std::vector<std::vector<Edge>>& graph) {
    if (id_to_idx.find(id) == id_to_idx.end()) {
        id_to_idx[id] = idx_to_id.size();
        idx_to_id.emplace_back(id);
        graph.emplace_back();
    }
    return id_to_idx[id];
}

int64_t median(std::vector<int64_t> v) {
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

double mean(const std::vector<int64_t>& v) {
    double sum = 0;
    for (const auto& i : v) {
        sum += i;
    }
    return sum / v.size();
}

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::string line;

    std::ifstream conn_file(argv[1]);
    std::ifstream neurons_file(argv[2]);
    std::ifstream class_file(argv[3]);

    std::vector<int64_t> idx_to_id;
    std::unordered_map<int64_t, int> id_to_idx;

    std::map<std::pair<int64_t, int64_t>, int> weight;
    std::vector<std::vector<Edge>> graph;

    std::getline(conn_file, line);
    strip_cr(line);
    while (std::getline(conn_file, line)) {
        strip_cr(line);
        auto relations = split(line);
        int64_t pre_root_id = std::stoll(relations[0]);
        int64_t post_root_id = std::stoll(relations[1]);
        int syn_count = std::stoi(relations[3]);

        weight[{get_id(pre_root_id, idx_to_id, id_to_idx, graph), get_id(post_root_id, idx_to_id, id_to_idx, graph)}] += syn_count;
    }

    for (const auto& [edge, w] : weight) {
        graph[edge.first].push_back({edge.second, w});
    }

    std::map<std::pair<std::string, std::string>, int> cross;
    std::vector<std::string> flow(graph.size()); // id to flow
    std::vector<std::string> super_class(graph.size()); // id to super_class

    std::getline(class_file, line);
    strip_cr(line);
    while (std::getline(class_file, line)) {
        strip_cr(line);
        auto relations = split(line);

        auto id = get_id(std::stoll(relations[0]), idx_to_id, id_to_idx, graph);
        auto f = relations[1];
        auto sc = relations[2];

        cross[std::make_pair(f, sc)]++;

        if (id >= (int64_t)flow.size()) {
            flow.resize(graph.size());
            super_class.resize(graph.size());
        }

        flow[id] = f;
        super_class[id] = sc;
    }

    for (const auto& [p, cnt] : cross) {
        std::cout << p.first << " " << p.second << " " << cnt << std::endl;
    }

    for (size_t i = 0; i < graph.size(); ++i) {
        if ((super_class[i] == "descending" || super_class[i] == "motor" || super_class[i] == "endocrine") && flow[i] != "efferent") {
            std::cout << idx_to_id[i] << " " << flow[i] << " " << super_class[i] << std::endl;
        }
    }

    std::vector<int64_t> out_deg(graph.size(), 0);
    std::vector<int64_t> in_deg(graph.size(), 0);

    std::vector<int64_t> out_syn(graph.size(), 0);
    std::vector<int64_t> in_syn(graph.size(), 0);

    for (size_t i = 0; i < graph.size(); ++i) {
        for (const auto& [to, w] : graph[i]) {
            out_deg[i]++;
            in_deg[to]++;

            out_syn[i] += w;
            in_syn[to] += w;
        }
    }

    std::unordered_map<std::string, std::vector<int64_t>> out_deg_by_flow;
    std::unordered_map<std::string, std::vector<int64_t>> in_deg_by_flow;
    std::unordered_map<std::string, std::vector<int64_t>> out_syn_by_flow;
    std::unordered_map<std::string, std::vector<int64_t>> in_syn_by_flow;

    std::unordered_map<std::string, int64_t> zero_in_by_flow;
    std::unordered_map<std::string, int64_t> zero_out_by_flow;

    for (size_t i = 0; i < graph.size(); ++i) {
        out_deg_by_flow[flow[i]].push_back(out_deg[i]);
        in_deg_by_flow[flow[i]].push_back(in_deg[i]);
        out_syn_by_flow[flow[i]].push_back(out_syn[i]);
        in_syn_by_flow[flow[i]].push_back(in_syn[i]);

        if (out_deg[i] == 0) {
            zero_out_by_flow[flow[i]]++;
        }
        if (in_deg[i] == 0) {
            zero_in_by_flow[flow[i]]++;
        }
    }

    std::unordered_map<std::string, std::vector<int64_t>> out_deg_by_super_class;
    std::unordered_map<std::string, std::vector<int64_t>> in_deg_by_super_class;
    std::unordered_map<std::string, std::vector<int64_t>> out_syn_by_super_class;
    std::unordered_map<std::string, std::vector<int64_t>> in_syn_by_super_class;

    std::unordered_map<std::string, int64_t> zero_in_by_super_class;
    std::unordered_map<std::string, int64_t> zero_out_by_super_class;

    for (size_t i = 0; i < graph.size(); ++i) {
        out_deg_by_super_class[super_class[i]].push_back(out_deg[i]);
        in_deg_by_super_class[super_class[i]].push_back(in_deg[i]);
        out_syn_by_super_class[super_class[i]].push_back(out_syn[i]);
        in_syn_by_super_class[super_class[i]].push_back(in_syn[i]);

        if (out_deg[i] == 0) {
            zero_out_by_super_class[super_class[i]]++;
        }
        if (in_deg[i] == 0) {
            zero_in_by_super_class[super_class[i]]++;
        }
    }

    for (const auto& [f, _] : out_deg_by_flow) {
        std::cout << f << ": " << out_deg_by_flow[f].size() << std::endl;
        std::cout << median(out_deg_by_flow[f]) << " " << median(in_deg_by_flow[f]) << " " << median(out_syn_by_flow[f]) << " " << median(in_syn_by_flow[f]) << std::endl;
        std::cout << mean(out_deg_by_flow[f]) << " " << mean(in_deg_by_flow[f]) << " " << mean(out_syn_by_flow[f]) << " " << mean(in_syn_by_flow[f]) << std::endl;
        std::cout << zero_in_by_flow[f] / static_cast<double>(out_deg_by_flow[f].size()) << " " << zero_out_by_flow[f] / static_cast<double>(out_deg_by_flow[f].size()) << std::endl;
    }

    for (const auto& [sc, _] : out_deg_by_super_class) {
        std::cout << sc << ": " << out_deg_by_super_class[sc].size() << std::endl;
        std::cout << median(out_deg_by_super_class[sc]) << " " << median(in_deg_by_super_class[sc]) << " " << median(out_syn_by_super_class[sc]) << " " << median(in_syn_by_super_class[sc]) << std::endl;
        std::cout << mean(out_deg_by_super_class[sc]) << " " << mean(in_deg_by_super_class[sc]) << " " << mean(out_syn_by_super_class[sc]) << " " << mean(in_syn_by_super_class[sc]) << std::endl;
        std::cout << zero_in_by_super_class[sc] / static_cast<double>(out_deg_by_super_class[sc].size()) << " " << zero_out_by_super_class[sc] / static_cast<double>(out_deg_by_super_class[sc].size()) << std::endl;
    }
}