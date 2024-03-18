extern volatile unsigned short int x;
extern volatile unsigned short int enter;
extern volatile unsigned long int Menu_selection_key_LASTTIME;
extern volatile unsigned long int Current_time;
extern const unsigned int JITTERTIME;
extern void Menu_selection() {
  Current_time=millis();
  if(Current_time-Menu_selection_key_LASTTIME>=JITTERTIME){
    Menu_selection_key_LASTTIME=Current_time;
      if(Menu_selection_key_LASTTIME)
  if (x >= 13 && x <=43&& enter==1) {
    x += 10;
  }
  if (x >43) {
      x = 13;
    }
  }

}
