extern void displayout(char *pt,int x,int y);
/*显示电源标志*/
extern void display_battery()
{
int points[][2] = {
    {4, -1}, {3, 0}, {2, 1}, {1, 2}, {0, 3},
    {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3},
    {5, 3}, {4, 4}, {3, 5}, {2, 6}, {1, 7}
};


    for (int i = 0; i < sizeof(points) / sizeof(points[0]); ++i) {
        displayout(".", points[i][0], points[i][1]);
    }
    //display.display();
    }
