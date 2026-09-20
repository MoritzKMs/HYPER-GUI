#pragma once
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>
namespace steam_local {
struct User {
    std::uint64_t id = 0, timestamp = 0;
    std::string name;
    bool recent = false;
};
inline std::uint64_t Number(const std::string &s) {
    if (s.empty() || !std::all_of(s.begin(), s.end(), [](unsigned char c) { return c >= '0' && c <= '9'; }))
        return 0;
    try {
        return std::stoull(s);
    } catch (...) {
        return 0;
    }
}
inline std::vector<std::string> Tokens(const std::string &source) {
    std::vector<std::string> out;
    for (size_t i = 0; i < source.size();) {
        if (source[i] == '/' && i + 1 < source.size() && source[i + 1] == '/') {
            while (i < source.size() && source[i] != '\n')
                ++i;
            continue;
        }
        if (source[i] == '{' || source[i] == '}') {
            out.emplace_back(1, source[i++]);
            continue;
        }
        if (source[i++] != '"')
            continue;
        std::string token;
        while (i < source.size()) {
            char c = source[i++];
            if (c == '"')
                break;
            if (c == '\\' && i < source.size()) {
                char next = source[i++];
                if (next == 'n')
                    token += ' ';
                else if (next == 't')
                    token += ' ';
                else
                    token += next;
            } else
                token += c;
        }
        out.push_back(token);
    }
    return out;
}
inline std::vector<User> Parse(const std::string &source) {
    auto tokens = Tokens(source);
    std::vector<User> users;
    int depth = 0;
    User current;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto &t = tokens[i];
        if (t == "{") {
            ++depth;
            continue;
        }
        if (t == "}") {
            if (depth == 2 && current.id) {
                users.push_back(current);
                current = {};
            }
            --depth;
            continue;
        }
        if (depth == 1 && i + 1 < tokens.size() && tokens[i + 1] == "{") {
            current = {};
            current.id = Number(t);
            continue;
        }
        if (depth != 2 || i + 1 >= tokens.size() || tokens[i + 1] == "{" || tokens[i + 1] == "}")
            continue;
        std::string key = t;
        std::transform(key.begin(), key.end(), key.begin(),
                       [](unsigned char c) { return char(std::tolower(c)); });
        auto value = tokens[++i];
        if (key == "personaname")
            current.name = value;
        else if (key == "timestamp")
            current.timestamp = Number(value);
        else if (key == "mostrecent")
            current.recent = value == "1";
    }
    return users;
}
inline User Select(const std::vector<User> &users, std::uint32_t active) {
    if (active)
        for (const auto &user : users)
            if (user.id == 76561197960265728ULL + active)
                return user;
    User best;
    for (const auto &user : users)
        if (!best.id || (user.recent && !best.recent) ||
            (user.recent == best.recent && user.timestamp > best.timestamp))
            best = user;
    return best;
}
} // namespace steam_local
