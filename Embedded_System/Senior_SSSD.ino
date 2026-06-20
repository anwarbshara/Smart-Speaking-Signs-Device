#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define MPU_addr 0x68
// إعداد BLE
BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

// UUIDs يجب أن تطابق ما في تطبيق Flutter
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"



bool imuAvailable = false;
const int flexPins[5] = {32, 33, 34, 35, 36};
int flexReadings[5];
int16_t accel[3];
int16_t gyro[3];

unsigned long lastReadTime = 0;
const unsigned long readInterval = 5000;
int readCounter = 0;
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("✅ Connected");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("❌ Disconnected");
  }
};

struct Gesture {
  String code;
  int flexMin[5];
  int flexMax[5];
  int16_t accelMin[3];
  int16_t accelMax[3];
  int16_t gyroMin[3];
  int16_t gyroMax[3];
};

const Gesture gestures[] = {
  { "000001",  // 1- شكراً
    {3145,  3057, 3195, 2755, 2791},   // flexMin[]
    {3262,  3071, 3203, 2763, 2985},   // flexMax[]
    {-4536, -9664, 12260},             // accelMin[]
    {-4188, -9508, 12424},             // accelMax[]
    {-215,  102,  -65},                // gyroMin[]
    {-45,   218,   45}                 // gyroMax[]
  },
  { "000010",  // 2- دكتور
      {3660, 2960, 3630, 3290, 2770},    // flexMin[] = original min - 50
      {3770, 3074, 4064, 3443, 2895},    // flexMax[] = original max + 50
      {4068, -14736, 5432},              // accelMin[] = original min - 200
      {4716, -13596, 7024},              // accelMax[] = original max + 200
      {-407,  27,  -436},                // gyroMin[] = original min - 100
      {571,  408, 166}                   // gyroMax[] = original max + 100
  },
  { "000011",  // 3- العيون
    {3487, 2885, 3469, 3178, 3166},    // flexMin[] = min - 50
   {4009, 2996, 3764, 3357, 3379},    // flexMax[] = max + 50
   {1400, -16372, -780},              // accelMin[] = min - 200
   {1996, -15840, -188},              // accelMax[] = max + 200
   {-263,  76, -132},                 // gyroMin[] = min - 100
  {  34, 317, 109}                   // gyroMax[] = max + 100
  },
  { "000100",  // 4- الألم
    {3611, 3516, 3646, 3277, 3111},    // flexMin[] = min - 50
  {3740, 3671, 3851, 3399, 3217},    // flexMax[] = max + 50
  {1120, -15584, -5668},            // accelMin[] = min - 200
  {1740, -15092, -5064},            // accelMax[] = max + 200
  {-319,   -10, -129},              // gyroMin[] = min - 100
  {  61,  373,  177}                // gyroMax[] = max + 100
  },
  { "000101",  // 5- كثير
    {3582, 2920, 3565, 3230, 2778},   // flexMin = min - 50
  {3787, 3042, 3937, 3439, 2919},   // flexMax = max + 50
  {712, -16144, 4156},              // accelMin = min - 200
  {2568, -14932, 5896},             // accelMax = max + 200
  {-412, -435, -820},               // gyroMin = min - 100
  {476,  704,  208}                 // gyroMax = max + 100
  },
  { "000110",  // 6- اليوم
    {3582, 2869, 3513, 3272, 2773},    // flexMin = min - 50
  {3719, 2999, 3938, 3407, 2947},    // flexMax = max + 50
  {1820, 12804, 9256},               // accelMin = min - 200
  {3100, 13528, 10356},              // accelMax = max + 200
  {-352,  -59, -178},                // gyroMin = min - 100
  {122,   489, 179}                  // gyroMax = max + 100
  },
  { "000111",  // 7- مكان
    {3385, 3254, 3438, 2869, 2773},   // flexMin[] = min - 50
  {3572, 3385, 3554, 2991, 2900},   // flexMax[] = max + 50
  {276,  4668, 15404},              // accelMin[] = min - 200
  {1384, 5320, 16032},              // accelMax[] = max + 200
  {-365,  90, -126},                // gyroMin[] = min - 100
  { 92,  399, 178}                  // gyroMax[] = max + 100
  },
  { "001000",  // 8- أين
    {3123, 2926, 3342, 2884, 2814},   // flexMin = min - 50
  {3373, 3042, 3714, 3029, 3009},   // flexMax = max + 50
  {2868, -15012, -7800},            // accelMin = min - 200
  {5820, -12876, -6816},            // accelMax = max + 200
  {-644, -357, -983},               // gyroMin = min - 100
  {368, 649, 453}                   // gyroMax = max + 100
  },
  { "001001",  // 9- لو سمحت
    {3487, 4000, 3600, 3200, 3200},   // flexMin = min - 50
  {3716, 4095, 3715, 3300, 3300},   // flexMax = max + 50
  {1412, -14664, -9068},            // accelMin = min - 200
  {2816, -13324, -7328},            // accelMax = max + 200
  {-1152, -327, -510},              // gyroMin = min - 100
  {277, 750, 323}                   // gyroMax = max + 100
  },
  { "001010",  // 10- أشعر
    {2990, 4090, 3135, 2676, 2700},    // flexMin = min - 50
  {3099, 4099, 3450, 2725, 2840},    // flexMax = max + 50
  {11596, -7652, 7880},              // accelMin = min - 200
  {12748, -6460, 9232},              // accelMax = max + 200
  {-585, -257, -302},                // gyroMin = min - 100
  {368, 743, 255}                    // gyroMax = max + 100
  },
   { "001011",  // 11- الدفع
    {3180, 3091, 3533, 3295, 2638},     // flexMin = min - 50
  {3317, 3215, 3804, 3424, 2851},     // flexMax = max + 50
  {2088, -7684, -15524},             // accelMin = min - 200
  {3472, -6776, -14428},             // accelMax = max + 200
  {-502, -31, -211},                 // gyroMin = min - 100
  {243, 580, 273}                    // gyroMax = max + 100
  },
   { "001100",  // 12- انابخير
   {2955, 3204, 2951, 2650, 2782},     // flexMin[]
  {3240, 3358, 4145, 2776, 2919},     // flexMax[]
  {-2720, -16668, -4144},             // accelMin[]
  {1620, -15444, 636},                // accelMax[]
  {-471, -4059, -960},                // gyroMin[]
  {1353, 365, 1117}                   // gyroMax[]
  },
   { "001101",  // 13- اريد التحدث مع الطبيب
    {3310, 3320, 3460, 2260, 2700}, 
    {3400, 3410, 3590, 2760, 2900},
    {1160, -8100, 7600}, 
    {1880, -7600, 8400},
    {-4800, 100, -60}, 
    {90, 270, 90}
   },
   { "001110",  // 14- مرحبا
    {3020, 4000, 3170, 2700, 2700},    // flexMin[]
  {3090, 4095, 3300, 2770, 2850},    // flexMax[]
  {1052, -16300, -2704},             // accelMin[]
  {1728, -15752, -1836},             // accelMax[]
  {-287, 0, -190},                   // gyroMin[]
  {144, 506, 262}                    // gyroMax[]
   },
   { "001111",  // 15- عمل جيد
   {3500, 4000, 3400, 3200, 2790},     // flexMin[]
  {3600, 4095, 3900, 3300, 2900},     // flexMax[]
  {13872, -8600, -168},              // accelMin[]
  {15904, -6188, 2328},              // accelMax[]
  {-900, -1991, -1313},              // gyroMin[]
  {3380, 1113, 1507}                 // gyroMax[]
  },
   { "010000",  // 16- الم في البطن 
    {2972, 2830, 3015, 2652, 2800},
  {3170, 4099, 3290, 2900, 3117},
  {16084, 292, -1416},
  {16796, 1080, -52},
  {-573, -15, -213},
  {175, 377, 202}
  },
   { "010001",  // 17- صعوبة في التنفس
    {3193, 2986, 3045, 2318, 2716},     // flexMin[]
  {3427, 3329, 3442, 2754, 2875},     // flexMax[]
  {10032, -12348, 2588},              // accelMin[]
  {10768, -11752, 3532},              // accelMax[]
  {-318, -183, -173},                 // gyroMin[]
  {58,   420, 220}                    // gyroMax[]
  },
   { "010010",  // 18- أريد التكلم
    {2967, 2875, 2915, 2645, 2765},      // flexMin[]
  {3093, 3031, 3253, 2767, 2925},      // flexMax[]
  {-1192, -13580, 8432},               // accelMin[]
  {340, -12208, 10768},                // accelMax[]
  {-947, 96, -458},                    // gyroMin[]
  {1235, 484, 1055}                    // gyroMax[]
  },
   { "010011",  // 19- سؤال
     {3580, 3198, 3603, 3244, 3115},
  {3732, 3311, 3768, 3367, 3244},
  {1000, -16444, 764},
  {3404, -15516, 2108},
  {-707, -422, -225},
  {281, 971, 392}
  },
   { "010100",  // 20- أراك لاحقا
    {3511, 2860, 3505, 3225, 2771},     // flexMin[]
  {3708, 3028, 3847, 3449, 2916},     // flexMax[]
  {11212, -11184, 1948},             // accelMin[]
  {13236, -9240, 4208},              // accelMax[]
  {-1066, -75, -656},                // gyroMin[]
  {294, 529, 213}                    // gyroMax[]
  },
   { "010101",  // 21- #2
    {3021, 2881, 3454, 2907, 2968},     // flexMin[]
  {3279, 3013, 3844, 3121, 3121},     // flexMax[]
  {468, -16496,  980},               // accelMin[]
  {1576, -15512, 2292},              // accelMax[]
  {-880, -88, -207},                 // gyroMin[]
  {212, 450, 545}                    // gyroMax[]
  },
   { "010110",  // 22- #3
     {3005, 2826, 3214, 3007, 2820},     // flexMin[]
  {3453, 2969, 3711, 3152, 2949},     // flexMax[]
  {-1728, -16292, -6980},             // accelMin[]
  {572, -14704, -2068},               // accelMax[]
  {-857, -186, -663},                 // gyroMin[]
  {549, 500, 298}                     // gyroMax[]
  },
   { "010111",  // 23- #5
    {3646, 3116, 3582, 3111, 2846},        // flexMin[]
  {3936, 3328, 3935, 3312, 2996},        // flexMax[]
  {-2888, -16832, -4992},                // accelMin[]
  {2744, -14920, 1420},                  // accelMax[]
  {-492, -8305, -2539},                  // gyroMin[]
  {603, 554, 229}                        // gyroMax[]
  },
   { "011000",  // 24- #10
    {3662, 2862, 3523, 2692, 2928},        // flexMin[]
  {3842, 3041, 3927, 2848, 3069},        // flexMax[]
  {-3328, -16720, -4036},                // accelMin[]
  {-1560, -14892, -764},                 // accelMax[]
  {-917, -330, -306},                    // gyroMin[]
  {644, 818, 473}                        // gyroMax[]
  },
   { "011001",  // 25- من
     {3552, 3500, 3383, 1973, 2880},        // flexMin[]
  {3792, 3730, 3751, 2840, 3357},        // flexMax[]
  {920, -18528, -9292},                  // accelMin[]
  {4752, -13108, -1044},                 // accelMax[]
  {-770, -640, -1101},                   // gyroMin[]
  {807, 12408, 6196}                     // gyroMax[]
  },
   { "011010",  // 26- مستشفى 
  {3021, 2942, 3470, 3216, 2820},       // flexMin[]
  {3185, 3092, 3747, 3364, 2962},       // flexMax[]
  {13708, -4868, 5448},                 // accelMin[]
  {15472, -3656, 6840},                 // accelMax[]
  {-323, -37, -81},                     // gyroMin[]
  {66, 418, 254}                        // gyroMax[]
  },
   { "011011",  // 27- #6
     {2917, 2856, 3150, 3277, 2700},       // flexMin[]
  {3192, 3039, 3444, 3497, 2979},       // flexMax[]
  {400, -16476, -4952},                // accelMin[]
  {2800, -15120, -2028},               // accelMax[]
  {-722, -159, -205},                  // gyroMin[]
  {28, 511, 228}                       // gyroMax[]
  },
   { "011100",  // 28- #7
    {2945, 2897, 3589, 2677, 2796},       // flexMin[]
  {3118, 3137, 4145, 2809, 3010},       // flexMax[]
  {468, -16564, -3972},                // accelMin[]
  {2600, -15220, -1856},               // accelMax[]
  {-486, -134, -288},                  // gyroMin[]
  {553, 583, 158}                      // gyroMax[]
  },
   { "011101",  // 29- #8
    {3150, 3600, 3000, 3150, 400}, 
    {3300, 3750, 3200, 3250, 650},
    {1160, -8100, 7600}, 
    {1880, -7600, 8400},
    {-4800, 100, -60}, 
    {90, 270, 90}
  },
   { "011110",  // 30- مراقبة
    {3395, 4045, 3394, 2847, 2846},      // flexMin[]
  {3618, 4095, 3709, 3111, 3042},      // flexMax[]
  {-932, -16684, 896},                 // accelMin[]
  {1708, -15324, 3092},                // accelMax[]
  {-535, -320, -423},                  // gyroMin[]
  {526, 829, 552}                      // gyroMax[]
  },
   { "011111",  // 31- االاحد
     {3006, 4045, 3476, 2814, 3111},     // flexMin[]
  {3256, 4095, 3699, 2944, 3267},     // flexMax[]
  {-184, -16688, -4972},              // accelMin[]
  {4332, -15212, -320},               // accelMax[]
  {-1195, -162, -511},                // gyroMin[]
  {891, 1203, 1184}                   // gyroMax[]
  },
    { "100000",  // 32- اثنين
    {3157, 4045, 3245, 2703, 2980},     // flexMin[]
  {3320, 4095, 3489, 2848, 3369},     // flexMax[]
  {-1628, -16440, -5436},             // accelMin[]
  {1220, -14936, -3132},              // accelMax[]
  {-582, -804, -754},                 // gyroMin[]
  {427, 833, 584}                     // gyroMax[]
  },
  { "100001",  // 33- ثلاثا
    {3150, 3600, 3000, 3150, 400},
    {3300, 3750, 3200, 3250, 650},
    {1160, -8100, 7600},
    {1880, -7600, 8400},
    {-4800, 100, -60},
    {90, 270, 90}
  },
  { "100010",  // 34-خميس
    {3470, 3134, 3560, 3193, 2765},     // flexMin[]
  {3654, 4145, 3697, 3330, 2900},     // flexMax[]
  {2872, -16320, 4},                  // accelMin[]
  {7772, -13764, 3588},               // accelMax[]
  {-1098, -1819, -824},               // gyroMin[]
  {1085, 1736, 1452}                  // gyroMax[]
  },
  { "100011",  // 35- قليل
    {3570, 4000, 3740, 3260, 2750},     // flexMin[]
{3690, 4120, 3850, 3375, 2950},     // flexMax[]
{1100, -16250, 700},                // accelMin[]
{1450, -14600, 1650},               // accelMax[]
{-1215, 150, -1350},                // gyroMin[]
{500, 700, 150}                     // gyroMax[]
  },
  { "100100",  // 36- العصر
    {3000, 4000, 3140, 2735, 2800},     // flexMin[]
{3150, 4120, 3210, 2775, 2820},     // flexMax[]
{950, -16200, -2600},              // accelMin[]
{2700, -15800, 1000},              // accelMax[]
{-180, 120, -90},                  // gyroMin[]
{600, 310, 340}                    // gyroMax[]

  },
  { "100101",  // 37- استمر بالقوة
    {3450, 4080, 3400, 2680, 2850},      // flexMin[]
{3680, 4120, 3700, 2750, 2950},      // flexMax[]
{-200, -9800, -13750},               // accelMin[]
{250, -9400, -13600},                // accelMax[]
{-350, 140, -90},                    // gyroMin[]
{0, 330, 60}                         // gyroMax[]
  },
  { "100110",  // 38- ماذا تريد
    {3240, 3100, 3200, 2765, 2785},     // flexMin[]
{3310, 3190, 3430, 2820, 2920},     // flexMax[]
{4700, -4000, -16050},              // accelMin[]
{5500, -2200, -15500},              // accelMax[]
{-570, -50, -160},                  // gyroMin[]
{770, 750, 420}                     // gyroMax[]

  },
  { "100111",  // 39- هذا رهيب
    {3215, 4080, 3180, 2650, 2730},     // flexMin[]
{3285, 4145, 3360, 2730, 2910},     // flexMax[]
{-3600, -15180, -6200},             // accelMin[]
{-2500, -14700, -5250},             // accelMax[]
{-540, -230, -600},                 // gyroMin[]
{500, 520, 200}                     // gyroMax[]

  },
  { "101000",  // 40- محبة
    {3150, 3600, 3000, 3150, 400},
    {3300, 3750, 3200, 3250, 650},
    {1160, -8100, 7600},
    {1880, -7600, 8400},
    {-4800, 100, -60},
    {90, 270, 90}
  }
};


