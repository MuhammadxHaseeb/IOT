#include <ArduinoWebsockets.h>
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "Arduino.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include "fr_flash.h"

// --- FIREBASE LIBRARIES ---
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// ==========================================
//            FIREBASE CONFIGURATION
// ==========================================
#define API_KEY "AIzaSyA3PYNmydTmKsf2DJNNaX0zx5e-nWKZRpo"

#define DATABASE_URL "e-lock-1d607-default-rtdb.asia-southeast1.firebasedatabase.app"

// Firebase Objects (No Auth objects needed anymore)
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ==========================================
//              USER CONFIGURATION
// ==========================================
const char* ssid = "Pixel :3";
const char* password = "6fm82ifndqs22ck";

// Camera Model
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h" 

// Hardware Pins
#define relay_pin 2       
#define green_led_pin 12  
#define red_led_pin 14
#define FLASH_PIN 4        
#define FLASH_CHANNEL 2    

// Face Recognition Settings
#define ENROLL_CONFIRM_TIMES 5
#define FACE_ID_SAVE_NUMBER 7

// ==========================================
//               WEB INTERFACE
// ==========================================
// PASTE YOUR ORIGINAL HTML CONST CHAR INDEX_HTML[] HERE
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SECURE ACCESS TERMINAL</title>
    <style>
        :root {
            --bg-color: #050505;
            --panel-bg: rgba(20, 20, 20, 0.95);
            --neon-blue: #00f3ff;
            --neon-green: #0aff0a;
            --neon-red: #ff003c;
            --neon-amber: #ffaa00;
            --text-main: #e0e0e0;
            --font-tech: 'Courier New', Courier, monospace;
        }
        
        * { box-sizing: border-box; transition: all 0.2s ease; }

        body {
            font-family: var(--font-tech);
            background-color: var(--bg-color);
            color: var(--text-main);
            margin: 0;
            padding: 20px;
            display: flex;
            flex-direction: column;
            align-items: center;
            min-height: 100vh;
            background-image: 
                linear-gradient(rgba(0, 243, 255, 0.03) 1px, transparent 1px),
                linear-gradient(90deg, rgba(0, 243, 255, 0.03) 1px, transparent 1px);
            background-size: 30px 30px;
        }

        /* HEADER */
        header {
            width: 100%;
            padding: 20px;
            border-bottom: 2px solid var(--neon-blue);
            text-align: center;
            text-shadow: 0 0 10px var(--neon-blue);
            letter-spacing: 4px;
            background: rgba(0,0,0,0.8);
            margin-bottom: 20px;
            box-shadow: 0 0 20px rgba(0, 243, 255, 0.1);
        }
        h1 { margin: 0; font-size: 1.5rem; text-transform: uppercase; }
        .status-dot {
            display: inline-block; width: 10px; height: 10px;
            background: var(--neon-green); border-radius: 50%;
            box-shadow: 0 0 10px var(--neon-green); margin-right: 10px;
            animation: pulse 2s infinite;
        }

        /* MAIN CONTAINER */
        .main-interface {
            display: grid;
            grid-template-columns: 1fr;
            gap: 20px;
            width: 95%;
            max-width: 900px;
        }
        @media(min-width: 800px) { .main-interface { grid-template-columns: 1.5fr 1fr; } }

        /* PANELS */
        .panel {
            background: var(--panel-bg);
            border: 1px solid #333;
            box-shadow: 0 0 15px rgba(0,0,0,0.5);
            padding: 15px;
            position: relative;
        }
        .panel::before {
            content: ""; position: absolute; top: 0; left: 0; right: 0; height: 2px;
            background: var(--neon-blue); box-shadow: 0 0 10px var(--neon-blue);
        }
        h2 {
            margin: 0 0 15px 0; font-size: 1rem; color: var(--neon-blue);
            text-transform: uppercase; letter-spacing: 2px;
            border-bottom: 1px solid #333; padding-bottom: 5px;
        }

        /* VIDEO FEED */
        .video-wrapper {
            position: relative;
            border: 1px solid var(--neon-blue);
            padding: 5px;
            box-shadow: 0 0 15px rgba(0, 243, 255, 0.1);
            background: #000;
        }
        .video-wrapper::after { /* Scanline */
            content: " "; display: block; position: absolute;
            top: 0; left: 0; bottom: 0; right: 0;
            background: linear-gradient(rgba(18, 16, 16, 0) 50%, rgba(0, 0, 0, 0.25) 50%), linear-gradient(90deg, rgba(255, 0, 0, 0.06), rgba(0, 255, 0, 0.02), rgba(0, 0, 255, 0.06));
            z-index: 2; background-size: 100% 2px, 3px 100%; pointer-events: none;
        }
        img { width: 100%; display: block; }

        /* LOG BOX */
        #log-display {
            font-size: 0.9rem; color: var(--neon-green);
            background: #000; border: 1px solid #333;
            padding: 10px; margin-top: 15px; height: 40px;
            display: flex; align-items: center; justify-content: center;
            text-transform: uppercase; letter-spacing: 1px;
        }

        /* --- BUTTONS --- */
        button { cursor: pointer; font-family: var(--font-tech); text-transform: uppercase; font-weight: bold; }
        button:active { transform: scale(0.98); }

        /* GIANT UNLOCK BUTTON */
        .btn-override {
            width: 100%; padding: 20px; margin-top: 20px;
            background: rgba(0, 255, 10, 0.05);
            border: 2px solid var(--neon-green);
            color: var(--neon-green);
            font-size: 1.2rem; letter-spacing: 3px;
            transition: all 0.3s;
            box-shadow: 0 0 10px rgba(10, 255, 10, 0.1);
            animation: breathe 3s infinite;
        }
        .btn-override:hover {
            background: var(--neon-green); color: #000;
            box-shadow: 0 0 30px var(--neon-green);
        }

        /* SECONDARY BUTTONS */
        .sub-controls { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 15px; }
        .btn-sec {
            padding: 12px; background: #111;
            border: 1px solid #444; color: #888;
        }
        .btn-sec:hover { border-color: var(--neon-blue); color: var(--neon-blue); box-shadow: 0 0 10px rgba(0, 243, 255, 0.2); }
        .btn-danger { border-color: #500; color: #a00; }
        .btn-danger:hover { background: var(--neon-red); color: #fff; border-color: var(--neon-red); box-shadow: 0 0 20px var(--neon-red); }

        /* FLASH CONTROL */
        .flash-section {
            margin-top: 20px; border-top: 1px solid #333; padding-top: 15px;
        }
        .flash-header {
            display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;
            color: var(--neon-amber); font-size: 0.9rem;
        }
        /* SLIDER */
        input[type=range] {
            -webkit-appearance: none; width: 100%; background: transparent;
        }
        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none; height: 15px; width: 15px;
            border: 1px solid var(--neon-amber); background: #000;
            cursor: pointer; margin-top: -5px; box-shadow: 0 0 10px var(--neon-amber);
        }
        input[type=range]::-webkit-slider-runnable-track {
            width: 100%; height: 5px; cursor: pointer;
            background: #333; border: 1px solid #444;
        }

        /* USER LIST */
        #user-list { list-style: none; padding: 0; margin: 0; max-height: 400px; overflow-y: auto; }
        .user-item {
            display: flex; justify-content: space-between; align-items: center;
            background: rgba(255,255,255,0.03); margin-bottom: 5px;
            padding: 8px 12px; border-left: 2px solid #333;
        }
        .user-item:hover { border-left-color: var(--neon-blue); background: rgba(0, 243, 255, 0.1); }
        .btn-del {
            background: transparent; border: 1px solid var(--neon-red);
            color: var(--neon-red); padding: 2px 8px; font-size: 0.8rem;
        }
        .btn-del:hover { background: var(--neon-red); color: white; box-shadow: 0 0 10px var(--neon-red); }

        /* ENROLL AREA */
        .enroll-area { margin-top: 20px; border-top: 1px solid #333; padding-top: 15px; display: flex; gap: 5px; }
        input[type="text"] {
            flex-grow: 1; padding: 10px; background: #000;
            border: 1px solid var(--neon-blue); color: var(--neon-blue);
            font-family: var(--font-tech);
        }
        .btn-go { background: var(--neon-blue); border: none; color: #000; font-weight: bold; padding: 0 20px; }
        .btn-go:hover { background: #fff; box-shadow: 0 0 15px white; }

        @keyframes pulse { 0% { opacity: 1; } 50% { opacity: 0.5; } 100% { opacity: 1; } }
        @keyframes breathe { 0% { box-shadow: 0 0 10px rgba(10, 255, 10, 0.1); } 50% { box-shadow: 0 0 25px rgba(10, 255, 10, 0.3); } 100% { box-shadow: 0 0 10px rgba(10, 255, 10, 0.1); } }
    </style>
</head>
<body>

    <header>
        <h1><span class="status-dot"></span>SECURE GATEWAY</h1>
    </header>

    <div class="main-interface">
        
        <div class="panel">
            <h2>Live Feed // Cam_01</h2>
            <div class="video-wrapper">
                <img id="stream" src="" alt="INITIALIZING VIDEO LINK...">
            </div>

            <div id="log-display">SYSTEM STANDBY</div>

            <button class="btn-override" onclick="sendCommand('manual_open')">
                [ OVERRIDE LOCK ]
            </button>

            <div class="flash-section">
                <div class="flash-header">
                    <span>ILLUMINATION CONTROL</span>
                    <button class="btn-sec" style="color:var(--neon-amber); border-color:var(--neon-amber); padding:5px 15px;" onclick="toggleFlash()">ON/OFF</button>
                </div>
                <input type="range" id="flashSlider" min="0" max="255" value="0" oninput="updateFlash(this.value)">
            </div>
        </div>

        <div class="panel">
            <h2>Authorized Personnel</h2>
            
            <ul id="user-list">
                <li style="text-align:center; color:#555; padding:20px;">Scanning Database...</li>
            </ul>

            <div class="enroll-area">
                <input type="text" id="face_name" placeholder="IDENTITY_NAME">
                <button class="btn-go" onclick="startEnroll()">ADD</button>
            </div>
            
            <button onclick="if(confirm('WARNING: PURGE ALL DATA?')) sendCommand('delete_all')" 
                    style="width:100%; margin-top:15px; padding:15px; background:none; border:2px solid #500; color:#c00; cursor:pointer; font-weight:900; letter-spacing:1px; text-transform:uppercase;"
                    onmouseover="this.style.background='#300'; this.style.color='#f00'" onmouseout="this.style.background='transparent'; this.style.color='#c00'">
                ⚠ PURGE ALL DATA
            </button>
        </div>

    </div>

<script>
    var socket = new WebSocket("ws://" + window.location.hostname + ":82");
    
    socket.onmessage = function(event) {
        if (event.data instanceof Blob) {
            var urlObject = URL.createObjectURL(event.data);
            document.getElementById("stream").src = urlObject;
        } else {
            var msg = event.data;
            var log = document.getElementById("log-display");
            
            if (msg === "delete_faces") {
                document.getElementById("user-list").innerHTML = "";
            }
            else if (msg.startsWith("listface:")) {
                addUserToList(msg.substring(9));
            }
            else if (msg === "door_open") {
                log.style.color = "var(--neon-green)";
                log.innerText = "ACCESS GRANTED";
            }
            else if (msg.includes("FACE NOT")) {
                log.style.color = "var(--neon-red)";
                log.innerText = "ACCESS DENIED // UNAUTHORIZED";
            }
            else if (msg !== "detecting" && msg !== "streaming") {
                log.style.color = "var(--neon-blue)";
                log.innerText = msg;
            }
        }
    };

    socket.onopen = function() {
        document.getElementById("log-display").innerText = "CONNECTION ESTABLISHED";
        sendCommand('recognise');
    };

    function sendCommand(cmd) { socket.send(cmd); }

    function startEnroll() {
        var name = document.getElementById("face_name").value;
        if(name) {
            sendCommand("capture:" + name);
            document.getElementById("face_name").value = "";
        } else { alert("ENTER NAME"); }
    }

    function removeUser(name) {
        if(confirm("Revoke access for " + name + "?")) {
            sendCommand("remove:" + name);
        }
    }

    function addUserToList(name) {
        var list = document.getElementById("user-list");
        if(list.firstElementChild && list.firstElementChild.innerText.includes("Scanning")) {
            list.innerHTML = "";
        }
        var li = document.createElement("li");
        li.className = "user-item";
        li.innerHTML = `<span>${name}</span><button class="btn-del" onclick="removeUser('${name}')">X</button>`;
        list.appendChild(li);
    }

    // FLASH LOGIC
    var flashState = false;
    function updateFlash(val) {
        sendCommand("flash:" + val);
    }
    function toggleFlash() {
        flashState = !flashState;
        var val = flashState ? 255 : 0;
        document.getElementById("flashSlider").value = val;
        updateFlash(val);
    }
</script>
</body>
</html>
)rawliteral";

// ==========================================
//            MAIN LOGIC & VARIABLES
// ==========================================
using namespace websockets;
WebsocketsServer socket_server;

camera_fb_t * fb = NULL;

long current_millis;
long last_detected_millis = 0;

unsigned long door_opened_millis = 0;
unsigned long red_led_millis = 0;
long interval = 5000;           
long red_led_duration = 2000;   

bool face_recognised = false;

void app_facenet_main();
void app_httpserver_init();

typedef struct
{
  uint8_t *image;
  box_array_t *net_boxes;
  dl_matrix3d_t *face_id;
} http_img_process_result;

static inline mtmn_config_t app_mtmn_config()
{
  mtmn_config_t mtmn_config = {0};
  mtmn_config.type = FAST;
  mtmn_config.min_face = 80;
  mtmn_config.pyramid = 0.707;
  mtmn_config.pyramid_times = 4;
  mtmn_config.p_threshold.score = 0.6;
  mtmn_config.p_threshold.nms = 0.7;
  mtmn_config.p_threshold.candidate_number = 20;
  mtmn_config.r_threshold.score = 0.7;
  mtmn_config.r_threshold.nms = 0.7;
  mtmn_config.r_threshold.candidate_number = 10;
  mtmn_config.o_threshold.score = 0.7;
  mtmn_config.o_threshold.nms = 0.7;
  mtmn_config.o_threshold.candidate_number = 1;
  return mtmn_config;
}
mtmn_config_t mtmn_config = app_mtmn_config();

face_id_name_list st_face_list;
static dl_matrix3du_t *aligned_face = NULL;

httpd_handle_t camera_httpd = NULL;

typedef enum
{
  START_STREAM,
  START_DETECT,
  SHOW_FACES,
  START_RECOGNITION,
  START_ENROLL,
  ENROLL_COMPLETE,
  DELETE_ALL,
} en_fsm_state;
en_fsm_state g_state;

typedef struct
{
  char enroll_name[ENROLL_NAME_LEN];
} httpd_resp_value;

httpd_resp_value st_name;

static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, index_html, strlen(index_html));
}

httpd_uri_t index_uri = {
  .uri       = "/",
  .method    = HTTP_GET,
  .handler   = index_handler,
  .user_ctx  = NULL
};

void app_httpserver_init ()
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  if (httpd_start(&camera_httpd, &config) == ESP_OK)
    Serial.println("httpd_start");
  {
    httpd_register_uri_handler(camera_httpd, &index_uri);
  }
}

