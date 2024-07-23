//——————————————————————————————————————————————————————————————————————————————
// Demonstration of control and monitoring of 2 moteus controllers
// running on a CANBed FD from longan labs.
//  * https://mjbots.com/products/moteus-r4-11
//  * https://www.longan-labs.cc/1030009.html
// ——————————————————————————————————————————————————————————————————————————————

//
// 시리얼에 "1 0.1" 입력 -> 1번 모터 기준 0.1만큼 포지션 이동 (2번 모터는 -0.1만큼)
// 시리얼에 "2 0.3" -> 2번 모터 기준 0.3만큼 포지션 이동 (1번 모터는 -0.3만큼)
//

#include <ACAN2517FD.h>
#include <Moteus.h>

//——————————————————————————————————————————————————————————————————————————————
//  The following pins are selected for the CANBed FD board.
//——————————————————————————————————————————————————————————————————————————————

static const byte MCP2517_SCK = 13 ; // SCK input of MCP2517
static const byte MCP2517_SDI =  11 ; // SDI input of MCP2517
static const byte MCP2517_SDO =  12 ; // SDO output of MCP2517

static const byte MCP2517_CS  = 10 ; // CS input of MCP2517

//——————————————————————————————————————————————————————————————————————————————
//  ACAN2517FD Driver object
//——————————————————————————————————————————————————————————————————————————————

ACAN2517FD can (MCP2517_CS, SPI, 255) ; // Last argument is 255 -> no interrupt pin

Moteus moteus1(can, []() {
  Moteus::Options options;
  options.id = 1; // Moteus ID (by Tview)
  return options;
}());

//Moteus moteus2(can, []() {
//  Moteus::Options options;
//  options.id = 14; // Moteus ID (by Tview)
//  return options;
//}());

Moteus::PositionMode::Command position_cmd1;
Moteus::PositionMode::Command position_cmd2;
Moteus::PositionMode::Format position_fmt;



void setup() {
  pinMode (LED_BUILTIN, OUTPUT);

  // Let the world know we have begun!
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println(F("started"));

  SPI.begin();

  // This operates the CAN-FD bus at 1Mbit for both the arbitration
  // and data rate.  Most arduino shields cannot operate at 5Mbps
  // correctly, so the moteus Arduino library permanently disables
  // BRS.
  ACAN2517FDSettings settings(
      ACAN2517FDSettings::OSC_40MHz, 1000ll * 1000ll, DataBitRateFactor::x1);

  // The atmega32u4 on the CANbed has only a tiny amount of memory.
  // The ACAN2517FD driver needs custom settings so as to not exhaust
  // all of SRAM just with its buffers.
  settings.mArbitrationSJW = 2;
  settings.mDriverTransmitFIFOSize = 1;
  settings.mDriverReceiveFIFOSize = 2;


  const uint32_t errorCode = can.begin(settings, NULL);

  while (errorCode != 0) {
    Serial.print(F("CAN error 0x"));
    Serial.println(errorCode, HEX);
    delay(1000);
  }

  position_fmt.position = Moteus::kInt16;
  position_fmt.velocity = Moteus::kIgnore;
  position_fmt.maximum_torque = Moteus::kInt16;
  position_fmt.feedforward_torque = Moteus::kIgnore;
  position_fmt.kp_scale = Moteus::kIgnore;
  position_fmt.kd_scale = Moteus::kIgnore;
  position_fmt.stop_position = Moteus::kIgnore;
  position_fmt.velocity_limit = Moteus::kInt16;
  position_fmt.accel_limit = Moteus::kInt16;

  position_cmd1.velocity_limit = 0.5;
  position_cmd1.accel_limit = 0.1;

  position_cmd2.velocity_limit = 0.5;
  position_cmd2.accel_limit = 0.1;

  // To clear any faults the controllers may have, we start by sending
  // a stop command to each.
  moteus1.SetStop();
  //moteus2.SetStop();
  Serial.println(F("all stopped"));

  auto print_state = [&]() {
    const auto& v = moteus1.last_result().values;
    Serial.print(F(" mode="));
    Serial.print(static_cast<int>(v.mode));
    Serial.print(F(" pos="));
    Serial.print(v.position, 3);
    Serial.print(F(" vel="));
    Serial.print(v.velocity, 3);
    Serial.print(F(" torque="));
    Serial.print(v.torque, 3);
    Serial.print(F(" q_current="));
    Serial.print(v.q_current, 3);
    Serial.print(F(" d_current="));
    Serial.print(v.d_current, 3);
    Serial.print(F(" abs_position="));
    Serial.print(v.abs_position, 3);
    Serial.print(F(" motor_temperature="));
    Serial.print(v.motor_temperature, 3);
    Serial.print(F(" voltage="));
    Serial.print(v.voltage, 3);
    Serial.println();
/*
    const auto& p = moteus2.last_result().values;
    Serial.print(F(" mode="));
    Serial.print(static_cast<int>(p.mode));
    Serial.print(F(" pos="));
    Serial.print(p.position, 3);
    Serial.print(F(" vel="));
    Serial.print(p.velocity, 3);
    Serial.print(F(" torque="));
    Serial.print(p.torque, 3);
    Serial.print(F(" q_current="));
    Serial.print(p.q_current, 3);
    Serial.print(F(" d_current="));
    Serial.print(p.d_current, 3);
    Serial.print(F(" abs_position="));
    Serial.print(p.abs_position, 3);
    Serial.print(F(" motor_temperature="));
    Serial.print(p.motor_temperature, 3);
    Serial.print(F(" voltage="));
    Serial.print(p.voltage, 3);
    Serial.println();
    */
  };

  print_state();
}

