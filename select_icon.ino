extern void displayout(char *pt,int x,int y);
/*绘制选择图标*/
extern void select_icon(int cur) {
  int sur = cur + 5;
  int a = cur, b = sur;
  for (int x = 0; x < 5; x++) {
    for (int y = cur; y < sur; y++) {
      if (cur <= (a + b) / 2 && sur >= (a + b) / 2) {
        displayout(".", x, y);
      }
    }
    sur -= 1;
    cur += 1;
  }
}
