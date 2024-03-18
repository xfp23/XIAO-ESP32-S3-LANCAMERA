extern volatile unsigned short int enter;
extern volatile unsigned short int x;
extern volatile unsigned long int Enter_key_LASTTIME;
extern volatile unsigned long int Current_time;
extern const unsigned int JITTERTIME;
extern volatile bool Dection_status;
/*确认键设置*/
extern void enter_down() { 
  Current_time=millis();
  if(Current_time-Enter_key_LASTTIME>=JITTERTIME)
  {
    Enter_key_LASTTIME=Current_time;
      if (enter >= 0 && enter <= 2) {
    enter += 1;
  }
   if(x==22&&Dection_status==false)
   {
    Dection_status=true;
    }
  }

}
