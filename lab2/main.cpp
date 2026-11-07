#include <climits>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>

enum class NT {
    UNKNOWN,
    ACH,
    GABA,
    GLUT,
    DA,
    SER,
    OCT
};

NT parse_nt(const std::string& s) {
    if (s == "ACH")  return NT::ACH;
    if (s == "GABA") return NT::GABA;
    if (s == "GLUT") return NT::GLUT;
    if (s == "DA")   return NT::DA;
    if (s == "SER")  return NT::SER;
    if (s == "OCT")  return NT::OCT;
    return NT::UNKNOWN;
}

int sign_of(NT nt) {
    switch (nt) {
        case NT::ACH:  return +1;
        case NT::GABA:
        case NT::GLUT: return -1;
        case NT::DA:
        case NT::SER:
        case NT::OCT:  return 0;
        case NT::UNKNOWN: break;
    }
    return 2;
}

// split string by commas
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

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::string line;

    std::ifstream neurons_file(argv[1]);
    std::ifstream conn_file(argv[2]);

    std::unordered_map<int64_t, int> id_to_idx;  // root_id -> idx 0..N-1
    std::vector<int64_t> idx_to_id;              // idx -> root_id
    auto get_idx = [&](int64_t id) -> int {
        if (id_to_idx.find(id) == id_to_idx.end()) {
            id_to_idx[id] = idx_to_id.size();
            idx_to_id.emplace_back(id);
        }
        return id_to_idx[id];
    };

    std::unordered_map<int, NT> idx_to_nt;
    std::getline(neurons_file, line);
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    while (std::getline(neurons_file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        auto relation = split(line);
        idx_to_nt[get_idx(std::stoll(relation[0]))] = parse_nt(relation[2]);
    }

    std::unordered_map<uint64_t, int> weight;  // (pre, post) -> synapse count  
    std::unordered_map<uint64_t, std::unordered_set<std::string>> neuropils; // (pre, post) -> neuropils;
    std::unordered_map<int64_t, std::unordered_set<NT>> neurotransmitters; // (pre, post) -> neurotransmitters
    std::unordered_map<int, std::pair<int64_t, int64_t>> sign_to_edges; // sign -> (edges, synapses)
    int64_t rows = 0, total_syn = 0;

    int min_row_syn = INT_MAX;

    int rows_match = 0;
    int rows_mismatch = 0;
    int rows_pre_unknown = 0;

    std::getline(conn_file, line);
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    while (std::getline(conn_file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        auto relation = split(line);
        int64_t pre_root_id = std::stoll(relation[0]);
        int64_t post_root_id = std::stoll(relation[1]);
        std::string neuropil = relation[2];
        int syn_count = std::stoi(relation[3]);
        std::string nt = relation[4];
        
        uint64_t pre_root_id_idx = get_idx(pre_root_id);
        uint64_t post_root_id_idx = get_idx(post_root_id);

        weight[pre_root_id_idx << 32 | post_root_id_idx] += syn_count;
        neuropils[pre_root_id_idx << 32 | post_root_id_idx].insert(neuropil);
        neurotransmitters[pre_root_id_idx].insert(parse_nt(nt));

        rows++;
        total_syn += syn_count;
        min_row_syn = std::min(min_row_syn, syn_count);

        if (idx_to_nt[pre_root_id_idx] == NT::UNKNOWN) {
            rows_pre_unknown++;
        } else if (idx_to_nt[pre_root_id_idx] == parse_nt(nt)) {
            rows_match++;
        } else {
            rows_mismatch++;
        }
    }

    int pair_with_difference_neuropils = 0;
    for (const auto& [_, nrpls] : neuropils) {
        if (nrpls.size() > 1) {
            pair_with_difference_neuropils++;
        }
    }

    int64_t total_weight = 0;
    int64_t min_weight = INT64_MAX;

    int64_t max_weight = 0;
    uint64_t max_row_syn_pre = 0;
    uint64_t max_row_syn_post = 0;
    for (const auto [edge, w] : weight) {
        total_weight += w;
        min_weight = std::min<int64_t>(min_weight, w);

        if (w > max_weight) {
            max_weight = w;
            max_row_syn_pre = idx_to_id[edge >> 32];
            max_row_syn_post = idx_to_id[edge & ((1ULL << 32) - 1)];
        }

        sign_to_edges[sign_of(idx_to_nt[edge >> 32])].first++;
        sign_to_edges[sign_of(idx_to_nt[edge >> 32])].second += w;
    }

    uint64_t pre_neurons_mixed = 0;
    for (const auto& [_, nts] : neurotransmitters) {
        if (nts.size() > 1) {
            pre_neurons_mixed++;
        }
    }

    std::cout << rows << " " << idx_to_id.size() << " " << neuropils.size() << " " << total_syn << std::endl;
    std::cout << total_weight << std::endl;
    std::cout << min_row_syn << " " << min_weight << std::endl;
    std::cout << pair_with_difference_neuropils << std::endl;
    std::cout << max_weight << " " << max_row_syn_pre << " " << max_row_syn_post << std::endl;
    std::cout << rows_match << " " << rows_mismatch << " " << rows_pre_unknown << std::endl;
    std::cout << neurotransmitters.size() << " " << pre_neurons_mixed << std::endl;

    for (const auto& [sign, edge] : sign_to_edges) {
        std::cout << sign << " " << edge.first << " " << edge.second << std::endl;
    }
}