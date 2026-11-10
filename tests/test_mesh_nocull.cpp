#include "../gpu/vk/mesh_nocull.h"

#include <cstdio>

#define CHECK(name, condition) do { if (!(condition)) { std::fprintf(stderr, "FAIL %s at line %d\n", name, __LINE__); return 1; } } while (0)

int main() {
    using namespace mkwvk::mesh_nocull;
    CHECK("RX 6600 on RADV", affected_radv_device(true, "AMD Radeon RX 6600 (RADV NAVI23)"));
    CHECK("Navi 23 spelling", affected_radv_device(true, "AMD Radeon Graphics (RADV NAVI 23)"));
    CHECK("Steam Deck", affected_radv_device(true, "AMD RADV VANGOGH"));
    CHECK("not proprietary AMD", !affected_radv_device(false, "AMD Radeon RX 6600"));
    CHECK("not another RADV GPU", !affected_radv_device(true, "AMD Radeon RX 7800 XT (RADV NAVI32)"));

    CHECK("automatic known shader", wanted(kOotGridCullTask, nullptr, true));
    CHECK("automatic only affected GPU", !wanted(kOotGridCullTask, nullptr, false));
    CHECK("automatic only known shader", !wanted(0xdeadbeef, nullptr, true));
    CHECK("explicit zero disables", !wanted(kOotGridCullTask, "0", true));
    CHECK("explicit prefix enables", wanted(kOotGridCullTask, "2c56dc", false));
    CHECK("explicit unrelated prefix", !wanted(kOotGridCullTask, "deadbeef", true));
    CHECK("explicit empty disables", !wanted(kOotGridCullTask, "", true));
    std::puts("mesh_nocull: all checks passed");
    return 0;
}
