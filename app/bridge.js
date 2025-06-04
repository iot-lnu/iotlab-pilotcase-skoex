const aedes = require('aedes')()
const http = require('http')
const websocketStream = require('websocket-stream')
const mqtt = require('mqtt')

const server = http.createServer()
websocketStream.createServer({ server }, aedes.handle)
server.listen(5500, () => {
  console.log('WebSocket MQTT server listening on http://localhost:5500')
})

const upstreamClient = mqtt.connect('tcp://broker.mqtt.cool:1883', {
  clientId: 'bridge-client',
})

upstreamClient.on('connect', () => {
  console.log('Connected to upstream MQTT broker')
  upstreamClient.subscribe('#')
})

upstreamClient.on('message', (topic, payload) => {
  aedes.publish({ topic, payload })
})

aedes.on('publish', (packet, client) => {
  if (client) {
    upstreamClient.publish(packet.topic, packet.payload)
  }
})
