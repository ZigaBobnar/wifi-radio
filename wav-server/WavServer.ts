import express, { Express } from "express";
import moment from "moment";
import { getClientId } from "./utils";
import { Player } from "./Player";
import { ServerConfig } from "./config";

class WavServer {
  app: Express;
  player: Player;

  constructor(public readonly serverConfig: ServerConfig) {
    this.app = express();

    this.player = new Player(
      serverConfig.tracks,
      serverConfig.chunkSize,
      serverConfig.sampleRate
    );

    /**
     * GET /
     * HTML page that displays current running track
     */
    this.app.get("/", async (req, res) => {
      const details = await this.player.getPlayerPlaybackDetails();

      res.statusCode = 200;

      const playingDetails = this.player.getPlayerPlaybackDetails();
      let response = `<body style="font-family: 'Roboto', 'Arial'"><h1>WiFi Radio controller</h1>`;

      response += `<div><h3>Tracks in playlist</h3><div>`;
      for (let track of details.tracks) {
        response += `<div style="padding: 8px"><span style="font-size: 1.2em; font-weight: 600">${
          track.definition.artist ? track.definition.artist + " - " : ""
        }${
          track.definition.title ?? track.definition.path
        }</span><br />Status: ${
          track.isLoaded ? "Loaded" : "Not loaded"
        }<br />${
          track.isLoaded
            ? `Duration: ${track.totalLengthMs} ms, Sample rate: ${track.sampleRate} Hz, Chunk length: ${track.chunkSize} bytes`
            : ""
        }</div>`;
      }
      response += `</div></div>`;

      response += `<div><h3>Connected clients</h3><div>`;
      for (let clientId in details.clients) {
        const client = details.clients[clientId];
        response += `<div style="padding: 8px"><span style="font-size: 1.2em; font-weight: 600">${clientId}</span><br />Track id: ${
          client.trackId
        }, Chunk id: ${client.chunkId}<br />${
          client.trackId >= 0
            ? `<div>${details.tracks[client.trackId].definition.path}</div><div><a href="/previous?clientId=${clientId}">Previous</a> | <a href="/next?clientId=${clientId}">Next</a></div>`
            : ""
        }</div>`;
      }
      response += `</div></div>`;

      /*if (playingDetails.track) {
        response += `<div>Currently playing: ${playingDetails.track.path}</div>
          <div>Progress: ${playingDetails.track.percentage}% [chunk ${playingDetails.track.currentChunk} / ${playingDetails.track.totalChunks}]</div>`;
      } else {
        response += `<div>Currently playing: (none)</div>
          <div>Progress: 0% [chunk 0]</div>`;
      }

      response += `<div>Listeners: ${playingDetails.listeners.length}</div>
          <div>
              <div>Playlist:</div>
              <ul>${playingDetails.tracks
                .map(
                  (i) =>
                    `<li>${i.path} ${
                      Math.round(i.totalLengthMs / 10 / 60) / 100
                    }</li>`
                )
                .join("")}</ul>
          </div>

          <div><a href="/previous">Previous track</a></div>
          <div><a href="/next">Next track</a></div>`;*/

      if (!req.query.no_refresh) {
        response += `<script>
            setTimeout(() => {
                location.reload();
            }, 800);
        </script>`;
      }

      response += `</body>`;

      res.send(response);
    });

    /**
     * GET /next
     * Skips to next track and redirects back to info page
     */
    this.app.get("/next", async (req, res) => {
      this.player.moveForward(getClientId(req));

      res.redirect("/");
    });

    /**
     * GET /previous
     * Skips to previous track and redirects back to info page
     */
    this.app.get("/previous", async (req, res) => {
      this.player.moveBackward(getClientId(req));

      res.redirect("/");
    });

    this.app.get("/next-chunk", async (req, res) => {
      const clientId = getClientId(req);

      console.log(`Client ${clientId} requested next chunk`);

      const chunk = await this.player.getNextChunk(clientId);
      if (!chunk || chunk.data.length == 0) {
        res.statusCode = 404;
        res.send("NOTFOUND");
      } else {
        console.log(
          `Client ${clientId} => sending track ${chunk.trackId}, chunk ${chunk.chunkId}`
        );

        res.statusCode = 200;
        res.write(chunk.data);
      }

      res.end();
    });

    /**
     * GET /track/current
     * Retrieves current track information
     * Response is in format PLAYING <TODO: track_path>;<track_length_ms>;<sampling_frequency>;<current_chunk>;<total_chunks>
     * or STOPPED
     */
    /*this.app.get("/track/current", async (req, res) => {
      console.log(
        `Client requested current track info (${req.socket.remoteAddress})`
      );

      const trackInfo = this.streamer.getCurrentlyPlayingDetails();

      if (trackInfo.track) {
        res.send(
          `PLAYING ${Math.round(trackInfo.track.totalLengthMs)};${
            trackInfo.track.samplingFrequency
          };${trackInfo.track.currentChunk};${trackInfo.track.totalChunks}`
        );
      } else {
        res.send(`STOPPED`);
      }

      res.end();
    });*/

    /**
     * GET /track/:trackIndex
     * Retrieves track information for specified trackIndex (index in playlist)
     * Response is in format <track_path>;<track_length_ms>;<sampling_frequency>;<total_chunks>
     */
    /*this.app.get("/track/:id", async (req, res) => {
      console.log(
        `Client requested track info for ${req.params.id} (${req.socket.remoteAddress})`
      );

      const trackInfo = this.streamer.getTrackDetails(parseInt(req.params.id));
      if (!trackInfo) {
        res.status(404);
        res.end();
        return;
      }

      res.send(`${trackInfo.samplingFrequency}${trackInfo.totalChunks}`);

      res.end();
    });*/

    /**
     * GET /time/now
     * Returns current time in local timezone in format YYYY-MM-DD HH:mm:ss
     */
    this.app.get("/time/now", async (req, res) => {
      console.log(
        `Client requested current time info (${req.socket.remoteAddress})`
      );

      res.send(moment().format("YYYY-MM-DD HH:mm:ss"));

      res.end();
    });
  }

  async startServer() {
    await this.player.init();

    this.app.listen(this.serverConfig.port, () => {
      console.info(
        `WavServer is running on http://localhost:${this.serverConfig.port}`
      );
    });

    // await this.player.loadMedia();
  }
}

export { WavServer };
