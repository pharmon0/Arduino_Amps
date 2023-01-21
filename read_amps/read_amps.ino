/* ESP32 Min132 CT Current Reading Function Code           |
 * Written by Phillip Harmon for UNCC and Duke Energy One  |
 * Last Update 1/20/23                                    */

//============================================================================================
// Settings for CT Current Reading
//============================================================================================
  //CT Reading Pin (Must be an analog-capable pin. Table below lists options.)
  const int CT_PIN = 34;
  //  | Pin# | Pin Label | ADC Channel |
  //  |  34  |   IO34    |     0       |
  //  |  35  |   IO35    |     1       |
  //  |  36  |    SVP    |     2       |
  //  |  39  |    SVN    |     3       |

  //current reading RMS sample window (milliseconds)
  const int RMS_WINDOW = 5000;

  //12-bit ADC step/volt conversion factor (V/step)
  const double ADC_FACTOR = 3.3 / 4095.0; // 3.3V supply / (2^12 - 1) ADC steps

  //amps-per-volt factor of the CT (A/V)
  const double CT_FACTOR = 150.0; // 50A / 333mV = 50 * 3 A/V

//============================================================================================
// Function: setup()
//  Performs system setup at boot.
// Arguments: -
// Returns: -
// Global Access: -
//============================================================================================
void setup() {
  //enable the Serial monitor at 115200 Baud and wait a moment
  Serial.begin(115200);
  delay(1000);
}

//============================================================================================
// Function: loop()
//  Program Main Function, Loops indefinitely.
// Arguments: -
// Returns: -
// Global Access: -
//============================================================================================
void loop() {
  //read the amps once per minute and report to the Serial monitor
  double amps = read_amps();
  Serial.print("Amps\t");
  Serial.println(amps);
  delay(60000); //60s blocking delay
}

//============================================================================================
// Function: read_amps()
//  Reads a CT on a set analog pin to determine RMS AC Amperage.
//  Note: See the Current Reading Settings section for details.
// Arguments: -
// Returns: RMS amperage seen over a set number of milliseconds as a double-precision float
// Global Access:
//  - RMS_WINDOW: number of milliseconds to sample
//  - CT_PIN: IO Pin# to read CT on
//  - ADC_FACTOR: Volt per step ADC conversion factor
//  - CT_FACTOR: Volt per Amp CT conversion factor
//============================================================================================
double read_amps(){
  //instantiate an array to store readings
  int readings[RMS_WINDOW];

  //take a reading every 1ms for the duration of the sample window
  for(int i = 0; i < RMS_WINDOW; i++){
    readings[i] = analogRead(CT_PIN);
    delay(1);
  }
  
  //Now that the sampling (time-sensitive part) is done, its time for data processing
  
  //convert the readings from ADC steps into Amps, and tally the square-sum
  double square_sum = 0;
  for(int i = 0; i < RMS_WINDOW; i++){
    double value = readings[i] * ADC_FACTOR * CT_FACTOR;
    square_sum += value * value;
  }

  //Calculate and return the RMS Amperage from the square-sum
  return sqrt(square_sum / RMS_WINDOW);
}