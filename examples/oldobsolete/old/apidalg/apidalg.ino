// JUST A SIMPLE PID CONTROLLER
// Jens

#define epsilon 0.01
#define dt 0.01             //100ms loop time
#define MAX  4                   //For Current Saturation
#define MIN -4
#define Kp  0.1
#define Kd  0.01
#define Ki  0.005

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
}

void loop(void) {
}
