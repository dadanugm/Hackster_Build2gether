#include <Wire.h>
#include "BluetoothA2DPSource.h"
#include "piano16bit.h"
#include "person_sensor.h"

const int32_t SAMPLE_DELAY_MS = 200;
BluetoothA2DPSource a2dp_source;
SoundData *data = new OneChannelSoundData((int16_t*)piano16bit_raw, piano16bit_raw_len/2);
uint8_t detection_count = 0;

void setup() {
  Wire.begin(22,23,100000); // I2C init (SDA,SCL, Freq)
  Serial.begin(9600); // Baudrate UART
    //a2dp_source.set_auto_reconnect(false);
  a2dp_source.start("LEXON MINO");  // Speaker BL Target name
  a2dp_source.set_volume(60); // Set volume
  //a2dp_source.write_data(data);
}

void loop() {
  person_sensor_results_t results = {};
  // Perform a read action on the I2C address of the sensor to get the
  // current face information detected.
  if (!person_sensor_read(&results)) {
    Serial.println("No person sensor results found on the i2c bus");
    delay(SAMPLE_DELAY_MS);
    return;
  }

  Serial.println("********");
  Serial.print(results.num_faces);
  Serial.println(" faces found");
  for (int i = 0; i < results.num_faces; ++i) {
    const person_sensor_face_t* face = &results.faces[i];
    Serial.print("Face #");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(face->box_confidence);
    Serial.print(" confidence, (");
    Serial.print(face->box_left);
    Serial.print(", ");
    Serial.print(face->box_top);
    Serial.print("), (");
    Serial.print(face->box_right);
    Serial.print(", ");
    Serial.print(face->box_bottom);
    Serial.print("), ");
    if (face->is_facing) {
      Serial.println("facing");
      detection_count += 1;
    } else {
      Serial.println("not facing");
    }
  }
  delay(SAMPLE_DELAY_MS);

  // Perform detection sampling
  if (detection_count > 4){
    detection_count = 0;
    a2dp_source.write_data(data); // play music, send to wireless Speaker
    delay(10000); // delay music to play
  }
  
}
