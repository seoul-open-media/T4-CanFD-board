////////////////////////////////////////////////////////////////////
/////////////////////// EBIMU FUNCTION /////////////////////////////
// EBIMU is conneting to the Serial2 input
https://seoul-open-media.slack.com/archives/C0536BN2HQS/p1724106851417339?thread_ts=1723927283.018599&cid=C0536BN2HQS
 

#define HWSERIAL Serial2

#define SBUF_SIZE 64

char sbuf[SBUF_SIZE];
signed int sbuf_cnt=0;


int EBimuAsciiParser(float *item, int number_of_item)
{
  int n,i;
  int rbytes;
  char *addr; 
  int result = 0;
  
  rbytes = HWSERIAL.available();
  for(n=0;n<rbytes;n++)
  {
    sbuf[sbuf_cnt] = HWSERIAL.read();
    if(sbuf[sbuf_cnt]==0x0a)
       {
           addr = strtok(sbuf,",");
           for(i=0;i<number_of_item;i++)
           {
              item[i] = atof(addr);
              addr = strtok(NULL,",");
           }

           result = 1;

         // HWSERIAL.print("\n\r");
         // for(i=0;i<number_of_item;i++)  {  HWSERIAL.print(item[i]);  Serial.print(" "); }
       }
     else if(sbuf[sbuf_cnt]=='*')
       {   sbuf_cnt=-1;
       }

     sbuf_cnt++;
     if(sbuf_cnt>=SBUF_SIZE) sbuf_cnt=0;
  }
  
  return result;
}
/////////////////////// EBIMU FUNCTION /////////////////////////////
////////////////////////////////////////////////////////////////////


void setup() {
  Serial.begin(115200);
  HWSERIAL.begin(57600); // Set the configuration with GTKTERM <lf> <sb4><sor10> : factory reset, baudrate 9600, report every 10ms.
  Serial.println("EBIMU test");
}

void loop() {
  float euler[3];
  
  if(EBimuAsciiParser(euler, 3))
  {
     Serial.print("\n\r");
     Serial.print(euler[0]);   Serial.print(" ");
     Serial.print(euler[1]);   Serial.print(" ");
     Serial.print(euler[2]);   Serial.print(" ");
  }
}
