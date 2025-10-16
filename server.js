const express = require("express"); //importing express framework  (simplifies building web servers in Node.js)
const cors = require("cors"); //Cross-Origin Resource Sharing  (shares resources for example frontend and backend run on different ports)
const app = express();

app.use(cors());
app.use(express.json());

// Mock task database
const devices = {
  "tag-001": {
    task_id: 47,
    title: "Design Login UI",
    assignee: "Alex",
    status: "todo",
    last_column: 1,
  },
  "tag-002": {
    task_id: 48,
    title: "Write Unit Tests",
    assignee: "Sam",
    status: "todo",
    last_column: 1,
  },
  "tag-003": {
    task_id: 49,
    title: "Deploy to Production",
    assignee: "Jordan",
    status: "todo",
    last_column: 1,
  },
};

// Map beacon Major values to column names
const columnMap = {
  1: "todo",
  2: "in_progress",
  3: "review",
  4: "done",
};

// Endpoint: E-ink tag reports location update

//defining the post endpoint route
// This route UPDATES location (changes server state)
app.post("/api/eink-tags/:deviceId/location", (req, res) => {
  //request and response
  const { deviceId } = req.params; //route parameter
  const { major, rssi } = req.body; //major is the beacon major value, rssi is the signal strength

  const newStatus = columnMap[major]; //map major to column name
  const timestamp = new Date().toISOString(); //current time in ISO format

  console.log("\n" + "=".repeat(50)); //debugging console logs
  console.log(`LOCATION UPDATE`);
  console.log(`Device: ${deviceId}`);
  console.log(`Beacon Major: ${major} → Column: ${newStatus}`);
  console.log(`RSSI: ${rssi} dBm`);
  console.log(`Time: ${timestamp}`);

  if (devices[deviceId]) {
    //if device exists in mock database update its status
    const oldStatus = devices[deviceId].status;
    devices[deviceId].status = newStatus;
    devices[deviceId].last_column = major;

    console.log(`Task #${devices[deviceId].task_id} moved`);
    console.log(`   ${oldStatus} → ${newStatus}`);
    console.log(`   "${devices[deviceId].title}"`);
  } else {
    //if device does not exist in mock database log unknown device
    console.log(`Unknown device: ${deviceId}`);
  }
  console.log("=".repeat(50) + "\n");

  res.json({
    success: true,
    device: devices[deviceId],
    column: newStatus,
  });
});

// Endpoint: E-ink tag requests its assigned task

//defining the get endpoint route
// This route READS task info (no changes to server state)
app.get("/api/eink-tags/:deviceId/task", (req, res) => {
  const { deviceId } = req.params;

  console.log(`Task requested by ${deviceId}`);

  if (devices[deviceId]) {
    res.json(devices[deviceId]);
  } else {
    res.status(404).json({ error: "No task assigned" });
  }
});

// Status dashboard
// Handles API("creation"), when X happens run Y piece of code
// This route READS all device statuses (no changes to server state)
app.get("/api/status", (req, res) => {
  res.json({
    devices,
    columns: columnMap,
  });
});

// Start server (site runs on port 3000)
const PORT = 3000;
app.listen(PORT, () => {
  console.log(`\n${"=".repeat(50)}`);
  console.log(`Mock Backend Running`);
  console.log(`http://localhost:${PORT}`);
  console.log(`${"=".repeat(50)}\n`);
  console.log(`Endpoints:`);
  console.log(`  POST /api/eink-tags/:deviceId/location`);
  console.log(`  GET  /api/eink-tags/:deviceId/task`);
  console.log(`  GET  /api/status\n`);
});

// For Noman's understanding of express
// Raw Node.js (without Express)
/*
const http = require('http');

const server = http.createServer((req, res) => {
  // You have to manually check the URL
  if (req.url === '/api/status' && req.method === 'GET') {
    res.writeHead(200, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ devices, columns: columnMap }));
  } else if (req.url.startsWith('/api/eink-tags/') && req.method === 'GET') {
    // Extract deviceId manually... painful!
  } else {
    res.writeHead(404);
    res.end('Not Found');
  }
});

server.listen(3000);
*/

// With Express
/*
const express = require('express');
const app = express();

app.get('/api/status', (req, res) => {
  res.json({ devices, columns: columnMap });
});

app.listen(3000);
*/