void app_facenet_main()
{
  face_id_name_init(&st_face_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
  aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
  read_face_id_from_flash_with_name(&st_face_list);
}

static inline int do_enrollment(face_id_name_list *face_list, dl_matrix3d_t *new_id)
{
  ESP_LOGD(TAG, "START ENROLLING");
  int left_sample_face = enroll_face_id_to_flash_with_name(face_list, new_id, st_name.enroll_name);
  ESP_LOGD(TAG, "Face ID %s Enrollment: Sample %d",
           st_name.enroll_name,
           ENROLL_CONFIRM_TIMES - left_sample_face);
  return left_sample_face;
}

static esp_err_t send_face_list(WebsocketsClient &client)
{
  client.send("delete_faces"); 
  face_id_node *head = st_face_list.head;
  char add_face[64];
  for (int i = 0; i < st_face_list.count; i++) 
  {
    sprintf(add_face, "listface:%s", head->id_name);
    client.send(add_face);
    head = head->next;
  }
  return ESP_OK;
}

static esp_err_t delete_all_faces(WebsocketsClient &client)
{
  delete_face_all_in_flash_with_name(&st_face_list);
  client.send("delete_faces");
  return ESP_OK;
}

void open_door(WebsocketsClient &client) {
  if (digitalRead(relay_pin) == HIGH) {
    digitalWrite(green_led_pin, HIGH);
    digitalWrite(relay_pin, LOW);
    
    Serial.println("Door Unlocked");
    client.send("door_open");
    door_opened_millis = millis();
  }
}

// --- NEW HELPER FUNCTION FOR FIREBASE LOGGING ---
void logAccessToCloud(String name) {
  // Removed signupOK check. If Firebase is ready, we send.
  if (Firebase.ready()) {
    FirebaseJson json;
    json.set("name", name);
    // This inserts the server time automatically
    json.set("timestamp", "timestamp"); 
    
    // Push to path /access_logs
    if (Firebase.RTDB.pushJSON(&fbdo, "/access_logs", &json)) {
      Serial.print("LOGGED TO CLOUD: ");
      Serial.println(fbdo.dataPath());
    } else {
      Serial.print("CLOUD ERROR: ");
      Serial.println(fbdo.errorReason());
    }
  }
}

void handle_message(WebsocketsClient &client, WebsocketsMessage msg)
{
  if (msg.data() == "stream") {
    g_state = START_STREAM;
    client.send("STREAMING");
  }
  if (msg.data() == "detect") {
    g_state = START_DETECT;
    client.send("DETECTING");
  }
  
  // MANUAL UNLOCK
  if (msg.data() == "manual_open") {
    open_door(client);
    // Optionally log manual overrides too
    logAccessToCloud("MANUAL_OVERRIDE"); 
  }

  // --- FLASH CONTROL LOGIC ---
  if (msg.data().substring(0, 6) == "flash:") {
     String valStr = msg.data().substring(6);
     int val = valStr.toInt();
     ledcWrite(FLASH_CHANNEL, val);
  }

  if (msg.data().substring(0, 8) == "capture:") {
    g_state = START_ENROLL;
    char person[FACE_ID_SAVE_NUMBER * ENROLL_NAME_LEN] = {0,};
    msg.data().substring(8).toCharArray(person, sizeof(person));
    memcpy(st_name.enroll_name, person, strlen(person) + 1);
    client.send("CAPTURING");
  }
  if (msg.data() == "recognise") {
    g_state = START_RECOGNITION;
    client.send("RECOGNISING");
  }
  if (msg.data().substring(0, 7) == "remove:") {
    char person[ENROLL_NAME_LEN * FACE_ID_SAVE_NUMBER];
    msg.data().substring(7).toCharArray(person, sizeof(person));
    delete_face_id_in_flash_with_name(&st_face_list, person);
    send_face_list(client);
  }
  if (msg.data() == "delete_all") {
    delete_all_faces(client);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  
  // Initialize pins
  digitalWrite(relay_pin, HIGH);
  pinMode(relay_pin, OUTPUT);
  
  pinMode(green_led_pin, OUTPUT);
  digitalWrite(green_led_pin, LOW);
  
  pinMode(red_led_pin, OUTPUT);
  digitalWrite(red_led_pin, LOW);
  
  // Initialize Flash LED PWM
  ledcSetup(FLASH_CHANNEL, 5000, 8);
  ledcAttachPin(FLASH_PIN, FLASH_CHANNEL);
  ledcWrite(FLASH_CHANNEL, 0);
  
  camera_config_t config_cam; // Renamed to avoid conflict with Firebase config
  config_cam.ledc_channel = LEDC_CHANNEL_0;
  config_cam.ledc_timer = LEDC_TIMER_0;
  config_cam.pin_d0 = Y2_GPIO_NUM;
  config_cam.pin_d1 = Y3_GPIO_NUM;
  config_cam.pin_d2 = Y4_GPIO_NUM;
  config_cam.pin_d3 = Y5_GPIO_NUM;
  config_cam.pin_d4 = Y6_GPIO_NUM;
  config_cam.pin_d5 = Y7_GPIO_NUM;
  config_cam.pin_d6 = Y8_GPIO_NUM;
  config_cam.pin_d7 = Y9_GPIO_NUM;
  config_cam.pin_xclk = XCLK_GPIO_NUM;
  config_cam.pin_pclk = PCLK_GPIO_NUM;
  config_cam.pin_vsync = VSYNC_GPIO_NUM;
  config_cam.pin_href = HREF_GPIO_NUM;
  config_cam.pin_sscb_sda = SIOD_GPIO_NUM;
  config_cam.pin_sscb_scl = SIOC_GPIO_NUM;
  config_cam.pin_pwdn = PWDN_GPIO_NUM;
  config_cam.pin_reset = RESET_GPIO_NUM;
  config_cam.xclk_freq_hz = 20000000;
  config_cam.pixel_format = PIXFORMAT_JPEG;
  
  if (psramFound()) {
    config_cam.frame_size = FRAMESIZE_UXGA;
    config_cam.jpeg_quality = 10;
    config_cam.fb_count = 2;
  } else {
    config_cam.frame_size = FRAMESIZE_SVGA;
    config_cam.jpeg_quality = 12;
    config_cam.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config_cam);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
  s->set_vflip(s, 1);      
  s->set_hmirror(s, 1);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // --- FIREBASE INITIALIZATION (SIMPLIFIED FOR TEST MODE) ---
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // THIS IS THE IMPORTANT LINE: BYPASS SSL AUTHENTICATION
  config.signer.test_mode = true; 

  Serial.println("Firebase: Initializing in Test Mode (No Auth)...");
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  // ------------------------------

  app_httpserver_init();
  app_facenet_main();
  socket_server.listen(82);
  // DEFAULT: RECOGNITION MODE
  g_state = START_RECOGNITION;

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  auto client = socket_server.accept();
  client.onMessage(handle_message);
  dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, 320, 240, 3);
  http_img_process_result out_res = {0};
  out_res.image = image_matrix->item;

  send_face_list(client);
  client.send("STREAMING");
  
  while (client.available()) {
    client.poll();

    if (millis() - interval > door_opened_millis) {
      digitalWrite(relay_pin, HIGH);
      digitalWrite(green_led_pin, LOW);
    }
    
    if (millis() - red_led_duration > red_led_millis) {
      digitalWrite(red_led_pin, LOW);
    }

    fb = esp_camera_fb_get();

    if (g_state == START_DETECT || g_state == START_ENROLL || g_state == START_RECOGNITION)
    {
      out_res.net_boxes = NULL;
      out_res.face_id = NULL;

      fmt2rgb888(fb->buf, fb->len, fb->format, out_res.image);

      out_res.net_boxes = face_detect(image_matrix, &mtmn_config);
      
      if (out_res.net_boxes)
      {
        if (align_face(out_res.net_boxes, image_matrix, aligned_face) == ESP_OK)
        {
          out_res.face_id = get_face_id(aligned_face);
          last_detected_millis = millis();
          if (g_state == START_DETECT) {
            client.send("FACE DETECTED");
          }

          if (g_state == START_ENROLL)
          {
            int left_sample_face = do_enrollment(&st_face_list, out_res.face_id);
            char enrolling_message[64];
            sprintf(enrolling_message, "SAMPLE NUMBER %d FOR %s", ENROLL_CONFIRM_TIMES - left_sample_face, st_name.enroll_name);
            client.send(enrolling_message);
            
            if (left_sample_face == 0)
            {
              ESP_LOGI(TAG, "Enrolled Face ID: %s", st_face_list.tail->id_name);
              g_state = START_RECOGNITION;
              
              char captured_message[64];
              sprintf(captured_message, "FACE CAPTURED FOR %s", st_face_list.tail->id_name);
              client.send(captured_message);
              send_face_list(client);
            }
          }

          if (g_state == START_RECOGNITION  && (st_face_list.count > 0))
          {
            face_id_node *f = recognize_face_with_name(&st_face_list, out_res.face_id);
            if (f)
            {
              char recognised_message[64];
              sprintf(recognised_message, "DOOR OPEN FOR %s", f->id_name);
              open_door(client);
              client.send(recognised_message);
              
              // --- CLOUD LOGGING HERE ---
              logAccessToCloud(String(f->id_name));
            }
            else
            {
              Serial.println("Unrecognized face - Access Denied");
              digitalWrite(red_led_pin, HIGH);
              red_led_millis = millis();
              client.send("FACE NOT RECOGNISED");
            }
          }
          dl_matrix3d_free(out_res.face_id);
        }

      }
      else
      {
        if (g_state != START_DETECT) {
          client.send("NO FACE DETECTED");
        }
      }

      if (g_state == START_DETECT && millis() - last_detected_millis > 500) { 
        client.send("DETECTING");
      }

    }

    client.sendBinary((const char *)fb->buf, fb->len);

    esp_camera_fb_return(fb);
    fb = NULL;
  }
}