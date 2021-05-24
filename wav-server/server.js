const fs = require('fs')
const express = require('express')
const WaveFile = require('wavefile').WaveFile;

const app = express()
const port = 20343

const wav = new WaveFile(fs.readFileSync('./Kvatropirci - Ti si zame cokolada.wav'));
wav.toBitDepth(8, /* changeResolution */ true);
wav.toSampleRate(12000);

app.get('/stream', (req, res) => {
    res.statusCode = 200
    
    const samples = wav.getSamples(false, Int8Array)
    
    res.write(new Uint8Array(samples))

    res.end()
})

app.listen(port, () => {
    console.info(`Server is running on http://localhost:${port}`)
})
