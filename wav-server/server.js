const fs = require('fs')
const express = require('express')
const WaveFile = require('wavefile').WaveFile;
const { sleep } = require('./utils');
const e = require('express');

const port = 20343;

class StreamMedia {
    constructor(path, sample_rate, chunk_size) {
        this.path = path;
        
        const wav = new WaveFile(fs.readFileSync(path));
        wav.toBitDepth(8, true);
        wav.toSampleRate(sample_rate);
        
        this.samples = wav.getSamples(false, Int8Array);
        this.chunk_size = chunk_size;
        this.chunk_count = Math.ceil(this.samples.length / chunk_size);
    }
}

class Listener {
    constructor(name, fn) {
        this.name = name;
        this.fn = fn;
    }
}

class Streamer {
    constructor() {
        this.currently_playing = '(none)';
        this.currently_playing_index = -1;
        this.chunk_size = 50000;//11520;
        this.sample_rate = 50000;//11520;
        this.current_chunk = 0;
        this.current_media = null;
        this.playlist = [
            './music/Kvatropirci - Ti si zame cokolada (2).wav',
            //'./music/Kvatropirci - Ti si zame cokolada.wav',
            './music/Cuki - Bam bam bam.wav',
            './music/Veseli svatje - Pod cvetocimi kostanji.wav',
            './music/Parni valjak - Zastave.wav',
        ];
        this.media = [];
        this.listeners = [];
    }

    startStream() {
        for (const media_path of this.playlist) {
            this.loadMedia(media_path);
        }

        if (this.currently_playing_index == -1) {
            this.currently_playing_index = 0;
            this.currently_playing = this.playlist[this.currently_playing_index];

            this.playMedia(this.currently_playing);
        }

        setTimeout(() => {
            this.sendChunk(true);
        }, 990);
    }

    sendChunk(noIncrement = false) {
        if (!noIncrement) {
            this.current_chunk++;
        }

        if (this.current_chunk > this.current_media.chunk_count) {
            console.log('Finished current media');
            this.playNext();
        }

        if (this.current_media) {
            console.log(`Streamer chunk [${Math.round(10000 * this.current_chunk / this.current_media.chunk_count) / 100}%]`)
            
            const chunk = this.current_media.samples.slice(this.current_chunk * this.current_media.chunk_size, (this.current_chunk + 1) * this.current_media.chunk_size);
            
            for (let listener of this.listeners) {
                listener.fn(new Uint8Array(chunk));
            }
        } else {
            console.log('No media')
        }

        setTimeout(() => {
            this.sendChunk();
        }, 990);
    }

    playNext() {
        this.currently_playing_index++;

        if (this.currently_playing_index >= this.playlist.length) {
            this.currently_playing_index = 0;
        }

        this.currently_playing = this.playlist[this.currently_playing_index];

        this.playMedia(this.currently_playing);
    }

    playMedia(path) {
        const media = this.media.find(m => m.path == path);
        if (!media) {
            console.log(`Media ${path} is not loaded`)
        }
        
        console.log(`Playing ${path}`)

        this.current_media = media;
        this.current_chunk = 0;
    }

    loadMedia(path) {
        console.log(`Loading ${path}`)

        const load_media = new StreamMedia(path, this.sample_rate, this.chunk_size);
        this.media.push(load_media)

        console.log(`Loaded ${load_media.path}, ${load_media.samples.length} samples`);
    }

    attachListener(listener) {
        console.log(`Attaching listener ${listener.name}`)

        this.listeners.push(listener);
    }

    detachListener(listener) {
        console.log(`Detaching listener ${listener.name}`)

        this.listeners = this.listeners.filter(l => l.name != listener.name);
    }
};

const streamer = new Streamer();

const app = express()

app.get('/', async (req, res) => {
    res.statusCode = 200;

    res.send(`<div>Currently playing: ${streamer.currently_playing}</div>
    <div>Progress: ${Math.round(10000 * streamer.current_chunk / streamer.current_media.chunk_count) / 100}% [chunk ${streamer.current_chunk} / ${streamer.current_media.chunk_count}]</div>
    <div>Listeners: ${streamer.listeners.length}</div>
    <a href="/next">Next track</a>

    <script>
        setTimeout(() => {
            location.reload();
        }, 800);
    </script>`);
})

app.get('/next', async (req, res) => {
    streamer.playNext();

    res.redirect('/');
})

app.get('/stream', async (req, res) => {
    console.log(`Client connected (${req.socket.remoteAddress})`)

    const listener = new Listener(Math.random().toString(36).substr(2, 5), (chunk) => {
        res.write(chunk);
    });
    
    let client_connected = true;

    streamer.attachListener(listener);

    req.on('close', () => {
        console.log(`Client disconnected (${req.socket.remoteAddress})`)

        streamer.detachListener(listener);

        client_connected = false;
    });

    res.statusCode = 200;

    while (client_connected) {
        await sleep(500);
    }

    res.end()
})

app.listen(port, () => {
    console.info(`Server is running on http://localhost:${port}`)
})

streamer.startStream();