void loop() {
   auto print_state = [&]() {
    const auto& v = moteus1.last_result().values;
    Serial.print(F(" mode="));
    Serial.print(static_cast<int>(v.mode));
    Serial.print(F(" pos="));
    Serial.print(v.position, 3);
    Serial.print(F(" vel="));
    Serial.print(v.velocity, 3);
    Serial.print(F(" torque="));
    Serial.print(v.torque, 3);
    Serial.print(F(" q_current="));
    Serial.print(v.q_current, 3);
    Serial.print(F(" d_current="));
    Serial.print(v.d_current, 3);
    Serial.print(F(" abs_position="));
    Serial.print(v.abs_position, 3);
    Serial.print(F(" motor_temperature="));
    Serial.print(v.motor_temperature, 3);
    Serial.print(F(" voltage="));
    Serial.print(v.voltage, 3);
    Serial.println();
/*
    const auto& p = moteus2.last_result().values;
    Serial.print(F(" mode="));
    Serial.print(static_cast<int>(p.mode));
    Serial.print(F(" pos="));
    Serial.print(p.position, 3);
    Serial.print(F(" vel="));
    Serial.print(p.velocity, 3);
    Serial.print(F(" torque="));
    Serial.print(p.torque, 3);
    Serial.print(F(" q_current="));
    Serial.print(p.q_current, 3);
    Serial.print(F(" d_current="));
    Serial.print(p.d_current, 3);
    Serial.print(F(" abs_position="));
    Serial.print(p.abs_position, 3);
    Serial.print(F(" motor_temperature="));
    Serial.print(p.motor_temperature, 3);
    Serial.print(F(" voltage="));
    Serial.print(p.voltage, 3);
    Serial.println();
    */
  };

  if (Serial.available()) {

    String val = Serial.readStringUntil('\n');
    if (val.indexOf("stat") != -1) {
      Serial.println(can.available());
      print_state();
      return;
    }
    int firstIndex = val.indexOf(' ');
    
    String firstValue = val.substring(0, firstIndex);
    String secondValue = val.substring(firstIndex + 1);

    if (firstValue.toInt() == 1 || firstValue.toInt() == 2) {
      run2walk(firstValue.toInt(), secondValue.toFloat());
      return;
    }

  }
}

void run2walk(int motor, double pos) {
  position_cmd1.velocity_limit = 2.0;
  position_cmd1.accel_limit = 2.0;
  position_cmd1.maximum_torque = 6.0;
  position_cmd1.velocity = 2.0;
  position_cmd2.velocity_limit = 2.0;
  position_cmd2.accel_limit = 2.0;
  position_cmd2.maximum_torque = 6.0;
  position_cmd2.velocity = 2.0;
  if (motor == 1) {
    Serial.println(pos);
    double origin1 = moteus1.last_result().values.position;
  //  double origin2 = moteus2.last_result().values.position;
  //  setDoublePosition(origin1 - pos, origin2 + pos, 0.001);
  } else if (motor == 2) {
    Serial.println(pos);
    double origin1 = moteus1.last_result().values.position;
 //   double origin2 = moteus2.last_result().values.position;
 //   setDoublePosition(origin1 + pos, origin2 - pos, 0.001);
  }
}

void setDoublePosition(double pos1, double pos2, double period_s) {

  position_cmd1.position = pos1;
  position_cmd2.position = pos2;

  int count = 2;
  while (true) {
    const bool got_result1 = moteus1.SetPosition(position_cmd1, &position_fmt);
 //   const bool got_result2 = moteus2.SetPosition(position_cmd2, &position_fmt);
//    if (got_result1 && got_result2) {
//      count = count - 1;
//      if (count < 0) { count = 0; }
//    }
    
//    if (count == 0 && got_result1 && got_result2 && moteus1.last_result().values.trajectory_complete && moteus2.last_result().values.trajectory_complete) {
//      return;
//    }

    delay(static_cast<unsigned long>(period_s * 1000));
  }
}
