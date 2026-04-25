const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const cors = require('cors');
const { SerialPort, ReadlineParser } = require('serialport');

const app = express();
app.use(cors());
const server = http.createServer(app);
const io = new Server(server, { cors: { origin: '*', methods: ['GET', 'POST'] } });

// Configure Serial Port
const SERIAL_PORT_PATH = 'COM6'; // Matches standard UNO assignment
const BAUD_RATE = 115200;

let port;
let parser;

function connectSerial() {
  console.log(`[SYS] Attempting to connect to Serial on ${SERIAL_PORT_PATH}...`);
  
  port = new SerialPort({ path: SERIAL_PORT_PATH, baudRate: BAUD_RATE, autoOpen: false });
  parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

  port.open((err) => {
    if (err) {
      console.log(`[WARN] Serial Port Error: ${err.message}. Retrying in 5s...`);
      setTimeout(connectSerial, 5000);
      return;
    }
    console.log(`[SYS] Serial connection established on ${SERIAL_PORT_PATH}`);
  });

  port.on('close', () => {
    console.log(`[SYS] Serial Port Closed. Attempting reconnect in 5s...`);
    setTimeout(connectSerial, 5000);
  });

  port.on('error', (err) => console.log(`[SYS] Serial Error: `, err.message));

  parser.on('data', (data) => {
    try {
      const raw = data.trim();
      // Handle the strict protocol <SCAN:angle,distance> and <LOCK:angle,distance>
      if (raw.startsWith('<') && raw.endsWith('>')) {
        const payload = raw.slice(1, -1);
        const colonIdx = payload.indexOf(':');
        
        if (colonIdx > 0) {
          const type = payload.substring(0, colonIdx);
          const values = payload.substring(colonIdx + 1);
          const [angleStr, distanceStr] = values.split(',');
          
          if (angleStr && distanceStr) {
            const angle = parseFloat(angleStr);
            const distance = parseFloat(distanceStr);
            
            if (type === 'SCAN') {
              io.emit('telemetry_update', { angle, distance });
            } else if (type === 'LOCK') {
              io.emit('critical_alert', { angle, distance });
            }
          }
        }
      }
    } catch (e) {
      console.log(`[SYS] Parse Error: `, e.message, ` | Raw: ${data.trim()}`);
    }
  });
}

connectSerial();

io.on('connection', (socket) => {
  console.log(`[SYS] Client connected: ${socket.id}`);
  socket.on('disconnect', () => console.log(`[SYS] Client disconnected: ${socket.id}`));
});

const PORT = 3001;
server.listen(PORT, () => {
  console.log(`[A.R.T.E.M.I.S.] Hardware Bridge Server active on port ${PORT}`);
});
