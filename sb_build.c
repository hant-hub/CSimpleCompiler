#define SB_IMPL
#include "sb.h"


int main(int argc, char* argv[]) {
    sb_BUILD(argc, argv) {
        sb_target_dir("build/");
        sb_mkdir("build");
        sb_EXEC() {
            sb_set_out("app");

            sb_add_flag("Iinclude");

            sb_add_flag("g");

            sb_add_file("src/main.c");
            sb_add_file("src/util.c");
            sb_add_file("src/symboltable.c");
            sb_add_file("src/tokenizer.c");
            sb_add_file("src/stringstore.c");

            sb_export_command();
        }
        sb_fence();
        sb_CMD() {
            sb_cmd_main("./build/app");
        }
    }
}
