'use strict';
let WebSocket = require('ws');
let catbox = require('./catbox-v8');

let isolate = new catbox.Isolate({ inspector: true });
(async function() {
	let context = await isolate.createContext({ inspector: true });
	let script = await isolate.compileScript('for(;;)debugger;', { filename: 'example.js' });
	await script.run(context);
}()).catch(console.error);

let channel = isolate.createInspectorSession();
// used as a dummy transport layer
let wss = new WebSocket.Server({ port: 10000 });

wss.on('connection', function(ws) {
	let channel = isolate.createInspectorSession();
	function dispose() {
		try {
			channel.dispose();
		} catch (err) {}
	}
	ws.on('error', dispose);
	ws.on('close', dispose);

	ws.on('message', function(message) {
		try {
			channel.dispatchProtocolMessage(message);
		} catch (err) {
			ws.close();
		}
	});

	function send(message) {
		try {
			ws.send(message);
		} catch (err) {
			dispose();
		}
	}
	channel.onResponse = (callId, message) => send(message);
	channel.onNotification = send;
});
console.log('Inspector: chrome-devtools://devtools/bundled/inspector.html?experiments=true&v8only=true&ws=127.0.0.1:10000');
