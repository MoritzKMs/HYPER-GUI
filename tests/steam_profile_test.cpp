#include "steam_profile_data.hpp"
#include <iostream>
int main() {
    int failures = 0;
    auto check = [&](bool good, const char *label) {
        if (!good) {
            std::cerr << label << '\n';
            ++failures;
        }
    };
    auto users = steam_local::Parse(R"vdf("users" {
 "76561197960265738" { "PersonaName" "Older \"Alias\"" "Timestamp" "100" "MostRecent" "1" }
 "76561197960265748" { "PersonaName" "Newer" "Timestamp" "200" }
 })vdf");
    check(users.size() == 2, "parse accounts");
    check(steam_local::Select(users, 20).name == "Newer", "active user wins over recent flag");
    check(steam_local::Select(users, 0).name == "Older \"Alias\"", "recent flag and escaped name");
    for (auto &u : users)
        u.recent = false;
    check(steam_local::Select(users, 1).name == "Newer",
          "unknown active account falls back to newest timestamp");
    check(steam_local::Select({}, 0).id == 0, "empty cache");
    check(steam_local::Number("184467440737095516160") == 0, "overflow timestamp");
    check(steam_local::Number("12x") == 0, "malformed number");
    auto malformed = steam_local::Parse("\"users\" { \"invalid\" { \"PersonaName\" \"Name\" } }");
    check(malformed.empty(), "reject invalid account ID");
    auto comment =
        steam_local::Parse("// comment\n\"users\" { \"76561197960265738\" { \"PersonaName\" \"Name\" } }");
    check(comment.size() == 1, "skip comments");
    return failures ? 1 : 0;
}
