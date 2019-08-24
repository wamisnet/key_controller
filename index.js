'use strict';
const express = require('express');
const bodyParser = require('body-parser');
const Obniz = require("obniz");

const obniz = new Obniz("your obniz");
const max = 512;

let motor = -1;
const app = express();
app.use(bodyParser.json());
app.use(express.static('public'));

let position = -1;
let connection = false;
let key_config = {
    open: { pos: 246 },
    close: { pos: 171 }
}
let key_status = "free";

app.get('/api/v1/position', (req, res) => {
    res.json({ position: position });
});

app.get('/api/v1/setting', (req, res) => {
    console.log(req);
    if (req.query.mode === "open") {
        key_config.open.pos = position;
        res.json({ position: position });
    } else if (req.query.mode === "close") {
        key_config.close.pos = position;
        res.json({ position: position });
    } else if (req.query.mode === "save") {
        console.log(key_config)
        res.json({ state: "success" });
    } else {
        res.json({ state: "error" });
    }
});

app.post('/api/v1/key_control', async (req, res) => {
    console.log(req)
    if (req.body && req.body.command) {
        console.log(req.body.command)
        if (!connection) {
            res.json({ status: "error", description: "device offline" });
            return;
        }
        if (req.body.command === "lock") {
            key_status = "close";
            await start_key();
        } else {
            key_status = "open";
            await start_key();
        }
    }
    res.json({ position: position });
});

app.listen(3456, () => console.log('Listening on port 3000'));

obniz.onclose = function () {
    console.log("obniz.onclose");
    position = -1;
    connection = false;
}

obniz.onconnect = async function () {
    connection = true;
    motor = obniz.wired("DCMotor", { forward: 3, back: 4 });
    obniz.display.clear();
    obniz.display.print("Connecting ObnizKey");
    obniz.io5.output(false);
    obniz.io0.output(false);
    obniz.setVccGnd(6, 8, '5v');
    obniz.getAD(7).start(async function (value) {
        position = Math.floor(value * 100);

        if (key_status === "open") {
            console.log("open ps:" + position + " conf:" + key_config.open.pos + " diff:" + Math.abs(position - key_config.open.pos));
            if (Math.abs(position - key_config.open.pos) < 20) {
                await finish_key();
            }
        } else if (key_status === "close") {
            console.log("close ps:" + position + " conf:" + key_config.close.pos + " diff:" + Math.abs(position - key_config.close.pos));
            if (Math.abs(position - key_config.close.pos) < 20) {
                await finish_key()
            }
        } else {
            console.log("position:" + position);
        }
    });
    setInterval(async () => {
        console.log("setInterval");
        if (key_status === "free") {
            obniz.io2.output(true);
            obniz.io1.output(false);
            await obniz.wait(100);
            obniz.io2.output(false);
        }
    }, 1000);
}

async function start_key() {
    motor.power(100);
    motor.move(true);
    obniz.io2.output(false);
    obniz.io1.output(true);
    await obniz.wait(100);
    obniz.io1.output(false);
    obniz.io1.end();
    obniz.io2.end();
}

async function finish_key() {
    console.log("finish_key");
    key_status = "free";
    motor.stop();
    obniz.io2.output(false);
    obniz.io1.output(true);
    await obniz.wait(100);
    obniz.io1.output(false);
    await obniz.wait(100);
    obniz.io2.output(true);
    obniz.io1.output(false);
    await obniz.wait(100);
    obniz.io2.output(false);
    obniz.io1.end();
    obniz.io2.end();
}