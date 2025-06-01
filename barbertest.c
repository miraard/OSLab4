#include "types.h"
#include "user.h"

void test_barber() {
printf(1, "Starting Barber Test...\n");

// آرایشگر رو به صورت یه فرآیند جدا اجرا کن
if (fork() == 0) {
barber();
exit();
}

// ایجاد چند مشتری
for (int i = 0; i < 5; i++) {
if (fork() == 0) {
customer();
exit();
}
}

// صبر کن تا همه مشتری‌ها کارشون تموم بشه
for (int i = 0; i < 5; i++) {
wait();
}

// آرایشگر رو متوقف کن (برای تست، چون آرایشگر توی حلقه بی‌نهایته)
kill(getpid() + 1); // pid آرایشگر (اولین فرزند)
wait(); // صبر کن تا آرایشگر تموم بشه

printf(1, "Barber Test finished.\n");
}

int main(void) {
barber_init();
test_barber();
exit();
}