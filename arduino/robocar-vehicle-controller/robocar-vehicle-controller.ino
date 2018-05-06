#include "VehicleController.h"

VehicleController vhcl_cntl;

void setup() {
  Serial.begin(115200);
  vhcl_cntl.setup();
}
void loop() {
  vhcl_cntl.loop();
}
