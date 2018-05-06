#include "VehicleController.h"

#include <Arduino.h>

VehicleController::VehicleController()
  : servo_motor_pin_(9), servo_steering_pin_(10), cmdbufpos_(0),
    timeout_ms_(1000), max_throttle_(1.0f),
    last_command_ts_(0), running_(false), throttle_(0.0f), steering_(0.0f) {
}
void VehicleController::setup() {
  servo_motor_.attach(servo_motor_pin_);
  servo_steering_.attach(servo_steering_pin_);
}
void VehicleController::loop() {
  // process incoming data
  if (Serial.available()) {
    int data = Serial.read();
    if (data >= 0) {
      ProcessCommandByte((uint8_t)(((unsigned int) data) & 0xFF));
    }
  }

  // check for timeout
  unsigned long curr_ts = millis();
  if (curr_ts > (last_command_ts_ + timeout_ms_)) {
    // timeout exceeded, halt system
    running_ = false;
  }

  // set throttle to zero if not running
  if (!running_) {
    throttle_ = 0.0f;
  }

  // update motor and steering servo values
  float motor_val_f = 90.0f + (90.0f * throttle_);
  int motor_val_i = (int) motor_val_f;
  if (motor_val_i < 0) {
    motor_val_i = 0;
  }
  if (motor_val_i > 180) {
    motor_val_i = 180;
  }
  servo_motor_.write(motor_val_i);


  float steering_val_f = 90.0f + steering_;
  int steering_val_i = (int) steering_val_f;
  if (steering_val_i < 0) {
    steering_val_i = 0;
  }
  if (steering_val_i > 180) {
    steering_val_i = 180;
  }
  servo_steering_.write(steering_val_i);
}
CommandResult VehicleController::ProcessCommandD(char* line) {
  char* argp = &line[1];

  // check if numeric arg
  if (*argp < '0' || *argp > '9') {
    return VC_INVALID_PARAM;
  }
  // get int value
  int decval = String(argp).toInt();
  // update value
  timeout_ms_ = decval;
  return VC_OK;
}
CommandResult VehicleController::ProcessCommandI(char* line) {
  Serial.print("D=");
  Serial.print(timeout_ms_);
  Serial.print(", M=");
  Serial.print(max_throttle_);
  Serial.print(", R=");
  Serial.print(running_);
  Serial.print(", T=");
  Serial.print(throttle_);
  Serial.print(", S=");
  Serial.print(steering_);
  Serial.println();
  return VC_OK;
}
CommandResult VehicleController::ProcessCommandM(char* line) {
  char* argp = &line[1];

  // check if numeric arg
  if (*argp < '0' || *argp > '9') {
    return VC_INVALID_PARAM;
  }
  // get float value
  float decval = String(argp).toFloat();
  // check if within allowed range
  if (decval < 0.0f || decval > 1.0f) {
    return VC_INVALID_PARAM;
  }
  // update value
  max_throttle_ = decval;
  return VC_OK;
}
CommandResult VehicleController::ProcessCommandR(char* line) {
  running_ = true;
  throttle_ = 0.0f;
  steering_ = 0.0f;
  last_command_ts_ = millis();
  return VC_OK;
}
CommandResult VehicleController::ProcessCommandS(char* line) {
  char* argp = &line[1];

  bool minus = false;
  if (*argp == '-') {
    minus = true;
    argp++;
  }
  // check if numeric arg
  if (*argp < '0' || *argp > '9') {
    return VC_INVALID_PARAM;
  }
  // get float value
  float decval = String(argp).toFloat();
  // check if within allowed range
  if (decval < 0.0f || decval > 90.0f) {
    return VC_INVALID_PARAM;
  }
  // only update if running
  if (!running_) {
    return VC_INVALID_STATE;
  }
  // update state
  steering_ = minus ? -decval : decval;
  return VC_OK;
}
CommandResult VehicleController::ProcessCommandT(char* line) {
  char* argp = &line[1];

  bool minus = false;
  if (*argp == '-') {
    minus = true;
    argp++;
  }
  // check if numeric arg
  if (*argp < '0' || *argp > '9') {
    return VC_INVALID_PARAM;
  }
  // get float value
  float decval = String(argp).toFloat();
  if (decval < 0.0f || decval > 1.0f) {
    return VC_INVALID_PARAM;
  }
  // bounded by throttle max
  if (decval > max_throttle_) {
    decval = max_throttle_;
  }
  // only update if running
  if (!running_) {
    return VC_INVALID_STATE;
  }
  // update state
  throttle_ = minus ? -decval : decval;
  last_command_ts_ = millis();
  return VC_OK;
}
void VehicleController::ProcessCommandByte(uint8_t udata) {
  char cdata = (char) udata;

  if (cdata >= 0x20 && cdata < 0x80) {
    // got regular character

    // append to cmdbuf_
    if (cmdbufpos_ < (sizeof(cmdbuf_)/sizeof(cmdbuf_[0])) - 1) {
      cmdbuf_[cmdbufpos_++] = cdata;
    } else {
      // no space left in buffer
    }
  } else if (cdata == '\n') {
    // got line feed

    // append null terminator and process line
    cmdbuf_[cmdbufpos_++] = '\0';
    ProcessCommandLine(&cmdbuf_[0]);

    // clear cmdbuf_
    cmdbufpos_ = 0;
  }
}
void VehicleController::ProcessCommandLine(char* line) {
  CommandResult result;

  switch (line[0]) {
  case 'D':
    result = ProcessCommandD(line);
    break;
  case 'I':
    result = ProcessCommandI(line);
    break;
  case 'M':
    result = ProcessCommandM(line);
    break;
  case 'R':
    result = ProcessCommandR(line);
    break;
  case 'S':
    result = ProcessCommandS(line);
    break;
  case 'T':
    result = ProcessCommandT(line);
    break;
  default:
    result = VC_INVALID_COMMAND;
    break;
  }

  if (result == VC_OK) {
    Serial.println("OK");
  } else {
    Serial.print("E");
    Serial.println((int)result);
  }

}
