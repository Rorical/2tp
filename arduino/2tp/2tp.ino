#include <sha1.h>
#include <TOTP.h>
uint8_t hmacKey[] = {0x4d, 0x79, 0x4c, 0x65, 0x67, 0x6f, 0x44, 0x6f, 0x6f, 0x72};
TOTP* totp = new TOTP(hmacKey, 10);
void setup() {
  
  Serial.begin(9600);
}

void loop() {
  Serial.println(totp->getCode(12322222));
  delay(5000);

}
