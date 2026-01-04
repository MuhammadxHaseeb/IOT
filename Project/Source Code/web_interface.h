// camera_index.h
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