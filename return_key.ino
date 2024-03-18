extern volatile unsigned short int enter;
extern volatile unsigned short int  x;
extern volatile unsigned long int Return_key_LASTTIME;
extern const unsigned int JITTERTIME;
extern volatile unsigned long int Current_time;
extern volatile bool Web_status;
extern volatile bool Dection_status;
extern bool clc;
extern void return_down() {
  Current_time=millis();
  if(Current_time-Return_key_LASTTIME>=JITTERTIME){
    Return_key_LASTTIME=Current_time;
    if (enter > 0 && enter <= 2) {
    enter -= 1;
  } else {
    enter = 0;
    clc=true;
    x = 13;
  }
  if(x==33&&Web_status==true)
  {
    Web_status=false;
    }
   if(x==22&&Dection_status==true)
    {
      Dection_status=false; 
      }
  }
  
}
