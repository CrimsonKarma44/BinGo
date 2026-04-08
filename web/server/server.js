const http = require('http');

const PORT = 3000;
const ESP32_HOST = '10.246.73.176';
const ESP32_PORT = 80;

const server = http.createServer((req, res) => {
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'POST, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

    if (req.method === 'OPTIONS') {
        res.writeHead(200);
        res.end();
        return;
    }

  if (req.method === 'POST' && req.url === '/command') {
        console.log("calling the '/command' endpoint")
        let body = '';

        req.on('data', chunk => {
            body += chunk.toString();
        });

        req.on('end', () => {
            try {
                const data = JSON.parse(body);
                const command = data.command;

                if (!command) {
                    res.writeHead(400, { 'Content-Type': 'text/plain' });
                    res.end('Error: Missing command field');
                    return;
                }

                console.log(`Forwarding command to ESP32: ${command}`);

                const esp32Req = http.request({
                    hostname: ESP32_HOST,
                    port: ESP32_PORT,
                    path: '/command',
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                        'Content-Length': Buffer.byteLength(body)
                    }
                }, (esp32Res) => {
                    let esp32Body = '';
                    esp32Res.on('data', chunk => { esp32Body += chunk.toString(); });
                    esp32Res.on('end', () => {
                        console.log(`ESP32 response: ${esp32Body}`);
                        res.writeHead(200, { 'Content-Type': 'textc/plain' });
                        res.end(esp32Body);
                    });
                });

                esp32Req.on('error', (err) => {
                    console.error(`ESP32 error: ${err.message}`);
                    res.writeHead(502, { 'Content-Type': 'text/plain' });
                    res.end('Error: Cannot connect to ESP32');
                });

                esp32Req.write(body);
                esp32Req.end();

            } catch (error) {
                res.writeHead(400, { 'Content-Type': 'text/plain' });
                res.end('Error: Invalid JSON');
            }
        });
    } else {
        res.writeHead(404, { 'Content-Type': 'text/plain' });
        res.end('Error: Not Found');
    }
});

server.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
    console.log(`Forwarding commands to ESP32 at ${ESP32_HOST}:${ESP32_PORT}`);
});
