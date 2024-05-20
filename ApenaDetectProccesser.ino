#include <WiFi.h>

#include "Helper.h"
#include "WsServer.h"
#include "MPU6050.h"
#include "AI.h"
#include "FFT.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

unsigned long nextTime = 0;
unsigned int step = 25;

double predictValue = 0;
bool isSend = false;

sensors_event_t a, g, temp;

const char *ssid = "VIVA_PHONGLANH2.4";
const char *password = "68686868";

int soLanNgungThoLienTuc = 0;

void TaskDataTranfer(void *pvParameters);
void TaskPrintProccesserInfo(void *pvParameters);

void setup() {
  Serial.begin(115200);
  /*_______________________________________WiFi settup______________________________________*/
  /*
  |
  */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  /*
  |
  */
  /*________________________________________________________________________________________*/
  /*_________________________________________WS settup______________________________________*/
  /*
  |
  */
  initWebSocket();
  /*
  |
  */
  /*________________________________________________________________________________________*/

  /*_________________________________________MPU SETUP______________________________________*/
  /*
  |
  */
  SetupMPU();
  /*
  |
  */
  /*________________________________________________________________________________________*/
  /*_________________________________________AI MODEL SETUP______________________________________*/
  /*
  |
  */
  SetupAIModel();
  /*
  |
  */
  /*________________________________________________________________________________________*/

  /*_________________________________________RTOS settup______________________________________*/
  /*
  |
  */
  xTaskCreatePinnedToCore(
    TaskDataTranfer, "Task read and internet", 3 * 1024, NULL, 1  // Priority
    ,
    NULL  // With task handle we will be able to manipulate with this task.
    ,
    ARDUINO_RUNNING_CORE);

  // xTaskCreatePinnedToCore(
  //   TaskPrintProccesserInfo, "Task read and internet", 3 * 1024, NULL, 1  // Priority
  //   ,
  //   NULL  // With task handle we will be able to manipulate with this task.
  //   ,
  //   ARDUINO_RUNNING_CORE);
  /*
  |
  */
  /*________________________________________________________________________________________*/

  Serial.println("FINISH CREATE TASK!");
}

void loop() {
  /*
  * - Cứ sau 25ms thì lấy 1 điểm dữ liệu
  */
  if (millis() < nextTime) {
    return;
  }
  if (nextTime == 0) {
    nextTime = millis();
  }
  nextTime += step;


  mpu.getEvent(&a, &g, &temp);
  addData(a.acceleration.x, a.acceleration.y, a.acceleration.z, g.gyro.x, g.gyro.y, g.gyro.z);

  // Khi data trong mảng đạt đủ 1200 điểm (Khi bắt đầu chạy sau 5s hoặc 1s sau khi detect)
  if (currPos == 1200) {
    // Tính toán min max std mean từ các điểm dữ liệu (1200 điểm) gần đây nhất và lưu vào DATA_FEATURES
    caculateFeature();
    model_input->data.f[0] = DATA_FEATURES[0];
    model_input->data.f[1] = DATA_FEATURES[1];
    model_input->data.f[2] = DATA_FEATURES[2];
    model_input->data.f[3] = DATA_FEATURES[3];

    // Detect
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      error_reporter->Report("Invoke failed on input: %f\n");
    }
    // lấy ra giá trị output: 1: ngưng thở, 0: bình thường
    double predict = model_output->data.f[0];
    Serial.print("\tPREDICT VALUE: ");
    Serial.println(predict);

  //Lấy data trục AZ để tính nhịp thở trong luồng tính nhịp thở
    getAzData();
    //Bật điều khiển thông báo gửi data đến mobile app
    isSend = true;
    predictValue = predict;
  }
}

void TaskDataTranfer(void *pvParameters) {
  for (;;) {
    ws.cleanupClients();
    /*
    * - Nếu detect >= 0.5 thì là đang ngưng thở => số lần ngưng thở (số giây ngưng thở ) + 1
    * - Nếu detect < 0.5 là không ngưng thở thì reset lại số lần ngưng thở về 0
    */
    if (isSend) {
      float breathInMinutes = 0;
      if (predictValue < 0.5) {
        soLanNgungThoLienTuc = 0;
      }
      else{
        soLanNgungThoLienTuc += 1;
      }
      if(soLanNgungThoLienTuc < 10){
        float azMean = arrMean(AZ_DATA, 200);
        for (size_t i = 0; i < 200; i++) {
          AZ_DATA[i] = AZ_DATA[i] - azMean;
        }
        breathInMinutes = getMaxFrequency(AZ_DATA) * 60;
      }

      String content = "{\"nhipTho\": " + String(breathInMinutes) + "}";

      ws.textAll(content);
      
      isSend = false;
    }
  }
}

void TaskPrintProccesserInfo(void *pvParameters) {
  for (;;) {
    printDeviceRam();
    delay(10000); 
  }
}
