//callback for open/close signal from GOOSE-> will trigger process threat

static char switch_open = 0;

void XCBR_open()
{
  switch_open = 1;
}

void XCBR_close()
{
  switch_open = 0;
}

void XCBR_callback(ref)
{
  int input = 0;
  //read input-data from ref into input (could come from GOOSE, SMV or local data update)
  if(input == 1)
    XCBR_open();
  else
    XCBR_close();
  
}

void XCBR_init()
{
  //start simulation threat

  //register callbacks for GOOSE-subscription
}

//threath for process-simulation: open/close switch
void XCBR_simulate_switch()
{
  while(1)
  {
    while(switch_open == 0){ sleep(10); }
    printf("XCBR: opening\n");
    //stVal = 00
    //send GOOSE stVal
    sleep(20);
    printf("XCBR: opened\n");
    //stVal = 01
    while(switch_open == 1){ sleep(10); }
    printf("XSWI: closing\n");
    //stVal = 00
    //send GOOSE stVal
    sleep(20);
    printf("XCBR: closed\n");
    //stVal = 10
    //send GOOSE stVal
  }
}