void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  if (Wire.endTransmission() == 0) {
    imuAvailable = true;
    Serial.println("✔️ MPU6050 متصل بنجاح!");
  } else {
    Serial.println("⚠️ MPU6050 غير متصل");
  }

  Serial.println("✅ الكود بدأ تشغيله بنجاح!");

  BLEDevice::init("SSSD");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("📡 BLE Advertising started...");
}



void loop() {
  if (millis() - lastReadTime >= readInterval) {
    lastReadTime = millis();
    readCounter++;

  if (readCounter >= 6) {
  Serial.println("📦 تم أخذ 6 قراءات");
  readCounter = 0;  // إعادة العد
}

    readSensors();

    String code = detectGesture();

    if (code != "") {
      Serial.println("✅ تم التعرف على إشارة: " + code);
    } else {
      Serial.println("❌ لا يوجد تطابق - سيتم إرسال 000000");
    }

    if (deviceConnected) {
      pCharacteristic->setValue(code != "" ? code.c_str() : "000000");
      pCharacteristic->notify();
      Serial.println("📤 تم الإرسال عبر BLE: " + String(code != "" ? code : "000000"));
    }

    Serial.println("------");
  }
}


void readSensors() {
  Serial.println("---- Flex Sensors ----");
  for (int i = 0; i < 5; i++) {
    flexReadings[i] = analogRead(flexPins[i]);
    Serial.print("Flex["); Serial.print(i); Serial.print("] = ");
    Serial.println(flexReadings[i]);
  }

  if (imuAvailable) {
    Serial.println("---- MPU6050 ----");
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14, true);

    accel[0] = Wire.read() << 8 | Wire.read();
    accel[1]= Wire.read() << 8 | Wire.read();
    accel[2] = Wire.read() << 8 | Wire.read();
    Wire.read(); Wire.read(); // تجاهل الحرارة
    gyro[0]= Wire.read() << 8 | Wire.read();
    gyro[1] = Wire.read() << 8 | Wire.read();
    gyro[2] = Wire.read() << 8 | Wire.read();

    Serial.print("Accel: X="); Serial.print(accel[0]);
    Serial.print(" | Y="); Serial.print(accel[1]);
    Serial.print(" | Z="); Serial.println(accel[2]);

    Serial.print("Gyro: X="); Serial.print(gyro[0]);
    Serial.print(" | Y="); Serial.print(gyro[1]);
    Serial.print(" | Z="); Serial.println(gyro[2]);
  }else {
    Serial.println("⚠️ MPU6050 ");
  }

  Serial.println("------------------------");
}

String detectGesture() {
  for (int i = 0; i < sizeof(gestures) / sizeof(Gesture); i++) {
    bool flexMatch = true;
    for (int j = 0; j < 5; j++) {
      if (flexReadings[j] < gestures[i].flexMin[j] || flexReadings[j] > gestures[i].flexMax[j]) {
        flexMatch = false;
        break;
      }
    }

    if (flexMatch) {
      if (imuAvailable) {
        bool imuMatch = true;
        for (int j = 0; j < 3; j++) {
          if (accel[j] < gestures[i].accelMin[j] || accel[j] > gestures[i].accelMax[j])
            imuMatch = false;
          if (gyro[j] < gestures[i].gyroMin[j] || gyro[j] > gestures[i].gyroMax[j])
            imuMatch = false;
        }

        if (imuMatch) {
          Serial.println("🌟 تطابق عالي الدقة (مع IMU).");
        } else {
          Serial.println("⚠️ تطابق بالفليكس فقط (بدون IMU).");
        }
      }

      return gestures[i].code;
    }
  }

  return "";  // لا يوجد تطابق
}