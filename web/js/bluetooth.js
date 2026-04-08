let bluetoothDevice;
let characteristic;

function addLog(msg, type = "") {
    const log = document.getElementById("log");
    const empty = document.getElementById("log-empty");
    if (empty) empty.remove();
    const entry = document.createElement("div");
    entry.className = `log-entry${type ? " log-" + type : ""}`;
    const now = new Date();
    const time = `${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
    entry.innerHTML = `<span class="log-time">${time}</span><span class="log-msg">${msg}</span>`;
    log.appendChild(entry);
    log.scrollTop = log.scrollHeight;
}

function clearLog() {
    document.getElementById("log").innerHTML = '<div class="log-empty" id="log-empty">No activity.</div>';
}

async function connectBluetooth() {
    if (!navigator.bluetooth) {
        addLog("Web Bluetooth not supported", "err");
        return;
    }
    try {
        addLog("Requesting Bluetooth device...", "cmd");
        bluetoothDevice = await navigator.bluetooth.requestDevice({
            filters: [
                { services: ['ffe0'] },
                { name: 'HM-10' },
                { name: 'HMSoft' }
            ],
            optionalServices: ['ffe0']
        });
        addLog(`Connecting to ${bluetoothDevice.name}...`, "cmd");
        const server = await bluetoothDevice.gatt.connect();
        addLog("Connected to GATT Server", "ok");
        const service = await server.getPrimaryService('ffe0');
        characteristic = await service.getCharacteristic('ffe1');
        addLog("Ready to send commands!", "ok");
        document.getElementById('status-bar').className = 'status-bar connected';
        document.getElementById('status-text').textContent = 'Connected';
        document.getElementById('connect-btn').className = 'connect-btn connected';
        document.getElementById('connect-btn').innerHTML = '<svg viewBox="0 0 24 24"><path d="M9 16.17L4.83 12l-1.42 1.41L9 19 21 7l-1.41-1.41z"/></svg> Connected';
        document.querySelectorAll('.checkpoint-btn').forEach(btn => {
            btn.disabled = false;
            btn.querySelector('.cp-status').textContent = 'READY';
        });
        document.querySelectorAll('.signal-bar').forEach(bar => bar.classList.add('active'));
        bluetoothDevice.addEventListener('gattserverdisconnected', onDisconnected);
    } catch (error) {
        addLog(`Error: ${error.message}`, "err");
    }
}

async function sendCommand(command) {
    if (!characteristic) {
        addLog("Not connected!", "err");
        return;
    }
    try {
        addLog(`→ CMD checkpoint-0${command}`, "cmd");
        const encoder = new TextEncoder();
        await characteristic.writeValue(encoder.encode(command));
        addLog(`✓ Command sent`, "ok");
    } catch (error) {
        addLog(`✗ Error: ${error.message}`, "err");
    }
}

function onDisconnected() {
    addLog("Device disconnected", "err");
    document.getElementById('status-bar').className = 'status-bar';
    document.getElementById('status-text').textContent = 'Disconnected';
    document.getElementById('connect-btn').className = 'connect-btn';
    document.getElementById('connect-btn').innerHTML = '<svg viewBox="0 0 24 24"><path d="M17.71 7.71L12 2h-1v7.59L6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 11 14.41V22h1l5.71-5.71-4.3-4.29 4.3-4.29z"/></svg> Connect to Rover';
    document.querySelectorAll('.checkpoint-btn').forEach(btn => {
        btn.disabled = true;
        btn.querySelector('.cp-status').textContent = 'OFFLINE';
    });
    document.querySelectorAll('.signal-bar').forEach(bar => bar.classList.remove('active'));
}
