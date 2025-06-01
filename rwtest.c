#include "types.h"
#include "user.h"

void test_reader_writer() {
int i;
printf(1, "Starting Reader-Writer Test...\n");

for (i = 0; i < 3; i++) {
if (fork() == 0) {
reader();
exit();
}
}

for (i = 0; i < 2; i++) {
if (fork() == 0) {
writer();
exit();
}
}

// صبر کن تا همه فرآیندهای فرزند تموم بشن
for (i = 0; i < 5; i++) { // 3 Reader + 2 Writer = 5 فرزند
wait();
}
printf(1, "Reader-Writer Test finished.\n");
}

int main(void) {
rw_init();
test_reader_writer();
exit();
}