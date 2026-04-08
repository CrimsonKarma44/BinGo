let serverUrl = "http://localhost:3000";

function pad(n) { return String(n).padStart(2, "0"); }
function timestamp() {
    const d = new Date();
    return `${pad(d.getHours())}:${pad(d.getMinutes())}:${pad(d.getSeconds())}`;
}

function updateClock() {
    document.getElementById("footer-time").textContent = timestamp();
}
setInterval(updateClock, 1000);
updateClock();

function addLog(msg, type = "") {
    const log = document.getElementById("log");
    const empty = document.getElementById("log-empty");
    if (empty) empty.remove();
    const entry = document.createElement("div");
    entry.className = `log-entry${type ? " log-" + type : ""}`;
    entry.innerHTML = `<span class="log-time">${timestamp()}</span><span class="log-msg">${msg}</span>`;
    log.appendChild(entry);
    log.scrollTop = log.scrollHeight;
}

function clearLog() {
    document.getElementById("log").innerHTML = '<div class="log-empty" id="log-empty">No commands sent.</div>';
}

async function sendCommand(command) {
    try {
        addLog(`→ CMD nav checkpoint-0${command}`, "cmd");
        const response = await fetch(`${serverUrl}/command`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ command: command }),
        });
        const text = await response.text();
        if (response.ok && text.startsWith("OK:")) {
            addLog(`✓ ACK — routing to checkpoint ${command}`, "ok");
        } else if (text.startsWith("ERROR:")) {
            addLog(`✗ Server error: ${text}`, "err");
        } else {
            addLog(`✗ Server error: ${response.status}`, "err");
        }
    } catch (error) {
        addLog(`✗ Connection failed: ${error.message}`, "err");
    }
}
