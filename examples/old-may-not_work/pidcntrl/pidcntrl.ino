#define epsilon 0.01
#define dt 0.01             //100ms loop time
#define MAX  4                   //For Current Saturation
#define MIN -4
#define Kp  0.1
#define Kd  0.01
#define Ki  0.005

// JDN speed approx 10 kHz with if (abs(..)) chg to if (1)

float PIDcal(float setpoint,float actual_position)
{
	static float pre_error = 0;
	static float integral = 0;
	float error;
	float derivative;
	float output;

	//Caculate P,I,D
	error = setpoint - actual_position;

	//In case of error too small then stop intergration
	if(abs(error) > epsilon)
	{
		integral = integral + error*dt;
	}
	derivative = (error - pre_error)/dt;
	output = Kp*error + Ki*integral + Kd*derivative;

	//Saturation Filter
	if(output > MAX)
	{
		output = MAX;
	}
	else if(output < MIN)
	{
		output = MIN;
	}
        //Update error
        pre_error = error;

 return output;
}

void setup()
{
  Serial.begin(9600);
}

unsigned long t1,t2;
void loop(void) {
int i;
float ref,f2,f3;
   t1 = millis();
   ref = 1.25; 
   f2 = 1.0;
   for (i=0; i< 30000; i++) {
 f3 =  PIDcal(ref,f2); 
    
   }
  t2 = millis();
 
  Serial.print(t1); Serial.print(" ");
   Serial.print(t2); Serial.print(" ");
     Serial.print(t2-t1); Serial.print(" ");
  Serial.print(1.0*(t2-t1)/30000.0); Serial.println(" ");

}
