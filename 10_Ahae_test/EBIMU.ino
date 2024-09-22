int EBimuAsciiParser(float *item, int number_of_item)
{
  int n,i;
  int rbytes;
  char *addr; 
  int result = 0;
  
  rbytes = IMU_SERIAL.available();
  for(n=0;n<rbytes;n++)
  {
    sbuf[sbuf_cnt] = IMU_SERIAL.read();
    if(sbuf[sbuf_cnt]==0x0a)
       {
           addr = strtok(sbuf,",");
           for(i=0;i<number_of_item;i++)
           {
              item[i] = atof(addr);
              addr = strtok(NULL,",");
           }

           result = 1;

         // IMU_SERIAL.print("\n\r");
         // for(i=0;i<number_of_item;i++)  {  IMU_SERIAL.print(item[i]);  Serial.print(" "); }
       }
     else if(sbuf[sbuf_cnt]=='*')
       {   sbuf_cnt=-1;
       }

     sbuf_cnt++;
     if(sbuf_cnt>=SBUF_SIZE) sbuf_cnt=0;
  }
  
  return result;
}
