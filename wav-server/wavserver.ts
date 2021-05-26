import express, { Express } from "express";
import { Streamer } from "./Streamer";
import moment from "moment";
import { StreamListener } from "./StreamListener";

async function sleep(time_ms: number) {
  return new Promise((resolve) => setTimeout(resolve, time_ms));
}

class WavServer {
  app: Express;
  streamer: Streamer;

  constructor(public port: number, playlist: string[]) {
    this.app = express();
    this.streamer = new Streamer(playlist);

    /**
     * GET /
     * HTML page that displays current running track
     */
    this.app.get("/", async (req, res) => {
      res.statusCode = 200;

      const playingDetails = this.streamer.getCurrentlyPlayingDetails();
      let response = `<h1>WiFi Radio controller</h1>`;

      if (playingDetails.track) {
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
          <div><a href="/next">Next track</a></div>`;

      if (!req.query.noRefresh) {
        response += `<script>
            setTimeout(() => {
                location.reload();
            }, 800);
        </script>`;
      }

      res.send(response);
    });

    /**
     * GET /next
     * Skips to next track and redirects back to info page
     */
    this.app.get("/next", async (req, res) => {
      this.streamer.playNextTrackFromPlaylist();

      res.redirect("/");
    });

    /**
     * GET /previous
     * Skips to previous track and redirects back to info page
     */
    this.app.get("/previous", async (req, res) => {
      this.streamer.playPreviousTrackFromPlaylist();

      res.redirect("/");
    });

    this.app.get("/stream", async (req, res) => {
      console.log(`Client connected (${req.socket.remoteAddress})`);

      const listener = new StreamListener(
        Math.random().toString(36).substr(2, 5),
        (chunk) => {
          res.write(chunk);
        }
      );

      let client_connected = true;

      req.on("close", () => {
        console.log(`Client disconnected (${req.socket.remoteAddress})`);

        this.streamer.detachListener(listener);

        client_connected = false;
      });

      res.statusCode = 200;

      this.streamer.attachListener(listener);

      while (client_connected) {
        await sleep(500);
      }

      res.end();
    });

    /**
     * GET /track/current
     * Retrieves current track information
     * Response is in format PLAYING <TODO: track_path>;<track_length_ms>;<sampling_frequency>;<current_chunk>;<total_chunks>
     * or STOPPED
     */
    this.app.get("/track/current", async (req, res) => {
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
    });

    /**
     * GET /track/:trackIndex
     * Retrieves track information for specified trackIndex (index in playlist)
     * Response is in format <track_path>;<track_length_ms>;<sampling_frequency>;<total_chunks>
     */
    this.app.get("/track/:id", async (req, res) => {
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
    });

    /**
     * GET /track/:trackIndex/chunk/:chunkIndex
     * Get the raw data of specified chunk of specified track.
     */
    this.app.get("/track/:id/chunk/:chunkIndex", async (req, res) => {
      console.log(
        `Client requested track ${req.params.id}, chunk ${req.params.chunkIndex} (${req.socket.remoteAddress})`
      );

      const item = this.streamer.playlist.getItem(parseInt(req.params.id));
      if (!item) {
        res.statusCode = 404;
        res.send("NOTFOUND");
        res.end();
        return;
      }

      const chunk = item.track.getChunk(parseInt(req.params.chunkIndex));
      if (!chunk || chunk.length == 0) {
        res.statusCode = 404;
        res.send("NOTFOUND");
        res.end();
        return;
      }

      res.statusCode = 200;
      res.write(chunk);
      res.end();
    });

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

  startServer() {
    this.app.listen(this.port, () => {
      console.info(`WavServer is running on http://localhost:${this.port}`);
    });

    this.streamer.startPlaylist();
  }
}

export { WavServer };
