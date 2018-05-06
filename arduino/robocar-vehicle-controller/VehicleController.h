#ifndef VEHICLE_CONTROLLER_H_
#define VEHICLE_CONTROLLER_H_

#include <Servo.h>

//
// Status code for commands
//
enum CommandResult {
  VC_OK = 0,
  VC_INVALID_COMMAND = 1,
  VC_INVALID_PARAM = 2,
  VC_INVALID_STATE = 3
};

//
// VehicleController
//
class VehicleController {
public:
  VehicleController();
  void setup();
  void loop();
  
private:
  int servo_motor_pin_;
  int servo_steering_pin_;

  Servo servo_motor_;
  Servo servo_steering_;
  char cmdbuf_[16];
  uint8_t cmdbufpos_;

  // controller config vars
  unsigned long timeout_ms_;
  float max_throttle_;
  
  // controller state vars
  unsigned long last_command_ts_;
  bool running_;
  float throttle_;
  float steering_;

  void ProcessCommandByte(uint8_t data);
  void ProcessCommandLine(char* line);
  CommandResult ProcessCommandD(char* line);
  CommandResult ProcessCommandI(char* line);
  CommandResult ProcessCommandM(char* line);
  CommandResult ProcessCommandR(char* line);
  CommandResult ProcessCommandS(char* line);
  CommandResult ProcessCommandT(char* line);
};


#endif // VEHICLE_CONTROLLER_H_
