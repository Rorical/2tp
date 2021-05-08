#include <sha1.h>
#include <TOTP.h>
#include <MD5.h>
#include <FlashStorage.h>

const int maxKeys = 15;
const int maxKeyLength = 30;

typedef struct {
  String Name;
  String Description;
  int keyLength;
  uint8_t Key[maxKeyLength];
} Key;

typedef struct {
  int keysLength;
  Key keys[maxKeys];
  String pass;
} Keys;

FlashStorage(store, Keys);

String buff;
char charbuff[100];
Keys keys;
TOTP* totp;
MD5  hashMD5;

void setup() {
  SerialUSB.setTimeout(27000);
  SerialUSB.begin(9600);
  keys = store.read();
  while (!SerialUSB) { }
  String password = "password";
  String seed;
  while (!password.equals(buff)) {
    seed = SerialUSB.readStringUntil(' ');
    buff = SerialUSB.readStringUntil('\n');
    seed.concat(keys.pass);
    SerialUSB.println(keys.pass);
    seed.toCharArray(charbuff, 100);
    password = String(hashMD5.md5(charbuff));
  }
  //free(&password);
  //free(&seed); //为毛加上这句话就不行?? 哪位大神可以告诉我.
}

void loop() {
  if (SerialUSB.available()) {
    buff = SerialUSB.readStringUntil('\n');
    if (buff.equals("AddConfig")) {
      Key newKey;
      newKey.Name = SerialUSB.readStringUntil('\n');
      newKey.Description = SerialUSB.readStringUntil('\n');
      newKey.keyLength = SerialUSB.readBytesUntil('\n', newKey.Key, maxKeyLength);
      if (keys.keysLength >= maxKeys) {
        SerialUSB.println("Error: Storage Full");
      } else {
        keys.keys[keys.keysLength ++] = newKey;
        store.write(keys);
        SerialUSB.println(keys.keysLength);
      }
      free(&newKey);
    } else if (buff.equals("GetConfig")) {
      buff = SerialUSB.readStringUntil('\n');
      int pt = buff.toInt();
      if (pt < keys.keysLength) {
        SerialUSB.println(keys.keys[pt].Name);
        SerialUSB.println(keys.keys[pt].Description);
      }
      free(&pt);
    } else if (buff.equals("GetCode")) {
      buff = SerialUSB.readStringUntil('\n');
      int pt = buff.toInt();
      buff = SerialUSB.readStringUntil('\n');
      long int timestamp = buff.toInt();
      if (pt < keys.keysLength) {
        totp = new TOTP(keys.keys[pt].Key, keys.keys[pt].keyLength);
        SerialUSB.println(totp->getCode(timestamp));
      }
      free(&pt);
      free(&timestamp);
    } else if (buff.equals("GetAllConfig")) {
      for (int pt = 0; pt < keys.keysLength; pt ++) {
        SerialUSB.println(keys.keys[pt].Name);
        SerialUSB.println(keys.keys[pt].Description);
      }
    } else if (buff.equals("GetAllCode")) {
      buff = SerialUSB.readStringUntil('\n');
      long int timestamp = buff.toInt();
      for (int pt = 0; pt < keys.keysLength; pt ++) {
        totp = new TOTP(keys.keys[pt].Key, keys.keys[pt].keyLength);
        SerialUSB.println(totp->getCode(timestamp));
      }
      free(&timestamp);
    } else if (buff.equals("DeleteConfig")) {
      buff = SerialUSB.readStringUntil('\n');
      int pt = buff.toInt();
      if (keys.keysLength > 0 && pt < keys.keysLength) {
        for (int i = pt; i < maxKeys - 1; i++) keys.keys[i] = keys.keys[i + 1];
        keys.keysLength --;
        store.write(keys);
      }
      free(&pt);
    } else if (buff.equals("SetConfig")) {
      buff = SerialUSB.readStringUntil('\n');
      int pt = buff.toInt();
      if (pt < keys.keysLength) {
        keys.keys[pt].Name = SerialUSB.readStringUntil('\n');
        keys.keys[pt].Description = SerialUSB.readStringUntil('\n');
        keys.keys[pt].keyLength = SerialUSB.readBytesUntil('\n', keys.keys[pt].Key, maxKeyLength);
        store.write(keys);
      }
      free(&pt);
    }else if (buff.equals("SetPassWord")) {
      keys.pass = SerialUSB.readStringUntil('\n');
      store.write(keys);
    }
  }
  delay(1000);
}
