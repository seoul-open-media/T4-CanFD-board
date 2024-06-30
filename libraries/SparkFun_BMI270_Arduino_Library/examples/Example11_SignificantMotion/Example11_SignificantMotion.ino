#include <Wire.h>
#include "SparkFun_BMI270_Arduino_Library.h"

// Create a new sensor object
BMI270 imu;

// I2C address selection
uint8_t i2cAddress = BMI2_I2C_PRIM_ADDR; // 0x68
//uint8_t i2cAddress = BMI2_I2C_SEC_ADDR; // 0x69

// Pin used for interrupt detection
int interruptPin = 5;

// Flag to know when interrupts occur
volatile bool interruptOccurred = false;

void setup()
{
    // Start serial
    Serial.begin(115200);
    Serial.println("BMI270 Example 11 - Significant Motion");

    // Initialize the I2C library
    Wire.begin();

    // Check if sensor is connected and initialize
    // Address is optional (defaults to 0x68)
    while(imu.beginI2C(i2cAddress) != BMI2_OK)
    {
        // Not connected, inform user
        Serial.println("Error: BMI270 not connected, check wiring and I2C address!");

        // Wait a bit to see if connection is established
        delay(1000);
    }

    Serial.println("BMI270 connected!");

    // Here we enable the "significant motion" feature of the BMI270.
    // "Significant motion" is defined by Android as motion that might lead to a
    // change in user location. Examples include walking, biking, or sitting in
    // a vehicle. Examples do not include being in a user's pocket while
    // sitting, or lying at rest on a table
    // https://source.android.com/docs/core/interaction/sensors/sensor-types#significant_motion
    imu.enableFeature(BMI2_SIG_MOTION);

    // We can configure the parameters for the "significant motion" feature.
    // The sensor has default settings for each of these, described below:
    // 
    // .block_size - Time required to trigger an interrupt. 20ms resolution, up
    //               to 1310 seconds (default: 250 = 5s)
    // 
    // This example uses the default parameter values defined by the sensor, but
    // you can change these values by uncommenting the lines below
    // 
    // bmi2_sens_config config;
    // config.type = BMI2_SIG_MOTION;
    // config.cfg.sig_motion.block_size = 250;
    // imu.setConfig(config);

    // The BMI270 has 2 interrupt pins. All interrupt conditions can be mapped
    // to either pin, so we'll just choose the first one for this example
    imu.mapInterruptToPin(BMI2_SIG_MOTION_INT, BMI2_INT1);

    // Here we configure the interrupt pins using a bmi2_int_pin_config, which 
    // allows for both pins to be configured simultaneously if needed. Here's a
    // brief description of each value:
    // 
    // .pin_type  - Which pin(s) is being configured (INT1, INT2, or BOTH)
    // .int_latch - Latched or pulsed signal (applies to both pins)
    // .pin_cfg   - Array of settings for each pin (index 0/1 for INT1/2):
    //     .lvl       - Active high or low
    //     .od        - Push/pull or open drain output
    //     .output_en - Whether to enable outputs from this pin
    //     .input_en  - Whether to enable inputs to this pin (see datasheet)
    // 
    // In this case, we set INT1 to pulsed, active high, push/pull
    bmi2_int_pin_config intPinConfig;
    intPinConfig.pin_type = BMI2_INT1;
    intPinConfig.int_latch = BMI2_INT_NON_LATCH;
    intPinConfig.pin_cfg[0].lvl = BMI2_INT_ACTIVE_HIGH;
    intPinConfig.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
    intPinConfig.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
    intPinConfig.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;
    imu.setInterruptPinConfig(intPinConfig);
    
    // Setup interrupt handler
    attachInterrupt(digitalPinToInterrupt(interruptPin), myInterruptHandler, RISING);
}

void loop()
{
    // Wait for interrupt to occur
    if(interruptOccurred)
    {
        // Reset flag for next interrupt
        interruptOccurred = false;

        Serial.print("Interrupt occurred!");
        Serial.print("\t");

        // Get the interrupt status to know which condition triggered
        uint16_t interruptStatus = 0;
        imu.getInterruptStatus(&interruptStatus);

        // Check if this is the correct interrupt condition
        if(interruptStatus & BMI270_SIG_MOT_STATUS_MASK)
        {
            Serial.println("Significant motion trigger!");
        }
        else
        {
            Serial.println("Unknown interrupt condition!");
        }
    }
}

void myInterruptHandler()
{
    interruptOccurred = true;
}