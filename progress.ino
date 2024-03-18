extern void displayout(char *pt,int,int y);

/********************** 进度条服务函数 ****************************/
extern void display_bar()
{
    displayout("welcome!",45,39);
    for (int x = 10; x <100 ; x++)  
    {
        for (int y = 28; y < 40; y++)  
        {
            displayout(".", x, y);
        }
        if (x % 2 == 0)
        {
            delay(22);
            display.display();
        }
    }
}
