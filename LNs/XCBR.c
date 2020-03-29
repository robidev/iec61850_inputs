//callback for open/close signal from GOOSE-> will trigger process threat

static bool switch_open = false;

void open()
{
  switch_open = true;
}

void close()
{
  switch_open = false;
}

void init()
{
  //start simulation threat

  //register callbacks for GOOSE-subscription
}

//threath for process-simulation: open/close switch
void simulate_switch()
{
  while(1)
  {
    while(switch_open == false){ sleep(10); }
    printf("XCBR: opening\n");
    //stVal = 00
    //send GOOSE stVal
    sleep(20);
    printf("XCBR: opened\n");
    //stVal = 01
    while(switch_open == true){ sleep(10); }
    printf("XSWI: closing\n");
    //stVal = 00
    //send GOOSE stVal
    sleep(20);
    printf("XCBR: closed\n");
    //stVal = 10
    //send GOOSE stVal
  }
}